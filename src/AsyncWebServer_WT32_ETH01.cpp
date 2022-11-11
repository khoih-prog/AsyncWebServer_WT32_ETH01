/****************************************************************************************************************************
  AsyncWebServer_WT32_ETH01.cpp - Dead simple Ethernet AsyncWebServer.

  For LAN8720 Ethernet in WT32_ETH01 (ESP32 + LAN8720)

  AsyncWebServer_WT32_ETH01 is a library for the Ethernet LAN8720 in WT32_ETH01 to run AsyncWebServer

  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_WT32_ETH01
  Licensed under GPLv3 license

  Original author: Hristo Gochkov

  Copyright (c) 2016 Hristo Gochkov. All rights reserved.

  This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with this library;
  if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Version: 1.6.2

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.2.3   K Hoang      17/07/2021 Initial porting for WT32_ETH01 (ESP32 + LAN8720). Sync with ESPAsyncWebServer v1.2.3
  1.2.4   K Hoang      02/08/2021 Fix Mbed TLS compile error with ESP32 core v2.0.0-rc1+
  1.2.5   K Hoang      09/10/2021 Update `platform.ini` and `library.json`Working only with core v1.0.6-
  1.3.0   K Hoang      23/10/2021 Making compatible with breaking core v2.0.0+
  1.4.0   K Hoang      27/11/2021 Auto detect ESP32 core version
  1.4.1   K Hoang      29/11/2021 Fix bug in examples to reduce connection time
  1.5.0   K Hoang      01/10/2022 Fix AsyncWebSocket bug
  1.6.0   K Hoang      04/10/2022 Option to use cString instead of String to save Heap
  1.6.1   K Hoang      05/10/2022 Don't need memmove(), String no longer destroyed
  1.6.2   K Hoang      10/11/2022 Add examples to demo how to use beginChunkedResponse() to send in chunks
 *****************************************************************************************************************************/

#include "AsyncWebServer_WT32_ETH01.h"

bool WT32_ETH01_eth_connected = false;

/////////////////////////////////////////////////

void WT32_ETH01_onEvent()
{
  WiFi.onEvent(WT32_ETH01_event);
}

/////////////////////////////////////////////////

void WT32_ETH01_waitForConnect()
{
  while (!WT32_ETH01_eth_connected)
    delay(100);
}

/////////////////////////////////////////////////

bool WT32_ETH01_isConnected()
{
  return WT32_ETH01_eth_connected;
}

/////////////////////////////////////////////////

void WT32_ETH01_event(WiFiEvent_t event)
{
  switch (event)
  {
#if ( ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) ) && ( ARDUINO_ESP32_GIT_VER != 0x46d5afb1 ) )

    // For breaking core v2.0.0
    // Why so strange to define a breaking enum arduino_event_id_t in WiFiGeneric.h
    // compared to the old system_event_id_t, now in tools/sdk/esp32/include/esp_event/include/esp_event_legacy.h
    // You can preserve the old enum order and just adding new items to do no harm
    case ARDUINO_EVENT_ETH_START:
      AWS_LOG(F("\nETH Started"));
      //set eth hostname here
      ETH.setHostname("WT32-ETH01");
      break;

    case ARDUINO_EVENT_ETH_CONNECTED:
      AWS_LOG(F("ETH Connected"));
      break;

    case ARDUINO_EVENT_ETH_GOT_IP:
      if (!WT32_ETH01_eth_connected)
      {
        AWS_LOG3(F("ETH MAC: "), ETH.macAddress(), F(", IPv4: "), ETH.localIP());

        if (ETH.fullDuplex())
        {
          AWS_LOG0(F("FULL_DUPLEX, "));
        }
        else
        {
          AWS_LOG0(F("HALF_DUPLEX, "));
        }

        AWS_LOG1(ETH.linkSpeed(), F("Mbps"));

        WT32_ETH01_eth_connected = true;
      }

      break;

    case ARDUINO_EVENT_ETH_DISCONNECTED:
      AWS_LOG("ETH Disconnected");
      WT32_ETH01_eth_connected = false;
      break;

    case ARDUINO_EVENT_ETH_STOP:
      AWS_LOG("\nETH Stopped");
      WT32_ETH01_eth_connected = false;
      break;

#else

    // For old core v1.0.6-
    // Core v2.0.0 defines a stupid enum arduino_event_id_t, breaking any code for WT32_ETH01 written for previous core
    // Why so strange to define a breaking enum arduino_event_id_t in WiFiGeneric.h
    // compared to the old system_event_id_t, now in tools/sdk/esp32/include/esp_event/include/esp_event_legacy.h
    // You can preserve the old enum order and just adding new items to do no harm
    case SYSTEM_EVENT_ETH_START:
      AWS_LOG(F("\nETH Started"));
      //set eth hostname here
      ETH.setHostname("WT32-ETH01");
      break;

    case SYSTEM_EVENT_ETH_CONNECTED:
      AWS_LOG(F("ETH Connected"));
      break;

    case SYSTEM_EVENT_ETH_GOT_IP:
      if (!WT32_ETH01_eth_connected)
      {
        AWS_LOG3(F("ETH MAC: "), ETH.macAddress(), F(", IPv4: "), ETH.localIP());

        if (ETH.fullDuplex())
        {
          AWS_LOG0(F("FULL_DUPLEX, "));
        }
        else
        {
          AWS_LOG0(F("HALF_DUPLEX, "));
        }

        AWS_LOG1(ETH.linkSpeed(), F("Mbps"));

        WT32_ETH01_eth_connected = true;
      }

      break;

    case SYSTEM_EVENT_ETH_DISCONNECTED:
      AWS_LOG("ETH Disconnected");
      WT32_ETH01_eth_connected = false;
      break;

    case SYSTEM_EVENT_ETH_STOP:
      AWS_LOG("\nETH Stopped");
      WT32_ETH01_eth_connected = false;
      break;
#endif

    default:
      break;
  }
}


