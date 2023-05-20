#!/bin/bash

SRC=$(readlink -f ${BASH_SOURCE[0]})
SCRIPT_DIR=$(dirname "${SRC}")
cd $SCRIPT_DIR
cd ..

# Save current directory
CUR_DIR=$(pwd)

CubeMxProjectDir=$1
echo "Project directory: $CUR_DIR"
echo "Importing CubeMx project: $CubeMxProjectDir"

pushd $CubeMxProjectDir > /dev/null

BOARD=$(find ./ -name "*.ld" -execdir echo {} ';')
BOARD=$(echo ${BOARD:0:11})
BOARD_FAMILY=$(find ./Drivers -type d -iname "*HAL_Driver"  -execdir echo {} ';')
BOARD_FAMILY=$(echo ${BOARD_FAMILY:0:9})
echo "Board: $BOARD"
echo "Board family: $BOARD_FAMILY"

mv ./Core/Inc/main.h ./main.h
mv ./Core/Src/main.c ./main.c

for file in *.s; do
    mv -- "$file" "${file%.s}.S" 2>/dev/null
done

mkdir $CUR_DIR/src/$BOARD > /dev/null

cp -v ./*.S $CUR_DIR/src/$BOARD
cp -rv ./Core $CUR_DIR/CubeMx/$BOARD/
cp -rv ./Drivers $CUR_DIR/CubeMx/$BOARD/

mv ./main.h ./Core/Inc/main.h
mv ./main.c ./Core/Src/main.c

echo "Running template script..."
LD_FILE=$(find ./ -type f -iname "*.ld"  -execdir echo {} ';')
python3 $SCRIPT_DIR/import.py $BOARD_FAMILY $LD_FILE

cp -rv ./loader.ld $CUR_DIR/CubeMx/$BOARD/${BOARD}TX_FLASH.ld

mv ./peripherals.h $CUR_DIR/src/$BOARD
mv ./peripherals.cpp $CUR_DIR/src/$BOARD
mv ./pins.h $CUR_DIR/src/$BOARD
mv ./hal_driver.h $CUR_DIR/src/$BOARD

echo "Done."
exit 0
