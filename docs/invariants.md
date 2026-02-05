1) A job in RUNNING state is assigned to **AT MOST** one worker (*single owner for RUNNING job*)
    - **Why this matters**
        - If we have a job running on multiple workers, one might report it as complete while the other reports it as incomplete ==> consistency issue
    - **What breaks if violated**
        - Duplicate execution causes double side-effects
        - Coordinator may accept conflicting results (b/c job being ran on two different workers)
            ==> client sees nondeterministic results

2) Cancelling a job while **QUEUED** prevents it from running later (CANNOT be assigned after cancel)
    - **Why this matters**
        - Cancellation is a correctness guarantee to the client that a cancelled queued job is actually cancelled
    - **What breaks if violated**
        - API contract breaks (job may run despite cancellation)
        - Race between cancel and assignment becomes undefined / inconsistent

3) If a worker is dead, jobs assigned to it do NOT remain in RUNNING indefinitely
    - **Why this matters**
        - Jobs will never complete and will always appear as having work being done
            ==> must have some timeout/heartbeat/connection-drop mechanism to requeue or fail the job
    - **What breaks if violated**
        - Failure handling (worker failures) does not work
        - Client can wait forever / job stays RUNNING forever

4) State transitions are valid (i.e. no FAILED --> SUCCEEDED)
    - **Why this matters**
        - If invalid state transitions, clients can observe impossible-to-reach states
        - Retries/cleanup logic becomes unreliable/unsafe
        - Metrics lose meaning --> client receives unreliable status updates
    - **What breaks if violated**
        - Model of the entire system (status becomes untrustworthy)

# Execution Semantics:

5) Early system is **AT-LEAST-ONCE** (retries may re-run jobs)
    - Every job is guaranteed to execute AT LEAST one time (especially on worker failure, or result loss)
    - Duplicate execution can happen because of this guarantee (not necessarily concurrently)
    - **Why this matters**
        - Invariant (1) prevents concurrent duplicates (same job RUNNING on 2 workers at once)
            ==> but at-least-once still allows sequential duplicates over time (job runs again after requeue)
        - Coordinator must define how it treats late / duplicate results
            ==> rule: accept first terminal result; ignore and log any later results for that job_id
    - **What breaks if violated**
        - Jobs could be dropped permanently on first failure (worker dies mid-job and coordinator never retries)
        - Client may wait forever or never see completion upon failure if no retry / no timeout mechanism exists