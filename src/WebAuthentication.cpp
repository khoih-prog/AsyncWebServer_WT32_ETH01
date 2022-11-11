/****************************************************************************************************************************
  WebAuthentication.cpp - Dead simple Ethernet AsyncWebServer.

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

#include "WebAuthentication.h"
#include <libb64/cencode.h>

#include "mbedtls/md5.h"
#include "mbedtls/version.h"

/////////////////////////////////////////////////

// Basic Auth hash = base64("username:password")

bool checkBasicAuthentication(const char * hash, const char * username, const char * password)
{
  if (username == NULL || password == NULL || hash == NULL)
    return false;

  size_t toencodeLen = strlen(username) + strlen(password) + 1;
  size_t encodedLen = base64_encode_expected_len(toencodeLen);

  if (strlen(hash) != encodedLen)
    return false;

  char *toencode = new char[toencodeLen + 1];

  if (toencode == NULL)
  {
    return false;
  }

  char *encoded = new char[base64_encode_expected_len(toencodeLen) + 1];

  if (encoded == NULL)
  {
    delete[] toencode;

    return false;
  }

  sprintf(toencode, "%s:%s", username, password);

  if (base64_encode_chars(toencode, toencodeLen, encoded) > 0 && memcmp(hash, encoded, encodedLen) == 0)
  {
    delete[] toencode;
    delete[] encoded;

    return true;
  }

  delete[] toencode;
  delete[] encoded;

  return false;
}

/////////////////////////////////////////////////

static bool getMD5(uint8_t * data, uint16_t len, char * output)
{
  //33 bytes or more
  mbedtls_md5_context _ctx;
  uint8_t i;
  uint8_t * _buf = (uint8_t*)malloc(16);

  if (_buf == NULL)
    return false;

  memset(_buf, 0x00, 16);

  mbedtls_md5_init(&_ctx);

#if (MBEDTLS_VERSION_NUMBER < 0x02070000)
  // Superseded from v2.7.0
  mbedtls_md5_starts(&_ctx);
  mbedtls_md5_update(&_ctx, data, len);
  mbedtls_md5_finish(&_ctx, _buf);
#else
  mbedtls_md5_starts_ret(&_ctx);
  mbedtls_md5_update_ret(&_ctx, data, len);
  mbedtls_md5_finish_ret(&_ctx, _buf);
#endif

  for (i = 0; i < 16; i++)
  {
    sprintf(output + (i * 2), "%02x", _buf[i]);
  }

  free(_buf);

  return true;
}

/////////////////////////////////////////////////

static String genRandomMD5()
{
  uint32_t r = rand();
  char * out = (char*)malloc(33);

  if (out == NULL || !getMD5((uint8_t*)(&r), 4, out))
    return "";

  String res = String(out);
  free(out);

  return res;
}

/////////////////////////////////////////////////

static String stringMD5(const String& in)
{
  char * out = (char*)malloc(33);

  if (out == NULL || !getMD5((uint8_t*)(in.c_str()), in.length(), out))
    return "";

  String res = String(out);
  free(out);

  return res;
}

/////////////////////////////////////////////////

String generateDigestHash(const char * username, const char * password, const char * realm)
{
  if (username == NULL || password == NULL || realm == NULL)
  {
    return "";
  }

  char * out = (char*)malloc(33);
  String res = String(username);
  res.concat(":");
  res.concat(realm);
  res.concat(":");
  String in = res;
  in.concat(password);

  if (out == NULL || !getMD5((uint8_t*)(in.c_str()), in.length(), out))
    return "";

  res.concat(out);
  free(out);

  return res;
}

/////////////////////////////////////////////////

String requestDigestAuthentication(const char * realm)
{
  String header = "realm=\"";

  if (realm == NULL)
    header.concat("asyncesp");
  else
    header.concat(realm);

  header.concat( "\", qop=\"auth\", nonce=\"");
  header.concat(genRandomMD5());
  header.concat("\", opaque=\"");
  header.concat(genRandomMD5());
  header.concat("\"");

  return header;
}

/////////////////////////////////////////////////

bool checkDigestAuthentication(const char * header, const char * method, const char * username, const char * password,
                               const char * realm,
                               bool passwordIsHash, const char * nonce, const char * opaque, const char * uri)
{
  if (username == NULL || password == NULL || header == NULL || method == NULL)
  {
    AWS_LOGERROR(F("checkDigestAuthentication: AUTH FAIL, missing required fields"));

    return false;
  }

  String myHeader = String(header);
  int nextBreak = myHeader.indexOf(",");

  if (nextBreak < 0)
  {
    AWS_LOGERROR(F("checkDigestAuthentication: AUTH FAIL, no variables"));

    return false;
  }

  String myUsername = String();
  String myRealm = String();
  String myNonce = String();
  String myUri = String();
  String myResponse = String();
  String myQop = String();
  String myNc = String();
  String myCnonce = String();

  myHeader += ", ";

  do
  {
    String avLine = myHeader.substring(0, nextBreak);
    avLine.trim();
    myHeader = myHeader.substring(nextBreak + 1);
    nextBreak = myHeader.indexOf(",");

    int eqSign = avLine.indexOf("=");

    if (eqSign < 0)
    {
      AWS_LOGERROR(F("checkDigestAuthentication: AUTH FAIL, no = sign"));

      return false;
    }

    String varName = avLine.substring(0, eqSign);
    avLine = avLine.substring(eqSign + 1);

    if (avLine.startsWith("\""))
    {
      avLine = avLine.substring(1, avLine.length() - 1);
    }

    if (varName.equals("username"))
    {
      if (!avLine.equals(username))
      {
        AWS_LOGERROR(F("checkDigestAuthentication: AUTH FAIL, username"));

        return false;
      }

      AWS_LOGINFO1(F("checkDigestAuthentication: myUsername ="), myUsername);

      myUsername = avLine;
    }
    else if (varName.equals("realm"))
    {
      if (realm != NULL && !avLine.equals(realm))
      {
        AWS_LOGERROR(F("checkDigestAuthentication: AUTH FAIL, realm"));

        return false;
      }

      AWS_LOGINFO1(F("checkDigestAuthentication: myRealm ="), myRealm);

      myRealm = avLine;
    }
    else if (varName.equals("nonce"))
    {
      if (nonce != NULL && !avLine.equals(nonce))
      {
        AWS_LOGERROR(F("checkDigestAuthentication: AUTH FAIL, nonce"));

        return false;
      }

      AWS_LOGINFO1(F("checkDigestAuthentication: myNonce ="), myNonce);

      myNonce = avLine;
    }
    else if (varName.equals("opaque"))
    {
      if (opaque != NULL && !avLine.equals(opaque))
      {
        AWS_LOGERROR(F("checkDigestAuthentication: AUTH FAIL, opaque"));

        return false;
      }
    }
    else if (varName.equals("uri"))
    {
      if (uri != NULL && !avLine.equals(uri))
      {
        AWS_LOGERROR(F("checkDigestAuthentication: AUTH FAIL, uri"));

        return false;
      }

      AWS_LOGINFO1(F("checkDigestAuthentication: myUri ="), myUri);

      myUri = avLine;
    }
    else if (varName.equals("response"))
    {
      AWS_LOGINFO1(F("checkDigestAuthentication: myResponse ="), myResponse);

      myResponse = avLine;
    }
    else if (varName.equals("qop"))
    {
      AWS_LOGINFO1(F("checkDigestAuthentication: myQop ="), myQop);

      myQop = avLine;
    }
    else if (varName.equals("nc"))
    {
      AWS_LOGINFO1(F("checkDigestAuthentication: myNc ="), myNc);

      myNc = avLine;
    }
    else if (varName.equals("cnonce"))
    {
      AWS_LOGINFO1(F("checkDigestAuthentication: myCnonce ="), myCnonce);

      myCnonce = avLine;
    }
  } while (nextBreak > 0);

  String ha1 = (passwordIsHash) ? String(password) : stringMD5(myUsername + ":" + myRealm + ":" + String(password));
  String ha2 = String(method) + ":" + myUri;
  String response = ha1 + ":" + myNonce + ":" + myNc + ":" + myCnonce + ":" + myQop + ":" + stringMD5(ha2);

  if (myResponse.equals(stringMD5(response)))
  {
    AWS_LOGINFO(F("AUTH SUCCESS"));

    return true;
  }

  AWS_LOGINFO(F("AUTH FAIL: password"));

  return false;
}
