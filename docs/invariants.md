1) A job in RUNNING state is assigned to **AT MOST** one worker
2) Cancelling a queued job prevents it from running later (CANNOT requeue)
3) If a worker is dead, jobs assigned to it do NOT remain in RUNNING indefinitely
4) State transitions are valid (i.e. no FAILED --> SUCCEEDED)

# Execution Semantics:
5) Early system is **AT-LEAST-ONCE** (retries may re-run jobs)
    - Every job is guaranteed to execute AT LEAST one time (especially on "worker" failure (after execution but before result is recorded))
    - Duplicate jobs can happen because of this guarantee