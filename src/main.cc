// (c) 2017 Vigilatore

#include <algorithm>
#include <fstream>
#include <vector>

#include "cxxopts.hpp"

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
#include "traffic_monitor/tracker.h"

using namespace traffic_monitor;
using namespace std;

using namespace boost::property_tree;
typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;
typedef SimpleWeb::SocketServer<SimpleWeb::WS> WsServer;
typedef SimpleWeb::SocketClient<SimpleWeb::WS> WsClient;

int RunServer() {
  //  LOG(INFO) << "Starting the Websocket Server.";
  WsServer ws_server;
  ws_server.config.port = 8090;
  auto &echo = ws_server.endpoint["^/echo/?$"];

  echo.on_message = [](shared_ptr<WsServer::Connection> connection,
                       shared_ptr<WsServer::Message> message) {
    auto message_str = message->string();

    cout << "Server: Message received: \"" << message_str << "\" from "
         << connection.get() << endl;

    cout << "Server: Sending message \"" << message_str << "\" to "
         << connection.get() << endl;

    auto send_stream = make_shared<WsServer::SendStream>();
    *send_stream << message_str;
    // connection->send is an asynchronous function
    connection->send(send_stream, [](const SimpleWeb::error_code &ec) {
      if (ec) {
        cout << "Server: Error sending message. " <<
            // See
            // http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html,
            // Error Codes for error code meanings
            "Error: " << ec << ", error message: " << ec.message() << endl;
      }
    });

    // Alternatively, using a convenience function:
    // connection->send(message_str, [](const SimpleWeb::error_code & /*ec*/) {
    // /*handle error*/ });
  };

  echo.on_open = [](shared_ptr<WsServer::Connection> connection) {
    cout << "Server: Opened connection " << connection.get() << endl;
  };

  // See RFC 6455 7.4.1. for status codes
  echo.on_close = [](shared_ptr<WsServer::Connection> connection, int status,
                     const string & /*reason*/) {
    cout << "Server: Closed connection " << connection.get()
         << " with status code " << status << endl;
  };

  // See
  // http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html,
  // Error Codes for error code meanings
  echo.on_error = [](shared_ptr<WsServer::Connection> connection,
                     const SimpleWeb::error_code &ec) {
    cout << "Server: Error in connection " << connection.get() << ". "
         << "Error: " << ec << ", error message: " << ec.message() << endl;
  };

  //  LOG(INFO) << "Starting the HTTP Server.";
  HttpServer http_server;
  http_server.config.port = 8080;

  http_server.default_resource["GET"] = [](
      std::shared_ptr<HttpServer::Response> response,
      shared_ptr<HttpServer::Request> request) {
    try {
      auto web_root_path = boost::filesystem::canonical("webserver");
      auto path = boost::filesystem::canonical(web_root_path / request->path);
      // Check if path is within web_root_path
      if (distance(web_root_path.begin(), web_root_path.end()) >
              distance(path.begin(), path.end()) ||
          !equal(web_root_path.begin(), web_root_path.end(), path.begin()))
        throw invalid_argument("path must be within root path");
      if (boost::filesystem::is_directory(path)) path /= "index.html";

      SimpleWeb::CaseInsensitiveMultimap header;

//    Uncomment the following line to enable Cache-Control
//    header.emplace("Cache-Control", "max-age=86400");

#ifdef HAVE_OPENSSL
//    Uncomment the following lines to enable ETag
//    {
//      ifstream ifs(path.string(), ifstream::in | ios::binary);
//      if(ifs) {
//        auto hash =
//        SimpleWeb::Crypto::to_hex_string(SimpleWeb::Crypto::md5(ifs));
//        header.emplace("ETag", "\"" + hash + "\"");
//        auto it = request->header.find("If-None-Match");
//        if(it != request->header.end()) {
//          if(!it->second.empty() && it->second.compare(1, hash.size(), hash)
//          == 0) {
//            response->write(SimpleWeb::StatusCode::redirection_not_modified,
//            header);
//            return;
//          }
//        }
//      }
//      else
//        throw invalid_argument("could not read file");
//    }
#endif

      auto ifs = make_shared<ifstream>();
      ifs->open(path.string(), ifstream::in | ios::binary | ios::ate);

      if (*ifs) {
        auto length = ifs->tellg();
        ifs->seekg(0, ios::beg);

        header.emplace("Content-Length", to_string(length));
        response->write(header);

        // Trick to define a recursive function within this scope (for example
        // purposes)
        class FileServer {
         public:
          static void read_and_send(
              const shared_ptr<HttpServer::Response> &response,
              const shared_ptr<ifstream> &ifs) {
            // Read and send 128 KB at a time
            static vector<char> buffer(
                131072);  // Safe when server is running on one thread
            streamsize read_length;
            if ((read_length = ifs->read(&buffer[0], buffer.size()).gcount()) >
                0) {
              response->write(&buffer[0], read_length);
              if (read_length == static_cast<streamsize>(buffer.size())) {
                response->send(
                    [response, ifs](const SimpleWeb::error_code &ec) {
                      if (!ec)
                        read_and_send(response, ifs);
                      else
                        cerr << "Connection interrupted" << endl;
                    });
              }
            }
          }
        };
        FileServer::read_and_send(response, ifs);
      } else
        throw invalid_argument("could not read file");
    } catch (const exception &e) {
      response->write(SimpleWeb::StatusCode::client_error_bad_request,
                      "Could not open path " + request->path + ": " + e.what());
    }
  };

  http_server.on_error = [](shared_ptr<HttpServer::Request> /*request*/,
                            const SimpleWeb::error_code & /*ec*/) {
    // Handle errors here
  };

  thread http_server_thread([&http_server]() {
    // Start server
    LOG(INFO) << "Webserver started";
    http_server.start();
  });

  thread ws_server_thread([&ws_server]() {
    // Start WS-server
    LOG(INFO) << "Websocketsserver started";
    ws_server.start();
  });
  // Wait for server to start so that the client can connect
  this_thread::sleep_for(chrono::seconds(1));

  http_server_thread.join();
  ws_server_thread.join();
  return 0;
}

int main(int argc, char *argv[]) {
  //  google::InitGoogleLogging(argv[0]); does nothing, as we use miniglog

  cxxopts::Options options("Traffic Monitor", "todo: longer app description");

  // clang-format off
  options.add_options()
		  ("d,debug", "Enable debugging")
		  ("g,withgui", "Enable gui")
		  ("l,logfile", "Logging file name", cxxopts::value<std::string>()
		  );
  // clang-format on

  options.parse(argc, argv);
  Settings::with_gui_ = options["withgui"].as<bool>();
  Settings::debug_mode_ = options["debug"].as<bool>();
  std::string log_file = options["logfile"].as<std::string>();

  if (log_file.size() > 0) {
    Settings::path_to_log_file_ = log_file;
  }
  LOG_IF(INFO, not Settings::with_gui_)
      << "Starting traffic monitor without GUI";
  LOG_IF(INFO, Settings::debug_mode_)
      << "Starting traffic monitor in debug mode";
  LOG(INFO) << "Path to logging file is: " << Settings::path_to_log_file_;

  //  Tracker tracker;

  //  tracker.input_ = Tracker::camera;
  //  tracker.camera_number_ = 0;
  //  tracker.run();
  RunServer();

  return 0;
}
