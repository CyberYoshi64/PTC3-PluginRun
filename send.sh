#!/bin/sh

if [ -n "$1" ]; then
    3dslink -a 192.168.2.180 output/*.3dsx
else
    ./sendfile.py output/*.cia /cia 192.168.2.180 5000
fi