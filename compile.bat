@REM g++ -std=c++20 main.cpp -o program
g++ -std=c++20 -O3 -march=native -DNDDEBUG main.cpp -Iassimp -Lassimp/library -lassimp -lz -ISFML/include -LSFML/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -luuid -lws2_32 -static-libgcc -static-libstdc++ -pthread -o program
