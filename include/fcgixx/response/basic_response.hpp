#pragma once


namespace runpac { namespace fcgixx { namespace response {


template<typename Handler>
struct basic_response : public Handler
{

    typedef Handler handler;

};


} } } // ns
