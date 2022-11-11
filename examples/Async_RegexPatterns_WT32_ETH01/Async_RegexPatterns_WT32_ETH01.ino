/****************************************************************************************************************************
  Async_RegexPatterns_WT32_ETH01.ino - Dead simple AsyncWebServer for WT32_ETH01

  For LAN8720 Ethernet in WT32_ETH01 (ESP32 + LAN8720)

  AsyncWebServer_WT32_ETH01 is a library for the Ethernet LAN8720 in WT32_ETH01 to run AsyncWebServer

  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_WT32_ETH01
  Licensed under GPLv3 license
 *****************************************************************************************************************************/
//
// A simple server implementation with regex routes:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//
// Add buildflag ASYNCWEBSERVER_REGEX to enable the regex support
//
// For platformio: platformio.ini:
//  build_flags =
//      -DASYNCWEBSERVER_REGEX
//
// For arduino IDE: create/update platform.local.txt
// Windows: C:\Users\(username)\AppData\Local\Arduino15\packages\espxxxx\hardware\espxxxx\{version}\platform.local.txt
// Linux: ~/.arduino15/packages/espxxxx/hardware/espxxxx/{version}/platform.local.txt
//
// compiler.cpp.extra_flags=-DASYNCWEBSERVER_REGEX=1

#define ASYNCWEBSERVER_REGEX      true

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

AsyncWebServer    server(80);

const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request)
{
	request->send(404, "text/plain", "Not found");
}

void setup()
{
	Serial.begin(115200);

	while (!Serial && millis() < 5000);

	Serial.print(F("\nStart Async_RegexPatterns_WT32_ETH01 on "));
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

	server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
	{
		request->send(200, "text/plain", "Hello, world from Async_RegexPatterns_WT32_ETH01 on " + String(BOARD_NAME));
	});

	// Send a GET request to <IP>/sensor/<number>
	server.on("^\\/sensor\\/([0-9]+)$", HTTP_GET, [] (AsyncWebServerRequest * request)
	{
		String sensorNumber = request->pathArg(0);
		request->send(200, "text/plain", "Hello, sensor: " + sensorNumber);
	});

	// Send a GET request to <IP>/sensor/<number>/action/<action>
	server.on("^\\/sensor\\/([0-9]+)\\/action\\/([a-zA-Z0-9]+)$", HTTP_GET, [] (AsyncWebServerRequest * request)
	{
		String sensorNumber = request->pathArg(0);
		String action = request->pathArg(1);
		request->send(200, "text/plain", "Hello, sensor: " + sensorNumber + ", with action: " + action);
	});

	server.onNotFound(notFound);

	server.begin();

	Serial.print("Server started @ ");
	Serial.println(ETH.localIP());
}

void loop()
{
}
