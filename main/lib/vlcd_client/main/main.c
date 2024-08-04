/* ATTENTION: This is a test file */

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "vlcdc.h"
#include "lib/utils.h"
#include "lib/commands.h"

#define PORT 4210
#define TRANSITION 1024

void app_main(){

	// struct VLCDC vlcdc = vlcdc_init((char *)MY_WIFI_SSID, (char *)MY_WIFI_PASSWORD, PORT);
	struct VLCDC vlcdc = vlcdc_init((char *)"<WIFI-SSID>", (char *)"WIFI-PASSWORD", PORT);

	struct VC_FRAME_COMM _frame;
	_frame.buffer = (unsigned char *)malloc((_frame.width * _frame.height) * sizeof(unsigned char));
	_frame.x = 150;
	_frame.y = 150;
	_frame.width = 2;
	_frame.height = 2;

	while(1){

		printf("Init...\n");  // initialize
		struct VC_INIT_COMM _vic;
		_vic.width = 400;
		_vic.height = 400;
		_vic.color.red = 255;
		_vic.color.green = 255;
		_vic.color.blue = 0;
		_vic.mode = 1;  // full color
		vlcdc_screen_init(&vlcdc, _vic);
		vlcd_dlay(500);


		// printf("Fill...\n");  // fill
		// struct VC_FILL_COMM _fill;
		// _fill.color.red = 0;
		// _fill.color.green = 255;
		// _fill.color.blue = 0;
		// vlcdc_screen_fill(&vlcdc, _fill);
		// vlcd_dlay(500);


		// printf("SPIXEL...\n");  // fill
		// struct VC_SPIXEL_COMM _spixel;
		// _spixel.x = 100;
		// _spixel.y = 100;
		// _spixel.color.red = 0;
		// _spixel.color.green = 0;
		// _spixel.color.blue = 255;
		// vlcdc_screen_spixel(&vlcdc, &_spixel);
		// vlcd_dlay(500);


		printf("Frame...\n");  // fill
		// red
		_frame.buffer[0] = '\xFF';
		_frame.buffer[1] = '\xFF';
		_frame.buffer[2] = '\xFF';
		// blue
		_frame.buffer[3] = '\xFF';
		_frame.buffer[4] = '\xFF';
		_frame.buffer[5] = '\xFF';
		// green
		_frame.buffer[6] = '\xFF';
		_frame.buffer[7] = '\xFF';
		_frame.buffer[8] = '\xFF';
		// white
		_frame.buffer[9] = '\xFF';
		_frame.buffer[10] ='\xFF';
		_frame.buffer[11] ='\xFF';
		vlcdc_screen_frame(&vlcdc, &_frame);
		vlcd_dlay(500);
		free(_frame.buffer);

		printf("Reading...\n");
		struct VC_READ_COMM readc = vlcdc_screen_read(&vlcdc);
		if(readc.ecode == 0){
			for(int i = 0; i < readc.len; ++i){
				printf("R[%d] >> X: %d\n", i, readc.objs[i].x);
				printf("R[%d] >> Y: %d\n", i, readc.objs[i].y);
				printf("R[%d] >> {%d, %d, %d}\n", i, readc.objs[i].color.red, readc.objs[i].color.green, readc.objs[i].color.blue);
				printf("-------\n");
			}
			free(readc.objs);
			break;
		}

		// vlcdc_get_key(vlcdc);  // read given keys
		vlcd_dlay(500);
	}
}

