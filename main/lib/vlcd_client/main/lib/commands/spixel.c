#include "../tcpconn.h"
#include "../udpconn.h"
#include "../structs.h"
#include "../utils.h"

/* _vc_spixel_comm: color one pixel in the screen */
int _vc_spixel_comm(struct VLCDC *vlcdc, struct VC_SPIXEL_COMM *comm){

	int bufsiz = 8;
	unsigned char spixel[bufsiz];

	spixel[0] = '\x03';            // index

	spixel[1] = first_h(comm->x);   // X (a)
	spixel[2] = second_h(comm->x);  // X (b)

	spixel[3] = first_h(comm->y);   // Y (a)
	spixel[4] = second_h(comm->y);  // Y (b)

	spixel[5] = comm->color.red;    // value of red color
	spixel[6] = comm->color.green;  // value of green color
	spixel[7] = comm->color.blue;   // value of blue color

	return tcp_conn_send(&vlcdc->tconn, spixel, bufsiz);
}

