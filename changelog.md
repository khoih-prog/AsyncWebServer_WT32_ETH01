# AsyncWebServer_WT32_ETH01

[![arduino-library-badge](https://www.ardu-badge.com/badge/AsyncWebServer_WT32_ETH01.svg?)](https://www.ardu-badge.com/AsyncWebServer_WT32_ETH01)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/AsyncWebServer_WT32_ETH01.svg)](https://github.com/khoih-prog/AsyncWebServer_WT32_ETH01/releases)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/AsyncWebServer_WT32_ETH01.svg)](http://github.com/khoih-prog/AsyncWebServer_WT32_ETH01/issues)

<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Donate to my libraries using BuyMeACoffee" style="height: 50px !important;width: 181px !important;" ></a>
<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://img.shields.io/badge/buy%20me%20a%20coffee-donate-orange.svg?logo=buy-me-a-coffee&logoColor=FFDD00" style="height: 20px !important;width: 200px !important;" ></a>
<a href="https://profile-counter.glitch.me/khoih-prog/count.svg" title="Total Visitor count"><img src="https://profile-counter.glitch.me/khoih-prog/count.svg" style="height: 30px;width: 200px;"></a>
<a href="https://profile-counter.glitch.me/khoih-prog-AsyncWebServer_WT32_ETH01/count.svg" title="Visitor count"><img src="https://profile-counter.glitch.me/khoih-prog-AsyncWebServer_WT32_ETH01/count.svg" style="height: 30px;width: 200px;"></a>

---
---

## Table of contents

* [Changelog](#changelog)
  * [Releases v1.6.2](#releases-v162)
  * [Releases v1.6.1](#releases-v161)
  * [Releases v1.6.0](#releases-v160)
  * [Releases v1.5.0](#releases-v150)
  * [Releases v1.4.1](#releases-v141)
  * [Releases v1.4.0](#releases-v140)
  * [Releases v1.3.0](#releases-v130)
  * [Releases v1.2.5](#releases-v125)
  * [Releases v1.2.4](#releases-v124)
  * [Releases v1.2.3](#releases-v123)


---
---

## Changelog

#### Releases v1.6.2

1. Add examples [Async_AdvancedWebServer_SendChunked](https://github.com/khoih-prog/AsyncWebServer_RP2040W/tree/main/examples/Async_AdvancedWebServer_SendChunked) and [AsyncWebServer_SendChunked](https://github.com/khoih-prog/AsyncWebServer_RP2040W/tree/main/examples/AsyncWebServer_SendChunked) to demo how to use `beginChunkedResponse()` to send large `html` in chunks
2. Use `allman astyle` and add `utils`

#### Releases v1.6.1

1. Don't need `memmove()`, CString no longer destroyed. Check [All memmove() removed - string no longer destroyed #11](https://github.com/khoih-prog/Portenta_H7_AsyncWebServer/pull/11)

#### Releases v1.6.0

1. Support using `CString` to save heap to send `very large data`. Check [request->send(200, textPlainStr, jsonChartDataCharStr); - Without using String Class - to save heap #8](https://github.com/khoih-prog/Portenta_H7_AsyncWebServer/pull/8)
2. Add multiple examples to demo the new feature

#### Releases v1.5.0

1. Fix AsyncWebSocket bug

#### Releases v1.4.1

1. Fix bug in examples to reduce connection time

#### Releases v1.4.0

##### Warning: Releases v1.4.0+ can be used and autodetect ESP32 core v2.0.0+ or v1.0.6-.

1. Auto detect ESP32 core
2. Renew examples
3. Display compiler `#warning` only when DEBUG_LEVEL is 3+

#### Releases v1.3.0

##### Warning: Releases v1.3.0+ can be used for either ESP32 core v2.0.0+ or v1.0.6-. Default is v2.0.0+ now

1. Making compatible either with breaking core v2.0.0+ or core v1.0.6-
2. Renew examples


### Releases v1.2.5

##### Warning: Releases v1.2.5- to be used for ESP32 core v1.0.6- only

1. Update `platform.ini` and `library.json` to use original `khoih-prog` instead of `khoih.prog` after PIO fix


#### Releases v1.2.4

1. Fix library compile error with ESP32 core v2.0.0-rc1+. Check [Fix compiler error for ESP32-C3 and mbed TLS v2.7.0+ #970](https://github.com/me-no-dev/ESPAsyncWebServer/pull/970)
2. Verify compatibility with new ESP32 core v2.0.0-rc1+


#### Releases v1.2.3

1. Initial coding to port [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) to WT32_ETH01 boards using builtin LAN8720A Ethernet.
2. Add more examples.
3. Add debugging features.
4. Bump up to v1.2.3 to sync with [ESPAsyncWebServer v1.2.3](https://github.com/me-no-dev/ESPAsyncWebServer).


