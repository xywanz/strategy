#!/bin/bash

cd `dirname $0`

mkdir -p ../data ../log

rm -f ../conf/*autogen.json
rm -f ../data/**
rm -f ../log/**
if [ -f "strategy_startup.py" ]; then
    python3 manager.py stop_strategy strategy_startup.py
fi

conf_file="../conf/backtest.yaml"
if [ $# -gt 0 ]; then
    conf_file="../conf/backtest_$1.yaml"
fi
nohup ./backtester $conf_file ../log/backtester.log info &>../log/backtester.nohup &
