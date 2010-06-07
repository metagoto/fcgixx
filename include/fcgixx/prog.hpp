#pragma once

#include "fastcgi.hpp"
#include <ioxx/dispatch.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>


namespace runpac { namespace fcgixx {


template<class IOCore, class RequestHandler> //, typename App>
class ConnectionHandler : public FCGIProtocolDriver::OutputCallback
{
public:
    typedef IOCore                        io_core;
    typedef typename io_core::socket      socket;
    typedef typename socket::address      address;
    typedef typename socket::event_set    event_set;
    typedef typename socket::native_t     native_socket_t;

    static void accept(io_core & io, socket & listen_socket, typename RequestHandler::app_type& the_app)
    {
        boost::shared_ptr<ConnectionHandler> p;
        {
            native_socket_t s;
            address addr;
            listen_socket.accept(s, addr);
            p.reset(new ConnectionHandler(io, s, the_app));
        }
        p->_sock.modify(boost::bind(&ConnectionHandler::run, p, _1), socket::readable);
        p->_sock.set_nonblocking();
        p->_sock.set_linger_timeout(0);
    }

private:
    socket                _sock;
    FCGIProtocolDriver    _fcgi_driver;
    bool                  _terminate;
    std::vector<char>     _write_buffer;

    ConnectionHandler(io_core & io, native_socket_t sock, typename RequestHandler::app_type& app)
        : _sock(io, sock), _fcgi_driver(*this), _terminate(false), app(app)
    {
    }

    void shutdown()
    {
        _sock.modify(typename io_core::dispatch::handler(), socket::no_events);
    }

    void run(event_set ev)
    {
        try
        {
            if (ev & socket::readable)
            {
                char tmp[1024*10];
                char * data_end( _sock.read(tmp, tmp + sizeof(tmp)) );
                if (!data_end) return shutdown();
                _fcgi_driver.process_input(tmp, data_end - tmp);
                FCGIRequest* req( _fcgi_driver.get_request() );
                if (req)
                {
                    _terminate = (req->keep_connection == false);
                    req->handler_cb = new RequestHandler(app);
                    req->handler_cb->operator()(req);
                }
            }
            if (ev & socket::writable)
            {
                if (!_write_buffer.empty())
                {
                    char const * p( _sock.write(&_write_buffer[0], &_write_buffer[_write_buffer.size()]) );
                    if (!p) throw ioxx::system_error(errno, "cannot write to socket");
                    _write_buffer.erase(_write_buffer.begin(), _write_buffer.begin() + (p - &_write_buffer[0]));
                }
                if (_write_buffer.empty())
                    _sock.request(socket::readable);
            }
            if (_terminate && !_fcgi_driver.have_active_requests() && _write_buffer.empty())
                shutdown();
        }
        catch(std::exception const & e)
        {
            std::cerr << "Caught exception in FCGIProtocolDriver: " << e.what() << std::endl
                      << "Terminating connection " << _sock << "." << std::endl;
            return shutdown();
        }
        catch(...)
        {
            std::cerr << "Caught unknown exception in FCGIProtocolDriver; terminating connection "
                      << _sock << "." << std::endl;
            return shutdown();
        }
    }

    virtual void operator() (void const * buf, size_t count)
    {
        bool const start_writer( _write_buffer.empty() );
        _write_buffer.insert(_write_buffer.end(), static_cast<char const *>(buf), static_cast<char const *>(buf) + count);
        if (start_writer)
            _sock.request(socket::readable | socket::writable);
    }

    //App& app;
    typename RequestHandler::app_type& app;
};

//
// FastCGI Handler
//
template<typename App>
class RequestHandler : public FCGIRequest::handler
{

public:
    typedef App app_type;

    RequestHandler(app_type& app) : app(app) { }

private:
    virtual void operator()(FCGIRequest* req)
    {
        if (!req->stdin_eof)
            return;

        if (req->role != FCGIRequest::RESPONDER)
        {
            req->write("We can't handle any role but RESPONDER.", 39, FCGIRequest::STDERR);
            req->end_request(1, FCGIRequest::UNKNOWN_ROLE);
            return;
        }

        if (req->aborted)
        {
            req->write("Aborted", 7, FCGIRequest::STDERR);
            req->end_request(1, FCGIRequest::UNKNOWN_ROLE);
            std::cout << "Request aborted aborted\n";
            return;
        }

        app.process(req);
    }

    app_type& app;
};


template<typename App>
struct prog
{

    int run()
    {

        typedef ioxx::dispatch<>                                      io_core;
        typedef io_core::socket                                       socket;
        typedef ConnectionHandler<io_core, RequestHandler<App>/*, App*/ > fcgi_handler;

        io_core io;

        App app;

        io_core::socket acceptor( io, STDIN_FILENO );
        acceptor.close_on_destruction(false);
        acceptor.modify(boost::bind(fcgi_handler::accept, boost::ref(io), boost::ref(acceptor), boost::ref(app)), socket::readable);

        for (;;)
        {
            io.run();
            io.wait(io.max_timeout());
        }
        return 0;
    }

};



} } // ns


