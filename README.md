# Synchronization with POSIX Threads

This repository contains solutions to the Operating Systems homework focused on thread synchronization using POSIX threads, mutexes, spinlocks, and semaphores. The assignment consists of five problems, each demonstrating a fundamental synchronization technique. All implementations are written in C and tested on Ubuntu Server.

The project is organized into clearly separated folders for source files, reports, and execution output screenshots.

---

## Project Structure

```
Synchronization_in_C_with_Pthreads/
├── README.md
│
├── screenshots/
│   ├── problem1_none_output.png
│   ├── problem1_mutex_output.png
│   ├── problem1_spin_output.png
│   │
│   ├── problem2_mutex_short.png
│   ├── problem2_mutex_long.png
│   ├── problem2_spin_short.png
│   ├── problem2_spin_long.png
│   │
│   ├── problem3_producer_consumer.png
│   │
│   ├── problem4_ordered_abc.png
│   │
│   └── problem5_printers_pool.png
│
├── reports/
│   ├── problem1_report.md
│   ├── problem2_report.md
│   ├── problem3_report.md
│   ├── problem4_report.md
│   └── problem5_report.md
│
└── src/
    ├── problem1_counter.c
    ├── problem2_bank.c
    ├── problem3_bounded_buffer.c
    ├── problem4_ordered_abc.c
    └── problem5_printer_pool.c
```

---

## Requirements

* Ubuntu Server or any Linux environment supporting POSIX threads
* GCC compiler
* pthread library
* POSIX semaphores (`<semaphore.h>`)

Compile using GCC with pthread support:

```
gcc -pthread source_file.c -o executable
```

---

## Problem Summaries

### Problem 1: Broken Counter (Race Conditions, Mutex, Spinlock)

A multi-threaded counter is incremented by multiple threads. Three versions are implemented:

1. No synchronization
2. Mutex-protected increment
3. Spinlock-protected increment

The program prints the expected and actual counter values. The purpose is to demonstrate race conditions and how mutual exclusion fixes them.

### Problem 2: Bank Account (Mutex vs Spinlock, Short vs Long Critical Sections)

Simulates deposit and withdrawal threads operating on a shared bank balance. The program supports two synchronization modes (mutex and spinlock) and two critical-section modes (short and long using `usleep`). Execution time is measured for comparison.

### Problem 3: Bounded Buffer (Producer–Consumer)

Implements a classic producer–consumer system with a fixed-size circular buffer. Synchronization uses two semaphores (full and empty slots) and a mutex protecting buffer access.

### Problem 4: Ordered Printing (A → B → C)

Three threads print in a strict order using only semaphores to enforce sequencing. No busy-waiting or sleep-based ordering is used.

### Problem 5: Printer Pool (Counting Semaphore)

Simulates a pool of identical printers using a counting semaphore. Only a limited number of threads can enter the printing section simultaneously.

---

## How to Compile

Compile each problem individually from the `src/` folder. Examples:

```
gcc -pthread src/problem1_counter.c -o problem1_counter
./problem1_counter

gcc -pthread src/problem2_bank.c -o problem2_bank
./problem2_bank mutex short
```

If a Makefile is added later, instructions will be updated accordingly.

---

## Reports

Each problem has a dedicated Markdown report in the `reports/` directory. Reports contain short explanations required by the assignment along with execution screenshots located in the `screenshots/` folder.

---

## Notes

* All programs are tested on Ubuntu Server.
* Only POSIX-compliant synchronization primitives are used.
* Code is formatted in a consistent and readable style.
* Screenshot filenames match the program outputs for easier report referencing.

---

## Author

Assignment completed as part of an Operating Systems course. The repository contains original implementations demonstrating understanding of synchronization and multi-threaded programming fundamentals.
