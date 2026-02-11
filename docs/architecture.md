# Architecture

## Purpose

    - This distributed system has a central coordinator, who accepts jobs and schedules them amongst the workers within the system, while allowing
    the client to obtain real-time updates via the coordinator tracking job state.
    - The actors in this system include:
        - Coordinator
        - Worker
        - Client

## Components

1. **Coordinator responsibilities:**
    - Accept client requests (submit, status, cancel)
    - Schedule jobs (assign them to workers via scheduler policy)
    - Register workers
    - Track workers (connected / disconnected)
    - Track jobs and state transitions (source of truth)
    - Requeue jobs if worker dies or disconnects mid-run (policy defined below)
    - **Later:** Track heartbeats from workers, indicate as "dead" if necessary

2. **Worker responsibilities:**
    - Register with coordinator
    - Poll for/receive job assignment (depends on implementation)
    - Execute jobs (simulate work for now)
        - **Later:** real work
    - Report current status/result of job to coordinator
    - **Later:** Send heartbeats to coordinator

3. **Client responsibilities:**
    - Submit jobs
    - Query job status
    - Cancel jobs

4. **State each entity owns:**
    - Coordinator:
        - All state visible to clients i.e. (job as QUEUED, RUNNING, FAILED, SUCCEEDED, CANCELED) should be owned by the *coordinator*
            - We need a way to establish "truth"
                - if worker and coordinator both established truth --> client would get mixed feedback
                    - *split-brain*
    - Worker:
        - Local execution state only (what it is currently running)
        - Reports events (assigned, started, finished, failed) ==> coordinator updates official job state
    - Client:
        - None (stateless), but influences job state via submit/cancel/status

## Core Flows

1. Client submits --> Coordinator enqueues (**QUEUED**) --> Worker polls (for work) --> Coordinator assigns + marks **RUNNING** -->
   Worker executes --> Worker reports result (**SUCCEEDED/FAILED/CANCELED**) - --> Coordinator marks terminal state -->
   Client queries status

2. Client submits --> Coordinator enqueues (**QUEUED**) --> Client cancels while queued --> Coordinator marks **CANCELED**
   - ==> job is never assigned to a worker

3. Client submits --> Coordinator assigns + marks **RUNNING** --> Client cancels while running
   - ==> cancellation is **best effort** (later). For v0, cancel guarantee only applies for QUEUED jobs.

4. Client submits --> Coordinator assigns + marks **RUNNING** --> Worker disconnects/dies mid-job
   ==> coordinator requeues job (back to **QUEUED**) and later assigns again (at-least-once semantics)

## Scheduling Policy

1. What policy to start with?
    - FIFO by submit order
        ==> simplicity to start off

2. What "fairness" means in initial phase (if anything)?
    - Does not guarantee fairness in runtime/latency sense (big jobs can block small jobs)
    - FIFO is only "fair" in the sense of submit order

3. What is explicitly not being handled yet? (preemption, priorities)
    - Priority
    - Preemption
    - Aging

## Consistency & Execution Semantics

1. What guarantee is provided initially?
    - A job is executed/ran **AT LEAST** once

2. What does this guarantee imply for duplicate execution/results?
    - Retries may re-run jobs (duplicate execution over time is possible)
    - Coordinator accepts the first terminal result for a job_id (SUCCEEDED/FAILED/CANCELED)
        ==> any later results for the same job_id are ignored and logged
    - This means jobs should be idempotent (or handle duplicates) in early versions

## Concurrency Model

1. Where does concurrency exist?
    - When there are multiple clients
        - Submitting jobs concurrently
        - Cancelling jobs concurrently
            - Maybe even cancelling the same job at the same time
        - GET_STATUS concurrently with other operations
    - When there are multiple workers
        - Polling concurrently (race to retrieve the same queued job)
        - Results arrive as client cancels (race btw RESULT and CANCEL)
        - Worker dies while job is RUNNING (race btw requeue decision and late RESULT)
            - ==> for v0: coordinator decision wins; if job already terminal, late result is ignored

2. How are data races prevented?
    - Shared state protected by mutexes
    - All coordinator state transitions under ONE mutex for atomicity (v0)

3. What operations MUST be atomic?
    - State transitions
    - Worker assignment (pop from queue + mark RUNNING must be one atomic action)
    - Recording results (mark terminal + store metadata)
    - Creating a job and setting its metadata
    - Canceling a queued job (must be consistent with assignment)

## Failure Model

1. What failures can I assume happen?
    - Network failures (disconnects, dropped connections)
    - Worker failure (dies, or network partition)
    - Coordinator dies (SPOF)
        - **LATER:** duplication / persistence / restart semantics

2. What failures does the system detect NOW vs. what is ignored for NOW?
    - NOW:
        - Worker disconnects (connection drop)
    - LATER:
        - Heartbeat-based failure detection (timeouts)
        - Coordinator restart recovery

3. What does the system do when it detects a failure?
    - Worker disconnects while idle
        - ==> mark worker dead (drop from pool of workers)
    - Worker disconnects while executing a job
        - ==> job must eventually leave RUNNING state
        - ==> v0 policy: requeue the job back to QUEUED (at-least-once)

## Observability

1. What do I want to log and why?
    - Status
    - Worker_id of a job (to know who it was assigned to and debug that particular worker if needed)
    - Timestamps (created/started/finished) or elapsed time
    - Whenever we change states
        - So we know when it happens and can catch invalid transitions
    - Log every state transition as an event:
        job_id, old_state, new_state, worker_id, attempt_count, timestamp, priority (LATER)

2. How to debug failure with logs alone?
    - Can see the status of the job
    - Can see which worker the job was assigned to
    - Can see invalid state changes
    - Can see retries (attempt_count) and whether results arrived late and were ignored

## Testing Strategy

- Unit test
    - Protocol parse / message encode-decode roundtrip
    - State transition validation (illegal jumps)
    - FIFO ordering (A then B assigned in order)
- Integration test
    - Start cluster --> submit N jobs --> all succeed within timeout
    - Kill worker mid-job --> job eventually succeeds (at-least-once)
    - Cancel queued job never runs again