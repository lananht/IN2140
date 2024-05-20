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

#### Round Robin
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
<img width="808" alt="Screenshot 2024-05-09 at 15 58 38" src="https://github.com/lananht/IN2140/assets/169359150/be188f6c-78a2-4a07-8ebd-364e6e4bb117">



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
<img width="407" alt="Screenshot 2024-05-18 at 18 13 14" src="https://github.com/lananht/IN2140/assets/169359150/958a99cb-fea1-45d4-99ec-467738879e04">



### 🏡Addressing
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

### 🏙️Memory Layout 
#### Code segment 

- Reads  from the program, for example execve()
- Usually read-only
- Can be shared

#### Data segment 

- Global variables
- Here is the heap
- Dynamic allocated memory (malloc)
- Heap grows from low to high address

#### Stack segment 

- Data used during a active program
- Stack grows from hight to low address


### 👨‍🔧Memory Management for Multiprogramming
#### Swapping

- We remove the running process from memory and place it on the disk, then swap with a process from the disk that now is going to run in memory.
- Takes a lot of time!

#### Overlays 

- User decides whats in memory and load overlays during the running task 

#### Segmentation and paging 

- Memory splits up in small pieces
- Pieces in same size (usually)
- Swapping the pieces after own needs between the disk and memory

### 🔪Partitioning 
#### Fixed Partitioning

- Easy to implement
- Support swapping of processes
  
The memory is divided into static partitions at the systems initialization time, the partitions size is made before letting programs use the memory.
##### Equal-sized partitions

- Large programs cannot be executed
- Small programs dont use the entire partition

##### Unequal-sized partition

- Large programs can be loaded at once
- Less internal fragmentation
- One queue or one queue per partition
- Require assignment of jobs to partitions

#### Dynamic Partitioning 
The memory is divided at run-time 

- Partitions are created dynamically
- And gets removed after doing their job

The partiotions is divided while the programs work.
##### Algorithms 

- First fit
- Next fit
- Best fit

#### The Buddy System 
A mix of dynamic partitioning and fixed partitioning. \
Keyword: The power of 2

### Segmentation 
The size for memory is determined by a programmer. A process get dynamic allocated place in the memory. Different lenghts. \
**Pros:** 

- Dynamic
- No intert fragmentation
- Less external fragmentation

**Cons:** 

- Adds a step to the adress translation

### Paging 
A process gets its place in one or more memoryframes, where the size is decided by processes. \
**Pros:**

- No extern fragmentation

**Cons:** 

- Some intern fragmentation (based on framesize)

### Virtual Memory








## Storage/Disk and File Systems 

## IPC
