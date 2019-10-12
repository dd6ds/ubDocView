#!/bin/bash
ARCH_TRIPLET=$1
if [[ "$ARCH_TRIPLET" == "arm-linux-gnueabihf" ]]
  then
  CLICK_ARCH=armhf
elif [[ "$ARCH_TRIPLET" == "aarch64-linux-gnu" ]]
  then
  CLICK_ARCH=arm64
elif [[ "$ARCH_TRIPLET" == "x86_64-linux-gnu" ]]
  then
  CLICK_ARCH=amd64
else
  echo "unsupported ARCH_TRIPLET (\$1) !"
  exit 1
fi
./tools/get-click-deps -t ./.tmp-click-deps-$CLICK_ARCH -c ./click/disable-lo-features.sh ./click/docviewer-libs.json $CLICK_ARCH upstream-libs-$CLICK_ARCH
