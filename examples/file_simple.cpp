#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <pgm/pgm_index.hpp>

timespec endTime;                                                                                                       
timespec startTime;                                                                                                     
volatile unsigned errors(0);

void printit(__int128_t val, const char *valStr) {
  u_int64_t lo = (u_int64_t)(val & 0xFFFFFFFFFFFFFFFFUL);
  u_int64_t hi = (u_int64_t)((val>>64) & 0xFFFFFFFFFFFFFFFFUL); 
  printf("key: 0x%016lx %016lx '%s'\n", hi, lo, valStr);
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

    // printit(key, str.c_str());

    data->push_back(key);
  }
}

template<size_t N>
void benchmark(std::vector<__int128_t>& data, std::vector<__int128_t>& search) {
  // Construct the PGM-index
  timespec_get(&startTime, TIME_UTC);                                                                                 
  pgm::PGMIndex<__int128_t, N> index(data);
  timespec_get(&endTime, TIME_UTC);                                                                                   

  double elapsedNs = (double)endTime.tv_sec*1000000000.0+(double)endTime.tv_nsec -                                    
                     ((double)startTime.tv_sec*1000000000.0+(double)startTime.tv_nsec);                                 
  double nsPerOp = elapsedNs/(double)data.size();
  double opsPerSec = 1000000000.0 / nsPerOp;
  double ratio = (double)index.size_in_bytes()/((double)data.size()*sizeof(uint64_t));
  printf("make: elapsedNs: %10lf, ns/op: %lf, ops/sec: %lf, itemsInContainer: %lu, indexSizeBytes: %lu, indexRatio: %lf\n",
    elapsedNs, nsPerOp, opsPerSec, data.size(), index.size_in_bytes(), ratio);

  timespec_get(&startTime, TIME_UTC);                                                                                 
  for (unsigned i=0; i<search.size(); ++i) {                                                                            
    auto range = index.search(search[i]);
    // give compiler something to do with range so not optimized out
    errors += range.lo;
  }                                                                                                                   
  timespec_get(&endTime, TIME_UTC);                                                                                   
                                                                                                                        
  elapsedNs = (double)endTime.tv_sec*1000000000.0+(double)endTime.tv_nsec -                                           
              ((double)startTime.tv_sec*1000000000.0+(double)startTime.tv_nsec);
  nsPerOp = elapsedNs/(double)search.size();
  opsPerSec = 1000000000.0 / nsPerOp;
  printf("find: elapsedNs: %10lf, ns/op: %lf, ops/sec: %lf, searchKeys=%lu\n\n",
    elapsedNs, nsPerOp, opsPerSec, search.size());
}

int main(int argc, char **argv) {
  std::vector<__int128_t> data;
  std::vector<__int128_t> search;

  read_string_prefixes("../test.txt", sizeof(__int128_t), &data);
  read_string_prefixes("../search.txt", sizeof(__int128_t), &search);

  benchmark<1>(data, search);
  benchmark<2>(data, search);
  benchmark<4>(data, search);
  benchmark<8>(data, search);
  benchmark<16>(data, search);
  benchmark<32>(data, search);
  benchmark<64>(data, search);
  benchmark<128>(data, search);
  benchmark<256>(data, search);
  benchmark<512>(data, search);
  benchmark<1024>(data, search);
  benchmark<2048>(data, search);

  return 0;
}
