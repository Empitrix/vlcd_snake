#include <string.h>
#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>


#define MAXTRANS 1024

struct UDP_CONN {
	char *msg;
	int ecode; 
	int sock;
	struct sockaddr_in source_addr;
};

struct UDP_CONN udp_conn_init(int port){

	struct UDP_CONN udpsock;

	udpsock.ecode = 1;
	udpsock.msg = (char *)""; char msg[1000];


	// char rx_buffer[MAXTRANS];
	char addr_str[128];
	int addr_family;
	int ip_protocol;

	// Initialize Socket
	struct sockaddr_in destAddr;
	destAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(port);
	addr_family = AF_INET;
	ip_protocol = IPPROTO_IP;
	inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);

	udpsock.sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
	if (udpsock.sock < 0) {
		sprintf(msg, "Unable to create socket: errno %d\n", errno);
		udpsock.msg = msg;
		return udpsock;
	}

	if (bind(udpsock.sock, (struct sockaddr *)&destAddr, sizeof(destAddr)) < 0) {
		sprintf(msg, "Socket unable to bind: errno %d\n", errno);
		udpsock.msg = msg;
		return udpsock;
	}



	// Set the socket to non-blocking mode
	int flags = fcntl(udpsock.sock, F_GETFL, 0);
	fcntl(udpsock.sock, F_SETFL, flags | O_NONBLOCK);
	udpsock.ecode = 0;


	return udpsock;
}

int udp_conn_recv(struct UDP_CONN *udpsock, char *buffer, size_t siz){
if(udpsock->ecode != 0)
		return -1;

	__socklen_t socklen = sizeof(udpsock->source_addr);
	int len = recvfrom(udpsock->sock, buffer, siz, 0, (struct sockaddr *)&udpsock->source_addr, &socklen);

	// Check if the receive operation was successful or if an error occurred
	if (len < 0) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			// No data available right now, just return 0
			return 0;
		} else {
			// An actual error occurred
			return -1;
		}
	}

	return len;
}

// /* UDP RECV: gets and store it into the given buffer */
// int udp_conn_recv(struct UDP_CONN *udpsock, char *buffer, size_t siz){
// 	if(udpsock->ecode != 0)
// 		return -1;
// 	__socklen_t socklen = sizeof(udpsock->source_addr);
// 	int len = recvfrom(udpsock->sock, buffer, siz, 0, (struct sockaddr *)&udpsock->source_addr, &socklen);
// 	// buffer[len] = '\0';
// 	return len;
// }

/* UDP SEND: send the buffer and return the lenght of it */
int udp_conn_send(struct UDP_CONN *udpsock, char *buffer, size_t siz){
	if(udpsock->ecode != 0)
		return -1;
	return sendto(udpsock->sock, buffer, siz, 0, (struct sockaddr *)&udpsock->source_addr, sizeof(udpsock->source_addr));
}

