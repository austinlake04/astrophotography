#include <cstdio>
#include <string>
#include <vector>
#include <argparse/argparse.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>


int main(int argc, char* argv[]) {
  argparse::ArgumentParser program("Astrosight");
  program.add_argument("-o", "--output")
  .required()
  .help("Name of output file.");
  program.add_argument("--feature-detector")
  .help("Type of algorithimic detector for stellar features.")
  .choices("ORB", "SWIFT", "AKAZE")
  .default_value("ORB");
  program.add_argument("-v", "--verbose")
  .help("increase output verbosity")
  .default_value(false)
  .implicit_value(true);

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  const std::string output = program.get<std::string>("--output");
  const std::string feature_detector =
    program.get<std::string>("--feature-detector");
  const bool verbose = program.get<bool>("--verbose");

  std::cout << output << std::endl;
  std::cout << feature_detector << std::endl;
  std::cout << bool(verbose) << std::endl;
  printf("Hello world!");
}
