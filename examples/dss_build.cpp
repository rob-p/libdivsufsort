#include <iostream>
#include <string>
#include <cstdlib>
#include <limits>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>

#include "divsufsort.h"
#include "divsufsort64.h"
#include "CLI11.hpp"

#if HAVE_CONFIG_H
# include "config.h"
#endif

void read_input(const std::string& ip_path, std::string& text) {
    std::error_code ec;
    const auto file_size = std::filesystem::file_size(ip_path, ec);

    if(ec)
    {
        std::cerr << ip_path << " : " << ec.message() << "\n";
        std::exit(EXIT_FAILURE);
    }

    text.resize(file_size);
    std::ifstream input(ip_path);
    input.read(text.data(), file_size);
    input.close();
}


int main(int argc, char* argv[]) {
  CLI::App app{"Build the suffix array using the divsufsort algorithm"};
  std::string filename;
  std::string output;

  app.add_option("-i,--input", filename, "Input file on which to build SA")->required();
  app.add_option("-o,--output", output, "Ouput file where SA should be written");

  CLI11_PARSE(app, argc, argv);

  std::string text;
  read_input(filename, text);

  std::size_t n = text.size();
  if(n <= std::numeric_limits<int32_t>::max()) {
    std::cerr << "using 32-bit variant\n";
    std::vector<int32_t> sa(n, 0);
    int r = divsufsort(reinterpret_cast<const unsigned char*>(text.data()), 
                       reinterpret_cast<int32_t*>(sa.data()), (int32_t)n);
    if (r != 0) {
      std::cerr << "divsufsort failed with exit code : " << r << "\n";
      return r;
    } else {
      std::cerr << "divsufsort succeeded\n";
    }
    if (!output.empty()) {
      std::ofstream ofile(output, std::ios::binary);
      if (!ofile.is_open()) { 
        std::cerr << "couldn't open the output file " << output << "\n"; 
        return 1;
      }
      ofile.write(reinterpret_cast<char*>(&n), sizeof(n));
      ofile.write(reinterpret_cast<char*>(sa.data()), n * sizeof(int32_t));
      ofile.close();
    }
  } else {
    std::cerr << "using 64-bit variant\n";
    std::vector<int64_t> sa(n, 0);
    int r = divsufsort64(reinterpret_cast<const unsigned char*>(text.data()), 
                         reinterpret_cast<int64_t*>(sa.data()), (int64_t)n);
    if (r != 0) {
      std::cerr << "divsufsort failed with exit code : " << r << "\n";
      return r;
    } else {
      std::cerr << "divsufsort succeeded\n";
    }
    if (!output.empty()) {
      std::ofstream ofile(output, std::ios::binary);
      if (!ofile.is_open()) { 
        std::cerr << "couldn't open the output file " << output << "\n"; 
        return 1;
      }
      ofile.write(reinterpret_cast<char*>(&n), sizeof(n));
      ofile.write(reinterpret_cast<char*>(sa.data()), n * sizeof(int64_t));
      ofile.close();
    }
  }

  return 0;
}

