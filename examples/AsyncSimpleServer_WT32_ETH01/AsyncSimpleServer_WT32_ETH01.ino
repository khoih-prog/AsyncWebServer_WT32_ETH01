/****************************************************************************************************************************
  AsyncSimpleServer_WT32_ETH01.ino - Dead simple Ethernet AsyncWebServer.

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
 *****************************************************************************************************************************/

#if !( defined(ESP32) )
	#error This code is designed for WT32_ETH01 to run on ESP32 platform! Please check your Tools->Board setting.
#endif

#include <Arduino.h>

#define _ASYNC_WEBSERVER_LOGLEVEL_       2

// Select the IP address according to your local network
IPAddress myIP(192, 168, 2, 232);
IPAddress myGW(192, 168, 2, 1);
IPAddress mySN(255, 255, 255, 0);

// Google DNS Server IP
IPAddress myDNS(8, 8, 8, 8);

#include <AsyncTCP.h>

#include <AsyncWebServer_WT32_ETH01.h>

AsyncWebServer server(80);

const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request)
{
	request->send(404, "text/plain", "Not found");
}

void setup()
{
	Serial.begin(115200);

	while (!Serial && millis() < 5000);

	Serial.print(F("\nStart AsyncSimpleServer_WT32_ETH01 on "));
	Serial.print(BOARD_NAME);
	Serial.print(F(" with "));
	Serial.println(SHIELD_TYPE);
	Serial.println(ASYNC_WEBSERVER_WT32_ETH01_VERSION);

	// To be called before ETH.begin()
	WT32_ETH01_onEvent();

	//bool begin(uint8_t phy_addr=ETH_PHY_ADDR, int power=ETH_PHY_POWER, int mdc=ETH_PHY_MDC, int mdio=ETH_PHY_MDIO,
	//           eth_phy_type_t type=ETH_PHY_TYPE, eth_clock_mode_t clk_mode=ETH_CLK_MODE);
	//ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_TYPE, ETH_CLK_MODE);
	ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER);

	// Static IP, leave without this line to get IP via DHCP
	//bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = 0, IPAddress dns2 = 0);
	ETH.config(myIP, myGW, mySN, myDNS);

	WT32_ETH01_waitForConnect();

	IPAddress localIP = ETH.localIP();

	Serial.print(F("IP address: "));
	Serial.println(localIP);

	server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
	{
		request->send(200, "text/plain", "Hello, world from AsyncSimpleServer_WT32_ETH01");
	});

	// Send a GET request to <IP>/get?message=<message>
	server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request)
	{
		String message;

		if (request->hasParam(PARAM_MESSAGE))
		{
			message = request->getParam(PARAM_MESSAGE)->value();
		}
		else
		{
			message = "No message sent";
		}

		request->send(200, "text/plain", "Hello, GET: " + message);
	});

	// Send a POST request to <IP>/post with a form field message set to <message>
	server.on("/post", HTTP_POST, [](AsyncWebServerRequest * request)
	{
		String message;

		if (request->hasParam(PARAM_MESSAGE, true))
		{
			message = request->getParam(PARAM_MESSAGE, true)->value();
		}
		else
		{
			message = "No message sent";
		}

		request->send(200, "text/plain", "Hello, POST: " + message);
	});

	server.onNotFound(notFound);

	server.begin();
}

void loop()
{
}
