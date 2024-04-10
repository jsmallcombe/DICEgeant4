#!/bin/bash
sed -i 's/NaN/0.0/g' $1
echo "zxy 100 100 100" > tmp.txt
tail -n +10 $1 >> tmp.txt
mv tmp.txt $1
