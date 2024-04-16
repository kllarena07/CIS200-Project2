#include <time.h>

#include <fstream>
#include <iostream>
using std::cout;
using std::fstream;
using std::ios;

struct Job {
  char job_type;
  unsigned int arrival_time;
  unsigned int processing_time;
};

struct Node {
  struct Job data;
  Node* next;
  Node* prev;
};

const size_t MAX_A_JOBS = 2400;
const size_t MAX_B_JOBS = 2000;
const size_t MAX_C_JOBS = 1100;
const size_t MAX_D_JOBS = 1500;
const size_t MAX_JOBS = 7000;

int get_random_int(int min, int max) { return min + rand() % (max - min + 1); }

// Inserting a new node into its correct position in the linked list
int insert_node(Node*& head, struct Job new_job) {
  Node* new_node = new Node;
  new_node->data = new_job;
  new_node->next = nullptr;
  new_node->prev = nullptr;

  if (head == nullptr) {
    head = new_node;
    return 0;
  }

  Node* current = head;
  while (current->next != nullptr &&
         current->next->data.arrival_time < new_job.arrival_time) {
    current = current->next;
  }

  new_node->next = current->next;
  new_node->prev = current;
  if (current->next != nullptr) {
    current->next->prev = new_node;
  }
  current->next = new_node;

  return 0;
}

int main() {
  srand(time(NULL));

  // Initialize a sorted linked list with all the data and then write all of it
  // to the jobs.dat file

  Node* head = new Node;
  struct Job blank_job = {'_', 0, 0};
  head->data = blank_job;
  head->next = nullptr;
  head->prev = nullptr;

  for (size_t i = 0; i < MAX_A_JOBS; ++i) {
    struct Job new_job;
    new_job.job_type = 'A';
    new_job.arrival_time = get_random_int(6, 8);
    new_job.processing_time = get_random_int(1, 5);

    insert_node(head, new_job);
  }

  for (size_t i = 0; i < MAX_B_JOBS; ++i) {
    struct Job new_job;
    new_job.job_type = 'B';
    new_job.arrival_time = get_random_int(2, 8);
    new_job.processing_time = get_random_int(3, 13);

    insert_node(head, new_job);
  }

  for (size_t i = 0; i < MAX_C_JOBS; ++i) {
    struct Job new_job;
    new_job.job_type = 'C';
    new_job.arrival_time = get_random_int(4, 18);
    new_job.processing_time = get_random_int(6, 12);

    insert_node(head, new_job);
  }

  for (size_t i = 0; i < MAX_D_JOBS; ++i) {
    struct Job new_job;
    new_job.job_type = 'D';
    new_job.arrival_time = get_random_int(2, 12);
    new_job.processing_time = get_random_int(2, 22);

    insert_node(head, new_job);
  }

  // Overwriting all the data to try and make it more random
  fstream file("jobs.dat", ios::out | ios::in | ios::trunc);
  if (!file.is_open()) {
    cout << "Error opening file\n";
    return 1;
  }

  Node* current = head->next;
  while (current != nullptr) {
    file.write(reinterpret_cast<char*>(&current->data), sizeof(Job));
    current = current->next;
  }

  delete head;

  file.close();

  return 0;
}