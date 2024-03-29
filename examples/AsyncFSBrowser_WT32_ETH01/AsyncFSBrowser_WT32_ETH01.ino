/****************************************************************************************************************************
  AsyncFSBrowser_WT32_ETH01.ino - Dead simple AsyncWebServer for WT32_ETH01

  For LAN8720 Ethernet in WT32_ETH01 (ESP32 + LAN8720)

  AsyncWebServer_WT32_ETH01 is a library for the Ethernet LAN8720 in WT32_ETH01 to run AsyncWebServer

  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_WT32_ETH01
  Licensed under GPLv3 license
 *****************************************************************************************************************************/

#if !( defined(ESP32) )
	#error This code is designed for WT32_ETH01 to run on ESP32 platform! Please check your Tools->Board setting.
#else
	#include "mbedtls/version.h"

	#if (MBEDTLS_VERSION_NUMBER >= 0x02070000)
		#error Must use ESP32 core v1.0.6-, with MBEDTLS_VERSION_NUMBER < v2.7.0
	#endif

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

// SKETCH BEGIN
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
{
	if (type == WS_EVT_CONNECT)
	{
		Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());

		client->printf("Hello Client %u :)", client->id());
		client->ping();
	}
	else if (type == WS_EVT_DISCONNECT)
	{
		Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
	}
	else if (type == WS_EVT_ERROR)
	{
		Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
	}
	else if (type == WS_EVT_PONG)
	{
		Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char*)data : "");
	}
	else if (type == WS_EVT_DATA)
	{
		AwsFrameInfo * info = (AwsFrameInfo*)arg;
		String msg = "";

		if (info->final && info->index == 0 && info->len == len)
		{
			//the whole message is in a single frame and we got all of it's data
			Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);

			if (info->opcode == WS_TEXT)
			{
				for (size_t i = 0; i < info->len; i++)
				{
					msg += (char) data[i];
				}
			}
			else
			{
				char buff[6];

				for (size_t i = 0; i < info->len; i++)
				{
					sprintf(buff, "%02x ", (uint8_t) data[i]);
					msg += buff ;
				}
			}

			Serial.printf("%s\n", msg.c_str());

			if (info->opcode == WS_TEXT)
				client->text("I got your text message");
			else
				client->binary("I got your binary message");
		}
		else
		{
			//message is comprised of multiple frames or the frame is split into multiple packets

			if (info->index == 0)
			{
				if (info->num == 0)
					Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");

				Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
			}

			Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);

			if (info->opcode == WS_TEXT)
			{
				for (size_t i = 0; i < len; i++)
				{
					msg += (char) data[i];
				}
			}
			else
			{
				char buff[6];

				for (size_t i = 0; i < len; i++)
				{
					sprintf(buff, "%02x ", (uint8_t) data[i]);
					msg += buff ;
				}
			}

			Serial.printf("%s\n", msg.c_str());

			if ((info->index + len) == info->len)
			{
				Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);

				if (info->final)
				{
					Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");

					if (info->message_opcode == WS_TEXT)
						client->text("I got your text message");
					else
						client->binary("I got your binary message");
				}
			}
		}
	}
}

const char * hostName = "esp-async";
const char* http_username = "admin";
const char* http_password = "admin";

void setup()
{
	Serial.begin(115200);

	while (!Serial && millis() < 5000);

	delay(200);

	Serial.print(F("\nStart AsyncFSBrowser_WT32_ETH01 on "));
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

	ws.onEvent(onWsEvent);
	server.addHandler(&ws);

	events.onConnect([](AsyncEventSourceClient * client)
	{
		client->send("hello!", NULL, millis(), 1000);
	});

	server.addHandler(&events);

	server.on("/board", HTTP_GET, [](AsyncWebServerRequest * request)
	{
		request->send(200, "text/plain", String(BOARD_NAME));
	});

	server.onNotFound([](AsyncWebServerRequest * request)
	{
		Serial.printf("NOT_FOUND: ");

		if (request->method() == HTTP_GET)
			Serial.printf("GET");
		else if (request->method() == HTTP_POST)
			Serial.printf("POST");
		else if (request->method() == HTTP_DELETE)
			Serial.printf("DELETE");
		else if (request->method() == HTTP_PUT)
			Serial.printf("PUT");
		else if (request->method() == HTTP_PATCH)
			Serial.printf("PATCH");
		else if (request->method() == HTTP_HEAD)
			Serial.printf("HEAD");
		else if (request->method() == HTTP_OPTIONS)
			Serial.printf("OPTIONS");
		else
			Serial.printf("UNKNOWN");

		Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

		if (request->contentLength())
		{
			Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
			Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
		}

		int headers = request->headers();
		int i;

		for (i = 0; i < headers; i++)
		{
			AsyncWebHeader* h = request->getHeader(i);
			Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
		}

		int params = request->params();

		for (i = 0; i < params; i++)
		{
			AsyncWebParameter* p = request->getParam(i);

			if (p->isPost())
			{
				Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
			}
			else
			{
				Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
			}
		}

		request->send(404);
	});

	server.onRequestBody([](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total)
	{
		(void) request;

		if (!index)
			Serial.printf("BodyStart: %u\n", total);

		Serial.printf("%s", (const char*)data);

		if (index + len == total)
			Serial.printf("BodyEnd: %u\n", total);
	});

	server.begin();

	Serial.print("Server started @ ");
	Serial.println(ETH.localIP());
}

void loop()
{
	ws.cleanupClients();
}
