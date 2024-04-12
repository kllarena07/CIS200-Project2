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

struct Node {
  struct Job data;
  Node* next;
  Node* prev;
};

struct CPU {
  struct Job current_job;
  unsigned int idle_time;
  unsigned int busy_time;
  unsigned int total_time;
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

  struct Job blank_job = {'_', 0, 0, 0};
  size_t index = 0;
  while (!file.eof()) {
    file.write(reinterpret_cast<char*>(&blank_job), sizeof(Job));
    blank_job.idle_time = 0;

    file_queue[index] = blank_job;
    ++index;
  }

  size_t time = 0;

  Node* priority_q = new Node;
  priority_q->data = blank_job;
  priority_q->next = nullptr;
  priority_q->prev = nullptr;

  Node* regular_q = new Node;
  regular_q->data = blank_job;
  regular_q->next = nullptr;
  regular_q->prev = nullptr;

  Node* idle_q = new Node;
  idle_q->data = blank_job;
  idle_q->next = nullptr;
  idle_q->prev = nullptr;

  struct CPU cpu = {blank_job, 0, 0, 0};
  CPU cpus[1] = {cpu};

  // Start simulation for initial metrics
  while (time < 550) {
    for (CPU& cpu : cpus) {
    }

    ++time;
  }

  // Continue simulation for final metrics
  while (time < 10000) {
    ++time;
  }

  delete priority_q;
  delete regular_q;
  delete idle_q;

  return 0;
}
