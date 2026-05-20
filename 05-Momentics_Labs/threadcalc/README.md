# Threaded Calculator using POSIX Threads & Semaphores

## Overview

This project demonstrates:

* POSIX threads (`pthread`)
* Counting semaphores
* Thread synchronization
* Producer-consumer style design
* Multi-threaded task execution

The program creates multiple worker threads based on operations passed from the command line.

Example:

```bash
./thrdcalc + - /
```

Creates:

* Addition thread
* Subtraction thread
* Division thread

The main thread continuously receives two numbers from the user and distributes the work to all worker threads.

---

# Architecture

## Threads

| Thread            | Responsibility                            |
| ----------------- | ----------------------------------------- |
| Main Thread       | Reads user input and synchronizes workers |
| Worker Thread (+) | Performs addition                         |
| Worker Thread (-) | Performs subtraction                      |
| Worker Thread (/) | Performs division                         |

---

# Synchronization Model

The system uses:

## 1. Start Semaphores

Each worker thread has its own semaphore:

```c
sem_start[i]
```

Purpose:

* Blocks worker thread until new operands are available.

---

## 2. Done Semaphore

Shared counting semaphore:

```c
sem_done
```

Purpose:

* Each worker posts once after completing its operation.
* Main thread waits until all workers finish.

---

# Execution Flow

```text
                +------------------+
                |    Main Thread   |
                | Reads user input |
                +------------------+
                         |
                         |
                  Write shared data
                         |
            +------------+------------+
            |            |            |
            v            v            v
      sem_post()   sem_post()   sem_post()
            |            |            |
            v            v            v
       Worker +      Worker -     Worker /
       sem_wait      sem_wait     sem_wait
       Calculate     Calculate    Calculate
       Print         Print        Print
            |            |            |
            +------ sem_post(done) ---+
                         |
                         v
                 Main waits 3 times
```

---

# Shared Data

All worker threads read the same operands:

```c
shared_data.a
shared_data.b
```

The main thread updates these values before waking workers.

---

# Semaphore Behavior

## Worker Synchronization

Workers initially block:

```c
sem_wait(&sem_start[i]);
```

Main thread wakes them:

```c
sem_post(&sem_start[i]);
```

---

## Completion Synchronization

Each worker signals completion:

```c
sem_post(&sem_done);
```

Main thread waits for all workers:

```c
sem_wait(&sem_done);
sem_wait(&sem_done);
sem_wait(&sem_done);
```

---

# Counting Semaphore Concept

The semaphore internally contains a counter.

Example timeline:

```text
Initial:
sem_done = 0

Worker + finishes:
sem_done = 1

Worker - finishes:
sem_done = 2

Worker / finishes:
sem_done = 3

Main thread consumes:
3 -> 2 -> 1 -> 0
```

This naturally represents:

```text
Number of completed worker threads
```

---

# Program Flow

## Startup Phase

```text
1. Main creates worker threads
2. Each thread receives its operation
3. Workers block on semaphores
```

---

## Runtime Phase

```text
1. User enters two numbers
2. Main stores operands
3. Main wakes all workers
4. Workers execute concurrently
5. Workers print results
6. Workers signal completion
7. Main waits for all workers
8. Repeat
```

---

# Example Run

```text
$ ./thrdcalc + / -

Enter two numbers: 7 24
[THREAD +] 7 + 24 = 31
[THREAD /] 7 / 24 = 0.29
[THREAD -] 7 - 24 = -17
---- All operations completed ----

Enter two numbers: 63 6
[THREAD +] 63 + 6 = 69
[THREAD -] 63 - 6 = 57
[THREAD /] 63 / 6 = 10.50
---- All operations completed ----

Enter two numbers: 4 0
[THREAD +] 4 + 0 = 4
[THREAD /] Division by zero!
[THREAD -] 4 - 0 = 4
---- All operations completed ----

Enter two numbers: 
```

---

# Important Concepts Demonstrated

## POSIX Threads

Used to create concurrent execution paths:

```c
pthread_create()
```

---

## Blocking Synchronization

Threads sleep efficiently while waiting:

```c
sem_wait()
```

No busy waiting occurs.

---

## Event Signaling

Threads notify completion:

```c
sem_post()
```

---

## Producer-Consumer Pattern

| Role      | Description                             |
| --------- | --------------------------------------- |
| Producer  | Main thread produces operands           |
| Consumers | Worker threads consume and process data |

---

# Advantages of This Design

* Clean synchronization
* No race conditions
* No CPU waste
* Scalable architecture
* Deterministic behavior
* Reusable worker threads

---

# Compilation

```bash
gcc thrdcalc.c -o thrdcalc -pthread
```

---

# Key Takeaways

This project demonstrates a classic realtime/threading synchronization model commonly used in:

* Embedded systems
* Realtime operating systems
* QNX applications
* Worker pools
* Event-driven systems
* Producer-consumer architectures
