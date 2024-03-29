#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <cassert>

using namespace std;

#define MAXBUFSIZE 1024
#define BCAST_PORT 5800

class UdpClient {
	public:
		// public constants and variables
		int socketID = 0;
		char receiveData[MAXBUFSIZE] = {0};

		// constructor
		UdpClient(int port);

		// public methods
		int ReceivePacket();
		int ReceiveInt();
		int ReceiveCoords(int &x, int &y);
};

UdpClient::UdpClient(int port = BCAST_PORT){
	int srcaddrSize;
	struct sockaddr_in localUdp;
	const int bCastPort = port;
	struct sockaddr_in serv_addr;

	//setup udp socket
	if ((socketID = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("UDP socket failed");
	}

	if (setsockopt(socketID, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &srcaddrSize, sizeof(srcaddrSize))) {
		perror("setsockopt");
	}

	memset(&localUdp, 0, sizeof(serv_addr));
	localUdp.sin_family = AF_INET;
	localUdp.sin_addr.s_addr = INADDR_ANY;
	localUdp.sin_port = htons( bCastPort );

	if (bind(socketID, (struct sockaddr *)&localUdp,sizeof(localUdp))) {
		perror("UDP bind to port failed");
	}

};

int UdpClient::ReceivePacket(){
	struct sockaddr_in bCastRecv;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	ssize_t packetSize;

	std::cout << "Waiting for broadcast..." <<std::endl;
	memset(&bCastRecv, 0, sizeof(bCastRecv));
	memset(receiveData, 0, MAXBUFSIZE);
	packetSize = recvfrom(socketID, receiveData, MAXBUFSIZE, 0, (struct sockaddr *) &bCastRecv, &addrlen);

	cout<<"Packet Size: "<< packetSize <<endl;
	cout<<"Packet: "<< receiveData <<endl; 
    
	return packetSize;
}

int UdpClient::ReceiveInt(){
	int dataLen = ReceivePacket();
    
	if (dataLen != -1) {
		return atoi(receiveData);
	} else {
		return -1;
	}
}

int UdpClient::ReceiveCoords(int &x, int &y) {
	int dataLen = ReceivePacket();

	if (dataLen != -1) {
		std::string xtoken, ytoken;
		std::string delimiter = ":";
		std::string msg = string(receiveData); 
		xtoken = msg.substr(0, msg.find(delimiter)); 
		ytoken = msg.erase(0, msg.find(delimiter) + delimiter.length()); 
		x = atoi(xtoken.c_str()); 
		y = atoi(ytoken.c_str()); 
		return dataLen;
	} else {
		return -1;
	}

}

int main() {
	int msg_len; 
	int x, y;

	UdpClient *udp_client = new UdpClient();

	while (true) {
		msg_len = udp_client->ReceiveCoords(x, y);
		cout << "msg_len: " << msg_len << endl;
		cout << "x: " << x << " y: " << y << endl;
	}

	return 0;
}
