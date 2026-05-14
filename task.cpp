#include <bits/stdc++.h>
using namespace std;
struct Task
{
    int id;
    int arrival;
    int execution;
    int priority;
    int remaining;
};
class Strategy
{
public:
    virtual void run(vector<Task> tasks) = 0;
    virtual ~Strategy() {}
};

class FIFO : public Strategy
{
public:
    void run(vector<Task> tasks) override
    {

        sort(tasks.begin(), tasks.end(), [](Task &a, Task &b)
             { return a.arrival < b.arrival; });

        int curr_t = 0;

        cout << "\n--- FIFO ---\n";

        for (auto &t : tasks)
        {
            if (curr_t < t.arrival)
                curr_t = t.arrival;

            int start = curr_t;
            int end = start + t.execution;

            cout << "Task " << t.id
                 << " -> Start: " << start
                 << " End: " << end << endl;

            curr_t = end;
        }
    }
};

class SJF : public Strategy
{
public:
    void run(vector<Task> tasks) override
    {

        sort(tasks.begin(), tasks.end(), [](Task &a, Task &b)
             { return a.arrival < b.arrival; });

        auto cmp = [](Task &a, Task &b)
        {
            return a.execution > b.execution;
        };

        priority_queue<Task, vector<Task>, decltype(cmp)> pq(cmp);

        int i = 0, n = tasks.size(), curr_t = 0;

        cout << "\n--- SJF ---\n";

        while (i < n || !pq.empty())
        {

            if (pq.empty() && curr_t < tasks[i].arrival)
                curr_t = tasks[i].arrival;

            while (i < n && tasks[i].arrival <= curr_t)
            {
                pq.push(tasks[i]);
                i++;
            }

            Task t = pq.top();
            pq.pop();

            int start = curr_t;
            int end = start + t.execution;

            cout << "Task " << t.id
                 << " -> Start: " << start
                 << " End: " << end << endl;

            curr_t = end;
        }
    }
};

class PriorityScheduling : public Strategy
{
public:
    void run(vector<Task> tasks) override
    {

        // Sort by arrival time
        sort(tasks.begin(), tasks.end(), [](Task &a, Task &b)
             { return a.arrival < b.arrival; });

        // Min heap based on priority
        auto cmp = [](Task &a, Task &b)
        {
            if (a.priority == b.priority)
                return a.arrival > b.arrival;
            return a.priority > b.priority;
        };

        priority_queue<Task, vector<Task>, decltype(cmp)> pq(cmp);

        int i = 0, n = tasks.size(), curr_t = 0;

        cout << "\n--- Priority Scheduling ---\n";

        while (i < n || !pq.empty())
        {

            // CPU idle
            if (pq.empty() && curr_t < tasks[i].arrival)
                curr_t = tasks[i].arrival;

            // Add all available tasks
            while (i < n && tasks[i].arrival <= curr_t)
            {
                pq.push(tasks[i]);
                i++;
            }

            // Pick highest priority
            Task t = pq.top();
            pq.pop();

            int start = curr_t;
            int end = start + t.execution;

            cout << "Task " << t.id
                 << " -> Start: " << start
                 << " End: " << end
                 << " (priority=" << t.priority << ")"
                 << endl;

            curr_t = end;
        }
    }
};

class RoundRobin : public Strategy {
public:
    int quantum;

    RoundRobin(int q) { quantum = q; }

    void run(vector<Task> tasks) override {

        sort(tasks.begin(), tasks.end(), [](Task &a, Task &b){
            return a.arrival < b.arrival;
        });

        for(auto &t : tasks)
            t.remaining = t.execution;

        queue<Task> q;

        int i = 0, n = tasks.size(), curr_t = 0;

        cout << "\n--- Round Robin ---\n";

        while(i < n || !q.empty()){

            if(q.empty() && curr_t < tasks[i].arrival)
                curr_t = tasks[i].arrival;

            while(i < n && tasks[i].arrival <= curr_t){
                q.push(tasks[i]);
                i++;
            }

            Task t = q.front(); q.pop();

            int start = curr_t;

            int run_time = min(quantum, t.remaining);

            curr_t += run_time;
            t.remaining -= run_time;

            cout << "Task " << t.id
     << " | Start: " << start
     << " | End: " << curr_t
     << " | Duration: " << t.execution
     << endl;

            while(i < n && tasks[i].arrival <= curr_t){
                q.push(tasks[i]);
                i++;
            }

            if(t.remaining > 0)
                q.push(t);
        }
    }
};

class Scheduler {
private:
    Strategy* strategy;

public:
    Scheduler(Strategy* s) {
        strategy = s;
    }

    void execute(vector<Task> tasks) {
        strategy->run(tasks);
    }
};
// struct Compare
// {
//     bool operator()(Task &a, Task &b)
//     {
//         return a.execution > b.execution;
//     }
// };
// struct Comapre
// {
//     bool operator()(Task &a, Task &b)
//     {
//         return a.priority > b.priority;
//     }
// };
// class FIFO
// {
// public:
//     vector<Task> tasks;

//     void insert(int id, int arrival, int execution)
//     {
//         tasks.push_back({id, arrival, execution});
//     }

//     void run()
//     {

//         sort(tasks.begin(), tasks.end(), [](Task &a, Task &b)
//              { return a.arrival < b.arrival; });

//         int curr_t = 0;

//         cout << "\n--- FIFO Scheduling Execution ---\n";

//         for (auto &t : tasks)
//         {

//             if (curr_t < t.arrival)
//             {
//                 curr_t = t.arrival;
//             }

//             int start = curr_t;
//             int end = start + t.execution;

//             cout << "Task " << t.id
//                  << " -> Start: " << start
//                  << " End: " << end << endl;

//             curr_t = end;
//         }
//     }
// };

// class SJF
// {
// public:
//     vector<Task> tasks;

//     void insert(int id, int arrival, int execution)
//     {
//         tasks.push_back({id, arrival, execution});
//     }

//     void run()
//     {
//         // Sort by arrival time
//         sort(tasks.begin(), tasks.end(), [](Task &a, Task &b)
//              { return a.arrival < b.arrival; });

//         priority_queue<Task, vector<Task>, Compare> pq;

//         int n = tasks.size();
//         int i = 0;
//         int curr_t = 0;

//         cout << "\n--- SJF Scheduling ---\n";

//         while (i < n || !pq.empty())
//         {

//             // If no task available → jump time
//             if (pq.empty() && curr_t < tasks[i].arrival)
//             {
//                 curr_t = tasks[i].arrival;
//             }

//             // Push all available tasks into heap
//             while (i < n && tasks[i].arrival <= curr_t)
//             {
//                 pq.push(tasks[i]);
//                 i++;
//             }

//             // Pick shortest job
//             Task t = pq.top();
//             pq.pop();

//             int start = curr_t;
//             int end = start + t.execution;

//             cout << "Task " << t.id
//                  << " -> Start: " << start
//                  << " End: " << end << endl;

//             curr_t = end;
//         }
//     }
// };

// class Priority
// {
// public:
//     vector<Task> tasks;
//     void insert(int id, int arrival, int exe_t, int priority)
//     {
//         tasks.push_back({id, arrival, exe_t, priority});
//     }
//     void run()
//     {
//         sort(tasks.begin(), tasks.end(), [](Task &a, Task &b)
//              { return a.arrival < b.arrival; });
//         priority_queue<Task, vector<Task>, Comapre> pq;
//         int n = tasks.size();
//         int i = 0;
//         int curr_t = 0;

//         cout << "\n--- SJF Scheduling ---\n";

//         while (i < n || !pq.empty())
//         {

//             // If no task available → jump time
//             if (pq.empty() && curr_t < tasks[i].arrival)
//             {
//                 curr_t = tasks[i].arrival;
//             }

//             // Push all available tasks into heap
//             while (i < n && tasks[i].arrival <= curr_t)
//             {
//                 pq.push(tasks[i]);
//                 i++;
//             }

//             // Pick highest priority job
//             Task t = pq.top();
//             pq.pop();

//             int start = curr_t;
//             int end = start + t.execution;

//             cout << "Task " << t.id
//                  << " -> Start: " << start
//                  << " End: " << end << endl;

//             curr_t = end;
//         }
//     }
// };

// class RoundRobin {
// public:
//     vector<Task> tasks;
//     int quantum;

//     RoundRobin(int q) {
//         quantum = q;
//     }

//     void insert(int id, int arrival, int execution) {
//         tasks.push_back({id, arrival, execution, execution});
//     }

//     void run() {
//         // Sort by arrival time
//         sort(tasks.begin(), tasks.end(), [](Task &a, Task &b) {
//             return a.arrival < b.arrival;
//         });

//         queue<Task> q;

//         int n = tasks.size();
//         int i = 0;
//         int curr_t = 0;

//         cout << "\n--- Round Robin Scheduling ---\n";

//         while (i < n || !q.empty()) {

//             // If queue empty → jump to next arrival
//             if (q.empty() && curr_t < tasks[i].arrival) {
//                 curr_t = tasks[i].arrival;
//             }

//             // Push all tasks that have arrived
//             while (i < n && tasks[i].arrival <= curr_t) {
//                 q.push(tasks[i]);
//                 i++;
//             }

//             // Take front task
//             Task t = q.front();
//             q.pop();

//             int start = curr_t;

//             // Run for quantum or remaining time
//             int run_time = min(quantum, t.remaining);

//             curr_t += run_time;
//             t.remaining -= run_time;

//             cout << "Task " << t.id
//                  << " -> Start: " << start
//                  << " End: " << curr_t << endl;

//             // Add new tasks that arrived during execution
//             while (i < n && tasks[i].arrival <= curr_t) {
//                 q.push(tasks[i]);
//                 i++;
//             }

//             // If not finished → push back
//             if (t.remaining > 0) {
//                 q.push(t);
//             }
//         }
//     }
// };
// int main()
// {
//     // All tasks here are non preemptive

//     int n;
//     cout << "Enter number of tasks: ";
//     cin >> n;

//     return 0;
// }

int main(){

    int n;
    cout << "Enter number of tasks: ";
    cin >> n;

    vector<Task> tasks;

    cout << "\nEnter: arrival_time execution_time priority\n";

    for(int i = 0; i < n; i++){
        int arr, exe, pri;
        cin >> arr >> exe >> pri;
        tasks.push_back({i+1, arr, exe, pri, exe});
    }

    cout << "\nChoose Scheduling Algorithm:\n";
    cout << "1. FIFO\n2. SJF\n3. Priority\n4. Round Robin\n";

    int choice;
    cin >> choice;

    Strategy* strategy = nullptr;

    if(choice == 1) strategy = new FIFO();
    else if(choice == 2) strategy = new SJF();
    else if(choice == 3) strategy = new PriorityScheduling();
    else if(choice == 4){
        int q;
        cout << "Enter time quantum: ";
        cin >> q;
        strategy = new RoundRobin(q);
    }
    else{
        cout << "Invalid choice\n";
        return 0;
    }

    Scheduler scheduler(strategy);
    scheduler.execute(tasks);

    delete strategy;

    return 0;
}