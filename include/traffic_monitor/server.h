// (c) 2017 Vigilatore

#ifndef INCLUDE_TRAFFIC_MONITOR_SERVER_H_
#define INCLUDE_TRAFFIC_MONITOR_SERVER_H_

#include <functional>
#include <thread>

#include <glog/logging.h>

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

using namespace std;

class Server {
 public:

  HttpServer *http_server;

  std::string web_root_path_;

  std::thread http_server_thread;

  int RunServer();
  int Close();

  Server();
  virtual ~Server();

 private:
  void RunHttpServer();
};

}  // namespace traffic_monitor

#endif  // INCLUDE_TRAFFIC_MONITOR_SERVER_H_
