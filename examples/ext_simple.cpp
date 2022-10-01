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

const int k_SIZE = 10000;

int main() {
    std::vector<uint64_t> data(k_SIZE);                                                                                 
    std::generate(data.begin(), data.end(), std::rand);
    std::sort(data.begin(), data.end());

    pgm::PGMIndex<u_int64_t, 2> index(data);
  
    for(unsigned i=0; i<index.segments.size(); ++i) {
      printf("segment: %u, m: %16.15lf, b: %d, key: %llu\n", i, index.segments[i].slope, index.segments[i].intercept, index.segments[i].key);
    }
    printf("\n");

    for(unsigned i=0; i<index.levels_offsets.size(); ++i) {
      printf("level i: %u, index: %lu\n", i, index.levels_offsets[i]);
    }
    printf("\n");
    
    for (unsigned i=0; i<data.size(); ++i) {
      auto s = index.segment_for_key(data[i]);
      printf("segmentForData: key: %llu, m: %16.15lf, b: %d, key: %llu\n\n", data[i], s->slope, s->intercept, s->key);
    }
    printf("\n");

    for(unsigned i=0; i<data.size(); ++i) {
      printf("data: %u, value: %llu\n", i, data[i]);
    }

    return 0;
}
