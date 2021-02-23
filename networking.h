/*!
 * @file    networking.h
 * @brief   Networking functionality.
 *
 * Provides access to Wifi and Ethernet functionality.
 *
 * @author Matthias L. Jugel
 * @date   2018-11-29
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
#ifndef UBIRCH_NETWORKING_H
#define UBIRCH_NETWORKING_H

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

#define NETWORK_STA_READY   BIT1 // todo cleanup
#define NETWORK_ETH_READY   BIT2
#define NETWORK_GSM_READY   BIT3

#define WIFI_CONNECTED_BIT BIT4
#define WIFI_FAIL_BIT      BIT5

extern EventGroupHandle_t network_event_group;

#include "networking_wifi.h"
#include "networking_eth.h"

#endif //UBIRCH_NETWORKING_H
