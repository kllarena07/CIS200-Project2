/***************

Kieran Llarena, Alexander Cheayto, Jeeva Elango

Created 11-Apr-2024

Last Modified 15-Apr-2024

Purpose: Project Two - Program Two | CPU

****************/

#include <fstream>
#include <iostream>
using std::cout;
using std::endl;
using std::fstream;
using std::ios;

struct Job
{
  char job_type;
  int jobNum;
  unsigned int arrival_time;
  unsigned int processing_time;
  unsigned int idle_time;
};

struct Node
{
  struct Job data;
  Node *next;
  Node *prev;
};

struct CPU
{
  struct Job current_job;
  unsigned int idle_time;
  unsigned int busy_time;
  unsigned int total_time; // I don't see a need for this, remove it?
};

void queue_push_back(Node *&queue_head, Node *data)
{
  Node *current = queue_head;

  while (current != nullptr)
  {
    current = current->next;
  }
  current = data;
}

int getQueueSize(Node *&queue_head)
{
  int totalAmount = 0;
  Node *current = queue_head;

  while (current != nullptr)
  {
    current = current->next;
    ++totalAmount;
  }
  
  return totalAmount;
}

void incrementQueue(Node *&queue_head)
{
  int totalAmount = 0;
  Node *current = queue_head;

  while (current != nullptr)
  {
    current = current->next;
    ++current->data.idle_time;
  }
}

int main()
{
  // File stuff
  fstream file;
  file.open("jobs.dat", ios::out);

  fstream logFile("log.txt", ios::out | fstream::app); // Automatically overwrites the previous logs for ease-of-use.

  if (!file)
  {
    cout << "Error opening job file\n";
    return 1;
  }

  if (!logFile)
  {
    cout << "Error opening log file\n";
    return 1;
  }

  // Needed values
  const int numCPUs = 1;
  int arrivedAJobs = 0;
  int arrivedBJobs = 0;
  int arrivedCJobs = 0;
  int arrivedDJobs = 0;
  int completedAJobs = 0;
  int completedBJobs = 0;
  int completedCJobs = 0;
  int completedDJobs = 0;
  int totalQueueTime = 0;
  int totalCPUIdleTime = 0;
  int totalCPUProcessTime = 0;
  int totalQueueSize = 0;
  int highestQueueAmount = 0;

  // Create file queue
  Job file_queue[7000];

  struct Job blank_job = {'_', 0, 0, 0};
  size_t index = 0;
  while (!file.eof())
  {
    file.read(reinterpret_cast<char *>(&blank_job), sizeof(Job));
    blank_job.idle_time = 0;

    file_queue[index] = blank_job;
    ++index;
  }

  size_t time = 0;

  Node *priority_q = new Node;
  priority_q->data = blank_job;
  priority_q->next = nullptr;
  priority_q->prev = nullptr;

  Node *regular_q = new Node;
  regular_q->data = blank_job;
  regular_q->next = nullptr;
  regular_q->prev = nullptr;

  Node *idle_q = new Node;
  idle_q->data = blank_job;
  idle_q->next = nullptr;
  idle_q->prev = nullptr;

  struct CPU cpu = {blank_job, 0, 0, 0};
  CPU cpus[numCPUs] = {cpu};

  // Start simulation for initial metrics
  while (time < 550)
  {
    for (size_t i = 0; i < 7000; ++i)
    {
      if (file_queue[i].arrival_time > time)
        break;

      if (file_queue[i].arrival_time == time)
      {
        Node *new_node = new Node;
        new_node->data = file_queue[i];
        new_node->next = nullptr;
        new_node->prev = nullptr;

        if(file_queue[i].job_type == 'A')
        {
          ++ arrivedAJobs;
          logFile << "Time " << time <<  ":     Arrival: Overall Job:" << (completedAJobs+completedBJobs+completedCJobs+completedDJobs) 
          << ", Job " << file_queue[i].job_type << ":" << arrivedAJobs << ", Processing Time: " << file_queue[i].processing_time << endl;
        }
        else if(file_queue[i].job_type == 'B')
        {
          ++ arrivedBJobs;
          logFile << "Time " << time <<  ":     Arrival: Overall Job:" << (completedAJobs+completedBJobs+completedCJobs+completedDJobs) 
          << ", Job " << file_queue[i].job_type << ":" << arrivedBJobs << ", Processing Time: " << file_queue[i].processing_time << endl;
        }
        else if(file_queue[i].job_type == 'C')
        {
          ++ arrivedCJobs;
          logFile << "Time " << time <<  ":     Arrival: Overall Job:" << (completedAJobs+completedBJobs+completedCJobs+completedDJobs) 
          << ", Job " << file_queue[i].job_type << ":" << arrivedCJobs << ", Processing Time: " << file_queue[i].processing_time << endl;
        }
        else if(file_queue[i].job_type == 'D')
        {
          ++ arrivedDJobs;
          logFile << "Time " << time <<  ":     Arrival: Overall Job:" << (completedAJobs+completedBJobs+completedCJobs+completedDJobs) 
          << ", Job " << file_queue[i].job_type << ":" << arrivedDJobs << ", Processing Time: " << file_queue[i].processing_time << endl; 
        }

        file_queue[i].job_type == 'D' ? queue_push_back(priority_q, new_node)
                                      : queue_push_back(regular_q, new_node);
      }
    }

    for (CPU &cpu : cpus)
    {
      //Check if cpu is in use: if so, see if current process is priority or not, if not, sub a priority (or idle if no priority) in
      if (cpu.current_job.job_type != blank_job.job_type)//The blank job type is '_', so it will never happen unless cpu's current job is set to blank job
      { 
        if(cpu.current_job.job_type == 'D')//Just process it
        {
          //Process it, if cpu finishes, set cpu's current job to blankjob and reset idle & busy time
          ++cpu.busy_time;
        }      
        else//See if there is a priority job to do
        {
          //check priority list, if there's nothing, continue processing
          ++cpu.busy_time;
        }
      }

      else{//CPU isn't in use, so check if it's possible to give it a job
        //check priority
        //check idle
        //check regular

        //if nothing
        ++cpu.idle_time;
        ++totalCPUIdleTime;

      }
      //Do the work, if cpu finishes, then set cpu's current job to blankjob

      //If cpu not in use, load a priority queue item first, then idle, then regular
    }


    //Increment all queue idle times

    ++time;
  }

  // First set of metrics
  cout << "550 TIME UNITS COMPLETED.\nMETRICS FOR 550 TIME UNITS:" << endl;
  cout << "\nNumber of processor(s) being used: " << numCPUs << endl;
  cout << "Current queue size: " << getQueueSize(priority_q)+getQueueSize(regular_q)+getQueueSize(idle_q) << endl;
  cout << "Average queue size: " << (totalQueueSize/time) << endl;
  cout << "Maximum jobs in queue: " << highestQueueAmount << endl;
  cout << "Total time jobs are in queue: " << totalQueueTime << " time units" << endl;
  cout << "Average time jobs are in queue: " << (totalQueueTime/time) << " time units" << endl;
  cout << "Total number of A jobs arrived: " << arrivedAJobs << endl;
  cout << "Total number of A jobs completed: " << completedAJobs << endl;
  cout << "Total number of B jobs arrived: " << arrivedBJobs << endl;
  cout << "Total number of B jobs completed: " << completedBJobs << endl;
  cout << "Total number of C jobs arrived: " << arrivedCJobs << endl;
  cout << "Total number of C jobs completed: " << completedCJobs << endl;
  cout << "Total number of D jobs arrived: " << arrivedDJobs << endl;
  cout << "Total number of D jobs completed: " << completedDJobs << endl;
  cout << "Total jobs completed: " << (completedAJobs+completedBJobs+completedCJobs+completedDJobs) << endl;
  cout << "Total time CPU(s) were processing: " << totalCPUProcessTime << " time units" << endl; 
  cout << "Total time CPU(s) were idle: " << totalCPUIdleTime << " time units" << endl;

      // Continue simulation for final metrics
      while (time > 549 && time < 10000)
  {
    for (size_t i = 0; i < 7000; ++i)
    {
      if (file_queue[i].arrival_time > time)
        break;

      if (file_queue[i].arrival_time == time)
      {
        Node *new_node = new Node;
        new_node->data = file_queue[i];
        new_node->next = nullptr;
        new_node->prev = nullptr;

        if(file_queue[i].job_type == 'A')
        {
          ++ arrivedAJobs;
          logFile << "Time " << time <<  ":     Arrival: Overall Job:" << (completedAJobs+completedBJobs+completedCJobs+completedDJobs) 
          << ", Job " << file_queue[i].job_type << ":" << arrivedAJobs << ", Processing Time: " << file_queue[i].processing_time << endl;
        }
        else if(file_queue[i].job_type == 'B')
        {
          ++ arrivedBJobs;
          logFile << "Time " << time <<  ":     Arrival: Overall Job:" << (completedAJobs+completedBJobs+completedCJobs+completedDJobs) 
          << ", Job " << file_queue[i].job_type << ":" << arrivedBJobs << ", Processing Time: " << file_queue[i].processing_time << endl;
        }
        else if(file_queue[i].job_type == 'C')
        {
          ++ arrivedCJobs;
          logFile << "Time " << time <<  ":     Arrival: Overall Job:" << (completedAJobs+completedBJobs+completedCJobs+completedDJobs) 
          << ", Job " << file_queue[i].job_type << ":" << arrivedCJobs << ", Processing Time: " << file_queue[i].processing_time << endl;
        }
        else if(file_queue[i].job_type == 'D')
        {
          ++ arrivedDJobs;
          logFile << "Time " << time <<  ":     Arrival: Overall Job:" << (completedAJobs+completedBJobs+completedCJobs+completedDJobs) 
          << ", Job " << file_queue[i].job_type << ":" << arrivedDJobs << ", Processing Time: " << file_queue[i].processing_time << endl; 
        }

        file_queue[i].job_type == 'D' ? queue_push_back(priority_q, new_node)
                                      : queue_push_back(regular_q, new_node);
      }
    }

    for (CPU &cpu : cpus)
    {
      // load cpu with job from the queues according to the spec design of the
      // project and then do the processing
    }

    ++time;
  }

  delete priority_q;
  delete regular_q;
  delete idle_q;

  return 0;
}
