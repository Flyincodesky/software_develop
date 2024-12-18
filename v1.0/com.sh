#!/bin/bash
#chmod +x com.sh
CXX="g++"   # "g++"
TARGET_DIR="./build"
option_cmpl="-g --std=c++17 -c"   #-O2
option_link="-g --std=c++17  "  #-O2
if [ ! -d "$TARGET_DIR" ]; then   # 如果目录不存在，则创建它
  mkdir -p "$TARGET_DIR"
fi
rm ./build/*

for file in ./src/*.cpp; do $CXX $option_cmpl "$file" -o "./build/$(basename "$file" .cpp).o" ;done
# # # # backuper
for file in ./src/backuper/*.cpp; do $CXX $option_cmpl "$file" -o "./build/$(basename "$file" .cpp).o" ;done
# # # # # packer
# for file in ./src/pack/*.cpp; do $CXX $option_cmpl "$file" -o "./build/$(basename "$file" .cpp).o" ;done
# # # # # compress
# for file in ./src/compress/*.cpp; do $CXX $option_cmpl "$file" -o "./build/$(basename "$file" .cpp).o" ;done
# # # # # encrypt
# for file in ./src/encrypt/*.cpp; do $CXX $option_cmpl "$file" -o "./build/$(basename "$file" .cpp).o" ;done


# # # main.cpp
$CXX $option_cmpl ./src/main.cpp -o ./build/main.o     # g++ -g  ./src/main.cpp  -o ./build/main.o

# # # #   # # ./build/*.o -> compiler # # # # # # # # # 
$CXX  $option_link  ./build/*.o   -o ./build/backup 
 
./build/backup

#----GDB shell----
if false ;then
    gdb ./build/compiler 
    
    b primary.cpp : 11


    # b optimizer.cpp:439 if (int)strcmp(symbol.c_str(), "%main_and0_@main_0")==0
    # b optimizer.cpp:10
    # condition 5 ((int)strcmp(ident.c_str(), "res") == 0)
    ignore 11 10000
    ignore 11 0
fi
