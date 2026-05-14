from abc import ABC, abstractmethod
import heapq
from collections import deque
from dataclasses import dataclass


@dataclass
class Task:
    id: int
    arrival: int
    execution: int
    priority: int
    remaining: int

    completion_time: int = 0
    turnaround_time: int = 0
    waiting_time: int = 0
    response_time: int = -1
    started: bool = False

class Strategy(ABC):
    @abstractmethod
    def run(self, tasks: list[Task]) -> None:
        pass


class FIFO(Strategy):
    def run(self, tasks: list[Task]) -> list:
        tasks = list(tasks)
        tasks.sort(key=lambda t: t.arrival)

        curr_t = 0
        timeline = []

        for t in tasks:
            if curr_t < t.arrival:
                timeline.append({
                    "task": "IDLE",
                    "start": curr_t,
                    "end": t.arrival
                })
                curr_t = t.arrival

            start = curr_t
            end = start + t.execution

            timeline.append({
                "task": t.id,
                "start": start,
                "end": end
            })

            curr_t = end

        return timeline
    
class SJF(Strategy):
    def run(self, tasks: list[Task]) -> list:
        import heapq

        tasks = list(tasks)
        tasks.sort(key=lambda t: t.arrival)

        pq = []  # (execution, arrival, id, task)
        i = 0
        n = len(tasks)
        curr_t = 0
        timeline = []

        while i < n or pq:

            if not pq and i < n and curr_t < tasks[i].arrival:
                timeline.append({
                    "task": "IDLE",
                    "start": curr_t,
                    "end": tasks[i].arrival
                })
                curr_t = tasks[i].arrival

            while i < n and tasks[i].arrival <= curr_t:
                heapq.heappush(pq, (tasks[i].execution, tasks[i].arrival, tasks[i].id, tasks[i]))
                i += 1

            if pq:
                _, _, _, t = heapq.heappop(pq)

                start = curr_t
                end = start + t.execution

                timeline.append({
                    "task": t.id,
                    "start": start,
                    "end": end
                })

                curr_t = end

        return timeline
    
class PriorityScheduling(Strategy):
    def run(self, tasks: list[Task]) -> list:
        import heapq

        tasks = list(tasks)
        tasks.sort(key=lambda t: t.arrival)

        pq = []  # (priority, arrival, id, task)
        i = 0
        n = len(tasks)
        curr_t = 0
        timeline = []

        while i < n or pq:

            if not pq and i < n and curr_t < tasks[i].arrival:
                timeline.append({
                    "task": "IDLE",
                    "start": curr_t,
                    "end": tasks[i].arrival
                })
                curr_t = tasks[i].arrival

            while i < n and tasks[i].arrival <= curr_t:
                heapq.heappush(pq, (tasks[i].priority, tasks[i].arrival, tasks[i].id, tasks[i]))
                i += 1

            if pq:
                _, _, _, t = heapq.heappop(pq)

                start = curr_t
                end = start + t.execution

                timeline.append({
                    "task": t.id,
                    "start": start,
                    "end": end
                })

                curr_t = end

        return timeline
    
class RoundRobin(Strategy):
    def __init__(self, quantum: int):
        self.quantum = quantum

    def run(self, tasks: list[Task]) -> list:
        from collections import deque

        tasks = list(tasks)
        tasks.sort(key=lambda t: t.arrival)

        for t in tasks:
            t.remaining = t.execution

        q = deque()
        i = 0
        n = len(tasks)
        curr_t = 0
        timeline = []

        while i < n or q:

            if not q and i < n and curr_t < tasks[i].arrival:
                timeline.append({
                    "task": "IDLE",
                    "start": curr_t,
                    "end": tasks[i].arrival
                })
                curr_t = tasks[i].arrival

            while i < n and tasks[i].arrival <= curr_t:
                q.append(tasks[i])
                i += 1

            if q:
                t = q.popleft()

                start = curr_t
                run_time = min(self.quantum, t.remaining)

                curr_t += run_time
                t.remaining -= run_time

                timeline.append({
                    "task": t.id,
                    "start": start,
                    "end": curr_t
                })

                while i < n and tasks[i].arrival <= curr_t:
                    q.append(tasks[i])
                    i += 1

                if t.remaining > 0:
                    q.append(t)

        return timeline

def generate_metrics(tasks, timeline):

    completion = {}

    for block in timeline:
        if block["task"] != "IDLE":
            completion[block["task"]] = block["end"]

    for task in tasks:

        task.completion_time = completion[task.id]

        task.turnaround_time = (
            task.completion_time - task.arrival
        )

        task.waiting_time = (
            task.turnaround_time - task.execution
        )

        first_start = None

        for block in timeline:
            if block["task"] == task.id:
                first_start = block["start"]
                break

        task.response_time = (
            first_start - task.arrival
        )

    avg_wt = sum(t.waiting_time for t in tasks) / len(tasks)

    avg_tat = sum(t.turnaround_time for t in tasks) / len(tasks)

    avg_rt = sum(t.response_time for t in tasks) / len(tasks)

    return {
        "timeline": timeline,

        "metrics": [
            {
                "task": t.id,
                "waiting_time": t.waiting_time,
                "turnaround_time": t.turnaround_time,
                "response_time": t.response_time
            }
            for t in tasks
        ],

        "average_waiting_time": round(avg_wt, 2),

        "average_turnaround_time": round(avg_tat, 2),

        "average_response_time": round(avg_rt, 2)
    }

class Scheduler:
    def __init__(self, strategy):
        self.strategy = strategy

    def execute(self, tasks):

        timeline = self.strategy.run(tasks)

        return generate_metrics(tasks, timeline)



# if __name__ == "__main__":
#     n = int(input("Enter number of tasks: "))
#     tasks = []

#     print("\nEnter: arrival_time execution_time priority")

#     for i in range(n):
#         arr, exe, pri = map(int, input().split())
#         tasks.append(Task(i + 1, arr, exe, pri, exe))

#     print("\nChoose Scheduling Algorithm:")
#     print("1. FIFO\n2. SJF\n3. Priority\n4. Round Robin")

#     choice = int(input())

#     if choice == 1:
#         strategy = FIFO()
#     elif choice == 2:
#         strategy = SJF()
#     elif choice == 3:
#         strategy = PriorityScheduling()
#     elif choice == 4:
#         quantum = int(input("Enter time quantum: "))
#         strategy = RoundRobin(quantum)
#     else:
#         print("Invalid choice")
#         exit()

#     scheduler = Scheduler(strategy)
#     timeline = scheduler.execute(tasks)

#     print("\n--- Execution Timeline ---\n")

#     for block in timeline:
#         if block["task"] == "IDLE":
#             print(f"CPU Idle | Start: {block['start']} | End: {block['end']}")
#         else:
#             print(f"Task {block['task']} | Start: {block['start']} | End: {block['end']}")