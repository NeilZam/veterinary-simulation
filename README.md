# Veterinary Clinic Simulator

An event-driven scheduling simulator written in C that models a veterinary clinic operating over an 8-hour workday. The program processes cat patient arrivals from an input file and assigns them to one of two doctors using a queue, while tracking exposure risk using a stack.

---

## How It Works

The simulator runs a minute-by-minute loop across a 480-minute workday. Cats arrive at specific times and enter a shared FIFO waiting queue. Doctor Uno and Doctor Dos each pull from the queue when free. Any cat whose treatment would exceed the 480-minute closing time is rejected. All cats treated by Doctor Dos are pushed onto a LIFO exposure stack and printed in reverse order at the end as an exposure report.

---

## Getting Started

### Prerequisites

- GCC compiler
- Linux or Unix-based terminal (tested on Eustis)

### Compile

```bash
gcc -o vet vet.c
```

### Run

The program reads all patient data from `logs.txt` automatically:

```bash
./vet
```

---

## Input File Format

Each line contains an arrival time, cat name, and treatment duration in minutes. A `-1` signals end of input.

**Example input (logs.txt):**
```
0 Whiskers 30
10 Shadow 60
15 Luna 20
480 TooLate 10
-1
```

**Example output:**
```
Doctor Uno treated Whiskers at 0
Doctor Dos treated Shadow at 10
Doctor Uno treated Luna at 15
Cannot accommodate TooLate
Exposed Cats
Shadow
```

---

## Features

- Minute-by-minute event simulation across a 480-minute clinic day
- FIFO waiting queue shared between two doctors
- Sorted event list built via ordered insertion on arrival time
- Doctor Uno assigned first, Doctor Dos assigned second when both are free
- Cats tracked on a LIFO exposure stack for Doctor Dos patients
- Cats rejected if treatment would exceed closing time
- Full dynamic memory allocation and cleanup with no memory leaks

---

## Data Structures Used

- **Queue (FIFO)** — manages the shared patient waiting line
- **Stack (LIFO)** — tracks cats treated by Doctor Dos for exposure reporting
- **Singly Linked List** — stores and sorts incoming events by arrival time
- **Dynamic Memory Allocation** — all cat records and name strings heap-allocated

---

## Technologies

- C
- GCC
- Command Line

---

## Notes

- Clinic operates from minute 0 to minute 480
- Cats with zero treatment duration are ignored
- Cat names support up to 25 characters
- Input files use simple 8-bit ASCII encoding
- All execution commands have been tested on Eustis

---

## What I Learned

This project deepened my understanding of how queues and stacks model real-world scheduling problems. Implementing the minute-by-minute simulation loop and managing multiple independent data structures simultaneously strengthened my ability to reason about program state across time.
