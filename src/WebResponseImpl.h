/****************************************************************************************************************************
  WebResponseImpl.h - Dead simple Ethernet AsyncWebServer.

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

#ifndef ASYNCWEBSERVERRESPONSEIMPL_H_
#define ASYNCWEBSERVERRESPONSEIMPL_H_

#ifdef Arduino_h
  // arduino is not compatible with std::vector
  #undef min
  #undef max
#endif

#include <vector>

// It is possible to restore these defines, but one can use _min and _max instead. Or std::min, std::max.

/////////////////////////////////////////////////

class AsyncBasicResponse: public AsyncWebServerResponse
{
  private:
    String _content;

    char *_contentCstr;      // RSMOD
    String _partialHeader;

  public:
    AsyncBasicResponse(int code, const String& contentType = String(), const String& content = String());

    AsyncBasicResponse(int code, const String& contentType, const char *content = nullptr);     // RSMOD

    void _respond(AsyncWebServerRequest *request);
    size_t _ack(AsyncWebServerRequest *request, size_t len, uint32_t time);

    /////////////////////////////////////////////////

    inline bool _sourceValid() const
    {
      return true;
    }

    /////////////////////////////////////////////////
};

/////////////////////////////////////////////////

class AsyncAbstractResponse: public AsyncWebServerResponse
{
  private:
    String _head;
    // Data is inserted into cache at begin().
    // This is inefficient with vector, but if we use some other container,
    // we won't be able to access it as contiguous array of bytes when reading from it,
    // so by gaining performance in one place, we'll lose it in another.
    std::vector<uint8_t> _cache;
    size_t _readDataFromCacheOrContent(uint8_t* data, const size_t len);
    size_t _fillBufferAndProcessTemplates(uint8_t* buf, size_t maxLen);

  protected:
    AwsTemplateProcessor _callback;

  public:
    AsyncAbstractResponse(AwsTemplateProcessor callback = nullptr);
    void _respond(AsyncWebServerRequest *request);
    size_t _ack(AsyncWebServerRequest *request, size_t len, uint32_t time);

    /////////////////////////////////////////////////

    inline bool _sourceValid() const
    {
      return false;
    }

    /////////////////////////////////////////////////

    virtual size_t _fillBuffer(uint8_t *buf __attribute__((unused)), size_t maxLen __attribute__((unused)))
    {
      return 0;
    }

    /////////////////////////////////////////////////
};

/////////////////////////////////////////////////

#ifndef TEMPLATE_PLACEHOLDER
  #define TEMPLATE_PLACEHOLDER        '%'
#endif

#define TEMPLATE_PARAM_NAME_LENGTH    32

/////////////////////////////////////////////////

class AsyncFileResponse: public AsyncAbstractResponse
{
    using File = fs::File;
    using FS = fs::FS;

  private:
    File _content;
    String _path;
    void _setContentType(const String& path);

  public:
    AsyncFileResponse(FS &fs, const String& path, const String& contentType = String(), bool download = false,
                      AwsTemplateProcessor callback = nullptr);
    AsyncFileResponse(File content, const String& path, const String& contentType = String(), bool download = false,
                      AwsTemplateProcessor callback = nullptr);

    ~AsyncFileResponse();

    /////////////////////////////////////////////////

    inline bool _sourceValid() const
    {
      return !!(_content);
    }

    /////////////////////////////////////////////////

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

/////////////////////////////////////////////////

class AsyncStreamResponse: public AsyncAbstractResponse
{
  private:
    Stream *_content;

  public:
    AsyncStreamResponse(Stream &stream, const String& contentType, size_t len, AwsTemplateProcessor callback = nullptr);

    /////////////////////////////////////////////////

    inline bool _sourceValid() const
    {
      return !!(_content);
    }

    /////////////////////////////////////////////////

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

/////////////////////////////////////////////////

class AsyncCallbackResponse: public AsyncAbstractResponse
{
  private:
    AwsResponseFiller _content;
    size_t _filledLength;

  public:
    AsyncCallbackResponse(const String& contentType, size_t len, AwsResponseFiller callback,
                          AwsTemplateProcessor templateCallback = nullptr);

    /////////////////////////////////////////////////

    inline bool _sourceValid() const
    {
      return !!(_content);
    }

    /////////////////////////////////////////////////

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

/////////////////////////////////////////////////

class AsyncChunkedResponse: public AsyncAbstractResponse
{
  private:
    AwsResponseFiller _content;
    size_t _filledLength;

  public:
    AsyncChunkedResponse(const String& contentType, AwsResponseFiller callback,
                         AwsTemplateProcessor templateCallback = nullptr);

    /////////////////////////////////////////////////

    inline bool _sourceValid() const
    {
      return !!(_content);
    }

    /////////////////////////////////////////////////

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

/////////////////////////////////////////////////

class AsyncProgmemResponse: public AsyncAbstractResponse
{
  private:
    const uint8_t * _content;
    size_t _readLength;

  public:
    AsyncProgmemResponse(int code, const String& contentType, const uint8_t * content, size_t len,
                         AwsTemplateProcessor callback = nullptr);

    /////////////////////////////////////////////////

    inline bool _sourceValid() const
    {
      return true;
    }

    /////////////////////////////////////////////////

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

/////////////////////////////////////////////////

class cbuf;

/////////////////////////////////////////////////

class AsyncResponseStream: public AsyncAbstractResponse, public Print
{
  private:
    cbuf *_content;

  public:
    AsyncResponseStream(const String& contentType, size_t bufferSize);
    ~AsyncResponseStream();

    /////////////////////////////////////////////////

    inline bool _sourceValid() const
    {
      return (_state < RESPONSE_END);
    }

    /////////////////////////////////////////////////

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
    size_t write(const uint8_t *data, size_t len);
    size_t write(uint8_t data);
    using Print::write;
};

#endif /* ASYNCWEBSERVERRESPONSEIMPL_H_ */
