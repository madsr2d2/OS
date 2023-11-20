#!/bin/sh
#
# This is the configuration of the final run.
# The seed value will be a non-zero value that will not be disclosed.
#


SERVER=localhost
PORT=8000
SEED=1
TOTAL=100
START=1
DIFFICULTY=30000000
REP_PROB_PERCENT=50
DELAY_US=600000
PRIO_LAMBDA=0

#./client $SERVER $PORT $SEED $TOTAL $START $DIFFICULTY $REP_PROB_PERCENT $DELAY_US $PRIO_LAMBDA 
./client $SERVER $PORT $SEED $TOTAL $START $DIFFICULTY $REP_PROB_PERCENT $DELAY_US $PRIO_LAMBDA >> client.log