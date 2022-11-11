/****************************************************************************************************************************
  AsyncMultiWebServer_WT32_ETH01.h - Dead simple AsyncWebServer for WT32_ETH01

  For LAN8720 Ethernet in WT32_ETH01 (ESP32 + LAN8720)

  AsyncWebServer_WT32_ETH01 is a library for the Ethernet LAN8720 in WT32_ETH01 to run AsyncWebServer

  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_WT32_ETH01
  Licensed under GPLv3 license
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

unsigned int    analogReadPin []  = { 12, 13, 14 };

#define BUFFER_SIZE       500

#define HTTP_PORT1        8080
#define HTTP_PORT2        8081
#define HTTP_PORT3        8082

AsyncWebServer* server1;
AsyncWebServer* server2;
AsyncWebServer* server3;

AsyncWebServer*  multiServer  []  = { server1, server2, server3 };
uint16_t        http_port     []  = { HTTP_PORT1, HTTP_PORT2, HTTP_PORT3 };

#define NUM_SERVERS     ( sizeof(multiServer) / sizeof(AsyncWebServer*) )

unsigned int serverIndex;

String createBuffer()
{
	char temp[BUFFER_SIZE];

	memset(temp, 0, sizeof(temp));

	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;
	int day = hr / 24;

	snprintf(temp, BUFFER_SIZE - 1,
	         "<html>\
<head>\
<meta http-equiv='refresh' content='5'/>\
<title>%s</title>\
<style>\
body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
</style>\
</head>\
<body>\
<h1>Hello from %s</h1>\
<h2>running AsyncMultiWebServer_WT32_ETH01</h2>\
<h3>on %s</h3>\
<h3>Uptime: %d d %02d:%02d:%02d</h3>\
</body>\
</html>", BOARD_NAME, BOARD_NAME, SHIELD_TYPE, day, hr, min % 60, sec % 60);

	return temp;
}


void handleRoot(AsyncWebServerRequest * request)
{
	String message = createBuffer();
	request->send(200, F("text/html"), message);
}

String createNotFoundBuffer(AsyncWebServerRequest * request)
{
	String message;

	message.reserve(500);

	message = F("File Not Found\n\n");

	message += F("URI: ");
	message += request->url();
	message += F("\nMethod: ");
	message += (request->method() == HTTP_GET) ? F("GET") : F("POST");
	message += F("\nArguments: ");
	message += request->args();
	message += F("\n");

	for (uint8_t i = 0; i < request->args(); i++)
	{
		message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
	}

	return message;
}

void handleNotFound(AsyncWebServerRequest * request)
{
	String message = createNotFoundBuffer(request);
	request->send(404, F("text/plain"), message);
}

void setup()
{
	Serial.begin(115200);

	while (!Serial && millis() < 5000);

	delay(200);

	Serial.print(F("\nStart AsyncMultiWebServer_WT32_ETH01 on "));
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

	Serial.print("\nConnected to network. IP = ");
	Serial.println(ETH.localIP());

	for (serverIndex = 0; serverIndex < NUM_SERVERS; serverIndex++)
	{
		multiServer[serverIndex] = new AsyncWebServer(http_port[serverIndex]);

		if (multiServer[serverIndex])
		{
			Serial.printf("Initialize multiServer OK, serverIndex = %d, port = %d\n", serverIndex, http_port[serverIndex]);
		}
		else
		{
			Serial.printf("Error initialize multiServer, serverIndex = %d\n", serverIndex);

			while (1);
		}

		multiServer[serverIndex]->on("/", HTTP_GET, [](AsyncWebServerRequest * request)
		{
			handleRoot(request);
		});

		multiServer[serverIndex]->on("/hello", HTTP_GET, [](AsyncWebServerRequest * request)
		{
			String message = F("Hello from AsyncMultiWebServer_WT32_ETH01 using LAN8720 Ethernet, running on ");
			message       += BOARD_NAME;

			request->send(200, "text/plain", message);
		});

		multiServer[serverIndex]->onNotFound([](AsyncWebServerRequest * request)
		{
			handleNotFound(request);
		});

		multiServer[serverIndex]->begin();

		Serial.printf("HTTP server started at ports %d\n", http_port[serverIndex]);
	}
}

void loop()
{
}
