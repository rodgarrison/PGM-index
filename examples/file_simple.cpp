#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <pgm/pgm_index.hpp>

void printit(__int128_t val, const char *valStr) {
  u_int64_t lo = (u_int64_t)(val & 0xFFFFFFFFFFFFFFFFUL);
  u_int64_t hi = (u_int64_t)((val>>64) & 0xFFFFFFFFFFFFFFFFUL); 
  printf("key: 0x%016llx %016llx '%s'\n", hi, lo, valStr);
}

void read_string_prefixes(const std::string &path, size_t prefix_length, std::vector<__int128_t> *data) {
  data->clear();

  std::string str;
  std::ifstream in(path.c_str());

  while (std::getline(in, str)) {
    if (str.size() > prefix_length)
      str.resize(prefix_length);

    __int128_t key = 0;
    char *keyPtr = (char*)&key + sizeof(__int128_t)-1-4;
    
    for (unsigned i=0; i<str.size(); ++i, --keyPtr) {
      *keyPtr = static_cast<char>(str[i]);
    }

    printit(key, str.c_str());

    data->push_back(key);
  }
}

int main() {
    std::vector<__int128_t> data;
    read_string_prefixes("../test.txt", sizeof(__int128_t), &data);
    std::sort(data.begin(), data.end());                                                                                
    pgm::PGMIndex<__int128_t, 8> index(data);                                                                               
    return 0;
}
