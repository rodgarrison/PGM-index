#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>

#include <pgm/pgm_index.hpp>
#include <pgm/pgm_index_dynamic.hpp>

timespec endTime;                                                                                                       
timespec startTime;                                                                                                     
volatile unsigned errors(0);

u_int32_t p_SIZE = 12;
u_int32_t k_SIZE = 1000000;

void printit(__int128_t val, const char *valStr) {
  u_int64_t lo = (u_int64_t)(val & 0xFFFFFFFFFFFFFFFFUL);
  u_int64_t hi = (u_int64_t)((val>>64) & 0xFFFFFFFFFFFFFFFFUL); 
  printf("key: 0x%016lx %016lx '%s'\n", hi, lo, valStr);
}

void usageAndExit() {
  printf("example_file_simple [-n <int> -p <int>]\n");
  printf("\n");
  printf("    -n <int>  number of keys >= 1000. default %u\n", k_SIZE);
  printf("    -p <int>  maximum size 's' of key in bytes 1<=s<= 16. default %u\n", p_SIZE);
  printf("\n");
  exit(2);
}

void parseCommandLine(int argc, char **argv) {
  int opt;

  const char *switches = "n:p:";

  while ((opt = getopt(argc, argv, switches)) != -1) {
    switch (opt) {
      case 'n':
        {
          k_SIZE = atoi(optarg);
          if (k_SIZE<1000) {
            usageAndExit();
          }
        }
        break;

      case 'p':
        {
          p_SIZE = atoi(optarg);
          if (p_SIZE<1||p_SIZE>16) {
            usageAndExit();
          }
        }
        break;

      default:
        usageAndExit();
        break;
    }
  }
}

void readKeyPairs(const std::string &path, std::vector<std::pair<__int128_t, u_int32_t>> *data) {
  data->clear();
  data->resize(k_SIZE);

  std::string str;
  std::ifstream in(path.c_str());

  u_int32_t line = 0;

  while (std::getline(in, str)) {
    if (str.size() > p_SIZE) {
      str.resize(p_SIZE);
    }

    __int128_t key = 0;
    char *keyPtr = (char*)&key + sizeof(__int128_t)-1-4;
    
    for (unsigned i=0; i<str.size(); ++i, --keyPtr) {
      *keyPtr = static_cast<char>(str[i]);
    }

    // printit(key, str.c_str());

    data->operator[](line).first = key;
    data->operator[](line).second = line;

    if (++line==k_SIZE) {
      break;
    }
  }
}

void readKeys(const std::string &path, std::vector<__int128_t> *data) {
  data->clear();
  data->resize(k_SIZE);

  std::string str;
  std::ifstream in(path.c_str());

  u_int32_t line = 0;

  while (std::getline(in, str)) {
    if (str.size() > p_SIZE) {
      str.resize(p_SIZE);
    }

    __int128_t key = 0;
    char *keyPtr = (char*)&key + sizeof(__int128_t)-1-4;
    
    for (unsigned i=0; i<str.size(); ++i, --keyPtr) {
      *keyPtr = static_cast<char>(str[i]);
    }

    // printit(key, str.c_str());

    data->operator[](line) = key;

    if (++line==k_SIZE) {
      break;
    }
  }
}

template<size_t N>
void dynamicBenchmark(std::vector<std::pair<__int128_t, u_int32_t>>& data, std::vector<__int128_t>& search) {
  printf("Dynamic PGM Index==%lu\n", N);

  // Construct the PGM-index
  timespec_get(&startTime, TIME_UTC);                                                                                 
  pgm::DynamicPGMIndex<__int128_t, u_int32_t, pgm::PGMIndex<__int128_t, N>> index(data.begin(), data.end());
  timespec_get(&endTime, TIME_UTC);                                                                                   

  double elapsedNs = (double)endTime.tv_sec*1000000000.0+(double)endTime.tv_nsec -                                    
                     ((double)startTime.tv_sec*1000000000.0+(double)startTime.tv_nsec);                                 
  double nsPerOp = elapsedNs/(double)data.size();
  double opsPerSec = 1000000000.0 / nsPerOp;
  double ratio = (double)index.size_in_bytes()/((double)data.size()*sizeof(__int128_t));
  printf("make: elapsedNs: %10lf, ns/op: %lf, ops/sec: %lf, itemsInContainer: %lu, indexSizeBytes: %lu, indexRatio: %lf\n",
    elapsedNs, nsPerOp, opsPerSec, data.size(), index.size_in_bytes(), ratio);

  errors = 0;
  timespec_get(&startTime, TIME_UTC);                                                                                 
  for (unsigned i=0; i<search.size(); ++i) {                                                                            
    if (index.find(search[i])==index.end()) {
      ++errors;
    }
  }                                                                                                                   
  timespec_get(&endTime, TIME_UTC);                                                                                   
                                                                                                                        
  elapsedNs = (double)endTime.tv_sec*1000000000.0+(double)endTime.tv_nsec -                                           
              ((double)startTime.tv_sec*1000000000.0+(double)startTime.tv_nsec);
  nsPerOp = elapsedNs/(double)search.size();
  opsPerSec = 1000000000.0 / nsPerOp;
  printf("find: elapsedNs: %10lf, ns/op: %lf, ops/sec: %lf, searchKeys=%lu\n\n",
    elapsedNs, nsPerOp, opsPerSec, search.size());

  if (errors) {
    printf("ERROR: %u keys not found\n", errors);
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
  double ratio = (double)index.size_in_bytes()/((double)data.size()*sizeof(__int128_t));
  printf("make: elapsedNs: %10lf, ns/op: %lf, ops/sec: %lf, itemsInContainer: %lu, indexSizeBytes: %lu, indexRatio: %lf\n",
    elapsedNs, nsPerOp, opsPerSec, data.size(), index.size_in_bytes(), ratio);

  errors = 0;
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
  parseCommandLine(argc, argv);

  {
    std::vector<__int128_t> data;
    std::vector<__int128_t> search;

    readKeys("../test.txt", &data);
    readKeys("../search.txt", &search);

    benchmark<2>(data, search);
    benchmark<4>(data, search);
    benchmark<8>(data, search);
    benchmark<16>(data, search);
    benchmark<32>(data, search);
    benchmark<64>(data, search);
  }

  {
    std::vector<std::pair<__int128_t, u_int32_t>> data;
    std::vector<__int128_t> search;

    readKeyPairs("../test.txt", &data);
    readKeys("../search.txt", &search);

    dynamicBenchmark<2>(data, search);
    dynamicBenchmark<4>(data, search);
    dynamicBenchmark<8>(data, search);
    dynamicBenchmark<16>(data, search);
    dynamicBenchmark<32>(data, search);
    dynamicBenchmark<64>(data, search);
  }

  return 0;
}
