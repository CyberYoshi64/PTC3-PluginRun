#!/bin/sh

make clean

mkdir build
resources/mkSheet.sh
resources/mkIcons.sh

make $1 -j4

./send.sh
./send.sh 2