#!/bin/bash

# Configuration for the client and server runs
SERVER=localhost
PORT=8000
SEED=1
TOTAL=500
START=1
DIFFICULTY=30000000
DELAY_US=600000
PRIO_LAMBDA=0
SERVER_EXECUTABLE="./extended_server"

# Array of REP_PROB_PERCENT values
REP_PROB_VALUES=(0 20 40 60 80)

# Path to your data file
file="client.log"

# Clear or create the log file
truncate -s 0 "$file"

# Function to calculate statistics for the most recent run
calculate_stats() {
    local difficulty="$1"
    tac "$file" | awk -v difficulty="$difficulty" '
        BEGIN {count = 0; sum = 0; sumsq = 0; started = 0}
        $0 ~ "Running client with DIFFICULTY: " difficulty {exit}
        /.*\] [0-9]+$/ && started {
            count++
            sum += $NF
            sumsq += ($NF)^2
        }
        /Total requests:/ {started = 1}  # Adjust to match the start of relevant data
        END {
            if (count > 0) {
                mean = sum / count
                variance = (sumsq - sum^2 / count) / count
                stddev = sqrt(variance)
                stderr = stddev / sqrt(count)
            } else {
                mean = stddev = stderr = "NA"
            }
            print "Statistics for DIFFICULTY:", difficulty
            print "Mean Waiting Time:", mean
            print "Standard Error:", stderr
            print ""
        }
    ' | tac
    echo ""  # Add an additional empty line for clearer separation
}

# Function to start and stop the server
restart_server() {
    # Start the server in the background
    $SERVER_EXECUTABLE $PORT &
    SERVER_PID=$!

    # Wait a bit to ensure the server is ready (adjust the sleep duration as needed)
    sleep 2

    # Run the client and calculate stats
    echo "Running client with REP_PROB_PERCENT: $REP_PROB_PERCENT" >> "$file"
    ./client $SERVER $PORT $SEED $TOTAL $START $DIFFICULTY $REP_PROB_PERCENT $DELAY_US $PRIO_LAMBDA >> "$file"
    calculate_stats "$REP_PROB_PERCENT" >> "$file"
    calculate_stats "$REP_PROB_PERCENT"

    # Stop the server
    kill -SIGINT $SERVER_PID
    wait $SERVER_PID  # Wait for the server to shut down gracefully
}

# Loop over the REP_PROB_PERCENT values
for REP_PROB_PERCENT in "${REP_PROB_VALUES[@]}"; do
    restart_server
    echo "----------------------------------------" >> "$file"  # Separator line
done
