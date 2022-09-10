#!/bin/bash
base=$1

mkdir -p "/app/share/icons/hicolor/16x16/apps/"
mkdir -p "/app/share/icons/hicolor/32x32/apps/"
mkdir -p "/app/share/icons/hicolor/64x64/apps/"
mkdir -p "/app/share/icons/hicolor/128x128/apps/"
mkdir -p "/app/share/icons/hicolor/256x256/apps/"
mkdir -p "/app/share/icons/hicolor/512x512/apps/"

cp "$base/16.png" "/app/share/icons/hicolor/16x16/apps/org.openjkdf2.OpenJKDF2.png"
cp "$base/32.png" "/app/share/icons/hicolor/32x32/apps/org.openjkdf2.OpenJKDF2.png"
cp "$base/48.png" "/app/share/icons/hicolor/48x48/apps/org.openjkdf2.OpenJKDF2.png"
cp "$base/64.png" "/app/share/icons/hicolor/64x64/apps/org.openjkdf2.OpenJKDF2.png"
cp "$base/128.png" "/app/share/icons/hicolor/128x128/apps/org.openjkdf2.OpenJKDF2.png"
cp "$base/256.png" "/app/share/icons/hicolor/256x256/apps/org.openjkdf2.OpenJKDF2.png"
cp "$base/512.png" "/app/share/icons/hicolor/512x512/apps/org.openjkdf2.OpenJKDF2.png"