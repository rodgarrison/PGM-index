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
#include "pgm/pgm_index.hpp"

const int k_SIZE=10000000;                                                                                              
                                                                                                                        
timespec endTime;                                                                                                       
timespec startTime;                                                                                                     
volatile unsigned errors(0);
const int epsilon = 128; // space-time trade-off parameter

int main() {
    // Generate some random data
    std::vector<uint64_t> data(k_SIZE);
    std::vector<uint64_t> search(k_SIZE);

    std::generate(data.begin(), data.end(), std::rand);
    std::generate(search.begin(), search.end(), std::rand);

    std::sort(data.begin(), data.end());

    // Construct the PGM-index
    timespec_get(&startTime, TIME_UTC);                                                                                 
    pgm::PGMIndex<uint64_t, epsilon> index(data);
    timespec_get(&endTime, TIME_UTC);                                                                                   

    double elapsedNs = (double)endTime.tv_sec*1000000000.0+(double)endTime.tv_nsec -                                    
                     ((double)startTime.tv_sec*1000000000.0+(double)startTime.tv_nsec);                                 
    printf("make: elapsedNs: %10lf, ns/op: %-7.3lf, itemsInContainer: %d\n", elapsedNs, elapsedNs/(double)k_SIZE, k_SIZE);

    timespec_get(&startTime, TIME_UTC);                                                                                 
    for (unsigned i=0; i<search.size(); ++i) {                                                                            
      auto range = index.search(search[i]);
      // give compiler something to do with range so not optimized out
      errors += range.lo;
    }                                                                                                                   
    timespec_get(&endTime, TIME_UTC);                                                                                   
                                                                                                                        
    elapsedNs = (double)endTime.tv_sec*1000000000.0+(double)endTime.tv_nsec -                                           
                ((double)startTime.tv_sec*1000000000.0+(double)startTime.tv_nsec);
    printf("find: elapsedNs: %10lf, ns/op: %-7.3lf, searchKeys=%d, indexBytes: %lu\n",
      elapsedNs, elapsedNs/(double)k_SIZE, k_SIZE, index.size_in_bytes());

    return 0;
}
