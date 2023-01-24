# Efficent Encoder & Decoder + payload format

- ### The following format descripes the organization of the data sent on network
- ### it's flexibility is also put in plan
- ### maximum payload size is 250 bytes

| order | name | desciption | data | data types | size | comment | 
| --- | :--- | :--- | :---: | :---: | --- | :--- |
| 1 | time_stamp | time stamp | hh:mm:ss.sss | uint, uint, uint, uint16 | 5, 6, 6, 16 bits | [get high clock precision](https://stackoverflow.com/a/72409769/3168875), may later be replaced with bitfield values for compression and throughput efficency  |
| 2 | Location | GPS location | lat, lat_frac, long, long_frac | uint8, uint16, uint16, uint16 | 1, 2, 2, 2 bytes | <ul><li> Valid longitudes are from -180 to 180 degrees <li>Valid latitudes are from -85.05112878 to 85.05112878 degrees|
| 3 | heading |  | degrees relative to 0N | uint | 9 bits | | 
| 4 | speed | reading of speedometer/GPS | speed m/s | uint8 | 1 byte | data type may change later for higher presicion if needed |
| 5 | brakes | reading of brakes sensor | brakes | bool | 1 bit | data size depends on existance of other bitfield data |
| 6 | gps_updated | bool to indicated if gps data in payload is new or repeated (gps low update rate)| brakes | bool | 1 bit | data size depends on existance of other bitfield data |
| x | ~~FCS~~ | ~~Frame Check Sequence~~ | | | ~~4 bytes~~ | many frame error detection algorithm; already implemented in ESP-NOW data frame; no need to implement in the payload |
| total | | | | | 101/104 bits = 13 bytes| |

- # helpful links
  - https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html
  - https://mathiasbynens.be/demo/integer-range
  - https://www.electronicwings.com/sensors-modules/gps-receiver-module
  - https://stackoverflow.com/a/16743805/3168875 (long/lat distance scales)
  