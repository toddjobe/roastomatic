#!/usr/bin/env bash
# File: start_pio_tmux.sh
# Usage: . ./scripts/start_pio_tmux.sh   (run from your project root)

SESSION="pio"
PROJECT_DIR="$(pwd)/firmware/esp32-roastomatic"

# If session already exists just attach
tmux has-session -t $SESSION 2>/dev/null
if [ $? -eq 0 ]; then
  tmux attach -t $SESSION
  exit 0
fi

# Create new detached session
tmux new-session -d -s $SESSION -c "$PROJECT_DIR" -n code

# Pane 0: code editor
tmux send-keys -t $SESSION:0.0 "nano $PROJECT_DIR/src/main.cpp" C-m

# Split vertically → Pane 1: build/flash
tmux split-window -h -t $SESSION:0 -c "$PROJECT_DIR"
# Assumes you have pio in a virtual environmehnt at ./venv
tmux send-keys -t $SESSION:0.1 ". ../../venv/bin/activate" C-m
tmux send-keys -t $SESSION:0.1 'pio run' C-m

# Split Pane 1 horizontally → Pane 2: serial monitor
tmux split-window -v -t $SESSION:0.1 -c "$PROJECT_DIR"
tmux send-keys -t $SESSION:0.2 ". ../../venv/bin/activate" C-m
tmux send-keys -t $SESSION:0.2 'pio device monitor' C-m

# Create a second window for Git
tmux new-window -t $SESSION:1 -n git -c "./"
tmux send-keys -t $SESSION:1 'git status' C-m

# Attach to session
tmux select-window -t $SESSION:0
tmux attach -t $SESSION
