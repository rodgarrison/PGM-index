/*
 * This example shows how to use pgm::DynamicPGMIndex, a std::map-like container supporting inserts and deletes.
 * Compile with:
 *   g++ updates.cpp -std=c++17 -I../include -o updates
 * Run with:
 *   ./updates
 */

#include <vector>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include "pgm/pgm_index_dynamic.hpp"

const int k_SIZE=10000000;

timespec endTime;
timespec startTime;
volatile unsigned errors(0);

int main() {
    // Generate some random key-value pairs to bulk-load the Dynamic PGM-index
    std::vector<std::pair<uint64_t, uint64_t>> data(k_SIZE);
    std::vector<std::pair<uint64_t, uint64_t>> newdata(k_SIZE);
    std::vector<uint64_t> search(k_SIZE);

    std::generate(data.begin(), data.end(), [] { return std::make_pair(std::rand(), std::rand()); });
    std::generate(newdata.begin(), newdata.end(), [] { return std::make_pair(std::rand(), std::rand()); });
    std::generate(search.begin(), search.end(), std::rand);

    std::sort(data.begin(), data.end());

    // Construct and bulk-load the Dynamic PGM-index
    timespec_get(&startTime, TIME_UTC);
    pgm::DynamicPGMIndex<uint64_t, uint64_t> dynamic_pgm(data.begin(), data.end());
    timespec_get(&endTime, TIME_UTC);

    double elapsedNs = (double)endTime.tv_sec*1000000000.0+(double)endTime.tv_nsec -
                     ((double)startTime.tv_sec*1000000000.0+(double)startTime.tv_nsec);
    printf("make: elapsedNs: %10lf, ns/op: %-7.3lf, containerSizeBytes: %lu, indexBytes: %lu, itemsInContainer: %lu\n",
      elapsedNs, elapsedNs/(double)k_SIZE, dynamic_pgm.size_in_bytes(), dynamic_pgm.index_size_in_bytes(), dynamic_pgm.size());


    // Find data
    timespec_get(&startTime, TIME_UTC);
    for (unsigned i=0; i<search.size(); ++i) {
      if (dynamic_pgm.find(search[i])==dynamic_pgm.end()) {
        ++errors;
      }
    }
    timespec_get(&endTime, TIME_UTC);

    elapsedNs = (double)endTime.tv_sec*1000000000.0+(double)endTime.tv_nsec -
                ((double)startTime.tv_sec*1000000000.0+(double)startTime.tv_nsec);
    printf("find: elapsedNs: %10lf, ns/op: %-7.3lf, searchKeys=%d, containerSizeBytes: %lu, indexBytes: %lu, itemsInContainer: %lu\n", 
      elapsedNs, elapsedNs/(double)k_SIZE, k_SIZE, dynamic_pgm.size_in_bytes(), dynamic_pgm.index_size_in_bytes(), dynamic_pgm.size());


    // Insert more data
    timespec_get(&startTime, TIME_UTC);
    for (unsigned i=0; i<newdata.size(); ++i) {
      dynamic_pgm.insert_or_assign(newdata[i].first, newdata[i].second);
    }
    timespec_get(&endTime, TIME_UTC);

    elapsedNs = (double)endTime.tv_sec*1000000000.0+(double)endTime.tv_nsec -
                ((double)startTime.tv_sec*1000000000.0+(double)startTime.tv_nsec);
    printf("add : elapsedNs: %10lf, ns/op: %-7.3lf, containerSizeBytes: %lu, indexBytes: %lu, itemsInContainer: %lu\n",
      elapsedNs, elapsedNs/(double)k_SIZE, dynamic_pgm.size_in_bytes(), dynamic_pgm.index_size_in_bytes(), dynamic_pgm.size());


    // Find data
    errors = 0;
    timespec_get(&startTime, TIME_UTC);
    for (unsigned i=0; i<search.size(); ++i) {
      if (dynamic_pgm.find(search[i])==dynamic_pgm.end()) {
        ++errors;
      }
    }
    timespec_get(&endTime, TIME_UTC);

    elapsedNs = (double)endTime.tv_sec*1000000000.0+(double)endTime.tv_nsec -
                ((double)startTime.tv_sec*1000000000.0+(double)startTime.tv_nsec);
    printf("find: elapsedNs: %10lf, ns/op: %-7.3lf, searchKeys=%d, containerSizeBytes: %lu, indexBytes: %lu, itemsInContainer: %lu\n", 
      elapsedNs, elapsedNs/(double)k_SIZE, k_SIZE, dynamic_pgm.size_in_bytes(), dynamic_pgm.index_size_in_bytes(), dynamic_pgm.size());


    return 0;
}
