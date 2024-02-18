#!/bin/sh

## HOME Menu icon
bannertool makesmdh \
 -o build/icon.icn \
 -r japan,northamerica,europe,australia \
 -f visible,nosavebackups \
 -i resources/icon.png \
 -si resources/icon-small.png \
 \
 -s "SmileBASIC-CYX Launcher" \
 -l "SmileBASIC-CYX Launcher" \
 -p "CyberYoshi64" \
 \
 -gs "SmileBASIC-CYX-Launcher" \
 -gl "SmileBASIC-CYX-Launcher" \
 \
 -js "スマイルベーシック-CYX ランチャー" \
 -jl "スマイルベーシック-CYX ランチャー" \
 -jp "サイバーヨッシー64"

## Homebrew Launcher icon
bannertool makesmdh \
 -o build/icon.smdh \
 -i resources/icon.png \
 \
 -s "SmileBASIC-CYX Launcher" \
 -l "Launch the SmileBASIC-CYX modpack" \
 -p "CyberYoshi64" \
 \
 -gs "SmileBASIC-CYX-Launcher" \
 -gl "Starte SmileBASIC mit\ndem CYX-Plugin." \
 \
 -js "スマイルベーシック-CYX ランチャー" \
 -jl "プチコン3号でCYXプラグインを立ち上げる。" \
 -jp "サイバーヨッシー64"
