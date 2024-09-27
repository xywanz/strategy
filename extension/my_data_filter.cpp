#include "xyts/core/contract_table.h"
#include "xyts/core/log.h"
#include "xyts/extension/data_filter/data_filter.h"
#include "xyts/extension/data_filter/data_filter_factory.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

class MyDataFilter final : public DataFilter {
 public:
  explicit MyDataFilter(const YAML::Node& conf) {}

  bool Accept(const DepthData& depth) final {
    const auto* contract = ContractTable::At(depth.contract_id);
    LOG_INFO("Recv depth data: instr:{} exch_ts:{} last_price:{}", contract->instr,
             depth.exchange_timestamp, depth.last_price);
    return true;
  }
};

EXPORT_DATA_FILTER(MyDataFilter);

}  // namespace xyts
