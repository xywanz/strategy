#include "xyts/data_collector/collected_data_handler.h"
#include "xyts/data_collector/collected_data_handler_factory.h"
#include "yaml-cpp/yaml.h"

namespace xyts {

class MyCollectedDataHandler final : public CollectedDataHandler {
 public:
  explicit MyCollectedDataHandler(const YAML::Node& conf);

  void Handle(CollectedDataType type, const std::string& data) final;
};

EXPORT_COLLECTED_DATA_HANDLER(MyCollectedDataHandler);

}  // namespace xyts
