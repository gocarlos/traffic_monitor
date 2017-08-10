// (c) 2017 Vigilatore

#ifndef INCLUDE_TRAFFIC_MONITOR_SERVER_H_
#define INCLUDE_TRAFFIC_MONITOR_SERVER_H_

#include <functional>
#include <thread>

#include <glog/logging.h>

#include "client_ws.hpp"
#include "server_ws.hpp"

#include "client_http.hpp"
#include "server_http.hpp"
#define BOOST_SPIRIT_THREADSAFE
#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#ifdef HAVE_OPENSSL
#include "crypto.hpp"
#endif

namespace traffic_monitor {

using namespace boost::property_tree;
typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
// typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;
typedef SimpleWeb::SocketServer<SimpleWeb::WS> WsServer;
// typedef SimpleWeb::SocketClient<SimpleWeb::WS> WsClient;
using namespace std;

class Server {
 public:
  WsServer *ws_server;
  HttpServer *http_server;

  std::thread http_server_thread;
  std::thread ws_server_thread;
  int RunServer();
  int Close();

  Server();
  virtual ~Server();
};

}  // namespace traffic_monitor

#endif  // INCLUDE_TRAFFIC_MONITOR_SERVER_H_
