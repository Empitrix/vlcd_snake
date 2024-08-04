#include "../tcpconn.h"
#include "../udpconn.h"
#include "../structs.h"
#include "../utils.h"

#include <stdio.h>

/* _vc_frame_comm: Setup a frame */
int _vc_frame_comm(struct VLCDC *vlcdc, struct VC_FRAME_COMM *comm){

	int dsiz = 0;
	dsiz = (comm->width * comm->height) / 8;  // data size (1024)
	dsiz += 9;

	// int bufsiz = 9 + dsiz;
	unsigned char framebuff[dsiz];

	memset(framebuff, 1, dsiz);

	for(int i = 0; i < dsiz; ++i)
		framebuff[i] = '\x00';

	framebuff[0] = '\x02';             // index
	framebuff[1] = first_h(comm->x);   // X (a)
	framebuff[2] = second_h(comm->x);  // X (b)

	framebuff[3] = first_h(comm->y);   // Y (a)
	framebuff[4] = second_h(comm->y);  // Y (b)

	framebuff[5] = first_h(comm->width);   // Width (a)
	framebuff[6] = second_h(comm->width);  // Width (b)

	framebuff[7] = first_h(comm->height);   // Height (a)
	framebuff[8] = second_h(comm->height);  // Height (b)

	int rem = 9;  // remaining of buffer (ended at idx:8)

	for(int i = 0, kidx = 0; i < dsiz; ++i){
		framebuff[rem++] = ghex(comm->buffer[kidx++]);
		framebuff[rem] = '\x00';
	}

	return tcp_conn_send(&vlcdc->tconn, framebuff, dsiz);
}
