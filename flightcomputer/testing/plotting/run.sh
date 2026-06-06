echo Generating fake data 
g++ dataGenerator.cpp -o datagen
./datagen
echo Create json file
g++ jsonoutputter.cpp -o jsonoutput
./jsonoutput
echo Create graph
python3 exp.py
