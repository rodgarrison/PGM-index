#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <quadmath.h>
#include <sys/types.h>

#include <pgm/pgm_index.hpp>

timespec endTime;                                                                                                       
timespec startTime;                                                                                                     
bool debug = false;
std::string filename("in.txt");

void print_int128(__int128_t val) {
  char buffer[128];
  quadmath_snprintf(buffer, sizeof(buffer)-1, "%.0Qf", (__float128)val);
  printf("%s", buffer);
}

void usageAndExit() {
  printf("example_debug_file_simple [-f <filename> -v]\n");
  printf("\n");
  printf("    -f <file> containing 128-bit integer keys default %s\n", filename.c_str());
  printf("    -v        enable debug output default %d\n", debug);
  printf("\n");
  exit(2);
}

void parseCommandLine(int argc, char **argv) {
  int opt;

  const char *switches = "f:v";

  while ((opt = getopt(argc, argv, switches)) != -1) {
    switch (opt) {
      case 'f':
        {
          filename = optarg;
        }
        break;

      case 'v':
        {
          debug = true;
        }
        break;

      default:
        usageAndExit();
        break;
    }
  }
}

void readKeys(const std::string &path, std::vector<__int128_t> *data) {
  data->clear();

  std::string str;
  std::ifstream in(path.c_str());

  while (std::getline(in, str)) {
    __float128 keyFp = strtoflt128(str.c_str(), 0);
    __int128_t key = (__int128_t)(keyFp);
    data->push_back(key);
    
    if (debug) {
      printf("DEBUG: read key: ");
      print_int128(key);
      putchar('\n');
    }
  }
}

template<size_t N>
void benchmark(std::vector<__int128_t>& data) {
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

  for(unsigned i=0; i<index.segments.size(); ++i) {
    printf("segment: %u, m: %16.15lf, b: %d, key: ", i, index.segments[i].slope, index.segments[i].intercept);
    print_int128(index.segments[i].key);
    putchar('\n');
  }
  printf("\n");

  for(unsigned i=0; i<index.levels_offsets.size(); ++i) {
    printf("level i: %u, index: %lu\n", i, index.levels_offsets[i]);
  }
  printf("\n");
}

int main(int argc, char **argv) {
  parseCommandLine(argc, argv);

  std::vector<__int128_t> data;
  readKeys(filename,  &data);
  benchmark<16>(data);

  return 0;
}
