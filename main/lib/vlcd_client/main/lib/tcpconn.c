#include <string.h>
#include <sys/param.h>
#include <fcntl.h>
#include <errno.h>
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

struct TCP_CONN {
	char *msg;
	int ecode;
	int sock;
	int lsock;
	char addr_str[128];
	struct sockaddr_in source_addr;
};

struct TCP_CONN tcp_conn_init(int port) {
	int addr_family;
	int ip_protocol;

	struct TCP_CONN tcpconn;
	tcpconn.ecode = 1;
	char msg[1000];

	struct sockaddr_in destAddr;
	destAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(port);
	addr_family = AF_INET;
	ip_protocol = IPPROTO_IP;
	inet_ntoa_r(destAddr.sin_addr, tcpconn.addr_str, sizeof(tcpconn.addr_str) - 1);

	tcpconn.lsock = socket(addr_family, SOCK_STREAM, ip_protocol);
	if (tcpconn.lsock < 0) {
		sprintf(msg, "Unable to create socket: errno %d\n", errno);
		tcpconn.msg = msg;
		return tcpconn;
	}

	int err = bind(tcpconn.lsock, (struct sockaddr *)&destAddr, sizeof(destAddr));
	if (err != 0) {
		sprintf(msg, "Socket unable to bind: errno %d\n", errno);
		tcpconn.msg = msg;
		return tcpconn;
	}

	err = listen(tcpconn.lsock, 1);
	if (err != 0) {
		sprintf(msg, "Error occurred during listen: errno %d\n", errno);
		tcpconn.msg = msg;
		return tcpconn;
	}

	// Set the listening socket to non-blocking mode
	int flags = fcntl(tcpconn.lsock, F_GETFL, 0);
	fcntl(tcpconn.lsock, F_SETFL, flags | O_NONBLOCK);

	tcpconn.ecode = 0;
	tcpconn.sock = -1; // No client connected yet
	return tcpconn;
}

int tcp_conn_accept(struct TCP_CONN *tcpconn) {
	if (tcpconn->ecode != 0) {
		return -1;
	}

	uint addrLen = sizeof(tcpconn->source_addr);
	tcpconn->sock = accept(tcpconn->lsock, (struct sockaddr *)&tcpconn->source_addr, &addrLen);
	if (tcpconn->sock < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			// An error occurred
			return -1;
		}
		return 0; // No new connection yet
	}

	// Set the accepted socket to non-blocking mode
	int flags = fcntl(tcpconn->sock, F_GETFL, 0);
	fcntl(tcpconn->sock, F_SETFL, flags | O_NONBLOCK);
	
	inet_ntoa_r(((struct sockaddr_in *)&tcpconn->source_addr)->sin_addr.s_addr, tcpconn->addr_str, sizeof(tcpconn->addr_str) - 1);
	return 1; // New connection established
}

int tcp_conn_recv(struct TCP_CONN *tcpconn, unsigned char *buffer, int siz) {
	if (tcpconn->ecode != 0) {
		return -1;
	}

	// Accept a new connection if none is present
	if (tcpconn->sock < 0) {
		int result = tcp_conn_accept(tcpconn);
		if (result <= 0) {
			return result; // No new connection or an error occurred
		}
	}

	int len = recv(tcpconn->sock, buffer, siz - 1, 0);
	if (len < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return 0; // No data available yet
		}
		// An error occurred
		close(tcpconn->sock);
		tcpconn->sock = -1;
		return -1;
	}
	return len;
}

int tcp_conn_send(struct TCP_CONN *tcpconn, unsigned char *buffer, int siz) {
	if (tcpconn->ecode != 0) {
		return -1;
	}

	// Accept a new connection if none is present
	if (tcpconn->sock < 0) {
		int result = tcp_conn_accept(tcpconn);
		if (result <= 0) {
			return result; // No new connection or an error occurred
		}
	}

	int len = send(tcpconn->sock, buffer, siz, 0);
	if (len < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return 0; // Send operation would block
		}
		// An error occurred
		close(tcpconn->sock);
		tcpconn->sock = -1;
		return -1;
	}
	return len;
}

