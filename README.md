# Efficent Encoder & Decoder + payload format

- ### The following format descripes the organization of the data sent on network
- ### it's flexibility is also put in plan

| order | name | desciption | data | data types | size | comment | 
| --- | :--- | :--- | :---: | :---: | --- | :--- |
| 1 | time_stamp | time stamp | hh:mm:ss.sss | uint8, uint8, uint8, uint16 | 1, 1, 1, 2 bytes | [get high clock precision](https://stackoverflow.com/a/72409769/3168875), may later be replaced with bitfield values for compression and throughput efficency  |
| 2 | Location | GPS location | longitude, latitude | float, float | 4, 4 bytes | |
| 3 | Acceleration | Acceleration readings from GPS sensor | acc_Lat, acc_LONG | float, float | 4, 4 bytes | | 
| 4 | speed | reading of speedometer | speed m/s | uint8 | 1 | data type may be for higher presicion when needed |
| 5 | brakes | reading of brakes sensor | brakes | bool | 1 bit #bitfield or uint8 | data size depends on existance of other bitfield data |
   