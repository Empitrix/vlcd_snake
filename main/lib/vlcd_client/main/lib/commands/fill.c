#include "../tcpconn.h"
#include "../udpconn.h"
#include "../structs.h"
#include "../utils.h"

/* _vc_get_key: return struct for pressed key on VLCD */
int _vc_fill_comm(struct VLCDC *vlcdc, struct VC_FILL_COMM comm){

	int bufsiz = 4;
	unsigned char init[bufsiz];

	init[0] = '\x04';            // index
	init[1] = comm.color.red;    // value of red color
	init[2] = comm.color.green;  // value of green color
	init[3] = comm.color.blue;   // value of blue color

	return tcp_conn_send(&vlcdc->tconn, init, bufsiz);
}

