#!/bin/bash

# Configuration for the client and server runs
SERVER=localhost
PORT=9212
SEED=1
TOTAL=200
START=1
DELAY_US=600000
PRIO_LAMBDA=0
REP_PROB_VALUES=0
SERVER_EXECUTABLE="./single_thread_server"

# Array of difficulty values
DIFFICULTIES=(10 100 1000 10000 100000 1000000 30000000)

# Path to your data file
file="client.log"

# Clear or create the log file
truncate -s 0 "$file"

# Function to calculate statistics for the most recent run
calculate_stats() {
    local difficulty="$1"
    tac "$file" | awk -v difficulty="$difficulty" '
        BEGIN {count = 0; sum = 0; sumsq = 0; started = 0}
        $0 ~ "Running client with DIFFICULTIES: " difficulty {exit}
        /.*\] [0-9]+$/ && started {
            count++
            sum += $NF
            sumsq += ($NF)^2
        }
        /Results:/ {started = 1}
        END {
            if (count > 0) {
                mean = sum / count
                stddev = sqrt((sumsq - sum^2 / count) / count)
                stderr = stddev / sqrt(count)
            } else {
                mean = stddev = stderr = "NA"
            }
            print "Statistics for DIFFICULTIES:", difficulty
            print "Mean Waiting Time:", mean
            print "Standard Error:", stderr
            print ""
        }
    ' | tac
    echo ""  # Add an additional empty line for clearer separation
}




# Function to start and stop the server
restart_server() {
    local difficulty=$1

    # Start the server in the background
    $SERVER_EXECUTABLE $PORT &
    SERVER_PID=$!

    # Wait a bit to ensure the server is ready (adjust the sleep duration as needed)
    sleep 2

    # Run the client and calculate stats
    echo "Running client with DIFFICULTY: $difficulty" >> "$file"
    ./client $SERVER $PORT $SEED $TOTAL $START $difficulty $REP_PROB_VALUES $DELAY_US $PRIO_LAMBDA >> "$file"
    calculate_stats "$difficulty" >> "$file"
    calculate_stats "$difficulty"

    # Stop the server
    kill -SIGINT $SERVER_PID
    wait $SERVER_PID  # Wait for the server to shut down gracefully
}

# Loop over the difficulty values
for difficulty in "${DIFFICULTIES[@]}"; do
    restart_server $difficulty
    echo "----------------------------------------" >> "$file"  # Separator line
done
