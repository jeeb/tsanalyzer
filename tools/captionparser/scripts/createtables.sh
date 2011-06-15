#!/bin/bash
if [ -z "$1" ]; then
	echo "usage: $0 \"max\""
	exit 1
fi

LOOPCNT=$1

i=0;
echo -n "char* arr[${LOOPCNT}] = {"

while [ $i -ne $LOOPCNT ]; do
	CUTVAR=$((i*2))
	echo -n ""
#	if [ $i -lt 10 ]; then echo -n "0"; fi
#	echo -n $i
	echo -n "\".\" //"
	echo `dec2hex ${i}`
	i=$((i+1))
	if [ $i -ne $LOOPCNT ]; then echo -n ", "; fi
done
echo "}"
