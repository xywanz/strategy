#include "xyts/core/contract_table.h"
#include "xyts/extension/data_replayer/data_replayer.h"
#include "xyts/extension/data_replayer/data_replayer_factory.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

class MyDataReplayer : public DataReplayer {
 public:
  explicit MyDataReplayer(const YAML::Node& conf) {}

  void ReplayAll(const DepthDataHandler& handler) final {
    DepthData depth{};
    depth.local_timestamp = std::chrono::microseconds{1725454804309879};
    depth.exchange_timestamp = std::chrono::microseconds{1725454804000000};
    depth.volume = 1;
    depth.turnover = 30000;
    depth.ask_volume[0] = 1;
    depth.bid_volume[0] = 1;

    depth.contract_id = ContractTable::GetByInstrument("FUT_SHFE_rb-202501")->contract_id;
    depth.last_price = 3010;
    depth.ask[0] = 3011;
    depth.bid[0] = 3010;
    handler(depth);

    depth.contract_id = ContractTable::GetByInstrument("FUT_SHFE_hc-202501")->contract_id;
    depth.last_price = 3060;
    depth.ask[0] = 3061;
    depth.bid[0] = 3060;
    handler(depth);
  }
};

EXPORT_DATA_REPLAYER(MyDataReplayer);

}  // namespace xyts
