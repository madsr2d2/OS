#!/bin/bash

# Path to your data file
file="client.log"

# Use awk to extract the last number (waiting time) from each line and calculate the mean and standard error
awk '
    /.*\] [0-9]+$/ {
        count++
        sum += $NF
        sumsq += ($NF)^2
    }
    END {
        mean = sum / count
        stddev = sqrt(sumsq/count - mean^2)
        stderr = stddev / sqrt(count)
        print "Mean Waiting Time:", mean
        print "Standard Error:", stderr
    }
' "$file"
