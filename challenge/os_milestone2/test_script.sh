#!/bin/bash

# Function to display the correct usage of the script to the user.
usage() {
    echo "Usage: $0 <client_arg_number> <start> <stop> <number of steps> <server_port_1> <number_of_runs> [server_port_2]"
    exit 1
}

# Function to run the client program with the provided arguments.
run_client() {
    local port=$1
    local arg_value=$2
    # Construct the client arguments string.
    
    # This is the configuration of the final run.
    SERVER=localhost
    SEED=5041
    TOTAL=100
    START=0
    DIFFICULTY=300
    REP_PROB_PERCENT=20
    DELAY_US=600000
    PRIO_LAMBDA=1.5
    
    # Default client arguments.
    CLIENT_ARGS="localhost $port $SEED $TOTAL $START $DIFFICULTY $REP_PROB_PERCENT $DELAY_US $PRIO_LAMBDA"

    # Update the client argument based on the given value.
    CLIENT_ARGS=$(echo "$CLIENT_ARGS" | awk -v num="$ARG_NUMBER" -v value="$arg_value" '{$num=value; print}')
    # Execute the client program with the constructed arguments.
    ./client $CLIENT_ARGS | tail -n 1
}

# Function to calculate statistics (average, standard deviation, and 95% CI) from the provided data.
calculate_stats() {
    local data=("$@")
    local length=${#data[@]}
    local sum=0
    local sumsq=0
    # Calculate the sum and sum of squares for the data.
    for val in "${data[@]}"; do
        sum=$(bc <<< "$sum + $val")
        sumsq=$(bc <<< "$sumsq + $val * $val")
    done
    # Calculate average and standard deviation.
    local avg=$(bc <<< "scale=2; $sum / $length")
    local variance=$(bc <<< "scale=2; ($sumsq - ($sum * $sum / $length)) / ($length - 1)")
    local sd=$(bc <<< "scale=2; sqrt($variance)")
    # Calculate the 95% confidence interval using z-score for large sample sizes (1.96 for 95% CI).
    local se=$(bc <<< "scale=2; $sd / sqrt($length)") # Standard error
    local ci_lower=$(bc <<< "scale=2; $avg - (1.96 * $se)")
    local ci_upper=$(bc <<< "scale=2; $avg + (1.96 * $se)")
    
    # Returning average, standard deviation, lower and upper CI bounds.
    echo $avg $se $ci_lower $ci_upper
}

# Validate if the correct number of arguments are provided.
if [ "$#" -lt 5 ]; then
    usage
fi

# Set global variables based on user-provided arguments.
ARG_NUMBER=$1
ARG_START=$2
ARG_STOP=$3
STEPS=$4
SERVER_PORT1=$5
NUM_RUNS=$6  # Assign the 6th argument to NUM_RUNS

# If a 7th argument is provided, it's assumed to be SERVER_PORT2
SERVER_PORT2=${7:-} # Assign the 7th argument to SERVER_PORT2 if provided, otherwise default to empty


# Calculate the step increment based on the range and number of steps.
if [ $STEPS -eq 0 ]; then
    STEP=0
    STEPS=1
else
STEP=$(( ($ARG_STOP - $ARG_START) / ($STEPS-1) ))
fi


# Print header
printf "%-10s %-10s %-10s %-15s %-15s\n" "Arg Value" "Stat" "Average" "Std Error" "95% CI"
printf "%s\n" "--------------------------------------------------------------------------------"


# Main loop to execute the runs and calculate statistics.
for (( i=0; i<$STEPS; i++ )); do
    ARG_VALUE=$(( ARG_START + i*STEP ))
    VAL1_1_RUNS=()
    VAL2_1_RUNS=()
    VAL1_2_RUNS=()
    VAL2_2_RUNS=()

    # Execute the client program multiple times to gather data.
    for (( run=1; run<=$NUM_RUNS; run++ )); do
        # Execute the client program for the first server.
        IFS=' ' read -ra RESULT1 <<< "$(run_client "$SERVER_PORT1" "$ARG_VALUE")"
        VAL1_1_RUNS+=("${RESULT1[1]}")
        VAL2_1_RUNS+=("${RESULT1[2]}")

        # If a second server port is provided, execute for that server as well.
        if [ ! -z "$SERVER_PORT2" ]; then
            IFS=' ' read -ra RESULT2 <<< "$(run_client "$SERVER_PORT2" "$ARG_VALUE")"
            VAL1_2_RUNS+=("${RESULT2[1]}")
            VAL2_2_RUNS+=("${RESULT2[2]}")
        fi
    done

    # Calculate and print statistics for the first server.
    IFS=' ' read -ra STATS1_1 <<< "$(calculate_stats "${VAL1_1_RUNS[@]}")"
    IFS=' ' read -ra STATS2_1 <<< "$(calculate_stats "${VAL2_1_RUNS[@]}")"
   
   
    # print if only one server port is provided
    if [ -z "$SERVER_PORT2" ]; then
    printf "%-10d %-10s %-10.1f %-15.1f [%-6.1f, %-6.1f]\n" "$ARG_VALUE" "Stat1" "${STATS1_1[0]}" "${STATS1_1[1]}" "${STATS1_1[2]}" "${STATS1_1[3]}"
    printf "%-10s %-10s %-10.1f %-15.1f [%-6.1f, %-6.1f]\n" "" "Stat2" "${STATS2_1[0]}" "${STATS2_1[1]}" "${STATS2_1[2]}" "${STATS2_1[3]}"
        printf "%s\n" "--------------------------------------------------------------------------------"
        continue
    fi


    # If a second server port is provided, calculate and print comparative statistics.
    if [ ! -z "$SERVER_PORT2" ]; then
        IFS=' ' read -ra STATS1_2 <<< "$(calculate_stats "${VAL1_2_RUNS[@]}")"
        IFS=' ' read -ra STATS2_2 <<< "$(calculate_stats "${VAL2_2_RUNS[@]}")"

        # Calculate the average differences.
        AVG_DIFF1=$(bc <<< "scale=2; ${STATS1_1[0]} - ${STATS1_2[0]}")
        AVG_DIFF2=$(bc <<< "scale=2; ${STATS2_1[0]} - ${STATS2_2[0]}")

        # Calculate the standard error of the differences.
        SE_DIFF1=$(bc <<< "scale=2; sqrt(${STATS1_1[1]}*${STATS1_1[1]} / $NUM_RUNS + ${STATS1_2[1]}*${STATS1_2[1]} / $NUM_RUNS)")
        SE_DIFF2=$(bc <<< "scale=2; sqrt(${STATS2_1[1]}*${STATS2_1[1]} / $NUM_RUNS + ${STATS2_2[1]}*${STATS2_2[1]} / $NUM_RUNS)")
        # Calculate the 95% confidence intervals for the average differences.
        CI_LOWER_DIFF1=$(bc <<< "scale=2; $AVG_DIFF1 - (1.96 * $SE_DIFF1)")
        CI_UPPER_DIFF1=$(bc <<< "scale=2; $AVG_DIFF1 + (1.96 * $SE_DIFF1)")
        CI_LOWER_DIFF2=$(bc <<< "scale=2; $AVG_DIFF2 - (1.96 * $SE_DIFF2)")
        CI_UPPER_DIFF2=$(bc <<< "scale=2; $AVG_DIFF2 + (1.96 * $SE_DIFF2)")

        # Print formatted comparative results.
        printf "%-10d %-10s %-10.1f %-15.1f [%-6.1f, %-6.1f]\n" "$ARG_VALUE" "Stat1-Diff" $AVG_DIFF1 $SE_DIFF1 $CI_LOWER_DIFF1 $CI_UPPER_DIFF1
        printf "%-10s %-10s %-10.1f %-15.1f [%-6.1f, %-6.1f]\n" "" "Stat2-Diff" $AVG_DIFF2 $SE_DIFF2 $CI_LOWER_DIFF2 $CI_UPPER_DIFF2
    fi
    printf "%s\n" "--------------------------------------------------------------------------------"
done
