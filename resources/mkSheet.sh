#!/bin/sh

if ! cd resources/sheets; then
    exit 1
fi

for i in *.t3s; do
    echo $i
    NAME=$(head -c -5 <(echo "$i"))
    tex3ds -i "$i" -o "../../romfs/gfx/$NAME.t3x" -p "$i.png" -H "../../include/sheets/sheet_$NAME.h" -z auto
done

cd ../..
