/***************

Kieran Llarena, Alexander Cheayto, Jeeva Elango

Created 11-Apr-2024

Last Modified 16-Apr-2024

Purpose: Project Two - Program Two | CPU

****************/
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

struct Job
{
	char job_type;
	int arrival_time;
	int processing_time;
	int idle_time = 0;
	int jobSpecificNum = 0;
	int jobOverallNum = 0;
};

struct Node
{
	struct Job data;
	Node* next;
	Node* prev;
};

struct CPU
{
	struct Job current_job;
	unsigned int idle_time;
	unsigned int busy_time;
	int cpuNum;
};

void queue_push_back(Node*& queue_head, Node* data)
{
	Node* current = queue_head;

	while (current->next != nullptr)
	{
		current = current->next;
	}
	current->next = data;
}

int getQueueSize(Node*& queue_head)
{
	int totalAmount = 0;
	Node* current = queue_head;

	while (current != nullptr)
	{
		++totalAmount;
		current = current->next;
	}

	return totalAmount;
}

int getQueueTime(Node*& queue_head)
{
	int totalAmount = 0;
	Node* current = queue_head;

	while (current != nullptr)
	{
		totalAmount = totalAmount + current->data.idle_time;
		current = current->next;
	}

	return totalAmount;
}

void incrementQueue(Node*& queue_head)
{
	Node* current = queue_head;

	while (current != nullptr)
	{
		current->data.idle_time++; // Increment the idle time of the current node
		current = current->next; // Move to the next node
	}
}
int main() {
	ifstream file("jobs.txt");
	if (!file) {
		cout << "Error opening job file\n";
		return 1;
	}

	ofstream logFile("log.txt", ios::out);
	if (!logFile)
	{
		cout << "Error opening log file\n";
		return 1;
	}


	vector<Job> jobVector;
	Job temp_job;
	while (file >> temp_job.job_type >> temp_job.arrival_time >> temp_job.processing_time >> temp_job.idle_time >> temp_job.jobSpecificNum >> temp_job.jobOverallNum) {
		temp_job.jobOverallNum = jobVector.size() + 1;
		jobVector.push_back(temp_job);
	}

	file.close();

	// Needed values for metrics
	size_t time = 1;
	const int numCPUs = 8;
	int arrivedAJobs = 0;
	int arrivedBJobs = 0;
	int arrivedCJobs = 0;
	int arrivedDJobs = 0;
	int completedAJobs = 0;
	int completedBJobs = 0;
	int completedCJobs = 0;
	int completedDJobs = 0;
	double totalQueueTime = 0;
	int totalCPUIdleTime = 0;
	int totalCPUProcessTime = 0;
	int totalQueueSize = 0;
	int highestQueueAmount = 0;

	//A blank job structure for use later on
	struct Job blank_job = { '_', 0, 0, 0, 0, 0 };

	//The queues
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

	struct CPU cpu = { blank_job, 0, 0, 0 };
	CPU cpus[numCPUs] = { cpu }; //x amount of CPUs are created

	int i = 1;
	for (CPU& cpu : cpus) // Giving each CPU a number for later use in output, and setting all of them to have a blank job.
	{
		cpu.current_job = blank_job;
		cpu.cpuNum = i;
		++i;
	}

	while (time < 551) //To capture the first set of metrics
	{
		for (const auto& job : jobVector) {//Automatically sorts through the entire vector, searching for any jobs that have the same arrival time as current time


			if (job.arrival_time == time)
			{
				Node* new_node = new Node;
				new_node->data = job;
				new_node->next = nullptr;
				new_node->prev = nullptr;


				if (job.job_type == 'A')
				{
					++arrivedAJobs;
					logFile << "Time " << time << ":     Arrival: Overall Job:" << new_node->data.jobOverallNum
						<< ", Job " << job.job_type << ":" << job.jobSpecificNum << ", Processing Time: " << job.processing_time << endl;
					queue_push_back(regular_q, new_node);
				}
				else if (job.job_type == 'B')
				{
					++arrivedBJobs;
					logFile << "Time " << time << ":     Arrival: Overall Job:" << new_node->data.jobOverallNum
						<< ", Job " << job.job_type << ":" << job.jobSpecificNum << ", Processing Time: " << job.processing_time << endl;
					queue_push_back(regular_q, new_node);
				}
				else if (job.job_type == 'C')
				{
					++arrivedCJobs;
					logFile << "Time " << time << ":     Arrival: Overall Job:" << new_node->data.jobOverallNum
						<< ", Job " << job.job_type << ":" << job.jobSpecificNum << ", Processing Time: " << job.processing_time << endl;
					queue_push_back(regular_q, new_node);
				}
				else if (job.job_type == 'D')
				{
					++arrivedDJobs;
					logFile << "Time " << time << ":     Arrival: Overall Job:" << new_node->data.jobOverallNum
						<< ", Job " << job.job_type << ":" << job.jobSpecificNum << ", Processing Time: " << job.processing_time << endl;
					queue_push_back(priority_q, new_node);
				}

			}


		}

		for (CPU& cpu : cpus)
		{
			// Check if cpu is in use: if so, see if current process is priority or not, if not, sub a priority (or idle if no priority) in
			if (cpu.current_job.job_type != blank_job.job_type) // The blank job type is '_', so it will never happen unless cpu's current job is set to blank job
			{
				if (cpu.current_job.job_type == 'D') // Just process it
				{
					if ((cpu.current_job.processing_time - 1) > 0) // If there is still stuff left to process
					{
						--cpu.current_job.processing_time;
						++cpu.busy_time;
						++totalCPUProcessTime;
					}

					else if ((cpu.current_job.processing_time - 1) == 0) // This would be the last time for this item
					{
						logFile << "Time " << time << ":     Complete Processing Job:" << cpu.current_job.jobOverallNum << ", Job " << cpu.current_job.job_type
							<< ":" << cpu.current_job.jobSpecificNum << endl;
						++cpu.busy_time;
						++totalCPUProcessTime;


						if (cpu.current_job.job_type == 'A')
						{
							++completedAJobs;
						}
						else if (cpu.current_job.job_type == 'B')
						{
							++completedBJobs;
						}
						else if (cpu.current_job.job_type == 'C')
						{
							++completedCJobs;
						}
						else if (cpu.current_job.job_type == 'D')
						{
							++completedDJobs;
						}

						if (priority_q->next != nullptr) // check to see if priority queue has a job
						{
							cpu.current_job = priority_q->next->data; // Clone from priority_q to cpu

							Node* deleteNode = priority_q->next;
							cpu.current_job = priority_q->next->data;
							if (priority_q->next->next != nullptr)
							{
								priority_q->next->next->prev = priority_q; // Delete from priority_q
								priority_q->next = priority_q->next->next;
							}
							else
							{
								priority_q->next = nullptr;
							}
							delete deleteNode;

							cpu.idle_time = 0;
							cpu.busy_time = 0;

							logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
								<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
							totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
						}
						else if (idle_q->next != nullptr) // check to see if idle queue has a job
						{
							cpu.current_job = idle_q->next->data; // Clone from idle_q to cpu

							Node* deleteNode = idle_q->next;
							cpu.current_job = idle_q->next->data;
							if (idle_q->next->next != nullptr)
							{
								idle_q->next->next->prev = idle_q; // Delete from idle_q
								idle_q->next = idle_q->next->next;
							}
							else
							{
								idle_q->next = nullptr;
							}
							delete deleteNode;

							cpu.idle_time = 0;
							cpu.busy_time = 0;

							logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
								<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
							totalQueueTime = totalQueueTime + cpu.current_job.idle_time;

						}
						else if (regular_q->next != nullptr) // check to see if regular queue has a job
						{
							cpu.current_job = regular_q->next->data; // Clone from regular_q to cpu

							Node* deleteNode = regular_q->next;
							cpu.current_job = regular_q->next->data;
							if (regular_q->next->next != nullptr)
							{
								regular_q->next->next->prev = regular_q; // Delete from regular_q
								regular_q->next = regular_q->next->next;
							}
							else
							{
								regular_q->next = nullptr;
							}
							delete deleteNode;

							cpu.idle_time = 0;
							cpu.busy_time = 0;

							logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
								<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
							totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
						}
						else // if all of those fail, then there are no jobs for the CPU to do, so increment the idle times.
						{
							cpu.current_job = blank_job;
							++cpu.idle_time;
							++totalCPUIdleTime;
						}
					}
				}
				else // Current job is not priority, so check if there is a priority job or idle job in queue
				{
					// check priority list, if there's nothing, continue processing,
					if (priority_q->next != nullptr)
					{ // There is a job in priority queue, so clone the current job to the idle queue and clone the priority
						Node* new_node = new Node;
						new_node->data = cpu.current_job;
						new_node->next = nullptr; // Clone from cpu to idle_q
						new_node->prev = nullptr;
						queue_push_back(idle_q, new_node);

						cpu.current_job = priority_q->next->data; // Clone from priority_q to cpu

						Node* deleteNode = priority_q->next;
						cpu.current_job = priority_q->next->data;
						if (priority_q->next->next != nullptr)
						{
							priority_q->next->next->prev = priority_q; // Delete from priority_q
							priority_q->next = priority_q->next->next;
						}
						else
						{
							priority_q->next = nullptr;
						}
						delete deleteNode;
					}

					else if (idle_q->next != nullptr) // check to see if idle queue has a job
					{
						cpu.current_job = idle_q->next->data; // Clone from idle_q to cpu

						Node* deleteNode = idle_q->next;
						cpu.current_job = idle_q->next->data;
						if (idle_q->next->next != nullptr)
						{
							idle_q->next->next->prev = idle_q; // Delete from idle_q
							idle_q->next = idle_q->next->next;
						}
						else
						{
							idle_q->next = nullptr;
						}
						delete deleteNode;

						cpu.idle_time = 0;
						cpu.busy_time = 0;

						logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
							<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
						totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
					}

					// Process it
					if ((cpu.current_job.processing_time - 1) > 0) // If there is still stuff left to process
					{
						--cpu.current_job.processing_time;
						++cpu.busy_time;
						++totalCPUProcessTime;
					}
					else if ((cpu.current_job.processing_time - 1) == 0) // This would be the last time for this item
					{
						logFile << "Time " << time << ":     Complete Processing Job:" << cpu.current_job.jobOverallNum << ", Job " << cpu.current_job.job_type
							<< ":" << cpu.current_job.jobSpecificNum << endl;
						++cpu.busy_time;
						++totalCPUProcessTime;

						if (cpu.current_job.job_type == 'A')
						{
							++completedAJobs;
						}
						else if (cpu.current_job.job_type == 'B')
						{
							++completedBJobs;
						}
						else if (cpu.current_job.job_type == 'C')
						{
							++completedCJobs;
						}
						else if (cpu.current_job.job_type == 'D')
						{
							++completedDJobs;
						}

						if (priority_q->next != nullptr) // check to see if priority queue has a job
						{
							cpu.current_job = priority_q->next->data; // Clone from priority_q to cpu

							Node* deleteNode = priority_q->next;
							cpu.current_job = priority_q->next->data;
							if (priority_q->next->next != nullptr)
							{
								priority_q->next->next->prev = priority_q; // Delete from priority_q
								priority_q->next = priority_q->next->next;
							}
							else
							{
								priority_q->next = nullptr;
							}
							delete deleteNode;

							cpu.idle_time = 0;
							cpu.busy_time = 0;

							logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
								<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
							totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
						}
						else if (idle_q->next != nullptr) // check to see if idle queue has a job
						{
							cpu.current_job = idle_q->next->data; // Clone from idle_q to cpu

							Node* deleteNode = idle_q->next;
							cpu.current_job = idle_q->next->data;
							if (idle_q->next->next != nullptr)
							{
								idle_q->next->next->prev = idle_q; // Delete from idle_q
								idle_q->next = idle_q->next->next;
							}
							else
							{
								idle_q->next = nullptr;
							}
							delete deleteNode;

							cpu.idle_time = 0;
							cpu.busy_time = 0;

							logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
								<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
							totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
						}
						else if (regular_q->next != nullptr) // check to see if regular queue has a job
						{
							cpu.current_job = regular_q->next->data; // Clone from regular_q to cpu

							Node* deleteNode = regular_q->next;
							cpu.current_job = regular_q->next->data;
							if (regular_q->next->next != nullptr)
							{
								regular_q->next->next->prev = regular_q; // Delete from regular_q
								regular_q->next = regular_q->next->next;
							}
							else
							{
								regular_q->next = nullptr;
							}
							delete deleteNode;

							cpu.idle_time = 0;
							cpu.busy_time = 0;

							logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
								<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
							totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
						}
						else // if all of those fail, then there are no jobs for the CPU to do, so increment the idle times.
						{
							cpu.current_job = blank_job;
							++cpu.idle_time;
							++totalCPUIdleTime;
						}
					}
				}
			}


			else  // CPU isn't in use, so check if it's possible to give it a job
			{
				if (priority_q->next != nullptr) // check to see if priority queue has a job
				{
					cpu.current_job = priority_q->next->data; // Clone from priority_q to cpu

					Node* deleteNode = priority_q->next;
					cpu.current_job = priority_q->next->data;
					if (priority_q->next->next != nullptr)
					{
						priority_q->next->next->prev = priority_q; // Delete from priority_q
						priority_q->next = priority_q->next->next;
					}
					else
					{
						priority_q->next = nullptr;
					}
					delete deleteNode;

					cpu.idle_time = 0;
					cpu.busy_time = 0;

					logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
						<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
					totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
				}
				else if (idle_q->next != nullptr) // check to see if idle queue has a job
				{
					cpu.current_job = idle_q->next->data; // Clone from idle_q to cpu

					Node* deleteNode = idle_q->next;
					cpu.current_job = idle_q->next->data;
					if (idle_q->next->next != nullptr)
					{
						idle_q->next->next->prev = idle_q; // Delete from idle_q
						idle_q->next = idle_q->next->next;
					}
					else
					{
						idle_q->next = nullptr;
					}
					delete deleteNode;

					cpu.idle_time = 0;
					cpu.busy_time = 0;

					logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
						<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
					totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
				}
				else if (regular_q->next != nullptr) // check to see if regular queue has a job
				{
					cpu.current_job = regular_q->next->data; // Clone from regular_q to cpu

					Node* deleteNode = regular_q->next;
					cpu.current_job = regular_q->next->data;
					if (regular_q->next->next != nullptr)
					{
						regular_q->next->next->prev = regular_q; // Delete from regular_q
						regular_q->next = regular_q->next->next;
					}
					else
					{
						regular_q->next = nullptr;
					}
					delete deleteNode;

					cpu.idle_time = 0;
					cpu.busy_time = 0;

					logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
						<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
					totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
				}
				else // if all of those fail, then there are no jobs for the CPU to do, so increment the idle times.
				{
					cpu.current_job = blank_job;
					++cpu.idle_time;
					++totalCPUIdleTime;
				}
			}
		}

		// Increment all queued jobs' idle times
		incrementQueue(priority_q);
		incrementQueue(idle_q);
		incrementQueue(regular_q);

		// Every time increment requires a log

		string jobLog;
		if (priority_q->next == nullptr && idle_q->next == nullptr && regular_q->next == nullptr)
		{
			jobLog = "Empty; ";
		}
		else
		{
			int queueSize;
			string lastPart = " Job(s); ";

			queueSize = getQueueSize(priority_q) + getQueueSize(idle_q) + getQueueSize(regular_q);
			jobLog = std::to_string(queueSize) + lastPart;
		}

		logFile << "Time " << time << ":     Queue: " << jobLog;

		for (CPU& cpu : cpus)
		{
			if (cpu.busy_time > 0)
			{
				logFile << "CPU " << cpu.cpuNum << " Run Time:" << cpu.busy_time << ";  ";
			}
			else
			{
				logFile << "CPU " << cpu.cpuNum << " Idle Time:" << cpu.idle_time << ";  ";
			}
		}
		logFile << endl;


		if (highestQueueAmount < (getQueueSize(priority_q) + getQueueSize(idle_q) + getQueueSize(regular_q)))
		{
			highestQueueAmount = (getQueueSize(priority_q) + getQueueSize(idle_q) + getQueueSize(regular_q));
		}





		totalQueueSize = totalQueueSize + (getQueueSize(priority_q) + getQueueSize(regular_q) + getQueueSize(idle_q));

		++time;
	}

	//First set of metrics
	cout << "550 TIME UNITS COMPLETED.\nMETRICS FOR 550 TIME UNITS:" << endl;
	cout << "\nNumber of processor(s) being used: " << numCPUs << endl;
	cout << "Current queue size: " << getQueueSize(priority_q) + getQueueSize(regular_q) + getQueueSize(idle_q) << endl;
	cout << "Average queue size: " << (totalQueueSize / time) << endl;
	cout << "Maximum jobs in queue: " << highestQueueAmount << endl;
	cout << "Total time jobs are in queue: " << totalQueueTime << " time units" << endl;
	cout << "Average time jobs are in queue: " << (totalQueueTime / time) << " time units" << endl;
	cout << "Total number of A jobs arrived: " << arrivedAJobs << endl;
	cout << "Total number of A jobs completed: " << completedAJobs << endl;
	cout << "Total number of B jobs arrived: " << arrivedBJobs << endl;
	cout << "Total number of B jobs completed: " << completedBJobs << endl;
	cout << "Total number of C jobs arrived: " << arrivedCJobs << endl;
	cout << "Total number of C jobs completed: " << completedCJobs << endl;
	cout << "Total number of D jobs arrived: " << arrivedDJobs << endl;
	cout << "Total number of D jobs completed: " << completedDJobs << endl;
	cout << "Total jobs completed: " << (completedAJobs + completedBJobs + completedCJobs + completedDJobs) << endl;
	cout << "Total time CPU(s) were processing: " << totalCPUProcessTime << " time units" << endl;
	cout << "Total time CPU(s) were idle: " << totalCPUIdleTime << " time units" << endl;


	while (time > 550 && time < 10001 ) //Rest of the time alloted
	{
		for (const auto& job : jobVector) {//Automatically sorts through the entire vector, searching for any jobs that have the same arrival time as current time


			if (job.arrival_time == time)
			{
				Node* new_node = new Node;
				new_node->data = job;
				new_node->next = nullptr;
				new_node->prev = nullptr;


				if (job.job_type == 'A')
				{
					++arrivedAJobs;
					logFile << "Time " << time << ":     Arrival: Overall Job:" << new_node->data.jobOverallNum
						<< ", Job " << job.job_type << ":" << job.jobSpecificNum << ", Processing Time: " << job.processing_time << endl;
					queue_push_back(regular_q, new_node);
				}
				else if (job.job_type == 'B')
				{
					++arrivedBJobs;
					logFile << "Time " << time << ":     Arrival: Overall Job:" << new_node->data.jobOverallNum
						<< ", Job " << job.job_type << ":" << job.jobSpecificNum << ", Processing Time: " << job.processing_time << endl;
					queue_push_back(regular_q, new_node);
				}
				else if (job.job_type == 'C')
				{
					++arrivedCJobs;
					logFile << "Time " << time << ":     Arrival: Overall Job:" << new_node->data.jobOverallNum
						<< ", Job " << job.job_type << ":" << job.jobSpecificNum << ", Processing Time: " << job.processing_time << endl;
					queue_push_back(regular_q, new_node);
				}
				else if (job.job_type == 'D')
				{
					++arrivedDJobs;
					logFile << "Time " << time << ":     Arrival: Overall Job:" << new_node->data.jobOverallNum
						<< ", Job " << job.job_type << ":" << job.jobSpecificNum << ", Processing Time: " << job.processing_time << endl;
					queue_push_back(priority_q, new_node);
				}

			}


		}

		for (CPU& cpu : cpus)
		{
			// Check if cpu is in use: if so, see if current process is priority or not, if not, sub a priority (or idle if no priority) in
			if (cpu.current_job.job_type != blank_job.job_type) // The blank job type is '_', so it will never happen unless cpu's current job is set to blank job
			{
				if (cpu.current_job.job_type == 'D') // Just process it
				{
					if ((cpu.current_job.processing_time - 1) > 0) // If there is still stuff left to process
					{
						--cpu.current_job.processing_time;
						++cpu.busy_time;
						++totalCPUProcessTime;
					}

					else if ((cpu.current_job.processing_time - 1) == 0) // This would be the last time for this item
					{
						logFile << "Time " << time << ":     Complete Processing Job:" << cpu.current_job.jobOverallNum << ", Job " << cpu.current_job.job_type
							<< ":" << cpu.current_job.jobSpecificNum << endl;
						++cpu.busy_time;
						++totalCPUProcessTime;


						if (cpu.current_job.job_type == 'A')
						{
							++completedAJobs;
						}
						else if (cpu.current_job.job_type == 'B')
						{
							++completedBJobs;
						}
						else if (cpu.current_job.job_type == 'C')
						{
							++completedCJobs;
						}
						else if (cpu.current_job.job_type == 'D')
						{
							++completedDJobs;
						}

						if (priority_q->next != nullptr) // check to see if priority queue has a job
						{
							cpu.current_job = priority_q->next->data; // Clone from priority_q to cpu

							Node* deleteNode = priority_q->next;
							cpu.current_job = priority_q->next->data;
							if (priority_q->next->next != nullptr)
							{
								priority_q->next->next->prev = priority_q; // Delete from priority_q
								priority_q->next = priority_q->next->next;
							}
							else
							{
								priority_q->next = nullptr;
							}
							delete deleteNode;

							cpu.idle_time = 0;
							cpu.busy_time = 0;

							logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
								<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
							totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
						}
						else if (idle_q->next != nullptr) // check to see if idle queue has a job
						{
							cpu.current_job = idle_q->next->data; // Clone from idle_q to cpu

							Node* deleteNode = idle_q->next;
							cpu.current_job = idle_q->next->data;
							if (idle_q->next->next != nullptr)
							{
								idle_q->next->next->prev = idle_q; // Delete from idle_q
								idle_q->next = idle_q->next->next;
							}
							else
							{
								idle_q->next = nullptr;
							}
							delete deleteNode;

							cpu.idle_time = 0;
							cpu.busy_time = 0;

							logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
								<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
							totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
						}
						else if (regular_q->next != nullptr) // check to see if regular queue has a job
						{
							cpu.current_job = regular_q->next->data; // Clone from regular_q to cpu

							Node* deleteNode = regular_q->next;
							cpu.current_job = regular_q->next->data;
							if (regular_q->next->next != nullptr)
							{
								regular_q->next->next->prev = regular_q; // Delete from regular_q
								regular_q->next = regular_q->next->next;
							}
							else
							{
								regular_q->next = nullptr;
							}
							delete deleteNode;

							cpu.idle_time = 0;
							cpu.busy_time = 0;

							logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
								<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
							totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
						}
						else // if all of those fail, then there are no jobs for the CPU to do, so increment the idle times.
						{
							cpu.current_job = blank_job;
							++cpu.idle_time;
							++totalCPUIdleTime;
						}
					}
				}
				else // Current job is not priority, so check if there is a priority job or idle job in queue
				{
					// check priority list, if there's nothing, continue processing,
					if (priority_q->next != nullptr)
					{ // There is a job in priority queue, so clone the current job to the idle queue and clone the priority
						Node* new_node = new Node;
						new_node->data = cpu.current_job;
						new_node->next = nullptr; // Clone from cpu to idle_q
						new_node->prev = nullptr;
						queue_push_back(idle_q, new_node);

						cpu.current_job = priority_q->next->data; // Clone from priority_q to cpu

						Node* deleteNode = priority_q->next;
						cpu.current_job = priority_q->next->data;
						if (priority_q->next->next != nullptr)
						{
							priority_q->next->next->prev = priority_q; // Delete from priority_q
							priority_q->next = priority_q->next->next;
						}
						else
						{
							priority_q->next = nullptr;
						}
						delete deleteNode;
					}

					else if (idle_q->next != nullptr) // check to see if idle queue has a job
					{
						cpu.current_job = idle_q->next->data; // Clone from idle_q to cpu

						Node* deleteNode = idle_q->next;
						cpu.current_job = idle_q->next->data;
						if (idle_q->next->next != nullptr)
						{
							idle_q->next->next->prev = idle_q; // Delete from idle_q
							idle_q->next = idle_q->next->next;
						}
						else
						{
							idle_q->next = nullptr;
						}
						delete deleteNode;

						cpu.idle_time = 0;
						cpu.busy_time = 0;

						logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
							<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
						totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
					}

					// Process it
					if ((cpu.current_job.processing_time - 1) > 0) // If there is still stuff left to process
					{
						--cpu.current_job.processing_time;
						++cpu.busy_time;
						++totalCPUProcessTime;
					}
					else if ((cpu.current_job.processing_time - 1) == 0) // This would be the last time for this item
					{
						logFile << "Time " << time << ":     Complete Processing Job:" << cpu.current_job.jobOverallNum << ", Job " << cpu.current_job.job_type
							<< ":" << cpu.current_job.jobSpecificNum << endl;
						++cpu.busy_time;
						++totalCPUProcessTime;

						if (cpu.current_job.job_type == 'A')
						{
							++completedAJobs;
						}
						else if (cpu.current_job.job_type == 'B')
						{
							++completedBJobs;
						}
						else if (cpu.current_job.job_type == 'C')
						{
							++completedCJobs;
						}
						else if (cpu.current_job.job_type == 'D')
						{
							++completedDJobs;
						}

						if (priority_q->next != nullptr) // check to see if priority queue has a job
						{
							cpu.current_job = priority_q->next->data; // Clone from priority_q to cpu

							Node* deleteNode = priority_q->next;
							cpu.current_job = priority_q->next->data;
							if (priority_q->next->next != nullptr)
							{
								priority_q->next->next->prev = priority_q; // Delete from priority_q
								priority_q->next = priority_q->next->next;
							}
							else
							{
								priority_q->next = nullptr;
							}
							delete deleteNode;

							cpu.idle_time = 0;
							cpu.busy_time = 0;

							logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
								<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
							totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
						}
						else if (idle_q->next != nullptr) // check to see if idle queue has a job
						{
							cpu.current_job = idle_q->next->data; // Clone from idle_q to cpu

							Node* deleteNode = idle_q->next;
							cpu.current_job = idle_q->next->data;
							if (idle_q->next->next != nullptr)
							{
								idle_q->next->next->prev = idle_q; // Delete from idle_q
								idle_q->next = idle_q->next->next;
							}
							else
							{
								idle_q->next = nullptr;
							}
							delete deleteNode;

							cpu.idle_time = 0;
							cpu.busy_time = 0;

							logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
								<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
							totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
						}
						else if (regular_q->next != nullptr) // check to see if regular queue has a job
						{
							cpu.current_job = regular_q->next->data; // Clone from regular_q to cpu

							Node* deleteNode = regular_q->next;
							cpu.current_job = regular_q->next->data;
							if (regular_q->next->next != nullptr)
							{
								regular_q->next->next->prev = regular_q; // Delete from regular_q
								regular_q->next = regular_q->next->next;
							}
							else
							{
								regular_q->next = nullptr;
							}
							delete deleteNode;

							cpu.idle_time = 0;
							cpu.busy_time = 0;

							logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
								<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
							totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
						}
						else // if all of those fail, then there are no jobs for the CPU to do, so increment the idle times.
						{
							cpu.current_job = blank_job;
							++cpu.idle_time;
							++totalCPUIdleTime;
						}
					}
				}
			}


			else  // CPU isn't in use, so check if it's possible to give it a job
			{
				if (priority_q->next != nullptr) // check to see if priority queue has a job
				{
					cpu.current_job = priority_q->next->data; // Clone from priority_q to cpu

					Node* deleteNode = priority_q->next;
					cpu.current_job = priority_q->next->data;
					if (priority_q->next->next != nullptr)
					{
						priority_q->next->next->prev = priority_q; // Delete from priority_q
						priority_q->next = priority_q->next->next;
					}
					else
					{
						priority_q->next = nullptr;
					}
					delete deleteNode;

					cpu.idle_time = 0;
					cpu.busy_time = 0;

					logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
						<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
					totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
				}
				else if (idle_q->next != nullptr) // check to see if idle queue has a job
				{
					cpu.current_job = idle_q->next->data; // Clone from idle_q to cpu

					Node* deleteNode = idle_q->next;
					cpu.current_job = idle_q->next->data;
					if (idle_q->next->next != nullptr)
					{
						idle_q->next->next->prev = idle_q; // Delete from idle_q
						idle_q->next = idle_q->next->next;
					}
					else
					{
						idle_q->next = nullptr;
					}
					delete deleteNode;

					cpu.idle_time = 0;
					cpu.busy_time = 0;

					logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
						<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
					totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
				}
				else if (regular_q->next != nullptr) // check to see if regular queue has a job
				{
					cpu.current_job = regular_q->next->data; // Clone from regular_q to cpu

					Node* deleteNode = regular_q->next;
					cpu.current_job = regular_q->next->data;
					if (regular_q->next->next != nullptr)
					{
						regular_q->next->next->prev = regular_q; // Delete from regular_q
						regular_q->next = regular_q->next->next;
					}
					else
					{
						regular_q->next = nullptr;
					}
					delete deleteNode;

					cpu.idle_time = 0;
					cpu.busy_time = 0;

					logFile << "Time " << time << ":     Begin Processing Job:" << (cpu.current_job.jobOverallNum)
						<< ", Job " << cpu.current_job.job_type << ":" << cpu.current_job.jobSpecificNum << " in CPU " << cpu.cpuNum << endl;
					totalQueueTime = totalQueueTime + cpu.current_job.idle_time;
				}
				else // if all of those fail, then there are no jobs for the CPU to do, so increment the idle times.
				{
					cpu.current_job = blank_job;
					++cpu.idle_time;
					++totalCPUIdleTime;
				}
			}
		}

		// Increment all queued jobs' idle times
		incrementQueue(priority_q);
		incrementQueue(idle_q);
		incrementQueue(regular_q);

		// Every time increment requires a log

		string jobLog;
		if (priority_q->next == nullptr && idle_q->next == nullptr && regular_q->next == nullptr)
		{
			jobLog = "Empty; ";
		}
		else
		{
			int queueSize;
			string lastPart = " Job(s); ";

			queueSize = getQueueSize(priority_q) + getQueueSize(idle_q) + getQueueSize(regular_q);
			jobLog = std::to_string(queueSize) + lastPart;
		}

		logFile << "Time " << time << ":     Queue: " << jobLog;

		for (CPU& cpu : cpus)
		{
			if (cpu.busy_time > 0)
			{
				logFile << "CPU " << cpu.cpuNum << " Run Time:" << cpu.busy_time << ";  ";
			}
			else
			{
				logFile << "CPU " << cpu.cpuNum << " Idle Time:" << cpu.idle_time << ";  ";
			}
		}
		logFile << endl;


		if (highestQueueAmount < (getQueueSize(priority_q) + getQueueSize(idle_q) + getQueueSize(regular_q)))
		{
			highestQueueAmount = (getQueueSize(priority_q) + getQueueSize(idle_q) + getQueueSize(regular_q));
		}





		totalQueueSize = totalQueueSize + (getQueueSize(priority_q) + getQueueSize(regular_q) + getQueueSize(idle_q));

		++time;
	}

	//FInal metrics
	cout << "\n\n10000 TIME UNITS COMPLETED.\nTOTAL METRICS:" << endl;
	cout << "\nNumber of processor(s) being used: " << numCPUs << endl;
	cout << "Current queue size: " << getQueueSize(priority_q) + getQueueSize(regular_q) + getQueueSize(idle_q) << endl;
	cout << "Average queue size: " << (totalQueueSize / time) << endl;
	cout << "Maximum jobs in queue: " << highestQueueAmount << endl;
	cout << "Total time jobs are in queue: " << totalQueueTime << " time units" << endl;
	cout << "Average time jobs are in queue: " << (totalQueueTime / time) << " time units" << endl;
	cout << "Total number of A jobs arrived: " << arrivedAJobs << endl;
	cout << "Total number of A jobs completed: " << completedAJobs << endl;
	cout << "Total number of B jobs arrived: " << arrivedBJobs << endl;
	cout << "Total number of B jobs completed: " << completedBJobs << endl;
	cout << "Total number of C jobs arrived: " << arrivedCJobs << endl;
	cout << "Total number of C jobs completed: " << completedCJobs << endl;
	cout << "Total number of D jobs arrived: " << arrivedDJobs << endl;
	cout << "Total number of D jobs completed: " << completedDJobs << endl;
	cout << "Total jobs completed: " << (completedAJobs + completedBJobs + completedCJobs + completedDJobs) << endl;
	cout << "Total time CPU(s) were processing: " << totalCPUProcessTime << " time units" << endl;
	cout << "Total time CPU(s) were idle: " << totalCPUIdleTime << " time units" << endl;


	return 0;
}
