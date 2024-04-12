#!/bin/bash
for var in "$@"
do
    sed -i 's/NaN/0.0/g' $var
    echo "zxy 100 100 100" > tmp.txt
    tail -n +10 $var >> tmp.txt
    mv tmp.txt $var
done
