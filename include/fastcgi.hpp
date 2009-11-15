/*
 * Copyright (c) 2001-2007 Peter Simons <simons@cryp.to>
 *
 * This software is provided 'as-is', without any express or
 * implied warranty. In no event will the authors be held liable
 * for any damages arising from the use of this software.
 *
 * Copying and distribution of this file, with or without
 * modification, are permitted in any medium without royalty
 * provided the copyright notice and this notice are preserved.
 */

#ifndef FASTCGI_HPP_INCLUDED
#define FASTCGI_HPP_INCLUDED

#include <map>
#include <queue>
#include <vector>
#include <string>
#include <stdexcept>
#include <stdint.h>

#include <boost/unordered_map.hpp>

// Forward declarations.

class FCGIProtocolDriver;
class FCGIRequest;

// All exceptions we throw derive from fcgi_error.

struct fcgi_error : public std::runtime_error
{
  fcgi_error(std::string const & w) : runtime_error(w) { }
};

struct unsupported_fcgi_version : public fcgi_error
{
  unsupported_fcgi_version(std::string const & w) : fcgi_error(w) { }
};

struct duplicate_begin_request : public fcgi_error
{
  duplicate_begin_request(std::string const & w) : fcgi_error(w) { }
};

struct unknown_fcgi_request : public fcgi_error
{
  unknown_fcgi_request(std::string const & w) : fcgi_error(w) { }
};

struct fcgi_io_callback_error : public fcgi_error
{
  fcgi_io_callback_error(std::string const & w) : fcgi_error(w) { }
};

// The class representing a request.

class FCGIRequest
{
public:
  enum role_t
    { RESPONDER  = 1
    , AUTHORIZER = 2
    , FILTER     = 3
    };

  uint16_t const id;
  role_t const role;
  bool const keep_connection;
  bool aborted;

  //std::map<std::string,std::string> params;
  boost::unordered_map<std::string,std::string> params;
  std::string stdin_stream, data_stream;
  bool stdin_eof, data_eof;

  FCGIRequest(FCGIProtocolDriver& driver_, uint16_t id_, role_t role_, bool kc);
  ~FCGIRequest();

  enum ostream_type_t
    { STDOUT
    , STDERR
    };

  void write(std::string const & buf, ostream_type_t stream = STDOUT);
  void write(char const * buf, size_t count, ostream_type_t stream = STDOUT);

  enum protocol_status_t
    { REQUEST_COMPLETE = 0
    , CANT_MPX_CONN    = 1
    , OVERLOADED       = 2
    , UNKNOWN_ROLE     = 3
    };
  void end_request(uint32_t appStatus, protocol_status_t protStatus);

  struct handler
  {
    virtual ~handler() { }
    virtual void operator()(FCGIRequest*) = 0;
  };
  handler* handler_cb;

private:
  FCGIProtocolDriver& driver;
};

//
// The FCGI protocol driver class.
//

class FCGIProtocolDriver
{
public:
  struct OutputCallback
  {
    virtual ~OutputCallback() = 0;
    virtual void operator() (void const *, size_t) = 0;
  };

  FCGIProtocolDriver(OutputCallback& cb);
  ~FCGIProtocolDriver();

  void process_input(void const * buf, size_t count);
  FCGIRequest* get_request();
  bool have_active_requests();

private:                        // don't copy me
  FCGIProtocolDriver(FCGIProtocolDriver const &);
  FCGIProtocolDriver& operator= (FCGIProtocolDriver const &);

protected:
  friend class FCGIRequest;
  void terminate_request(uint16_t id);
  OutputCallback& output_cb;

private:
  void process_begin_request(uint16_t id, uint8_t const * buf, uint16_t len);
  void process_abort_request(uint16_t id, uint8_t const * buf, uint16_t len);
  void process_params(uint16_t id, uint8_t const * buf, uint16_t len);
  void process_stdin(uint16_t id, uint8_t const * buf, uint16_t len);
  void process_unknown(uint8_t type);

  typedef std::map<uint16_t,FCGIRequest*> reqmap_t;
  reqmap_t reqmap;
  std::queue<uint16_t> new_request_queue;

  std::vector<uint8_t> InputBuffer;
};

#endif // FASTCGI_HPP_INCLUDED
