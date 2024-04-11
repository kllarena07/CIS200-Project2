#include <fstream>
#include <iostream>
using std::cout;
using std::fstream;
using std::ios;

struct Job {
  char job_type;
  unsigned int arrival_time;
  unsigned int processing_time;
  unsigned int idle_time;
};

int main() {
  fstream file;
  file.open("jobs.dat", ios::out);

  if (!file) {
    cout << "Error opening file\n";
    return 1;
  }

  // Create file queue
  Job file_queue[7000];

  struct Job blank_job;
  size_t index = 0;
  while (!file.eof()) {
    file.write(reinterpret_cast<char*>(&blank_job), sizeof(Job));
    blank_job.idle_time = 0;

    file_queue[index] = blank_job;
    ++index;
  }

  size_t time = 0;

  // Start simulation for initial metrics
  while (time < 550) {
    ++time;
  }

  // Continue simulation for final metrics
  while (time < 10000) {
    ++time;
  }

  return 0;
}
