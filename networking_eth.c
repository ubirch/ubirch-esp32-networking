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

    tcpip_adapter_init();
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

