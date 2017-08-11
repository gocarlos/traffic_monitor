// (c) 2017 Vigilatore

#include <traffic_monitor/server.h>

namespace traffic_monitor {

Server::Server() {
  // TODO Auto-generated constructor stub

  ws_server = new WsServer;
  http_server = new HttpServer;
  ws_server->config.port = Settings::ws_server_port_;
  http_server->config.port = Settings::http_server_port_;

  web_root_path_= prefix_to_webserver;

}

int Server::Close() {
  LOG(INFO) << "Going to closing the server.";

  ws_server->stop();
  http_server->stop();

  http_server_thread.join();
  ws_server_thread.join();

  delete ws_server, http_server;

  LOG(INFO) << "Closing the server.";
  return 0;
}

Server::~Server() {
  ws_server = nullptr;
  http_server = nullptr;

  LOG(INFO) << "Destructing the server.";
}

void Server::PrintHelp() {
  // clang-format off
  std::cout << "The activity on the street is streamed to the page: http://localhost:8080." << std::endl;
  std::cout << "You can view statistics and adjust some settings. "<< std::endl;
  // clang-format on
}

void Server::SendMessage() {
  LOG(INFO) << "Connection size:" << ws_server->get_connections().size();
  LOG(INFO) << "Connection size:" << asdfasf;
}

void Server::RunHttpServer() {
  http_server->default_resource["GET"] = [](
      std::shared_ptr<HttpServer::Response> response,
      shared_ptr<HttpServer::Request> request) {
    try {
      auto web_root_path = boost::filesystem::canonical(web_root_path_);
      auto path = boost::filesystem::canonical(web_root_path / request->path);
      // Check if path is within web_root_path
      if (distance(web_root_path.begin(), web_root_path.end()) >
              distance(path.begin(), path.end()) ||
          (not equal(web_root_path.begin(), web_root_path.end(),
                     path.begin()))) {
        throw invalid_argument("path must be within root path");
      }
      if (boost::filesystem::is_directory(path)) {
        path /= "index.html";
      }

      SimpleWeb::CaseInsensitiveMultimap header;

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
                      if (not ec)
                        read_and_send(response, ifs);
                      else
                        std::cerr << "Connection interrupted" << endl;
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

  http_server->on_error = [](shared_ptr<HttpServer::Request> /*request*/,
                             const SimpleWeb::error_code & /*ec*/) {
    LOG(ERROR) << "Errors occurred with the http server...";
    // Handle errors here
  };

  http_server_thread = std::thread([&]() {
    // Start server
    LOG(INFO) << "Webserver started";
    http_server->start();
  });
}

void Server::RunWsServer() {
  auto &echo = ws_server->endpoint["^/echo/?$"];

  echo.on_message = [&](std::shared_ptr<WsServer::Connection> connection,
                        std::shared_ptr<WsServer::Message> message) {

    std::cout << "12341234123412341234: " << echo.get_connections().size()
              << std::endl;

    std::string car_left = std::to_string(Statistics::car_count_left_);
    std::string car_right = std::to_string(Statistics::car_count_right_);

    std::string message_str =
        "Since the beginning of the measurements traffic monitor saw " +
        car_left + " cars passing to the left and " + car_right +
        " passing to the right. ";

    LOG(INFO) << "Server: Message received: \"" << message_str << "\" from"
              << connection;

    LOG(INFO) << "Server: Sending message \"" << message_str << "\" to "
              << connection;

    auto send_stream = make_shared<WsServer::SendStream>();
    *send_stream << message_str;
    // connection->send is an asynchronous function
    connection->send(send_stream, [](const SimpleWeb::error_code &ec) {
      if (ec) {
        std::cout << "Server: Error sending message. " <<
            // See
            //              http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html,
            // Error Codes for error code meanings
            "Error: " << ec << ", error message: " << ec.message() << endl;
      }
    });

    // Alternatively, using a convenience function:
    // connection->send(message_str, [](const SimpleWeb::error_code & /*ec*/) {
    // /*handle error*/ });
  };

  echo.on_open = [&](shared_ptr<WsServer::Connection> connection) {
    std::cout << "Server: Opened connection " << connection << endl;

  };

  // See RFC 6455 7.4.1. for status codes
  echo.on_close = [](shared_ptr<WsServer::Connection> connection, int status,
                     const string & /*reason*/) {
    std::cout << "Server: Closed connection " << connection.get()
              << " with status code " << status << std::endl;
  };

  // See
  // http:// www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html,
  // Error Codes for error code meanings
  echo.on_error = [](shared_ptr<WsServer::Connection> connection,
                     const SimpleWeb::error_code &ec) {
    cout << "Server: Error in connection " << connection.get() << ". "
         << "Error: " << ec << ", error message: " << ec.message() << endl;
  };

  ws_server_thread = std::thread([&]() {
    // Start WS-server
    LOG(INFO) << "Websockets server started";
    ws_server->start();
    ws_server_started = true;
  });
}

int Server::RunServer() {
  RunHttpServer();
  RunWsServer();

  // Wait for server to start so that the client can connect
  this_thread::sleep_for(chrono::seconds(1));

  LOG(INFO) << "Server ready";
  return 0;
}

} /* namespace traffic_monitor */
