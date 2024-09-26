#!/bin/bash

cd `dirname $0`

mkdir -p ../data ../log

rm -f ../data/**
rm -f ../log/**

conf_file="../conf/backtest.yaml"
if [ $# -gt 0 ]; then
    conf_file="../conf/backtest_$1.yaml"
fi
nohup ./backtester $conf_file ../log/backtester.log info &>../log/backtester.nohup &
