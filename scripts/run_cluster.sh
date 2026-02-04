#!/usr/bin/env bash
set -euo pipefail

# Always run relative to repo root (so paths are stable)
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

# Kill any leftover processes from previous runs (ignore if none)
pkill -f "$ROOT_DIR/build/src/coordinator/coordinator" 2>/dev/null || true
pkill -f "$ROOT_DIR/build/src/worker/worker" 2>/dev/null || true

cleanup() {
  # Kill only processes started by this script (background jobs)
  jobs -pr | xargs -r kill 2>/dev/null || true
}
trap cleanup EXIT INT TERM

# Start coordinator + workers (log to terminal; keep it minimal)
"$ROOT_DIR/build/src/coordinator/coordinator" &
COORD_PID=$!

sleep 0.2

"$ROOT_DIR/build/src/worker/worker" &
W1_PID=$!

sleep 0.1

"$ROOT_DIR/build/src/worker/worker" &
W2_PID=$!

echo "coordinator pid: $COORD_PID"
echo "worker1 pid:     $W1_PID"
echo "worker2 pid:     $W2_PID"

# Keep script alive as long as coordinator is alive
wait "$COORD_PID"
