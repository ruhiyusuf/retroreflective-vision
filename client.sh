#!/bin/bash
echo "Press [CTRL+C] to stop..."

cam_input = 0
CLIENT_IP = localhost
while : 
do
	# echo "$(date +'%S')" | nc -u -w0 localhost 9999
	# cam_input = `echo $(read < /dev/udp/127.0.0.1/9999)`
	# nc -u -l -k 192.168.0.111  9999
	# cam_input = nc -u -l 8000

	# cam_input = netcat -ulzp 8125
	echo $cam_input

	if ["$cam_input" == 0]; then

		echo "cam input 0"

		gst-launch-1.0 -e nvarguscamerasrc sensor-id=0 ! 'video/x-raw(memory:NVMM),	width=(int)640, height=(int)480, format=(string)NV12, framerate=(fraction)60/1' ! nvv4l2h265enc bitrate=4000000 ! h265parse ! rtph265pay mtu=1400 ! udpsink host=CLIENT_IP port=15000 sync=false async=false
	
	elif ["$cam_input" == 1]; then

		echo "cam input 1"

		gst-launch-1.0 -e nvarguscamerasrc sensor-id=1 ! 'video/x-raw(memory:NVMM),	width=(int)640, height=(int)480, format=(string)NV12, framerate=(fraction)60/1' ! nvv4l2h265enc bitrate=4000000 ! h265parse ! rtph265pay mtu=1400 ! udpsink host=CLIENT_IP port=15000 sync=false async=false

	fi
sleep 1 
done
