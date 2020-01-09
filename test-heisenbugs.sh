#!/usr/bin/env bash

if [ $# -ne 2 ]; then
	echo "usage: ./test-heisenbugs.sh <repetitions> <time_to_wait>"
	exit 1
fi

FAIL_COUNT=0
for i in $(seq 1 $1); do
	# first, launch the thing in the background:
	make -j4 </dev/null &

	# then, wait for N seconds:
	sleep $2

	# then, check the output to see if we faulted:
	if [ $(grep "cpu exception" -c 'build/serialout.log') -ne 0 ]; then
		# ok, we had a fault.
		((FAIL_COUNT++))
		# don't kill the process -- wait for user to kill qemu.
		printf "\n\n\n"
		printf "\033[1m\033[31mtest failed\033[0m -- quit qemu to continue\n"
		wait $!
	else
		# ok, nothing happened -- kill the process ourselves.
		if kill -0 $!; then
			kill $!
			printf "\n"
			printf "\033[1m\033[32mtest passed\033[0m\n"
		fi
	fi
done

echo "failed $FAIL_COUNT out of $1"
