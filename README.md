![ubirch logo](https://ubirch.de/wp-content/uploads/2018/10/cropped-uBirch_Logo.png)

# ESP32 combined networking component

This component combines some networking functionality commonly found on the boards.

## Prerequisits

The following components are required for the functionality, see also
[CMakeLists.txt](https://github.com/ubirch/ubirch-esp32-networking/blob/master/CMakeLists.txt)

- [ethernet](https://github.com/espressif/esp-idf/tree/master/components/ethernet)

## Usage

If an ethernet connection is available, call `init_ethernet()` to initialize it.
Afterwards, all network related functions can be called from different modules.
> **Note:** some ethernet related [pins](https://github.com/ubirch/ubirch-esp32-networking/blob/master/networking_eth.c#L34-L36)
might have to be reconfigured.

To initialize a wifi network, call `init_wifi()`.
To Connect to the wifi network call `wifi_join("WIFI_SSID","WIFI_PWD")`.

To update the time from the network, call `sntp_update()`.







