/*!
 * @file    networking_eth.h
 * @brief   Ethernet initialization
 *
 * ...
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
#ifndef UBIRCH_NETWORKING_ETH_H
#define UBIRCH_NETWORKING_ETH_H

#include <esp_err.h>

/*!
 * Initialize the ethernet adapter.
 * @return ESP_OK or error code.
 */
esp_err_t init_ethernet();

#endif //UBIRCH_NETWORKING_ETH_H
