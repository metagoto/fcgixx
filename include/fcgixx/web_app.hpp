#pragma once


#include "../fastcgi.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstdio>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../scheduler.hpp"



namespace runpac { namespace fcgixx {


template<class T, typename App>
class Listener : public scheduler::event_handler
{
public:
    Listener(scheduler& sched, App& app) : mysched(sched), myapp(app)
    {
        scheduler::handler_properties properties;
        properties.poll_events  = POLLIN;
        properties.read_timeout = 0;
        mysched.register_handler(0, *this, properties);
    }
    virtual ~Listener()
    {
        mysched.remove_handler(0);
    }

private:
    virtual void fd_is_readable(int fd)
    {
        int       socket;
        sockaddr  sa;
        socklen_t sa_len = sizeof(sa);

        socket = accept(fd, &sa, &sa_len);
        if (socket >= 0)
            new T(mysched, socket, myapp);
        else
            throw std::runtime_error(std::string("accept() failed: ") + strerror(errno));
    }
    virtual void fd_is_writable(int)
    {
        throw std::logic_error("This routine should not be called.");
    }
    virtual void read_timeout(int)
    {
        throw std::logic_error("This routine should not be called.");
    }
    virtual void write_timeout(int)
    {
        throw std::logic_error("This routine should not be called.");
    }
    virtual void error_condition(int)
    {
        throw std::logic_error("This routine should not be called.");
    }
    virtual void pollhup(int)
    {
        throw std::logic_error("This routine should not be called.");
    }

    scheduler& mysched;
    App& myapp;
};

template<class T, typename App>
class ConnectionHandler : public scheduler::event_handler,
                          public FCGIProtocolDriver::OutputCallback
{
public:
    ConnectionHandler(scheduler& sched, int sock, App& app)
        : mysched(sched), mysocket(sock), is_write_handler_registered(false),
          driver(*this), terminate(false), myapp(app)
    {
        if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1)
            throw std::runtime_error(std::string("Can set non-blocking mode: ") + strerror(errno));

        properties.poll_events  = POLLIN;
        properties.read_timeout = 0;
        mysched.register_handler(mysocket, *this, properties);
    }
    ~ConnectionHandler()
    {
        mysched.remove_handler(mysocket);
        close(mysocket);
    }

private:
    virtual void operator() (void const * buf, size_t count)
    {
        if (write_buffer.empty())
        {
            int rc = write(mysocket, buf, count);
            if (rc >= 0)
                write_buffer.append(static_cast<char const *>(buf)+rc, count-rc);
            else if (errno != EINTR && errno != EAGAIN)
            {
                char tmp[1024];
                std::snprintf(tmp, sizeof(tmp), "An error occured while writing to fd %d: %s",
                              mysocket, strerror(errno));
                throw fcgi_io_callback_error(tmp);
            }
            else
                write_buffer.append(static_cast<char const *>(buf), count);
        }
        else
            write_buffer.append(static_cast<char const *>(buf), count);

        if (!write_buffer.empty() && is_write_handler_registered == false)
        {
            properties.poll_events   = POLLIN | POLLOUT;
            properties.write_timeout = 0;
            mysched.register_handler(mysocket, *this, properties);
            is_write_handler_registered = true;
        }
    }
    virtual void fd_is_readable(int fd)
    {
        char tmp[1024*10];
        int rc = read(fd, tmp, sizeof(tmp));
        if (rc > 0)
        {
            try
            {
                driver.process_input(tmp, rc);
                FCGIRequest* req = driver.get_request();
                if (req)
                {
                    if (req->keep_connection == false)
                        terminate = true;
                    req->handler_cb = new T(myapp);
                    req->handler_cb->operator()(req);
                }
            }
            catch(std::exception const & e)
            {
                std::cerr << "Caught exception thrown in FCGIProtocolDriver: " << e.what() << std::endl
                          << "Terminating connection " << mysocket << "." << std::endl;
                delete this;
                return;
            }
            catch(...)
            {
                std::cerr << "Caught unknown exception in FCGIProtocolDriver; terminating connection "
                          << mysocket << "." << std::endl;
                delete this;
                return;
            }
        }
        else if (rc <= 0 && errno != EINTR && errno != EAGAIN)
        {
            std::cerr << "An error occured while reading from fd " << mysocket << ": " << strerror(errno) << std::endl;
            delete this;
            return;
        }
        terminate_if_we_shall();
    }
    virtual void fd_is_writable(int fd)
    {
        if (write_buffer.empty())
        {
            properties.poll_events = POLLIN;
            mysched.register_handler(mysocket, *this, properties);
            is_write_handler_registered = false;
        }
        else
        {
            int rc = write(fd, write_buffer.data(), write_buffer.length());
            if (rc > 0)
                write_buffer.erase(0, rc);
            else if (rc < 0 && errno != EINTR && errno != EAGAIN)
            {
                std::cerr << "An error occured while writing to fd " << mysocket << ": " << strerror(errno) << std::endl;
                delete this;
                return;
            }
        }
        terminate_if_we_shall();
    }
    virtual void read_timeout(int)
    {
        throw std::logic_error("Not implemented yet.");
    }
    virtual void write_timeout(int)
    {
        throw std::logic_error("Not implemented yet.");
    }
    virtual void error_condition(int)
    {
        throw std::logic_error("Not implemented yet.");
    }
    virtual void pollhup(int)
    {
        throw std::logic_error("Not implemented yet.");
    }
    void terminate_if_we_shall()
    {
        if (terminate && driver.have_active_requests() == false && write_buffer.empty())
            delete this;
    }

    scheduler& mysched;
    scheduler::handler_properties properties;
    int mysocket;
    bool is_write_handler_registered;
    FCGIProtocolDriver driver;
    std::string write_buffer;
    bool terminate;
    App& myapp;
};

//
// FastCGI Handler
//

template<typename App>
class RequestHandler : public FCGIRequest::handler
{
public: RequestHandler(App& app) : app(app) { }

private:
    virtual void operator()(FCGIRequest* req)
    {
        if (!req->stdin_eof)
            return;

        // Make sure we are a responder.

        if (req->role != FCGIRequest::RESPONDER)
        {
            req->write("We can't handle any role but RESPONDER.", 39, FCGIRequest::STDERR);
            req->end_request(1, FCGIRequest::UNKNOWN_ROLE);
            return;
        }

        app.process(req);
    }

    App& app;
};


template<typename T>
struct web_app
{

    int run()
    {
        try {

            // Set a limit on the number of open files.
            rlimit rlim;
            getrlimit(RLIMIT_NOFILE, &rlim);
            rlim.rlim_cur = 8;
            rlim.rlim_max = 8;
            if (setrlimit(RLIMIT_NOFILE, &rlim) != 0)
                throw std::runtime_error("setrlimit() failed.");

            T app;

            scheduler sched;
            Listener< ConnectionHandler<RequestHandler<T>, T>, T> listener(sched, app);
            while(!sched.empty()) {
                sched.schedule();
                //sched.dump(std::cerr);
            }
        }
        catch(const std::exception &e) {
            std::cerr << "exception: " << e.what() << std::endl;
            throw;
        }
        catch(...) {
            std::cerr << "unknown exception." << std::endl;
            throw;
        }
        return 0;
    }



};



} } // ns


