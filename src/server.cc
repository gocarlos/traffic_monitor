// (c) 2017 Vigilatore

#include <traffic_monitor/server.h>

namespace traffic_monitor {

Server::Server() {

  http_server = new HttpServer;
  http_server->config.port = Settings::http_server_port_;

  web_root_path_= prefix_to_webserver;

}

int Server::Close() {
  LOG(INFO) << "Going to closing the server.";

  // ws_server->stop();
  http_server->stop();

  http_server_thread.join();
  // ws_server_thread.join();

  delete http_server;

  LOG(INFO) << "Closing the server.";
  return 0;
}

Server::~Server() {
  // ws_server = nullptr;
  http_server = nullptr;

  LOG(INFO) << "Destructing the server.";
}

void Server::RunHttpServer() {
  http_server->default_resource["GET"] = [&](
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
    // clang-format off
    LOG(INFO) << "The activity on the street is streamed to the page: http://localhost:8080." << std::endl;
    LOG(INFO) << "You can view statistics and adjust some settings. "<< std::endl;
    // clang-format on
    http_server->start();
  });
}

int Server::RunServer() {
  RunHttpServer();

  // Wait for server to start so that the client can connect
  this_thread::sleep_for(chrono::seconds(1));

  LOG(INFO) << "Server ready";
  return 0;
}

} // namespace traffic_monitor
