#include "../tcpconn.h"
#include "../udpconn.h"
#include "../structs.h"
#include "../utils.h"
#include <stdio.h>


/* _vc_frame_comm: Setup a frame */
struct VC_READ_COMM _vc_read_comm(struct VLCDC *vlcdc, int width, int height, int kmode){
	struct VC_READ_COMM comm = {1, 0, 0};

	int i;
	unsigned char txbuff[1] = {'\x05'};
	unsigned char rxbuff[1024];

	comm.objs = (struct VC_READ_OBJ *)malloc(((width * height) * 7) * sizeof(struct VC_READ_OBJ *));

	
	tcp_conn_send(&vlcdc->tconn, txbuff, 1);

	memset(rxbuff, 0, sizeof(rxbuff));

	int count = 0;

	if(tcp_conn_recv(&vlcdc->tconn, rxbuff, 1024) > 0){
		for(i = 0; i < (width * height) * 7; i += 7){
			struct VC_READ_OBJ obj;

			obj.x = hexm(rxbuff[i], rxbuff[i + 1]);
			obj.y = hexm(rxbuff[i + 2], rxbuff[i + 3]);

			obj.color.red = rxbuff[i + 4];
			obj.color.green = rxbuff[i + 5];
			obj.color.blue = rxbuff[i + 6];

			comm.objs[count] = obj;

			// printf("Xa: \\x%x\n", rxbuff[i]);
			// printf("Xb: \\x%x\n", rxbuff[i + 1]);
			// printf("Ya: \\x%x\n", rxbuff[i + 2]);
			// printf("Yb: \\x%x\n", rxbuff[i + 3]);
			// printf("R: \\x%x\n",  rxbuff[i + 4]);
			// printf("G: \\x%x\n",  rxbuff[i + 5]);
			// printf("B: \\x%x\n",  rxbuff[i + 6]);
			// printf("--------------\n");

			count++;
		}

	} else
		return comm;

	comm.ecode = 0;
	comm.len = count;
	printf("COUNT: %d\n", count);

	return comm;
}

