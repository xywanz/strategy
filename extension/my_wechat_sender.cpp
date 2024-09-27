#include <openssl/tls1.h>

#include <boost/asio.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/system/error_code.hpp>

#include "fmt/format.h"
#include "nlohmann/json.hpp"
#include "xyts/core/log.h"
#include "xyts/data_collector/collected_data_handler.h"
#include "xyts/data_collector/collected_data_handler_factory.h"
#include "yaml-cpp/yaml.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

namespace xyts {

class MyWechatSender final : public CollectedDataHandler {
 public:
  explicit MyWechatSender(const YAML::Node& conf);

  void Handle(CollectedDataType type, const std::string& data) final;

 private:
  void SendAlarmMsg(const std::string& data);

  net::io_context ioc_;
  net::ssl::context ssl_{net::ssl::context::tlsv12_client};
  std::string wechat_host_ = "qyapi.weixin.qq.com";
  std::string wechat_robot_key_;
  std::string target_;
};

MyWechatSender::MyWechatSender(const YAML::Node& conf)
    : wechat_robot_key_(conf["wechat_robot_key"].as<std::string>()),
      target_(fmt::format("/cgi-bin/webhook/send?key={}", wechat_robot_key_)) {}

void MyWechatSender::Handle(CollectedDataType type, const std::string& data) {
  switch (type) {
    case CollectedDataType::kAlarm: {
      SendAlarmMsg(data);
      break;
    }
    default: {
      break;
    }
  }
}

void MyWechatSender::SendAlarmMsg(const std::string& msg) {
  try {
    net::ssl::stream<tcp::socket> stream{ioc_, ssl_};
    if (!SSL_set_tlsext_host_name(stream.native_handle(), wechat_host_.c_str())) {
      throw beast::system_error(
          beast::error_code(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()));
    }

    tcp::resolver resolver(ioc_);
    auto endpoints = resolver.resolve(wechat_host_, "https");
    net::connect(stream.next_layer(), endpoints);
    stream.handshake(net::ssl::stream_base::client);

    http::request<http::string_body> request(http::verb::post, target_, 11);
    request.set(http::field::host, wechat_host_);
    request.set(http::field::user_agent, "boost");
    request.set(http::field::content_type, "application/json");
    nlohmann::json data{
        {"msgtype", "markdown"},
        {"markdown", {{"content", msg}}},
    };
    request.body() = data.dump();
    request.prepare_payload();
    http::write(stream, request);

    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);
    auto res_json = nlohmann::json::parse(res.body());
    if (res_json.contains("errorcode") && res_json.at("errorcode") != 0) {
      throw std::runtime_error(res.body());
    }

    boost::system::error_code ec;
    stream.shutdown(ec);
  } catch (const std::exception& e) {
    LOG_ERROR("Failed to send alarm msg due to exception: {}", e.what());
  } catch (...) {
    LOG_ERROR("Failed to send alarm msg due to unknown exception");
  }
}

EXPORT_COLLECTED_DATA_HANDLER(MyWechatSender);

}  // namespace xyts
