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
# 创建策略实例，策略第一次运行前需要执行该命令，之后就不再需要了
# ./create_strategy_instance.sh <strategy_name> <instance_name>
./create_strategy_instance.sh strategy_example strategy_example_rbhc
# 启动交易系统
./restart.sh
# 启动策略
./strategy_strategy.sh strategy_example_rbhc
```
