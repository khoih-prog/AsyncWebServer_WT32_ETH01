/****************************************************************************************************************************
  AsyncWebSynchronization.h - Dead simple Ethernet AsyncWebServer.

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

#ifndef ASYNCWEBSYNCHRONIZATION_H_
#define ASYNCWEBSYNCHRONIZATION_H_

// Synchronisation is only available on ESP32, as the ESP8266 isn't using FreeRTOS by default

#include "AsyncWebServer_WT32_ETH01.h"

/////////////////////////////////////////////////

// This is the ESP32 version of the Sync Lock, using the FreeRTOS Semaphore
class AsyncWebLock
{
  private:
    SemaphoreHandle_t _lock;
    mutable void *_lockedBy;

  public:
    AsyncWebLock()
    {
      _lock = xSemaphoreCreateBinary();
      _lockedBy = NULL;
      xSemaphoreGive(_lock);
    }

    /////////////////////////////////////////////////

    ~AsyncWebLock()
    {
      vSemaphoreDelete(_lock);
    }

    /////////////////////////////////////////////////

    bool lock() const
    {
      extern void *pxCurrentTCB;

      if (_lockedBy != pxCurrentTCB)
      {
        xSemaphoreTake(_lock, portMAX_DELAY);
        _lockedBy = pxCurrentTCB;

        return true;
      }

      return false;
    }

    /////////////////////////////////////////////////

    void unlock() const
    {
      _lockedBy = NULL;
      xSemaphoreGive(_lock);
    }
};

/////////////////////////////////////////////////

class AsyncWebLockGuard
{
  private:
    const AsyncWebLock *_lock;

  public:
    AsyncWebLockGuard(const AsyncWebLock &l)
    {
      if (l.lock())
      {
        _lock = &l;
      }
      else
      {
        _lock = NULL;
      }
    }

    /////////////////////////////////////////////////

    ~AsyncWebLockGuard()
    {
      if (_lock)
      {
        _lock->unlock();
      }
    }
};

/////////////////////////////////////////////////

#endif // ASYNCWEBSYNCHRONIZATION_H_
