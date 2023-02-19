#!/bin/bash
# echo "some text" > /dev/udp/127.0.0.1/9999
(while :
do
echo "456" | nc -u -w0 localhost  9999
done
)

