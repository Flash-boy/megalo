#ifndef __MEGALO_HTTP_PARSER_H_
#define __MEGALO_HTTP_PARSER_H_
#include "http.h"
#include "http11_parser.h"
#include "httpclient_parser.h"

namespace megalo{
namespace http{

class HttpRequestParser {
public:
  typedef std::shared_ptr<HttpRequestParser> ptr;
  HttpRequestParser();

  /**
   * @brief 解析协议
   * @param[in, out] data 协议文本内存
   * @param[in] len 协议文本内存长度
   * @return 返回实际解析的长度,并且将已解析的数据移除
   */
  size_t execute(char* data, size_t len);
  int isFinished();
  int hasError(); 
  HttpRequest::ptr getData() const { return m_data;}
  void setError(int v) { m_error = v;}
  uint64_t getContentLength();
  const http_parser& getParser() const { return m_parser;}
public:
  /**
   * @brief 返回HttpRequest协议解析的缓存大小
   */
  static uint64_t GetHttpRequestBufferSize();

  /**
   * @brief 返回HttpRequest协议的最大消息体大小
   */
  static uint64_t GetHttpRequestMaxBodySize();
private:
  http_parser m_parser;
  HttpRequest::ptr m_data;
  /// 错误码
  /// 1000: invalid method
  /// 1001: invalid version
  /// 1002: invalid field
  int m_error;
};

/**
 * @brief Http响应解析结构体
 */
class HttpResponseParser {
public:
  typedef std::shared_ptr<HttpResponseParser> ptr;

  HttpResponseParser();
  /**
   * @brief 解析HTTP响应协议
   * @param[in, out] data 协议数据内存
   * @param[in] len 协议数据内存大小
   * @param[in] chunck 是否在解析chunck
   * @return 返回实际解析的长度,并且移除已解析的数据
   */
  size_t execute(char* data, size_t len, bool chunck);
  int isFinished();
  int hasError(); 
  HttpResponse::ptr getData() const { return m_data;}
  void setError(int v) { m_error = v;}
  uint64_t getContentLength();
  const httpclient_parser& getParser() const { return m_parser;}
public:
  /**
   * @brief 返回HTTP响应解析缓存大小
   */
  static uint64_t GetHttpResponseBufferSize();

  /**
   * @brief 返回HTTP响应最大消息体大小
   */
  static uint64_t GetHttpResponseMaxBodySize();
private:
  httpclient_parser m_parser;
  HttpResponse::ptr m_data;
  /// 错误码
  /// 1001: invalid version
  /// 1002: invalid field
  int m_error;
};

}
}

#endif
