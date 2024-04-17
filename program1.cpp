/***************

Kieran Llarena, Alexander Cheayto, Jeeva Elango

Created 11-Apr-2024

Last Modified 16-Apr-2024

Purpose: Project Two - Program One | Jobs

****************/
#include <time.h>
#include <fstream>
#include <iostream>
using std::cout;
using std::fstream;
using std::ios;

struct Job
{
	char job_type;
	int arrival_time;
	int processing_time;
	int idle_time = 0;
	int jobSpecificNum = 0;
	int jobOverallNum = 0;
};

const int JOBSIZE = sizeof(Job);

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


	Node* head = new Node;
	struct Job blank_job = { '_', 0, 0, 0, 0, 0 };
	head->data = blank_job;
	head->next = nullptr;
	head->prev = nullptr;

	struct Job new_job;

	//From my understanding of the instructions, the arrival times start off like below, then are increased by (base + a random number between a range)

	int lastArrivalA = 7; //Makes setting arrival times easier
	int lastArrivalB = 5;
	int lastArrivalC = 11;
	int lastArrivalD = 7;

	int totalA = 0; //Makes setting job numbers easier
	int totalB = 0;
	int totalC = 0;
	int totalD = 0;

	for (size_t i = 0; i < MAX_A_JOBS; ++i) {//Runs for maximum number of A jobs
		new_job.job_type = 'A';
		new_job.arrival_time = lastArrivalA + (7 + get_random_int(-1, 1));
		lastArrivalA = new_job.arrival_time;
		new_job.processing_time = 3 + get_random_int(-2, 2);
		++totalA;
		new_job.jobSpecificNum = totalA;
		insert_node(head, new_job);
	}

	for (size_t i = 0; i < MAX_B_JOBS; ++i) {//Runs for maximum number of B jobs
		new_job.job_type = 'B';
		new_job.arrival_time = lastArrivalB + (5 + get_random_int(-3, 3));
		lastArrivalB = new_job.arrival_time;
		new_job.processing_time = 8 + get_random_int(-5, 5);
		++totalB;
		new_job.jobSpecificNum = totalB;
		insert_node(head, new_job);
	}

	for (size_t i = 0; i < MAX_C_JOBS; ++i) {//Runs for maximum number of C jobs
		new_job.job_type = 'C';
		new_job.arrival_time = lastArrivalC + (11 + get_random_int(-7, 7));
		lastArrivalC = new_job.arrival_time;
		new_job.processing_time = get_random_int(6, 12);
		++totalC;
		new_job.jobSpecificNum = totalC;
		insert_node(head, new_job);
	}

	for (size_t i = 0; i < MAX_D_JOBS; ++i) {//Runs for maximum number of D jobs
		new_job.job_type = 'D';
		new_job.arrival_time = lastArrivalD + (7 + get_random_int(-5, 5));
		lastArrivalD = new_job.arrival_time;
		new_job.processing_time = get_random_int(2, 22);
		++totalD;
		new_job.jobSpecificNum = totalD;
		insert_node(head, new_job);
	}



	fstream file("jobs.txt", ios::out);
	if (!file.is_open()) {
		cout << "Error opening file\n";
		return 1;
	}

	Node* current = head->next; //Writes each node into the file
	while (current != nullptr) {
		file << current->data.job_type << " "
		<< current->data.arrival_time << " "
		<< current->data.processing_time << " "
		<< current->data.idle_time << " "
		<< current->data.jobSpecificNum << " "
		<< current->data.jobOverallNum << "\n";

		current = current->next;
	}

	delete head;

	file.close();

	return 0;
}