#! /bin/bash

#!/bin/bash

SRC=$(readlink -f ${BASH_SOURCE[0]})
SCRIPT_DIR=$(dirname "${SRC}")
cd $SCRIPT_DIR
cd ..

# Save current directory
CUR_DIR=$(pwd)

rm -rfd ./u8g2
rm -rf ./lib/u8g2/

git clone --depth=1 https://github.com/olikraus/u8g2.git

mkdir ./lib/u8g2
mv ./u8g2/csrc/* ./lib/u8g2/
rm -rfd ./u8g2