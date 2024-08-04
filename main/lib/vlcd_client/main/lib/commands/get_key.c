#include "../tcpconn.h"
#include "../udpconn.h"
#include "../structs.h"
#include "../utils.h"

/* _vc_get_key: return struct for pressed key on VLCD */
struct VC_KEY_COMM _vc_get_key(struct VLCDC vlcdc){
	struct VC_KEY_COMM vckc = {0, 0, 0, 0, 0, 0, 0};
	
	int bufsiz = 9;
	
	char buff[bufsiz];
	memset(buff, 0, sizeof(buff));
	int s = udp_conn_recv(&vlcdc.uconn, buff, bufsiz);

	if(s > 0){
		vckc.key = buff[0];                     // key
		vckc.is_down = buff[1];                 // is down
		vckc.is_shift = buff[2];                // is shift
		vckc.is_ctrl = buff[3];                 // is ctrl
		vckc.mouse_key = buff[4];               // mouse-key
		vckc.mouse_x = hexm(buff[5], buff[6]);  // x coordinate
		vckc.mouse_y = hexm(buff[7], buff[8]);  // y coordinate
	}
	/*
	// for logging
	printf("KEY: %d\n", vckc.key);
	printf("isDown: %d\n", vckc.is_down);
	printf("isShift: %d\n", vckc.is_shift);
	printf("isCTRL: %d\n", vckc.is_ctrl);
	printf("mouse-k: %d\n", vckc.mouse_key);
	printf("mouse-x: %d\n", vckc.mouse_x);
	printf("mouse-y: %d\n\n", vckc.mouse_y);
	*/

	return vckc;
}

