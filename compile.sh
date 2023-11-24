#!/bin/sh

make clean

mkdir build
resources/mkSheet.sh
resources/mkIcons.sh

make $1 -j$(nproc)

./send.sh
./send.sh 2
