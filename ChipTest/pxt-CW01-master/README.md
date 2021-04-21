[![GitHub Issues](https://img.shields.io/github/issues/xinabox/pxt-CW01.svg)](https://github.com/xinabox/pxt-CW01/issues) 
![GitHub Commit](https://img.shields.io/github/last-commit/xinabox/pxt-CW01) 
![Maintained](https://img.shields.io/maintenance/yes/2020) 
![Build status badge](https://github.com/xinabox/pxt-CW01/workflows/microbit/badge.svg)

# XinaBox CW01 MakeCode extension

This library provides functions to access environmental data from the [XinaBox CW01](https://xinabox.cc/products/cw01?_pos=1&_sid=130924612&_ss=r).

![](sw01.jpg)

[Read more about it or purchase one here](https://xinabox.cc/products/cw01?_pos=1&_sid=130924612&_ss=r)

The CW01 uses ESP8266 core to transimit and receive data.
This libray enables you connect to IoT clouds specifically with:
* AllThingsTalk
* Microsoft Azure
* Ubidots

There are functions to:
* Connect to Wi-Fi
* Connect to IoT platforms (ATT, Ubidots or Azure)
* Transmit data
* Receive data

## How-to guides:

A comprehensive set of How-to guides that show you how to use the blocks is available online:


## Core functions: Common:

```blocks
// Connect to WiFi
cw01.connectToWifi("SSID", "PSK")

```


## Core functions: AllThingsTalk:

```blocks
// Connect
cw01.connectToATT("TOKEN", "ID")

//Send string data
cw01.IoTSendStringToATT("string", "asset_name")

//Send numerical data
cw01P.IoTSendValueToATT(0, "asset_name")

//Send boolean data
cw01.IoTSendStateToATT(false, "asset_name")

//Get data
cw01.IoTgetATTAssetValue("asset_name")

```

## Core functions: Azure:

```blocks
// Connect
cw01.connectToAzure("access_endpoint")

//Send string data
cw01.IoTSendStringToAzure("variable_name", "string")

//Send numerical data
cw01.IoTSendValueToAzure("variable_name", 0)

//Send boolean data
cw01.IoTSendStateToAzure("variable_name", false)

//Get data
cw01.IoTGetValueFromAzure("variable_name")

```

## Core functions: Ubidots:

```blocks
// Connect to Industrial or Education account type
cw01.connectToUbidots(USER.INDUSTRIAL, "TOKEN")

//Send numerical data to variable in device. Select true to enter GPS location
cw01.IoTSendValueToUbidots(0, "device_api", "variable_api", false)

//Get data
cw01.IoTgetValuefromUbidots("device_api", "variable_api")

//Add GPS location, latitude and longitude
cw01.IoTaddLocation(0, 0)

```
  


## License:

MIT

Copyright (c) 2019, XinaBox Limited

## Supported targets:

* PXT/microbit


