ARCH_TRIPLET=$1
if [[ "$ARCH_TRIPLET" == "arm-linux-gnueabihf" ]]
  then
  CLICK_ARCH=armhf
  else
  CLICK_ARCH=arm64
fi
./tools/get-click-deps -t ./.tmp-click-deps-$CLICK_ARCH -c ./click/disable-lo-features.sh ./click/docviewer-libs.json $CLICK_ARCH upstream-libs-$CLICK_ARCH
