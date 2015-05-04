#!/bin/bash
echo -e "\n**************************************************"
echo "      TESTS for project 2 in course "
echo -e "   Computer communication and networkd\n\n"
read lowerPort upperPort < /proc/sys/net/ipv4/ip_local_port_range
for (( port = lowerPort ; port <= upperPort ; port++ )); do
	echo "Testing availability of port $port."
    RESULT=$(netstat -lnu | grep -w "udp.*:$port")
    if [ -z "${RESULT}" ]; then
    	echo "Port $port is available. Starting server."
    	break
    else 
    	echo "Port $port is unavailable."
    fi
done

RETURN=0
./server -p $port &
RETURN=$?
SERVER_PID=$!
if [ "$RETURN" -ne 0 ]; then
	echo "SERVER ended with error (errno $RETURN). Message: $OUTPUT."
	exit 0
fi
echo "Server started on port $port. Process PID id $SERVER_PID."
echo -e "\n**************************************************"
echo "TEST 01: client help message:"
ecgi "./client --help"
RETURN=0
OUTPUT="$(./client --help)"
RETURN=$?
if [ "$RETURN" -ne 0 ]; then
	printf "SERVER ended with error (errno $RETURN). Message: $OUTPUT."
else
	printf "$OUTPUT\n\n"
fi
echo -e "\n**************************************************"
echo "TEST 02: client with 10 seconds timeout and 1 secnd interval:" 
echo "./client 127.0.0.1 -p $port -t 10 -i 1"
RETURN=0
./client 127.0.0.1 -p $port -t 10 -i 1
RETURN=$?
if [ "$RETURN" -ne 0 ]; then
	printf "SERVER ended with error (errno $RETURN). Message: $OUTPUT."
else
	echo -e "\nResults:"
	cat ipkperf-127.0.0.1-100-10
fi
rm ipkperf-*
kill $SERVER_PID