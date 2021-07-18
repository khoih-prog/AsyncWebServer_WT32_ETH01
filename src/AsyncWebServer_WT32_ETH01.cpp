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
  
  Version: 1.2.3

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.2.3   K Hoang      17/07/2021 Initial porting for WT32_ETH01 (ESP32 + LAN8720). Sync with ESPAsyncWebServer v1.2.3
 *****************************************************************************************************************************/

#include "AsyncWebServer_WT32_ETH01.h"

bool WT32_ETH01_eth_connected = false;

void WT32_ETH01_onEvent()
{
  WiFi.onEvent(WT32_ETH01_event);
}

void WT32_ETH01_waitForConnect()
{
  while (!WT32_ETH01_eth_connected)
    delay(100);
}

bool WT32_ETH01_isConnected()
{
  return WT32_ETH01_eth_connected;
}

void WT32_ETH01_event(WiFiEvent_t event)
{
  switch (event)
  {
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

    default:
      break;
  }
}


