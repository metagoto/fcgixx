#pragma once


namespace runpac { namespace fcgixx { namespace request {


template<typename Adapter
        ,template<typename> class Handler
        >
struct basic_request : public Handler<Adapter>
{

    typedef Adapter adapter;
    typedef Handler<Adapter> handler;
    typedef typename adapter::raw_type raw_type;


    void process(raw_type raw_request)
    {
        adapter::process(raw_request);
        handler::process();
    }


};


} } } // ns
