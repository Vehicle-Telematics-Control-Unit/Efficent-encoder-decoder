# Efficent Encoder & Decoder + payload format

# Encoder/Decoder Software for Efficient Payload Communication

The **Encoder/Decoder Software** is a crucial component of our innovative project aimed at enhancing vehicle safety through real-time collision prediction and effective payload communication. This repository contains the source code and documentation for the Encoder/Decoder software, which plays a pivotal role in enabling vehicles to share essential information with their surroundings efficiently.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Payload Communication](#payload-communication)
- [Contributing](#contributing)
- [License](#license)

## Overview

While the Encoder/Decoder Software is not directly involved in the collision prediction process, it serves as a dynamic means of creating compact payloads and facilitating their exchange over various communication methods, including DSRC (Dedicated Short Range Communications) and other alternatives. This software is an essential link in the vehicle's communication network, ensuring seamless sharing of standardized payload structures.

## Features

- Payload creation and formatting: The Encoder/Decoder software efficiently structures the data to be shared into compact payloads, ensuring optimal use of available bandwidth.
- Payload sharing: The software facilitates the sharing of payloads over DSRC (ESP) and other communication methods, allowing vehicles to transmit and receive standardized messages.
- Versatile communication: The Encoder/Decoder software supports various communication methods, making it adaptable for integration into diverse transportation ecosystems.
- Simplified integration: The software can be seamlessly integrated into vehicle systems, providing a standardized interface for payload exchange.

## Usage

1. Integrate the Encoder/Decoder software into your vehicle's system (or any other system) to establish a communication link.
2. Configure (includes/payloads.h) to define the payload structures you wish to share.
3. in "main" function inside "encoder.cpp" configure your code to send this payload on event
4. in "on_payload_received" function inside "encoder.cpp" configure your code to receive this paylaod and decode it. you can see examples in "encoder.cpp" 
6. Monitor the software's performance and ensure smooth payload exchange.

## Payload Communication

The Encoder/Decoder Software excels in creating and sharing compact messages among vehicles. By using standardized payload structures, vehicles equipped with the Encoder/Decoder software can effortlessly exchange information that is vital for collaborative safety measures. This dynamic payload sharing enhances the overall effectiveness of the collision prediction system.

## License

This project is licensed under the [MIT License](LICENSE).

---

- ### The following format describes the organization of the data sent over DSRC(ESP)
- ### it's flexibility is also put in plan
- ### maximum payload size can't exceed 250 bytes

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
  

By [Ahmed Samir](https://github.com/princeofguilty) | Created 2023-MAY
