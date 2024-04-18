Folders="A B C D"
# Folders="B C D"
# Folders="X Y Z"

rm GammaTest.root
for F in $Folders;
do

# 	screen -dmS screen$F bash -c "mkdir  '$F' && cd '$F' && cmake ../source && make -j3; exit"
	screen -dmS screen$F bash -c "cd '$F' && bash AutoGam.sh; exit"
# 	screen -dmS screen$F bash -c "cd '$F' && bash AutoDiceSort/QuickAutoEff.sh; exit"
done
top
