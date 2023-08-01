#include <bits/stdc++.h>
#include <fstream>
#include <sstream>
using namespace std;

struct Process 
{
    int pid;
    int arrival_time;
    int next_arrival_time;
    int burst_time;
    int io_time;
    int cpu_time;
    int cpu_remaining_time;
    int remaining_time;
    int started;
    int waiting_time;
    int turnaround_time;
    int response_time;
};

void printQueue(queue<Process> q)
{
	//printing content of queue 
	while (!q.empty()){
		cout<<" "<<q.front().pid;
		q.pop();
	}
	cout<<endl;
}

bool compare(Process a, Process b)
{
    return a.pid < b.pid;
}

bool compareArrival(Process a, Process b)
{
    return a.next_arrival_time < b.next_arrival_time;
}

int main() 
{
    // Read from file
    FILE *file = fopen("processes.txt", "r");
    vector<int> values;
    int n = 0;
    int c = 1;
    while (c != EOF) 
    {
        c = fgetc(file);
        if (c == '\n') 
        {
            ++n;
        }
    }
    fclose(file);
    file = fopen("processes.txt", "r");
    for (int i = 0; i < n; ++i) 
    {
        int a, b, c, d, e;
        fscanf(file, "%d;%d;%d;%d;%d ", &a, &b, &c, &d, &e);
        values.push_back(a);
        values.push_back(b);
        values.push_back(c);
        values.push_back(d);
        values.push_back(e);
    }
    //File closed


    int quantum, current_time = 0;
    queue<Process> ready_queue;
    vector<Process> processes;
    vector<Process> processes_store;
    vector<Process> gant_chart;

    Process *p_ptr = NULL;

    // input quantum time
    cout << "Enter the quantum time: ";
    cin >> quantum;
    cout << endl;

    for (int i = 0; i < n; i++) 
    {
        Process p;
        p.pid = values[0];
        values.erase(values.begin());
        cout << "Process ID: " << p.pid << endl;
        p.arrival_time = values[0];
        values.erase(values.begin());
        cout << "  Arrival time: " << p.arrival_time << endl;
        p.burst_time = values[0];
        values.erase(values.begin());
        cout << "  Burst time: " << p.burst_time << endl;
        p.io_time = values[0];
        values.erase(values.begin());
        cout << "  I/O time: " << p.io_time << endl;
        p.cpu_time = values[0];
        values.erase(values.begin());
        cout << "  CPU time: " << p.cpu_time << endl;
        p.remaining_time = p.burst_time;
        p.cpu_remaining_time = p.cpu_time;
        p.next_arrival_time = p.arrival_time;
        p.started = -1;
        processes.push_back(p);
    }

    cout << endl;
    // run the scheduler
    while (!ready_queue.empty() || !processes.empty()) 
    {
        // add new arrivals to the ready queue
        while (!processes.empty() && processes.front().next_arrival_time <= current_time) 
        {
            ready_queue.push(processes.front());
            processes.erase(processes.begin());
        }
        if(p_ptr != NULL)
        {
            ready_queue.push(*p_ptr);
            p_ptr = NULL;
        }

        //Print Ready Queue
        cout << "\tProcesses in ready queue at time " << current_time << ": ";
        printQueue(ready_queue);
        
        // process the current running process
        if (!ready_queue.empty()) 
        {
            Process p = ready_queue.front();
            ready_queue.pop();

            cout << "Time " << current_time << ": Process " << p.pid << " running" << endl;
            gant_chart.push_back(p);
            
            if(p.started == -1)
            {
                p.response_time = current_time-p.arrival_time;
                p.started = 0;
            }

            // run the process for a quantum
            int run_time = min(quantum, p.cpu_remaining_time);
            current_time += run_time;
            p.remaining_time -= run_time;
            p.cpu_remaining_time -= run_time;

            if(p.remaining_time != 0)
            {
                if(p.cpu_remaining_time == 0)
                {
                    //Go to io burst
                    p.next_arrival_time = current_time + p.io_time;
                    p.cpu_remaining_time = min(p.cpu_time, p.remaining_time);
                    processes.push_back(p);
                    sort(processes.begin(), processes.end(), compareArrival);
                }
                else
                {
                    //Complete remaining_cpu_burst 
                    p_ptr = &p;
                }
            }
            else
            {
                //finish process
                p.turnaround_time = current_time - p.arrival_time;
                p.waiting_time = p.turnaround_time - p.burst_time - p.io_time;
                cout << "Time " << current_time << ": Process " << p.pid << " finished CPU burst (waiting time " << p.waiting_time << ", turnaround time " << p.turnaround_time << ", response time " << p.response_time << ")" << endl;
                processes_store.push_back(p);
            }
        }
        else 
        {
            // no process is currently running
            current_time++;
        }
    }

    
    //Printing Gant Chart
    cout << endl;
    cout << endl;
    cout << "Gant Chart:" << endl;
    for(auto i : gant_chart)
    {
        cout << i.pid << " | ";
    }
    

    // calculate and print average waiting time, turnaround time, response time and cpu utilization and throughput
    int total_waiting_time = 0, total_turnaround_time = 0, total_response_time = 0, total_burst_time = 0;
    sort(processes_store.begin(), processes_store.end(), compare);
    cout << endl;
    cout << endl;
    cout << "PROCESS ID\t\tWAIT TIME\t\tTURNAROUND TIME\t\tRESPONSE TIME" << endl;

    for (Process p : processes_store) 
    {
        cout << p.pid << "\t\t\t" << p.waiting_time << "\t\t\t" << p.turnaround_time << "\t\t\t" << p.response_time << "\t\t\t" << endl;
        total_waiting_time += p.waiting_time;
        total_turnaround_time += p.turnaround_time;
        total_response_time += p.response_time;
        total_burst_time += p.burst_time;
    }
    cout << endl;
    cout << "Average waiting time: " << (double)total_waiting_time / n << endl;
    cout << "Average turnaround time: " << (double)total_turnaround_time / n << endl;
    cout << "Average response time: " << (double)total_response_time / n << endl;
    cout << "CPU Utilization: " << (double)total_burst_time*100/current_time << "%" << endl;
    cout << "Throughput: " << (double)n/current_time << endl;
    cout << endl;
    return 0;
}