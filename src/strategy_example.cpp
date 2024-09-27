#include "strategy_example_param_manager.h"
#include "xyts/strategy/strategy.h"
#include "xyts/strategy/strategy_context.h"
// 日志库
#include "xyts/core/log.h"
// 合约表
#include "xyts/core/contract_table.h"
// 项目里还有很多实用的库
#include "xyu/datetime.h"  // Python-like datetime
// ...

using namespace xyts;
using namespace xyts::strategy;

class StrategyExample final : public Strategy {
 public:
  // 构造函数必须如下所示，只传入一个StrategyContext指针。
  // 我们需要将ctx保存下来，用于接下来与交易环境进行交互。
  // 构造函数就相当于策略的初始化，构造策略时策略上下文已经完成了初始化。
  explicit StrategyExample(StrategyContext* ctx);

  ~StrategyExample();

  void OnDepth(const DepthData& depth) final;

  // 订单状态有更新
  void OnOrder(const OrderResponse& order) final;

 private:
  static std::string FormatTs(std::chrono::microseconds ts) {
    return xyu::datetime::datetime::fromtimestamp(ts).strftime("%Y-%m-%d %H:%M:%S.%f");
  }

  StrategyContext* ctx_;                // 用于保存StrategyContext
  StrategyExampleParamManager* param_;  // 策略参数

  ContractPtr leg1_contract_;
  ContractPtr leg2_contract_;

  double leg1_mid_price_ = std::numeric_limits<double>::quiet_NaN();
  double leg2_mid_price_ = std::numeric_limits<double>::quiet_NaN();
};

StrategyExample::StrategyExample(StrategyContext* ctx)
    : ctx_(ctx),
      param_(ctx->GetDerivedParamManager<StrategyExampleParamManager>()),
      leg1_contract_(ContractTable::GetByInstrument(param_->get_leg1_instr())),
      leg2_contract_(ContractTable::GetByInstrument(param_->get_leg2_instr())) {
  if (!leg1_contract_) {
    throw std::runtime_error("Unknown leg1 " + param_->get_leg1_instr());
  }
  if (!leg2_contract_) {
    throw std::runtime_error("Unknown leg2 " + param_->get_leg2_instr());
  }

  // 订阅leg1和leg2的行情和持仓信息
  ctx_->SubscribeMarketData({leg1_contract_->instr, leg2_contract_->instr});

  // 每30s打印一下PnL，如果亏损达到阈值则发送告警并停止策略
  ctx_->AddPeriodicCallback(std::chrono::seconds{30}, [this](auto) {
    double pnl = ctx_->GetPnl();
    LOG_INFO("{}|PnL: {:.2f}", FormatTs(ctx_->GetWallTime()), pnl);
    if (pnl <= param_->get_stop_loss()) {
      ctx_->SendAlarm(
          fmt::format("Stop loss. strategy:{} pnl:{:.2f}", ctx_->GetStrategyName(), pnl));
      ctx_->Stop();
    }
  });
}

StrategyExample::~StrategyExample() {
  LOG_INFO("Strategy {} stopped", ctx_->GetStrategyName());
  // 可以在析构函数中保存策略的一些状态到文件中，用于下次启动时加载
  // do sth.
}

void StrategyExample::OnDepth(const DepthData& depth) {
  // 竞价阶段bid[0]==ask[0]，我们不在该阶段交易
  if (std::abs(depth.bid[0] - depth.ask[0]) < 1e-6) {
    return;
  }

  // 这里没有考虑bid或ask没有挂单的情况，假设bid和ask价格都存在
  if (depth.contract_id == leg1_contract_->contract_id) {
    leg1_mid_price_ = (depth.bid[0] + depth.ask[0]) / 2;
  } else {
    // 为了简单，只在收到leg2行情的时候计算spread
    leg2_mid_price_ = (depth.bid[0] + depth.ask[0]) / 2;
    // leg1还没收到行情，无法计算spread
    if (std::isnan(leg1_mid_price_)) {
      return;
    }
    double spread = leg1_mid_price_ - leg2_mid_price_;
    LOG_INFO("{}|spread = {:.2f}", FormatTs(depth.exchange_timestamp), spread);
    auto position = ctx_->GetLogicalPosition(leg1_contract_->contract_id).position;
    if (spread >= param_->get_upper_line()) {
      // 超过上轨，如果仓位还没满做空spread
      if (position > -param_->get_max_position()) {
        ctx_->BuyMarket(leg2_contract_->contract_id, 1);
        ctx_->SellMarket(leg1_contract_->contract_id, 1);
      }
    } else if (spread <= param_->get_lower_line()) {
      // 跌破下轨，如果仓位还没满则做多spread
      if (position < param_->get_max_position()) {
        ctx_->SellMarket(leg2_contract_->contract_id, 1);
        ctx_->BuyMarket(leg1_contract_->contract_id, 1);
      }
    }
  }
}

void StrategyExample::OnOrder(const OrderResponse& order) {
  const auto* contract = ContractTable::At(order.contract_id);
  auto now = ctx_->GetWallTime();
  LOG_INFO("{}|{} {}{} {} {} px:{:.2f} fill/total:{}/{} order_id:{} client_order_id:{}",
           FormatTs(now), contract->instr, order.direction, order.position_effect, order.status,
           order.error_code, order.price, order.accum_trade_volume, order.original_volume,
           order.order_id, order.client_order_id);

  if (order.current_trade_volume > 0) {
    LOG_INFO("{}|{} {}{} {:.2f}@{}", FormatTs(now), contract->instr, order.direction,
             order.position_effect, order.current_trade_price, order.current_trade_volume);
  }
}

// 导出策略符号，使得能通过动态库的形式加载
EXPORT_STRATEGY(StrategyExample);
