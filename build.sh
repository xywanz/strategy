#!/bin/bash
set -e
cd `dirname $0`

local_path=`pwd`

build_type=Release

conf_for_test='backtest_example'
if [[ "$1" != "" ]]; then
    conf_for_test=$1
fi

# build cpp strategies
cd ${local_path}
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=${build_type}
make VERBOSE=1 -j4
cd ../
rsync build/lib/lib*so xyts-strategy-sdk/lib/

# rsync conf and scripts
rm -f xyts-strategy-sdk/conf/*autogen.json
rsync -r conf/base/ xyts-strategy-sdk/conf
rsync scripts/* xyts-strategy-sdk/bin/
rsync -r conf/$conf_for_test/* xyts-strategy-sdk/conf/
