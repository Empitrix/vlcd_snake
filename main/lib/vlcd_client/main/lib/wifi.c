#include <string.h>
#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"

#include "nvs.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_event.h"
#include "esp_wifi.h"
// #include "esp_log.h"
#include "esp_event_loop.h"
#include "tcpip_adapter.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"


#define GOT_IPV4_BIT BIT(0)
#define GOT_IPV6_BIT BIT(1)

#define CONNECTED_BITS (GOT_IPV4_BIT)

static EventGroupHandle_t conn_group;
static ip4_addr_t ipaddr;

static char wifi_name[32];
static char wifi_pass[32];



void check_err(int inpt, char *msg){
	if(inpt != 0){
		printf("%s\n", msg);
		exit(0);
	}
}


static void on_wifi_disconnect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
	system_event_sta_disconnected_t *event = (system_event_sta_disconnected_t *)event_data;
	printf("Wi-Fi disconnected, trying to reconnect...\n");
	if (event->reason == WIFI_REASON_BASIC_RATE_NOT_SUPPORT) {
		esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
	}
	ESP_ERROR_CHECK(esp_wifi_connect());
}

static void on_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
	ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
	memcpy(&ipaddr, &event->ip_info.ip, sizeof(ipaddr));
	xEventGroupSetBits(conn_group, GOT_IPV4_BIT);
}


static void wifi_start(void){
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL);
	esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, NULL);
	esp_wifi_init(&cfg);
	esp_wifi_set_storage(WIFI_STORAGE_RAM);

	wifi_config_t wifi_config = { 0 };

	strncpy((char *)&wifi_config.sta.ssid, wifi_name, 32);
	strncpy((char *)&wifi_config.sta.password, wifi_pass, 32);

	esp_wifi_set_mode(WIFI_MODE_STA);
	esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
	esp_wifi_start();
	esp_wifi_connect();
}

static void wifi_stop(void){
	esp_err_t err = esp_wifi_stop();

	if (err == ESP_ERR_WIFI_NOT_INIT) {
		return;
	}

	ESP_ERROR_CHECK(err);

	esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect);
	esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip);

	esp_wifi_deinit();
}

esp_err_t wifi_connect(void){
	if (conn_group != NULL) {
		return ESP_ERR_INVALID_STATE;
	}

	conn_group = xEventGroupCreate();
	wifi_start();

	// wait for connection
	xEventGroupWaitBits(conn_group, CONNECTED_BITS, true, true, portMAX_DELAY);

	// inform
	printf("Connected To: %s\n", wifi_name);
	printf("IP Address: %d.%d.%d.%d\n", IP2STR(&ipaddr));

	return ESP_OK;
}

esp_err_t wifi_disconnect(void){
	if (conn_group == NULL) {
		return ESP_ERR_INVALID_STATE;
	}
	vEventGroupDelete(conn_group);
	conn_group = NULL;
	wifi_stop();
	printf("Disconnected from %s\n", wifi_name);
	wifi_name[0] = '\0';
	return ESP_OK;
}

esp_err_t wifi_setup(const char *ssid, const char *passwd){
	strncpy(wifi_name, ssid, sizeof(wifi_name));
	strncpy(wifi_pass, passwd, sizeof(wifi_pass));


	check_err(nvs_flash_init(), (char *)"FAILED: nvs_flash_init");
	check_err(esp_netif_init(), (char *)"FAILED: esp_netif_init");
	check_err(esp_event_loop_create_default(), (char *)"FAILED: esp_event_loop_create_default");

	return ESP_OK;
}

