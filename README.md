# Efficent Encoder & Decoder + payload format

- ### The following format descripes the organization of the data sent on network
- ### it's flexibility is also put in plan
- ### maximum payload size is 250 bytes

| order | name | desciption | data | data types | size | comment | 
| --- | :--- | :--- | :---: | :---: | --- | :--- |
| 1 | time_stamp | time stamp | hh:mm:ss.sss | uint, uint, uint, uint16 | 5, 6, 6, 10 bits | [get high clock precision](https://stackoverflow.com/a/72409769/3168875) |
| 2 | Location | GPS location | lat, lon | double, double | 8, 8 bytes | <ul><li> Valid longitudes are from -180 to 180 degrees <li>Valid latitudes are from -85.05112878 to 85.05112878 degrees|
| 3 | heading |  | degrees relative to 0N | short int | 2 bytes | | 
| 4 | speed | reading of speedometer/GPS | speed m/s | int | 2 byte | data type may change later for higher presicion if needed |
| 5 | brakes | reading of brakes sensor | brakes | uint8 | 1 bit | data size depends on existance of other bitfield data |
| x | ~~FCS~~ | ~~Frame Check Sequence~~ | | | ~~4 bytes~~ | many frame error detection algorithms; already implemented in ESP-NOW data frame; no need to implement in the payload |

- # helpful links
  - https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html
  - https://mathiasbynens.be/demo/integer-range
  - https://www.electronicwings.com/sensors-modules/gps-receiver-module
  - https://stackoverflow.com/a/16743805/3168875 (long/lat distance scales)
  
