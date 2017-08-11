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

#include "traffic_monitor/settings.h"
#include "traffic_monitor/paths.h"
#include "traffic_monitor/statistics.h"

namespace traffic_monitor {

using namespace boost::property_tree;
typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
// typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;
typedef SimpleWeb::SocketServer<SimpleWeb::WS> WsServer;
// typedef SimpleWeb::SocketClient<SimpleWeb::WS> WsClient;
using namespace std;

class Server {
 public:
  int asdfasf;
  WsServer *ws_server;
  HttpServer *http_server;
  
  std::string web_root_path_;

  std::thread http_server_thread;
  std::thread ws_server_thread;
  bool ws_server_started=false;

  int RunServer();
  int Close();
  void PrintHelp();
  void SendMessage();

  Server();
  virtual ~Server();

 private:
  void RunHttpServer();
  void RunWsServer();
};

}  // namespace traffic_monitor

#endif  // INCLUDE_TRAFFIC_MONITOR_SERVER_H_
