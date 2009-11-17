#pragma once

#include <fastcgi.hpp>
#include <boost/noncopyable.hpp>


namespace runpac { namespace fcgixx { namespace request { namespace adapter {


struct fcgi : boost::noncopyable
{

    typedef FCGIRequest* raw_type;

    typedef std::string stream_type;

    typedef boost::unordered_map<std::string, std::string> params_type;


    fcgi()
        : raw(0)
    {
    }


    void process(raw_type raw_request)
    {
        raw = raw_request;
    }


    params_type& params()
    {
        // assert stuff!
        return raw->params;
    }


    stream_type& stdin()
    {
        return raw->stdin_stream;
    }


    void write(const stream_type& buf)
    {
        raw->write(buf);
    }


    void write(char const* buf, size_t count)
    {
        raw->write(buf, count);
    }


    void end(unsigned int app_status = 0
            ,FCGIRequest::protocol_status_t proto_status = FCGIRequest::REQUEST_COMPLETE)
    {
        raw->end_request(app_status, proto_status);
    }


private:
    raw_type raw;

};


} } } } // ns
