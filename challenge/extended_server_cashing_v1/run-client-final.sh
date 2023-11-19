#!/bin/sh
#
# This is the configuration of the final run.
# The seed value will be a non-zero value that will not be disclosed.
#


SERVER=localhost
PORT=9000
SEED=0
TOTAL=10
START=0
DIFFICULTY=30000000
REP_PROB_PERCENT=50
#DELAY_US=600000
DELAY_US=0

#PRIO_LAMBDA=1.5
PRIO_LAMBDA=0

./client $SERVER $PORT $SEED $TOTAL $START $DIFFICULTY $REP_PROB_PERCENT $DELAY_US $PRIO_LAMBDA 
#./client $SERVER $PORT $SEED $TOTAL $START $DIFFICULTY $REP_PROB_PERCENT $DELAY_US $PRIO_LAMBDA > client.log
#./client $SERVER $PORT $SEED $TOTAL $START $DIFFICULTY $REP_PROB_PERCENT $DELAY_US $PRIO_LAMBDA >> client.log
#./client $SERVER $PORT $SEED $TOTAL $START $DIFFICULTY $REP_PROB_PERCENT $DELAY_US $PRIO_LAMBDA >> client.log
#./client $SERVER $PORT $SEED $TOTAL $START $DIFFICULTY $REP_PROB_PERCENT $DELAY_US $PRIO_LAMBDA >> client.log
#./client $SERVER $PORT $SEED $TOTAL $START $DIFFICULTY $REP_PROB_PERCENT $DELAY_US $PRIO_LAMBDA >> client.log