# VLCD Client
A client for [VLCD](https://github.com/Empitrix/vlcd) in [ESP8266 RTOS SDK](https://github.com/espressif/ESP8266_RTOS_SDK).


## Usage
clone the project:
```bash
git clone "https://github.com/empitrix/vlcd_client"
```

first after cloning the project into a folder use the following header for your [ESP8266 RTOS SDK](https://github.com/espressif/ESP8266_RTOS_SDK) project:
```c
#include "vlcdc.h"
```

Connect the ESP8266 to the WIFI and initialize it with the PORT number:
```c
vlcdc_init("WIFI SSID", "WIFI PASSWORD", 4210);
```


To read the keys from VLCD you can use the following command which is return a structure for key status
```c
vlcdc_get_key(struct VLCDC vlcdc)
```

**The following commands are for VLCD screen and return a non-zero if everything goes well.**

To [**initialize**](https://github.com/empitrix/vlcd?tab=readme-ov-file#initialize) the VLCD screen:
```c
int vlcdc_screen_init(struct VLCDC *vlcdc, struct VC_INIT_COMM comm);
```

To [**color a pixel**](https://github.com/empitrix/vlcd?tab=readme-ov-file#set-pixel) by given coordinate and color:
```c
int vlcdc_screen_spixel(struct VLCDC *vlcdc, struct VC_SPIXEL_COMM comm);
```

To [**Fill**](https://github.com/empitrix/vlcd?tab=readme-ov-file#fill) the whole screen with given color use:
```c
int vlcdc_screen_fill(struct VLCDC *vlcdc, struct VC_FILL_COMM comm);
```

To set a [**Frame**](https://github.com/empitrix/vlcd?tab=readme-ov-file#frame) by given coordinate and size with it's buffer use:
```c
int vlcdc_screen_frame(struct VLCDC *vlcdc, struct VC_FRAME_COMM comm);
```

NOTE: the frame buffer need to be allocated, for example:
```c
struct VC_FRAME_COMM frame;
frame.x = 150;
frame.y = 150;
frame.width = 2;
frame.height = 2;
frame.buffer = (char *)malloc((_frame.width * _frame.height) * sizeof(char));
```
and the value of buffer is from `[0, 255]` or `unsigned char`, for more info see [this](https://github.com/empitrix/vlcd?tab=readme-ov-file#frame).


To read the frame data use the following command, which is returns a `struct VC_READ_COMM` that contains all the frame data like coordinate and size followd by a pointer to for objects `objs` for frame data (pixels):
```c
struct VC_READ_COMM vlcdc_screen_read(struct VLCDC *vlcdc);
```
