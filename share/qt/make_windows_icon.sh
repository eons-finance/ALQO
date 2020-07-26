#!/bin/bash
# create multiresolution windows icon
#mainnet
ICON_SRC=../../src/qt/res/icons/alqo.png
ICON_DST=../../src/qt/res/icons/alqo.ico
convert ${ICON_SRC} -resize 16x16 alqo-16.png
convert ${ICON_SRC} -resize 32x32 alqo-32.png
convert ${ICON_SRC} -resize 48x48 alqo-48.png
convert alqo-16.png alqo-32.png alqo-48.png ${ICON_DST}
#testnet
ICON_SRC=../../src/qt/res/icons/alqo_testnet.png
ICON_DST=../../src/qt/res/icons/alqo_testnet.ico
convert ${ICON_SRC} -resize 16x16 alqo-16.png
convert ${ICON_SRC} -resize 32x32 alqo-32.png
convert ${ICON_SRC} -resize 48x48 alqo-48.png
convert alqo-16.png alqo-32.png alqo-48.png ${ICON_DST}
rm alqo-16.png alqo-32.png alqo-48.png
