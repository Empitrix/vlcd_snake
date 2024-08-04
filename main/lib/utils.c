#include "rules.h"

#include "vlcd_client/main/vlcdc.h"

/* Game RULES */
struct GMAE_RULES {
	int ix, iy;   // X, Y directions (-1, 0, 1)
	int ln;       // snake length
	int gpoints;  // number of points
	int running;  // game running (game over)
	int gstep;    // game steps (SCALE)
};


// block types (HEAD, BODY, POINT)
enum BLOCK_TYPE {
	BODY_B,
	HEAD_B,
	POINT_B
};

struct BLOCK {
	int x, y;              // coordinates
	enum BLOCK_TYPE type;  // type
};


/* copy all of the blocks in 't' to 's' (S <- T) */
void blockcpy(struct BLOCK dst[], struct BLOCK src[], int ln){
	memset(dst, 0, MAX_BLOCK);
	int i;
	for(i = 0; i < ln; ++i){
		dst[i] = src[i];
	}
}


/* update_idx: update game movement & direction */
void update_idx(int *ix, int *iy, int pins[4], struct VLCDC *vlcdc){


	// RIGHT
	if(gpio_get_level(pins[0]) && *ix != -1){
		*ix = 1;
		*iy = 0;
		return;
	}

	// DOWN
	if(gpio_get_level(pins[3]) && *iy != -1){
		*iy = 1;
		*ix = 0;
		return;
	}

	// LEFT
	if(gpio_get_level(pins[1]) && *ix != 1){
		*ix = -1;
		*iy = 0;
		return;
	}

	// UP
	if(gpio_get_level(pins[2]) && *iy != 1){
		*iy = -1;
		*ix = 0;
		return;
	}

	struct VC_KEY_COMM vckc = vlcdc_get_key(*vlcdc);
	if(vckc.is_down){

		// right
		if(vckc.key == 79 && *ix != -1){
			*ix = 1;
			*iy = 0;
			return;
		}

		// down
		if(vckc.key == 81 && *iy != -1){
			*iy = 1;
			*ix = 0;
			return;
		}

		// left
		if(vckc.key == 80 && *ix != 1){
			*ix = -1;
			*iy = 0;
			return;
		}

		// up
		if(vckc.key == 82 && *iy != 1){
			*iy = -1;
			*ix = 0;
			return;
		}

	}

}

/* dlay: delay for given milliseconds */
void dlay(int ms){
	vTaskDelay(ms / portTICK_PERIOD_MS);
}


void cp_buffer(char *dst, unsigned char *src, int len){
	for(int i = 0; i < len; ++i)
		dst[i] = src[i];
}



/* Clear the given buffer */
void clear_buff(unsigned char *buff, int len){
	for(int i = 0; i < len; ++i)
		buff[i] = '\x00';
}

/* Clear the given buffer */
void copy_buff(unsigned char *dst, unsigned char *src, int len){
	for(int i = 0; i < len; ++i)
		dst[i] = src[i];
}
