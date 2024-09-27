clear

rm build/*
rm out*.txt

g++ -std=c++17 src/* -o build/main

./build/main