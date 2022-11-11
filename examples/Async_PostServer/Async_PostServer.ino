/****************************************************************************************************************************
  Async_PostServer.ino - Dead simple AsyncWebServer for WT32_ETH01

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

AsyncWebServer    server(80);

const int led = 2;

const String postForms =
  "<html>\
<head>\
<title>AsyncWebServer POST handling</title>\
<style>\
body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
</style>\
</head>\
<body>\
<h1>POST plain text to /postplain/</h1><br>\
<form method=\"post\" enctype=\"text/plain\" action=\"/postplain/\">\
<input type=\"text\" name=\'{\"hello\": \"world\", \"trash\": \"\' value=\'\"}\'><br>\
<input type=\"submit\" value=\"Submit\">\
</form>\
<h1>POST form data to /postform/</h1><br>\
<form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/postform/\">\
<input type=\"text\" name=\"hello\" value=\"world\"><br>\
<input type=\"submit\" value=\"Submit\">\
</form>\
</body>\
</html>";

void handleRoot(AsyncWebServerRequest *request)
{
	digitalWrite(led, 1);
	request->send(200, "text/html", postForms);
	digitalWrite(led, 0);
}

void handlePlain(AsyncWebServerRequest *request)
{
	if (request->method() != HTTP_POST)
	{
		digitalWrite(led, 1);
		request->send(405, "text/plain", "Method Not Allowed");
		digitalWrite(led, 0);
	}
	else
	{
		digitalWrite(led, 1);
		request->send(200, "text/plain", "POST body was:\n" + request->arg("plain"));
		digitalWrite(led, 0);
	}
}

void handleForm(AsyncWebServerRequest *request)
{
	if (request->method() != HTTP_POST)
	{
		digitalWrite(led, 1);
		request->send(405, "text/plain", "Method Not Allowed");
		digitalWrite(led, 0);
	}
	else
	{
		digitalWrite(led, 1);
		String message = "POST form was:\n";

		for (uint8_t i = 0; i < request->args(); i++)
		{
			message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
		}

		request->send(200, "text/plain", message);
		digitalWrite(led, 0);
	}
}

void handleNotFound(AsyncWebServerRequest *request)
{
	digitalWrite(led, 1);
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
	digitalWrite(led, 0);
}

void setup()
{
	pinMode(led, OUTPUT);
	digitalWrite(led, 0);

	Serial.begin(115200);

	while (!Serial && millis() < 5000);

	delay(200);

	Serial.print(F("\nStart Async_PostServer on "));
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
		handleRoot(request);
	});

	//server.on("/postplain/", handlePlain);
	server.on("/postplain/", HTTP_POST, [](AsyncWebServerRequest * request)
	{
		handlePlain(request);
	});

	//server.on("/postform/", handleForm);
	server.on("/postform/", HTTP_POST, [](AsyncWebServerRequest * request)
	{
		handleForm(request);
	});

	server.onNotFound(handleNotFound);

	server.begin();

	Serial.print(F("HTTP EthernetWebServer started @ IP : "));
	Serial.println(ETH.localIP());
}

void loop()
{
}
