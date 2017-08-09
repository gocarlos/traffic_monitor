// (c) 2017 Vigilatore

#include <fstream>
#include <vector>
#include <algorithm>

#include "cxxopts.hpp"

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

int RunServer() {

  HttpServer server;
  server.config.port = 8080;

  server.default_resource["GET"] = [](
      std::shared_ptr<HttpServer::Response> response,
      shared_ptr<HttpServer::Request> request) {
    try {
      auto web_root_path = boost::filesystem::canonical("web");
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

  server.on_error = [](shared_ptr<HttpServer::Request> /*request*/,
                       const SimpleWeb::error_code & /*ec*/) {
    // Handle errors here
  };

  thread server_thread([&server]() {
    // Start server
    server.start();
  });

  // Wait for server to start so that the client can connect
  this_thread::sleep_for(chrono::seconds(1));

  server_thread.join();

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

  Tracker tracker;

  tracker.input_ = Tracker::camera;
  tracker.camera_number_ = 0;
  RunServer();
  return tracker.run();
}
