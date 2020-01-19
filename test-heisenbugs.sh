#!/usr/bin/env bash

if [ $# -ne 2 ]; then
	echo "usage: ./test-heisenbugs.sh <repetitions> <time_to_wait>"
	exit 1
fi

touch tmp_output.log
trap "rm tmp_output.log" 1 2 3 15

FAIL_COUNT=0
for i in $(seq 1 $1); do
	# first, build the thing, so it doesn't count against the time limit:
	make -j4 diskimage

	# then launch the thing in the background:
	make -j4 run-only | tee tmp_output.log &

	# then, wait for N seconds:
	sleep $2

	# then, check the output to see if we faulted:
	if [ $(grep -i -c "cpu exception" 'tmp_output.log') -ne 0 ]; then
		# ok, we had a fault.
		((FAIL_COUNT++))
		# don't kill the process -- wait for user to kill qemu.
		printf "\n\n\n"
		printf "\033[1m\033[31mtest failed\033[0m ($i/$1) -- quit qemu to continue\n"

		(sleep 5; echo "timeout"; killall qemu-system-x86_64) &
		killerPid=$!

		wait $!
		kill -0 $killerPid && kill $killerPid
	else
		# ok, nothing happened -- kill the process ourselves.
		if kill -0 $!; then
			kill $!
			killall qemu-system-x86_64

			printf "\n"
			printf "\033[1m\033[32mtest passed\033[0m ($i/$1)\n"
		fi
	fi

	rm tmp_output.log
done

echo "failed $FAIL_COUNT out of $1"
