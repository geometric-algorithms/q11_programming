make clean
make
python3 generator.py $1
./trapmap < input.txt
python3 draw.py