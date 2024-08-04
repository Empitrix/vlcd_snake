#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
// #include "../../../lib/my_wifi.h"  // you might don't have this
#include "lib/wifi.h"

#include "lib/tcpconn.h"
#include "lib/udpconn.h"


#include "lib/commands/get_key.h"
#include "lib/commands/init.h"
#include "lib/commands/spixel.h"
#include "lib/commands/fill.h"
#include "lib/commands/frame.h"
#include "lib/commands/read.h"


static int kmode = 0;
static int fwidth = 0;
static int fheight = 0;

struct VLCDC vlcdc_init(char *ssid, char *password, int port){

	struct VLCDC vlcdc;

	wifi_setup(ssid, password);
	wifi_connect();
	printf("WIFI Connected! [%s]\n", ssid);

	struct UDP_CONN uc = udp_conn_init(port);
	struct TCP_CONN tc = tcp_conn_init(port);

	vlcdc.uconn = uc;
	vlcdc.tconn = tc;

	return vlcdc;
}



struct VC_KEY_COMM vlcdc_get_key(struct VLCDC vlcdc){
	return _vc_get_key(vlcdc);
}

int vlcdc_screen_init(struct VLCDC *vlcdc, struct VC_INIT_COMM comm){
	kmode = comm.mode;
	return _vc_init_comm(vlcdc, comm);
}

int vlcdc_screen_spixel(struct VLCDC *vlcdc, struct VC_SPIXEL_COMM *comm){
	return _vc_spixel_comm(vlcdc, comm);
}

int vlcdc_screen_fill(struct VLCDC *vlcdc, struct VC_FILL_COMM comm){
	return _vc_fill_comm(vlcdc, comm);
}

int vlcdc_screen_frame(struct VLCDC *vlcdc, struct VC_FRAME_COMM *comm){
	fwidth = comm->width;
	fheight = comm->height;
	return _vc_frame_comm(vlcdc, comm);
}

struct VC_READ_COMM vlcdc_screen_read(struct VLCDC *vlcdc){
	return _vc_read_comm(vlcdc, fwidth, fheight, kmode);
}

