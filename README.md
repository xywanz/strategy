# strategy

如何编译及运行demo策略回测？

```sh
# 回测
./build.sh
cd xyts-strategy-sdk/bin
./run_backtest.sh
# 查看回测日志
# less ../log/backtester_*.log
# 查看回测明细
# less ../data/backtest_stats.json
```

```sh
# 实盘
./build.sh real_example
cd xyts-strategy-sdk/bin
# 按需修改配置
# vim ../conf/account/ctp.yaml
# vim ../conf/data_feed/ctp.yaml
# vim ...

# 创建策略实例，策略第一次运行前需要执行该命令，之后就不再需要了
# ./create_strategy_instance.sh <strategy_name> <instance_name>
# demo里启动两个策略分别交易rb/hc和SF/SM
./create_strategy_instance.sh strategy_example strategy_example_rbhc strategy_example_SFSM
# 从柜台拉取合约列表
./query_contracts ../log/trader.log ../conf
# 启动交易系统
./restart.sh
# 启动策略
./strategy_strategy.sh strategy_example_rbhc strategy_example_SFSM
# 也可以在前台启动，并将log打印在当前终端，便于gdb debug
# ./strategy_loader strategy_example_rbhc console ../conf
# gdb --args ./strategy_loader strategy_example_rbhc console ../conf
```
