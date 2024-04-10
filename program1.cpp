#include <time.h>

#include <fstream>
#include <iostream>
using std::cout;
using std::ofstream;

struct Job {
  char job_type;
  unsigned int arrival_time;
  unsigned int processing_time;
};

int get_random_int(int min, int max) { return min + rand() % (max - min + 1); }

int main() {
  srand(time(NULL));

  Job type_a_jobs[2400];
  Job type_b_jobs[2000];
  Job type_c_jobs[1100];
  Job type_d_jobs[1500];

  for (Job& job : type_a_jobs) {
    job.job_type = 'A';
    job.arrival_time = get_random_int(6, 8);
    job.processing_time = get_random_int(1, 5);
  }

  for (Job& job : type_b_jobs) {
    job.job_type = 'B';
    job.arrival_time = get_random_int(2, 8);
    job.processing_time = get_random_int(3, 13);
  }

  for (Job& job : type_c_jobs) {
    job.job_type = 'C';
    job.arrival_time = get_random_int(4, 18);
    job.processing_time = get_random_int(6, 12);
  }

  for (Job& job : type_d_jobs) {
    job.job_type = 'D';
    job.arrival_time = get_random_int(2, 12);
    job.processing_time = get_random_int(2, 22);
  }
}