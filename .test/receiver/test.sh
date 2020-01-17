#!/usr/bin/env bash

message_count=${TEST_MESSAGE_COUNT:-10}

attempt_counter=0
max_attempts=${TEST_DURATION:-60}
sent_total=0

echo "Need to ${message_count} messages. Timeout ${max_attempts} sec ..."

until [ ${sent_total} -eq ${message_count} ]; do
    if [ ${attempt_counter} -eq ${max_attempts} ]; then
      exit 1
    fi

    content=$(curl -s -X GET http://iqlogger:8888/stats)
    sent_total=$(jq -r '.iqlogger.outputs.sent_total' <<< "${content}" )

    echo "IQLogger Sent ${sent_total}"

    attempt_counter=$(($attempt_counter+1))
    sleep 1
done

echo "Success receive ${sent_total} messages. Time: ${attempt_counter} sec"


