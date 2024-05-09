# Important terms in OS 

## 🎂Processes and CPU scheduling 
### 🧑‍🍳Processes 
Process is the execution of a program. \
Create a process: **fork()** \
Execute a process: **execve()** \
Let a process wait: **wait()** \
Terminate a process: **kill()** or **exit()** 

### 🔁Context Switches
- Switching context between two processes
- Take care of the first process before making the second process to work
- Expensive, overhead
- Interrupts
- Transition between kernel-mode and user-mode 

### 🧵Process vs. Threads 

### CPU Scheduling 
Scheduling has deciding as their job. They get to decide which task that may use the resource, i.e., determines order by which requests are serviced, using different ✨algorithms✨.

### ✨Scheduling algorithms
**FIFO** \
This algorithm can be described as fair. The first task/process that gets in is the one that gets to finish their task first. 
- Fair
- Simple
- Can have long finish and waiting time \
**Round Robind** \
This algorithm splits all the tasks up in same size (time based), then they do all the task in FIFO order, but with the splits.
- Better for interactivity
- Fair, but no task is luckier than others \
**Shortest Job First** \
This algorithm choose the shortes task (time based) and do their task first, before doing the other tasks.
- Simple
- Better average times compared to FIFO
- Hard to determine processing requirements
- Potentially huge finishing times and starvation (new shorter jobs arrive)

## 🧠Memory 

## Storage/Disk and File Systems 

## IPC
