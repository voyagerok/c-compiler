#include <iostream>
#include <sstream>

#include "file.h"

int main(int argc, char **argv) {
  if (argc == 1 || argc > 2) {
    std::cerr << "usage: acc <filepath>" << std::endl;
    exit(EXIT_FAILURE);
  }

  file f(argv[1]);

  // fs::path filepath{argv[1]};
  // std::ifstream ifs(filepath);
  // if (!ifs.is_open()) {
  //   std::cerr << "could not open the file \"" << filepath
  //             << "\": " << strerror(errno) << std::endl;
  //   exit(EXIT_FAILURE);
  // }

  // std::stringstream buffer;
  // buffer << ifs.rdbuf();

  // std::cout << buffer.str() << std::endl;

  exit(EXIT_SUCCESS);
}
