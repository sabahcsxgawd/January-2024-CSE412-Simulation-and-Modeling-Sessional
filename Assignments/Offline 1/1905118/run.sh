clear

rm main.out
rm out*.txt

g++ -std=c++17 -fsanitize=address src/* -o main.out

./main.out