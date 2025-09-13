@REM g++ -std=c++20 main.cpp -o program
g++ -std=c++20 -O3 -march=native -DNDDEBUG main.cpp -Iassimp -Lassimp/library -lassimp -lz -o program