/*
 * This example shows how to index and query a vector of random integers with the PGM-index.
 * Compile with:
 *   g++ simple.cpp -std=c++17 -I../include -o simple
 * Run with:
 *   ./simple
 */

#include <vector>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include "pgm/pgm_index.hpp"
                                                                                                                        
timespec endTime;                                                                                                       
timespec startTime;                                                                                                     
volatile unsigned errors(0);

int k_SIZE=1000;                                                                                              

void usageAndExit() {
  printf("example_simple [-n <int>]\n");
  printf("\n");
  printf("          -n <int>  number of keys >= 1000. default %d\n", k_SIZE);
  printf("\n");
  exit(2);
}

void processCommandLine(int argc, char **argv) {
  int opt;

  const char *switches = "n:";

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

      default:
        usageAndExit();
        break;
    }
  }
}

template<size_t N>
void benchmark(std::vector<uint64_t>& data, std::vector<uint64_t>& search) {
  // Construct the PGM-index
  timespec_get(&startTime, TIME_UTC);                                                                                 
  pgm::PGMIndex<uint64_t, N> index(data);
  timespec_get(&endTime, TIME_UTC);                                                                                   
  index.print();

  double elapsedNs = (double)endTime.tv_sec*1000000000.0+(double)endTime.tv_nsec -                                    
                     ((double)startTime.tv_sec*1000000000.0+(double)startTime.tv_nsec);                                 
  double nsPerOp = elapsedNs/(double)k_SIZE;
  double opsPerSec = 1000000000.0 / nsPerOp;
  double ratio = (double)index.size_in_bytes()/((double)k_SIZE*sizeof(uint64_t));
  printf("make: elapsedNs: %10lf, ns/op: %lf, ops/sec: %lf, itemsInContainer: %d, indexSizeBytes: %lu, indexRatio: %lf\n",
    elapsedNs, nsPerOp, opsPerSec, k_SIZE, index.size_in_bytes(), ratio);

  timespec_get(&startTime, TIME_UTC);                                                                                 
  for (unsigned i=0; i<data.size(); ++i) {                                                                            
    printf("searching for data[%u]=%lu\n", i, data[i]); 
    auto range = index.search(search[i]);
    // give compiler something to do with range so not optimized out
    errors += range.lo;
  }                                                                                                                   
  timespec_get(&endTime, TIME_UTC);                                                                                   
                                                                                                                        
  elapsedNs = (double)endTime.tv_sec*1000000000.0+(double)endTime.tv_nsec -                                           
              ((double)startTime.tv_sec*1000000000.0+(double)startTime.tv_nsec);
  nsPerOp = elapsedNs/(double)k_SIZE;
  opsPerSec = 1000000000.0 / nsPerOp;
  printf("find: elapsedNs: %10lf, ns/op: %lf, ops/sec: %lf, searchKeys=%d\n\n",
    elapsedNs, nsPerOp, opsPerSec, k_SIZE);
}

int main(int argc, char **argv) {
    processCommandLine(argc, argv);

    // Generate some random data
    std::vector<uint64_t> data(k_SIZE);
    std::vector<uint64_t> search(k_SIZE);

    std::generate(data.begin(), data.end(), std::rand);
    std::generate(search.begin(), search.end(), std::rand);

    std::sort(data.begin(), data.end());
    for (unsigned i=0; i<data.size(); ++i) {
      printf("data[%u]=%lu\n", i, data[i]);
    }

/*
    benchmark<1>(data, search);
    benchmark<2>(data, search);
    benchmark<4>(data, search);
*/
    benchmark<8>(data, search);
/*
    benchmark<16>(data, search);
    benchmark<32>(data, search);
    benchmark<64>(data, search);
    benchmark<128>(data, search);
    benchmark<256>(data, search);
    benchmark<512>(data, search);
    benchmark<1024>(data, search);
    benchmark<2048>(data, search);
*/

    return 0;
}
