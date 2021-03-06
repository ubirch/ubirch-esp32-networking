/*!
 * @file    sntp_time.h
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


#ifndef UBIRCH_SNTP_TIME_H
#define UBIRCH_SNTP_TIME_H

/*!
 * Get the time from NTP server and set up RTC.
 *
 * @note    This function tries to connect to the sntp server for 10 times and wait 1 second in between.
 */
void sntp_update(void);

#endif /* UBIRCH_SNTP_TIME_H */