#include "xyts/extension/matching/matching_engine.h"
#include "xyts/extension/matching/matching_engine_factory.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

class MyMatchingEngine final : public MatchingEngine {
 public:
  explicit MyMatchingEngine(const YAML::Node& conf) {}

  void AddOrder(const OrderRequest& order) final {
    // 订单都以涨停价成交
    PublishTradeEvent(order.order_id, order.volume, order.contract->upper_limit_price);
  }

  void CancelOrder(OrderId order_id, ContractId contract_id) final {}

  void MatchOnDepth(const DepthData& depth) final {}
};

EXPORT_MATCHING_ENGINE(MyMatchingEngine)

}  // namespace xyts
