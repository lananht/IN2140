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
#### FIFO 
This algorithm can be described as fair. The first task/process that gets in is the one that gets to finish their task first. 
- Fair
- Simple
- Can have long finish and waiting time 

#### Round Robind 
This algorithm splits all the tasks up in same size (time based), then they do all the task in FIFO order, but with the splits.
- Better for interactivity
- Fair, but no task is luckier than others 

#### Shortest Job First 
This algorithm choose the shortes task (time based) and do their task first, before doing the other tasks.
- Simple
- Better average times compared to FIFO
- Hard to determine processing requirements
- Potentially huge finishing times and starvation (new shorter jobs arrive)

### Priority & Preemption illustrated
(Sett inn bilde her) 


### More algorithms 
#### Rate Monotonic (RM) Scheduling

#### Earliest Deadline First (EDF)

## 🧠Memory 
### 💪Memory Management 
Goal: managing the systems memory resources. 
- Allocate space to process
- Protect the memory regions
- Provide a virtual view of memory giving the impression of having more bytes than availability
- Controll different levels of memory based on a hierarchy

### 🏔️Memory Hierarchies
(Sett inn bilde her) 

### Addressing
Every posistion in memory has their own address, just like all houses has their own address. 
#### Absolute addressing 
- Often used by hardware
- Have reserved memory regions
- Read data by referencing the byte numbers in memory

We cant predict which addresses a programs gets and we cant use addresses directly. 
#### Relative addressing 
- Addressing dynamic
- Base addresses
- Relative addresses
- Dynamic address translation

### Memory Layout 








## Storage/Disk and File Systems 

## IPC
