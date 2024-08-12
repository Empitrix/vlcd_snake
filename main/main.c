#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/gpio.h>
#include <ssd1306/ssd1306.h>

#include "lib/gpio.h"
#include "lib/display.h"
#include "lib/utils.h"
#include "lib/game.h"
#include "lib/rules.h"

#include "lib/vlcd_client/main/vlcdc.h"
#include "lib/my_wifi.h"  // you may don't have this, contains wifi ssid and password


#define VLCD_BUFF ((DISPLAY_WIDTH * DISPLAY_WIDTH) / 8) + 9

// main block array
static struct BLOCK blocks[MAX_BLOCK] = {
	{DISPLAY_WIDTH / 2 + 1, DISPLAY_HEIGHT / 2 + 3, HEAD_B},
	{DISPLAY_WIDTH / 2 + 1, DISPLAY_HEIGHT / 2 + 3, BODY_B},
	{DISPLAY_WIDTH / 2 + 1, DISPLAY_HEIGHT / 2 + 3, BODY_B},
	{50, 50, POINT_B},
};

static struct BLOCK tmpb[MAX_BLOCK];  // temporary block array (dummy)


void app_main(void){

	// set the following GPIOs to INPUT-MODE
	set_pin_mode(GPIO_RIGHT, INPUT_PINMODE);
	set_pin_mode(GPIO_DOWN , INPUT_PINMODE);
	set_pin_mode(GPIO_UP   , INPUT_PINMODE);
	set_pin_mode(GPIO_LEFT , INPUT_PINMODE);

	// initialize OLED display and font
	const font_info_t *font = font_builtin_fonts[FONT_FACE_GLCD5x7];
	ssd1306_t dev = init_display(SCL_PIN, SDA_PIN, DISPLAY_WIDTH, DISPLAY_HEIGHT);

	struct VLCDC vlcdc = vlcdc_init((char *)MY_WIFI_SSID, (char *)MY_WIFI_PASSWORD, 4210);

	struct VC_INIT_COMM init_comm;
	init_comm.color = (struct VC_COLOR){'\x70', '\xE0', '\xF7'};
	init_comm.width = DISPLAY_WIDTH;
	init_comm.height = DISPLAY_HEIGHT;
	init_comm.mode = 0;  // mono color

	int status, att = 0;
	do {
		printf("Attempt: %d\n", att);
		status = vlcdc_screen_init(&vlcdc, init_comm);
		att++;
		dlay(500);
	} while(status == 0);


	// game rules initialization
	struct GMAE_RULES rules;
	rules.ix = 1;       // move to right
	rules.iy = 0;       // UP/DOWN (Y) direction
	rules.gpoints = 0;  // 0 points
	rules.running = 1;  // game is running ( not over yet )
	rules.gstep = 5;    // game steps
	rules.ln = 4;       // current number of blocks (HEAD, BODY, POINT)



	struct VC_FRAME_COMM frame_comm;

	frame_comm.x = frame_comm.y = 0;
	frame_comm.width = DISPLAY_WIDTH;
	frame_comm.height = DISPLAY_HEIGHT;
	frame_comm.buffer = (unsigned char *)malloc((VLCD_BUFF) * sizeof(unsigned char));  // 1033
	clear_buff(frame_comm.buffer, VLCD_BUFF);

	int uploop = 0;

	while(1){
		// memset(fb, 0, BUFFER_SIZE);  // clear
		clear_buff(frame_comm.buffer, VLCD_BUFF);

		// get movements (update directions)
		update_idx(&rules.ix, &rules.iy, (int[4]){GPIO_RIGHT, GPIO_LEFT, GPIO_UP, GPIO_DOWN}, &vlcdc);

		/* UPDATE MOVEMENTs */
		blockcpy(tmpb, blocks, rules.ln);
		tmpb[0].x = blocks[0].x + (rules.ix * rules.gstep);  // head x
		tmpb[0].y = blocks[0].y + (rules.iy * rules.gstep);  // head y

		if(tmpb[0].x > DISPLAY_WIDTH || tmpb[0].x < 0 || tmpb[0].y > DISPLAY_HEIGHT || tmpb[0].y < 0){
			memset(frame_comm.buffer, 0, BUFFER_SIZE);
			rules.running = 0;
		}

		// detect if the game is over
		if(rules.running == 0){
			game_over_fn(dev, font, blocks, &rules, &vlcdc, &frame_comm);
			continue;
		}

		// Shift all of the block bodies to their previous positions
		int idx;
		for(idx = 1; idx < rules.ln; ++idx){
			if(tmpb[idx].type == BODY_B){
				tmpb[idx].x = blocks[idx - 1].x;
				tmpb[idx].y = blocks[idx - 1].y;
			}
		}
		blockcpy(blocks, tmpb, rules.ln);  // copy temporary block array to main one


		// snake hit the point (NEW POINT)
		if(blocks[0].x == blocks[idx - 1].x && blocks[0].y == blocks[idx - 1].y){
			rules.gpoints++;  // add 1 to points

			struct BLOCK np = get_point(blocks, DISPLAY_WIDTH, DISPLAY_HEIGHT, rules.gstep, rules.ln);

			blocks[idx - 1] = blocks[idx - 2];  // add one more block to body
			blocks[idx] = np;  // add the point block
			rules.ln++;
		} else {
			// the snake crossed itself
			if(any_crossed(blocks, blocks[0], rules.ln, 1) != 0){
				memset(frame_comm.buffer, 0, BUFFER_SIZE);
				rules.running = 0;
			}
		}


		// draw blocks
		for(int i = 0; i < rules.ln; i++){
			if(blocks[i].type == POINT_B)
				ssd1306_draw_circle(&dev, frame_comm.buffer, blocks[i].x + 2, blocks[i].y + 2, 2, OLED_COLOR_INVERT);  // draw point (r:2)
			else
				draw_box(&dev, frame_comm.buffer, rules.gstep, blocks[i].x, blocks[i].y);  // draw the snake
		}




		ssd1306_load_frame_buffer(&dev, frame_comm.buffer);  // load buffer into the display
		vlcdc_screen_frame(&vlcdc, &frame_comm);

		int s = 200 - rules.gpoints * 4;
		dlay((s  > 10) ? s : 10);
		uploop++;
	}

}

