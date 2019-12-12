/*!
 * @file    networking_eth.c
 * @brief   Ethernet initialization.
 *
 *
 * @author Matthias L. Jugel
 * @date   2018-11-26
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
#include <esp_eth.h>
#include <tcpip_adapter.h>
#include <esp_event_loop.h>
#include <eth_phy/phy_lan8720.h>
#include <esp_log.h>
#include <sys/socket.h>
#include "networking_eth.h"
#include "networking.h"

// TODO make pins configurable
#define PIN_SMI_MDC   23
#define PIN_SMI_MDIO  18
#define PIN_PHY_POWER 12

static const char *TAG = "ETH";

static void phy_device_power_enable_via_gpio(bool enable) {
    if (!enable) {
        phy_lan8720_default_ethernet_config.phy_power_enable(false);
    }
    gpio_pad_select_gpio(PIN_PHY_POWER);
    gpio_set_direction(PIN_PHY_POWER, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_PHY_POWER, (uint32_t) enable);

    // Allow the power up/down to take effect, min 300us
    vTaskDelay(1);

    if (enable) {
        phy_lan8720_default_ethernet_config.phy_power_enable(true);
    }
}

static void eth_gpio_config_rmii(void) {
    // RMII data pins are fixed:
    // TXD0 = GPIO19
    // TXD1 = GPIO22
    // TX_EN = GPIO21
    // RXD0 = GPIO25
    // RXD1 = GPIO26
    // CLK == GPIO0
    phy_rmii_configure_data_interface_pins();
    // MDC is GPIO 23, MDIO is GPIO 18
    phy_rmii_smi_configure_pins(PIN_SMI_MDC, PIN_SMI_MDIO);
}

static system_event_cb_t nested_callback = NULL;

static esp_err_t event_handler(void *ctx, system_event_t *event) {
    switch (event->event_id) {
        case SYSTEM_EVENT_ETH_GOT_IP:
            ESP_LOGI(TAG, "got ip: "
                    IPSTR
                    " ", IP2STR(&event->event_info.got_ip.ip_info.ip));
            xEventGroupSetBits(network_event_group, NETWORK_ETH_READY);
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            ESP_LOGD(TAG, "disconnected");
            xEventGroupClearBits(network_event_group, NETWORK_ETH_READY);
            break;

        default:
            if (nested_callback)
                return nested_callback(ctx, event);
    }

    return ESP_OK;
}

esp_err_t init_ethernet() {
	eth_config_t config = phy_lan8720_default_ethernet_config;
	esp_err_t ret;

	tcpip_adapter_ip_info_t ipInfo, myIp;

	IP4_ADDR(&myIp.ip, 10, 100, 40, 95);
	IP4_ADDR(&myIp.gw, 10, 100, 40, 1);
	IP4_ADDR(&myIp.netmask, 255, 255, 255, 0);

	tcpip_adapter_init();
	ret = tcpip_adapter_dhcpc_stop(
			TCPIP_ADAPTER_IF_ETH); // ret=0x5000 -> tcpip_adapter_invalid_params, very old esp-idf didn't implementated this yet.
	ESP_LOGI(TAG, "dhcp client stop RESULT: %d", ret);
	tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_ETH, &myIp);

	esp_event_loop_init(NULL, NULL);
	nested_callback = esp_event_loop_set_cb(event_handler, NULL);

	config.phy_addr = 0;
	config.gpio_config = eth_gpio_config_rmii;
	config.tcpip_input = tcpip_adapter_eth_input;
	config.phy_power_enable = phy_device_power_enable_via_gpio;
	config.clock_mode = ETH_CLOCK_GPIO17_OUT;

	ret = esp_eth_init(&config);
	if (ret != ESP_OK) {
		return ret;
	}

	return esp_eth_enable();
}
//
//esp_err_t init_static_ip(){
//	tcpip_adapter_ip_info_t ipInfo, myIp;
//
//	IP4_ADDR(&myIp.ip, 10, 100 , 40, 95);
//	IP4_ADDR(&myIp.gw, 10, 100 , 40, 1);
//	IP4_ADDR(&myIp.netmask, 255, 255 , 255, 0);
//
//
////	// myIp -> structure that save your static ip settings
////	inet_pton(AF_INET, myIp.ip,      &ipInfo.ip);
////	inet_pton(AF_INET, myIp.gw, &ipInfo.gw);
////	inet_pton(AF_INET, myIp.netmask, &ipInfo.netmask);
//
//	tcpip_adapter_init();
//	esp_err_t ret = tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_ETH); // ret=0x5000 -> tcpip_adapter_invalid_params, very old esp-idf didn't implementated this yet.
//	ESP_LOGI(TAG, "dhcp client stop RESULT: %d", ret);
//	tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_ETH, &myIp);
//
//	esp_event_loop_init(system_event_cb, NULL);
//	eth_config_t config = DEFAULT_ETHERNET_PHY_CONFIG; // Set the PHY address in the example configuration
//	config.phy_addr = CONFIG_PHY_ADDRESS;
//	config.gpio_config = eth_gpio_config_rmii;
//	config.tcpip_input = tcpip_adapter_eth_input;
//#ifdef CONFIG_PHY_USE_POWER_PIN
//	// Replace the default 'power enable' function with an example-specific
//    // one that toggles a power GPIO.
//    config.phy_power_enable = phy_device_power_enable_via_gpio;
//#endif
//	ret = esp_eth_init(&config); // Init the ETH interface of ESP32
//
//	if (ret == ESP_OK) {
//		ESP_LOGI(TAG, "ETH interface get inited correctly!!!");
//		esp_eth_enable();
//	} else {
//		ESP_LOGE(TAG, "ETH interface inited not correct!");
//	}
//
//}