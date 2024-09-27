clear

rm build/*
rm out*.txt

g++ -std=c++17 -fsanitize=address src/* -o build/main

./build/main