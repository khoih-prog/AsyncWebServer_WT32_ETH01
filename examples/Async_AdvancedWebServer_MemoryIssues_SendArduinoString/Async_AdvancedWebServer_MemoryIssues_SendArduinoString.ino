/****************************************************************************************************************************
  Async_AdvancedWebServer_MemoryIssues_SendArduinoString.ino -  Dead simple AsyncWebServer for WT32_ETH01

  For LAN8720 Ethernet in WT32_ETH01 (ESP32 + LAN8720)

  AsyncWebServer_WT32_ETH01 is a library for the Ethernet LAN8720 in WT32_ETH01 to run AsyncWebServer

  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_WT32_ETH01
  Licensed under GPLv3 license

  Copyright (c) 2015, Majenko Technologies
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:

  Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

  Neither the name of Majenko Technologies nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

// In bytes
#define STRING_SIZE                    40000

AsyncWebServer    server(80);

int reqCount = 0;                // number of requests received

#define BUFFER_SIZE         512
char temp[BUFFER_SIZE];

void handleRoot(AsyncWebServerRequest *request)
{
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;
	int day = hr / 24;

	snprintf(temp, BUFFER_SIZE - 1,
	         "<html>\
<head>\
<meta http-equiv='refresh' content='60'/>\
<title>AsyncWebServer-%s</title>\
<style>\
body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
</style>\
</head>\
<body>\
<h2>AsyncWebServer_WT32_ETH01!</h2>\
<h3>running on %s</h3>\
<p>Uptime: %d d %02d:%02d:%02d</p>\
<img src=\"/test.svg\" />\
</body>\
</html>", BOARD_NAME, BOARD_NAME, day, hr % 24, min % 60, sec % 60);

	request->send(200, "text/html", temp);
}

void handleNotFound(AsyncWebServerRequest *request)
{
	String message = "File Not Found\n\n";

	message += "URI: ";
	message += request->url();
	message += "\nMethod: ";
	message += (request->method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += request->args();
	message += "\n";

	for (uint8_t i = 0; i < request->args(); i++)
	{
		message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
	}

	request->send(404, "text/plain", message);
}

void PrintHeapData(String hIn)
{
	static uint32_t maxFreeHeap = 0xFFFFFFFF;
	static uint32_t totalHeap = ESP.getHeapSize();

	uint32_t freeHeap  = ESP.getFreeHeap();

	// Print and update only when larger heap
	if (maxFreeHeap > freeHeap)
	{
		maxFreeHeap = freeHeap;

		Serial.print("\nHEAP DATA - ");
		Serial.print(hIn);

		Serial.print("  Max heap: ");
		Serial.print(totalHeap);
		Serial.print("  Free heap: ");
		Serial.print(ESP.getFreeHeap());
		Serial.print("  Used heap: ");
		Serial.println(totalHeap - freeHeap);
	}
}

void PrintStringSize(String & out)
{
	static uint32_t count = 0;

	// Print only when cStr length too large and corrupting memory or every (20 * 5) s
	if ( (out.length() >= STRING_SIZE) || (++count > 20) )
	{
		Serial.print("\nOut String Length=");
		Serial.println(out.length());

		count = 0;
	}
}

void drawGraph(AsyncWebServerRequest *request)
{
	String out;

	out.reserve(STRING_SIZE);
	char temp[70];

	out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"1810\" height=\"150\">\n";
	out += "<rect width=\"1810\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"2\" stroke=\"rgb(0, 0, 0)\" />\n";
	out += "<g stroke=\"blue\">\n";
	int y = rand() % 130;

	for (int x = 10; x < 5000; x += 10)
	{
		int y2 = rand() % 130;
		sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"2\" />\n", x, 140 - y, x + 10, 140 - y2);
		out += temp;
		y = y2;
	}

	out += "</g>\n</svg>\n";

	PrintHeapData("Pre Send");

	PrintStringSize(out);

	request->send(200, "image/svg+xml", out);

	PrintHeapData("Post Send");
}

void setup()
{
	Serial.begin(115200);

	while (!Serial && millis() < 5000);

	delay(200);

	Serial.print("\nStart Async_AdvancedWebServer_MemoryIssues_SendArduinoString on ");
	Serial.print(BOARD_NAME);
	Serial.print(" with ");
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

	///////////////////////////////////

	server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
	{
		handleRoot(request);
	});

	server.on("/test.svg", HTTP_GET, [](AsyncWebServerRequest * request)
	{
		drawGraph(request);
	});

	server.on("/inline", [](AsyncWebServerRequest * request)
	{
		request->send(200, "text/plain", "This works as well");
	});

	server.onNotFound(handleNotFound);

	server.begin();

	Serial.print(F("HTTP EthernetWebServer is @ IP : "));
	Serial.println(ETH.localIP());

	PrintHeapData("Pre Create Arduino String");
}

void heartBeatPrint()
{
	static int num = 1;

	Serial.print(F("."));

	if (num == 80)
	{
		Serial.println();
		num = 1;
	}
	else if (num++ % 10 == 0)
	{
		Serial.print(F(" "));
	}
}

void check_status()
{
	static unsigned long checkstatus_timeout = 0;

#define STATUS_CHECK_INTERVAL     10000L

	// Send status report every STATUS_REPORT_INTERVAL (60) seconds: we don't need to send updates frequently if there is no status change.
	if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
	{
		heartBeatPrint();
		checkstatus_timeout = millis() + STATUS_CHECK_INTERVAL;
	}
}

void loop()
{
	check_status();
}
