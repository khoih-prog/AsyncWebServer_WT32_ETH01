/****************************************************************************************************************************
  AsyncWebServer_WT32_ETH01_Debug.h - Dead simple Ethernet AsyncWebServer.  
   
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
  
  Version: 1.2.4

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.2.3   K Hoang      17/07/2021 Initial porting for WT32_ETH01 (ESP32 + LAN8720). Sync with ESPAsyncWebServer v1.2.3
  1.2.4   K Hoang      02/08/2021 Fix Mbed TLS compile error with ESP32 core v2.0.0-rc1+
 *****************************************************************************************************************************/

#pragma once

#ifndef ASYNC_WEBSERVER_WT32_ETH01_DEBUG_H
#define ASYNC_WEBSERVER_WT32_ETH01_DEBUG_H

#include <stdio.h>

#ifdef DEBUG_ASYNC_WEBSERVER_PORT
#define AWS_DEBUG_OUTPUT       DEBUG_ASYNC_WEBSERVER_PORT
#else
#define AWS_DEBUG_OUTPUT       Serial
#endif

// Change _ASYNC_WEBSERVER_LOGLEVEL_ to set tracing and logging verbosity
// 0: DISABLED: no logging
// 1: ERROR: errors
// 2: WARN: errors and warnings
// 3: INFO: errors, warnings and informational (default)
// 4: DEBUG: errors, warnings, informational and debug

#ifndef _ASYNC_WEBSERVER_LOGLEVEL_
  #define _ASYNC_WEBSERVER_LOGLEVEL_       1
#endif

const char AWS_MARK[]  = "[AWS] ";
const char AWS_SPACE[] = " ";
const char AWS_LINE[]  = "========================================\n";

#define AWS_PRINT_MARK   AWS_PRINT(AWS_MARK)
#define AWS_PRINT_SP     AWS_PRINT(AWS_SPACE)
#define AWS_PRINT_LINE   AWS_PRINT(AWS_LINE)

#define AWS_PRINT        AWS_DEBUG_OUTPUT.print
#define AWS_PRINTLN      AWS_DEBUG_OUTPUT.println

///////////////////////////////////////

#define AWS_LOG(x)         { AWS_PRINTLN(x); }
#define AWS_LOG0(x)        { AWS_PRINT(x); }
#define AWS_LOG1(x,y)      { AWS_PRINT(x); AWS_PRINTLN(y); }
#define AWS_LOG2(x,y,z)    { AWS_PRINT(x); AWS_PRINT(y); AWS_PRINTLN(z); }
#define AWS_LOG3(x,y,z,w)  { AWS_PRINT(x); AWS_PRINT(y); AWS_PRINT(z); AWS_PRINTLN(w); }

///////////////////////////////////////

#define AWS_LOGERROR(x)         if(_ASYNC_WEBSERVER_LOGLEVEL_>0) { AWS_PRINT_MARK; AWS_PRINTLN(x); }
#define AWS_LOGERROR_LINE(x)    if(_ASYNC_WEBSERVER_LOGLEVEL_>0) { AWS_PRINT_MARK; AWS_PRINTLN(x); AWS_PRINT_LINE; }
#define AWS_LOGERROR0(x)        if(_ASYNC_WEBSERVER_LOGLEVEL_>0) { AWS_PRINT(x); }
#define AWS_LOGERROR1(x,y)      if(_ASYNC_WEBSERVER_LOGLEVEL_>0) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINTLN(y); }
#define AWS_LOGERROR2(x,y,z)    if(_ASYNC_WEBSERVER_LOGLEVEL_>0) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINTLN(z); }
#define AWS_LOGERROR3(x,y,z,w)  if(_ASYNC_WEBSERVER_LOGLEVEL_>0) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINT(z); AWS_PRINT_SP; AWS_PRINTLN(w); }

///////////////////////////////////////

#define AWS_LOGWARN(x)          if(_ASYNC_WEBSERVER_LOGLEVEL_>1) { AWS_PRINT_MARK; AWS_PRINTLN(x); }
#define AWS_LOGWARN_LINE(x)     if(_ASYNC_WEBSERVER_LOGLEVEL_>1) { AWS_PRINT_MARK; AWS_PRINTLN(x); AWS_PRINT_LINE; }
#define AWS_LOGWARN0(x)         if(_ASYNC_WEBSERVER_LOGLEVEL_>1) { AWS_PRINT(x); }
#define AWS_LOGWARN1(x,y)       if(_ASYNC_WEBSERVER_LOGLEVEL_>1) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINTLN(y); }
#define AWS_LOGWARN2(x,y,z)     if(_ASYNC_WEBSERVER_LOGLEVEL_>1) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINTLN(z); }
#define AWS_LOGWARN3(x,y,z,w)   if(_ASYNC_WEBSERVER_LOGLEVEL_>1) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINT(z); AWS_PRINT_SP; AWS_PRINTLN(w); }

///////////////////////////////////////

#define AWS_LOGINFO(x)          if(_ASYNC_WEBSERVER_LOGLEVEL_>2) { AWS_PRINT_MARK; AWS_PRINTLN(x); }
#define AWS_LOGINFO_LINE(x)     if(_ASYNC_WEBSERVER_LOGLEVEL_>2) { AWS_PRINT_MARK; AWS_PRINTLN(x); AWS_PRINT_LINE; }
#define AWS_LOGINFO0(x)         if(_ASYNC_WEBSERVER_LOGLEVEL_>2) { AWS_PRINT(x); }
#define AWS_LOGINFO1(x,y)       if(_ASYNC_WEBSERVER_LOGLEVEL_>2) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINTLN(y); }
#define AWS_LOGINFO2(x,y,z)     if(_ASYNC_WEBSERVER_LOGLEVEL_>2) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINTLN(z); }
#define AWS_LOGINFO3(x,y,z,w)   if(_ASYNC_WEBSERVER_LOGLEVEL_>2) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINT(z); AWS_PRINT_SP; AWS_PRINTLN(w); }

///////////////////////////////////////

#define AWS_LOGDEBUG(x)         if(_ASYNC_WEBSERVER_LOGLEVEL_>3) { AWS_PRINT_MARK; AWS_PRINTLN(x); }
#define AWS_LOGDEBUG_LINE(x)    if(_ASYNC_WEBSERVER_LOGLEVEL_>3) { AWS_PRINT_MARK; AWS_PRINTLN(x); AWS_PRINT_LINE; }
#define AWS_LOGDEBUG0(x)        if(_ASYNC_WEBSERVER_LOGLEVEL_>3) { AWS_PRINT(x); }
#define AWS_LOGDEBUG1(x,y)      if(_ASYNC_WEBSERVER_LOGLEVEL_>3) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINTLN(y); }
#define AWS_LOGDEBUG2(x,y,z)    if(_ASYNC_WEBSERVER_LOGLEVEL_>3) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINTLN(z); }
#define AWS_LOGDEBUG3(x,y,z,w)  if(_ASYNC_WEBSERVER_LOGLEVEL_>3) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINT(z); AWS_PRINT_SP; AWS_PRINTLN(w); }

#endif    // ASYNC_WEBSERVER_WT32_ETH01_DEBUG_H


