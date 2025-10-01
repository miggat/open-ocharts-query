#!/bin/bash

PDIR="$(dirname "$0")"
PDIR="$(readlink -f "$PDIR")"

# Allow passing a custom display as argument: ./run-with-xvfb.sh -a 101
if [ "$1" = "-a" ]; then
    shift
    display=$1
    shift
else
    display=99
fi

PLUGIN_DIR="$PDIR"
DATA_DIR="$PDIR/data"
CONFIG_DIR="$PDIR"
PORT="10689"

LOG_FILE="/tmp/ocharts-script.stdout.log"
echo "$(date) - script started" >> "$LOG_FILE"

# Log all output from here
exec >>"$LOG_FILE" 2>&1
set -x

# Ensure environment
export DISPLAY=:$display
export LD_LIBRARY_PATH=/home/pi/.var/app/org.opencpn.OpenCPN/lib

xvfb-run -n "$display" -s "-screen 0 512x512x24" \
  "$PLUGIN_DIR/build/open-ocharts-query" \
  "$PLUGIN_DIR" \
  "$DATA_DIR" \
  "$CONFIG_DIR" \
  "$PORT" \
  -l "$PLUGIN_DIR/logs/provider.log" \
  -u "/home/pi/Charts" \
  -d 1 \
  "$@"
