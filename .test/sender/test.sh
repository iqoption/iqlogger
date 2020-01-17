#!/usr/bin/env bash

message_count=${TEST_MESSAGE_COUNT:-10}

echo "Need to ${message_count} messages..."

let "count = message_count / 2"

for (( i=1; i <= ${count}; i++ ))
do
    echo -n '{"short_message":"A short message from UDP","team":"foo","service":"bar"}' | nc -w 1 -u iqlogger 12201
    echo -n -e '{"short_message": "A short message from TCP", "team": "foo", "service": "bar" }'"\0" | nc -w 1 iqlogger 12202
done

echo "Success send all ${message_count} messages... Start wait!"

while :;do
   sleep 1
done




