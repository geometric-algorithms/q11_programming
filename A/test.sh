python3 generator.py $1
g++ -o vd VD.cpp
./vd < input.txt
python3 draw.py
