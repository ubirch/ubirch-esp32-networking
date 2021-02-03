/*!
 * @file    wifi.c
 * @brief   Wifi initialization and connection functions
 *
 * @author Waldemar Gruenwald
 * @date   2018-10-10
 *
 * @copyright &copy; 2018 ubirch GmbH (https://ubirch.com)
 *
 * ```
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ```
 */
#include <string.h>
#include <tcpip_adapter.h>
#include <esp_event_loop.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include <freertos/event_groups.h>

#include "networking_wifi.h"
#include "networking.h"

#define EXAMPLE_ESP_MAXIMUM_RETRY  1

static const char *TAG = "WIFI";

/* FreeRTOS event group to signal when we are connected & ready to make a request */
EventGroupHandle_t network_event_group;
static int s_retry_num = 0;

static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		esp_wifi_connect();
	} else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
		if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
			esp_wifi_connect();
			s_retry_num++;
			ESP_LOGI(TAG, "retry to connect to the AP");
		} else {
			ESP_LOGI(TAG, "maximum retries exceeded");
		}
		ESP_LOGI(TAG,"connect to the AP fail");
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
		ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
		s_retry_num = 0;
		xEventGroupSetBits(network_event_group, WIFI_CONNECTED_BIT);
	}
}

void init_wifi(void) {
    esp_log_level_set("wifi", ESP_LOG_WARN);
    static bool initialized = false;
    if (initialized) {
        return;
    }
	ESP_ERROR_CHECK(esp_netif_init());
	network_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_got_ip;
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
	                                                    ESP_EVENT_ANY_ID,
	                                                    &event_handler,
	                                                    NULL,
	                                                    &instance_any_id));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
	                                                    IP_EVENT_STA_GOT_IP,
	                                                    &event_handler,
	                                                    NULL,
	                                                    &instance_got_ip));

    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
    initialized = true;
}


esp_err_t wifi_join(struct Wifi_login wifi, int timeout_ms) {
	// reset the connection establishing values
	s_retry_num = 0;
	ESP_ERROR_CHECK(esp_wifi_stop() );
	// now try again
	wifi_config_t wifi_config = {0};
	strncpy((char *) wifi_config.sta.ssid, wifi.ssid, wifi.ssid_length);
	if (wifi.pwd) {
		strncpy((char *) wifi_config.sta.password, wifi.pwd, wifi.pwd_length);
	}
	wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
	wifi_config.sta.pmf_cfg.capable = true;
	wifi_config.sta.pmf_cfg.required = false;

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
	ESP_ERROR_CHECK(esp_wifi_start() );

	ESP_LOGI(__func__, "wifi_init_sta finished.");

	/* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
	 * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
	EventBits_t bits = xEventGroupWaitBits(network_event_group,
	                                       WIFI_CONNECTED_BIT,
	                                       pdFALSE,
	                                       pdTRUE,
	                                       timeout_ms / portTICK_PERIOD_MS);
	/* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
	 * happened. */
	if (bits & WIFI_CONNECTED_BIT) {
		ESP_LOGI(__func__, "connected ");
		return ESP_OK;
	} else {
		ESP_LOGE(__func__, "UNEXPECTED EVENT");
		return ESP_FAIL;
	}
}

//todo clean this up
