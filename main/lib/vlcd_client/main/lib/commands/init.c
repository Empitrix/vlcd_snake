#include "../tcpconn.h"
#include "../udpconn.h"
#include "../structs.h"
#include "../utils.h"

/* _vc_get_key: return struct for pressed key on VLCD */
int _vc_init_comm(struct VLCDC *vlcdc, struct VC_INIT_COMM comm){

	int bufsiz = 9;

	unsigned char init[bufsiz];

	init[0] = '\x01';  // first command
	init[1] = comm.color.red;    // value of red color
	init[2] = comm.color.green;  // value of green color
	init[3] = comm.color.blue;   // value of blue color

	init[4] = first_h(comm.width);   // width
	init[5] = second_h(comm.width);  // width

	init[6] = first_h(comm.height);   // height
	init[7] = second_h(comm.height);  // height

	init[8] = comm.mode;  // color mode
	
	return tcp_conn_send(&vlcdc->tconn, init, bufsiz);
}

