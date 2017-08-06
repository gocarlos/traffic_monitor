// (c) 2017 Vigilatore

#include "traffic_monitor/tracker.h"
#include "argparse.hpp"

using namespace traffic_monitor;

int main(int argc, char *argv[]) {
  google::InitGoogleLogging(argv[0]);

  // TODO(gocarlos): finish this. 
  // make a new ArgumentParser
  ArgumentParser parser;

  // // add some arguments to search for
  // parser.addArgument("-withgui");
  // parser.addArgument("-b");
  // parser.addArgument("-c", "--cactus", 1);
  // parser.addArgument("-o", "--optional");
  // parser.addArgument("-r", "--required", 1, true);
  // parser.addArgument("--five", 5);
  // parser.addArgument("--atleast", '+');
  // parser.addArgument("--any", '*');
  // parser.addFinalArgument("output");
  //
  // // parse the command-line arguments - throws if invalid format
  // parser.parse(argc, argv);

  Tracker tracker;

  return tracker.run();
}
