/*!
 * @file    sntp_time.c
 * @brief   Synchronize the system clock with the SNTP time and provide
 *          functions to read the current time
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

#include <esp_log.h>
#include <lwip/apps/sntp.h>
#include "sntp_time.h"

#define TAG "SNTP"

static int initialized = 0;

void sntp_update(void) {
    if (!initialized) {
        ESP_LOGD(TAG, "initializing");
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, "pool.ntp.org");
        sntp_init();
        initialized = 1;
    }

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 20;
    while (timeinfo.tm_year < (2017 - 1900) && ++retry < retry_count) {
        ESP_LOGI(TAG, "waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }
    ESP_LOGI(TAG, "TIME = %02d.%02d.%04d %02d:%02d:%02d UTC\r\n",
             timeinfo.tm_mday, timeinfo.tm_mon + 1, (1900 + timeinfo.tm_year),
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    setenv("TZ", "UTC", 1);
    tzset();
}