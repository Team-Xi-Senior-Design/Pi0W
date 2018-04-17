/*
 * Author: MSOE Team Xi Senior Design 2017
 * Date: 12/2017
 * Description: This file contains all the function prototypes for the Bluetooth Connection on the Pi0W side
 */

#include "Bluetooth_Pi3.h"
#include "Global.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Audio.h"
#include <pthread.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/sco.h>

//static char* BluetoothAddr = "B8:27:EB:EF:25:F4";
static char* BluetoothAddr = "B8:27:EB:DF:93:BE";
#define CHANNEL_NUMBER 1
static int sock;
pthread_mutex_t bluetoothLock;
struct timeval timer = {5,0};
static int scoSock, scoClient;

/*
 * Description:
 * @param:
 * @return:
 */
int getAudio(packet_t* receivedAudio, int bufSize){
	fd_set socketReadSet;
	FD_ZERO(&socketReadSet);
	FD_SET(sock, &socketReadSet);
	if((select(sock+1, &socketReadSet, 0, 0, &timer)) < 0){
		perror("Failed to change socket direction");
		exit(EXIT_FAILURE);
	}
	int bytesRead = 0;
	bytesRead = recv(sock, receivedAudio,sizeof(packet_t), MSG_WAITALL);
	return bytesRead;
}

/*
 * Description: Sends audio across the Bluetooth connection to the Pi3
 * @param: audio
 * @return: NULL
 */
void sendAudio(packet_t* audio, int size){
	fd_set socketWriteSet;
	FD_ZERO(&socketWriteSet);
	FD_SET(sock, &socketWriteSet);
	if((select(sock, 0, &socketWriteSet, 0, &timer)) < 0){
		perror("Failed to change socket direction");
		exit(EXIT_FAILURE);
	}
	int bytesWritten = 0;
	bytesWritten = send(sock,audio,sizeof(packet_t),0);
}

/*
 * Description: Sends data across the Bluetooth connection to the Pi3
 * @param: data
 * @return:NULL
 */
void sendData(packet_t* data, int size){
	write(sock,data,sizeof(packet_t));
}

/*
 * Description: Initializes the connection from the Pi0W to the Pi3
 * @param: NULL
 * @return: NULL
 */
void initBluetooth_Pi3(){
  int d;
  struct sockaddr_rc laddr, raddr;
  struct hci_dev_info di;

  if((pthread_mutex_init(&bluetoothLock, NULL)) != 0){
  	perror("Mutex init failed");
	exit(EXIT_FAILURE);
  }

  if(hci_devinfo(0, &di) < 0) {
      perror("HCI device info failed");
      exit(1);
  }

  //printf("Local device %s\n", batostr(&di.bdaddr));

  //laddr.rc_family = AF_BLUETOOTH;
  //laddr.rc_bdaddr = di.bdaddr;
  //laddr.rc_channel = 1;

  raddr.rc_family = AF_BLUETOOTH;
  str2ba(BluetoothAddr,&raddr.rc_bdaddr);
  raddr.rc_channel = CHANNEL_NUMBER;

  // Create a Socket to Bind with RFCOMM
  if( (sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0) {
      perror("socket");
  }

  // Connect to the Bluetooth Address using RFCOMM
  if(connect(sock, (struct sockaddr *)&raddr, sizeof(raddr)) < 0) {
        perror("connect");
	exit(EXIT_FAILURE);
   }
   struct sockaddr_sco local, remote;
   if((scoSock = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_SCO)) < 0){
  	perror("SCO");
	exit(EXIT_FAILURE);
   }
   local.sco_family = AF_BLUETOOTH;
   local.sco_bdaddr = *BDADDR_ANY;
   if((bind(scoSock, (struct sockaddr *) &local, sizeof(struct sockaddr_sco))) < 0){
	perror("bind");
	exit(EXIT_FAILURE);
   }
   if((listen(scoSock, 1)) < 0){
	perror("listen");
	exit(EXIT_FAILURE);
   }
   unsigned int length = sizeof(struct sockaddr_sco);
   scoClient = accept(scoSock, (struct sockaddr *) &remote, &length);
}

/*
 * Description: Disconnects Bluetooth, closes socket, and frees receivedAudio
 * @param: NULL
 * @return: NULL
 */
void closeBluetooth_Pi0W(){
	close(sock);
	pthread_mutex_destroy(&bluetoothLock);
}

void* handleBluetoothSender(void* params){
	packet_t packet;
	packet.datatype = VOICE_DATA;
	while(1)
	{
		packet.size = captureAudio(packet.data,BUFFER_SIZE);
//		memset(packet.data,'a',10);
//		packet.data[10]=0;
/*
		if (packet.size != BUFFER_SIZE) {
			fprintf(stderr,"sadboi in bluetoothsender\n");
		}*/
		sendAudio(&packet,sizeof(packet_t));
//		playbackAudio(packet.data,packet.size);
//		printf("%d\n",bytesRead);
//		sendData(&packet, sizeof(packet_t));
//		write(1, packet.data, packet.size);
	}
}

void* handleBluetoothReceiver(void* params){
	packet_t packet;
	while(1)
	{
		getAudio(&packet, sizeof(packet_t));
		if (packet.size!=BUFFER_SIZE) {
			fprintf(stderr,"sadboi in bluetoothreciever\n");
		}
		if(packet.size!=BUFFER_SIZE) {
			fprintf(stderr,"this should never happen\n");
		}
//		printf("%d\n",packet.size);
		playbackAudio(packet.data,packet.size);
//		printf("%s\n",packet.data);
	}
}
