#include <freertos/FreeRTOS.h>

unsigned char ghex(char c){ return (unsigned char)c; }  // get hex from char

// merge two hex
int hexm(int primary, int secondary){
	unsigned int a = (unsigned char)primary;
	unsigned int b = (unsigned char)secondary;
	return (int)(a << 8) | b;
}



unsigned char first_h(int decimalNumber) {
	return (decimalNumber >> 8) & 0xFF;
}

// Function to get the second hexadecimal byte (lower byte)
unsigned char second_h(int decimalNumber) {
	return decimalNumber & 0xFF;
}


void vlcd_dlay(int ms){
	vTaskDelay(ms / portTICK_PERIOD_MS);
}

