energypoints="100 200 300 400 500 600 700 800 900 1000 1100 1200 1300 1400 1500"
# energypoints="100 300 500 700 900 1100 1300 1500"
# energypoints="400 800 1200"

betapoints="0.05 0.1 0.2 .5"
# betapoints="0.1"

NumberOfPoints=1000000
NumberOfBetaPoints=100000

echo "/run/initialize" > autodice.mac 
echo "/DetSys/world/material Vacuum" >> autodice.mac 
echo "/DetSys/gun/BeamSpot 1 mm" >> autodice.mac 
echo "/DetSys/det/RecordGun 1" >> autodice.mac 
echo "/DetSys/gun/position 0.0 0.0 0.0 mm " >> autodice.mac 

echo "/DetSys/gun/particle e-" >> autodice.mac 

echo "/DetSys/gun/coneMinPhi 210.0 deg" >> autodice.mac 
echo "/DetSys/gun/coneMaxPhi 330.0 deg" >> autodice.mac 

echo "/DetSys/world/TabMagneticField Field_m10Pg14_9.txt" >> autodice.mac 
echo "/DetSys/world/SetFieldOffset 0 25 0 mm" >> autodice.mac 
echo "/DetSys/world/SetFieldAntiMirror 1" >> autodice.mac 
echo "/DetSys/world/SetFieldMirror 2" >> autodice.mac 
echo "/DetSys/world/SetFieldAntiMirror 3" >> autodice.mac 

# echo "/DetSys/app/SetDiceField 0.18 tesla" >> autodice.mac 

# 	echo "DetSys/det/RemoveShield true" >> autodice.mac 

echo "/DetSys/det/AddBlocker true" >> autodice.mac 
echo "/DetSys/app/SetDiceArbA 25 mm" >> autodice.mac 
echo "/DetSys/app/SetDiceArbB 60 mm" >> autodice.mac 
echo "/DetSys/app/SetDiceArbC 15 mm" >> autodice.mac 
echo "/DetSys/app/SetDiceArbD -20 mm" >> autodice.mac 

echo "/DetSys/app/addDiceDetector 1" >> autodice.mac 


rm -rf flatorange*.root
for E in $energypoints;
do
	cat autodice.mac > autodice_main.mac 
	echo "/DetSys/gun/efficiencyEnergy $E keV" >> autodice_main.mac 
	echo "/run/beamOn $NumberOfPoints" >> autodice_main.mac 
# # 	#running the sim with macro created here
	./DICE10 autodice_main.mac 
	root -l -q DiceAutoSort/FlatOrangeScan.C"("$E","$NumberOfPoints*3")"
	
done
hadd -f SumDice.root *flatorange*.root
root -l -q DiceAutoSort/AutoPostProcess.C"(\"SumDice.root\")"
rm -rf *flatorange*.root
rm -rf autodice_main.mac

rm -rf DiceBetaSort.root
for B in $betapoints;
do
	for E in $energypoints;
	do
		cat autodice.mac > autodicebeta.mac 
		echo "/DetSys/gun/efficiencyEnergy $E keV" >> autodicebeta.mac 
		echo "/DetSys/gun/SetBeta $B" >> autodicebeta.mac 
		echo "/run/beamOn $NumberOfBetaPoints" >> autodicebeta.mac 
		./DICE10 autodicebeta.mac 
		mv g4out.root betadata$E.root	
	done
	
	hadd -f BetaData$B.root betadata*.root
	rm -rf betadata*.root
	
	root -l -q DiceAutoSort/FlatOrangePostSort.C"(\"SumDice.root\",\"Beta"$B"\","$B",\"DiceBetaSort.root\",\"BetaData"$B".root\")"

done
rm -rf autodice.mac
rm -rf autodicebeta.mac
rm -rf BetaData*.root


rm -rf DeltaSorted.root
cat autodice.mac > autodicedelta.mac 
echo "/DetSys/gun/efficiencyEnergy 50 keV" >> autodicedelta.mac 
echo "/DetSys/gun/SetBeta $B" >> autodicedelta.mac 
echo "/run/beamOn $NumberOfPoints" >> autodicedelta.mac 
./DICE10 autodicedelta.mac 
root -l -q DiceAutoSort/FlatOrangePostSort.C"(\"SumDice.root\",\"Deltas\",0,\"DeltaSorted.root\")"
rm -rf autodicedelta.mac


rm -rf GammaSorted.root

cat autodice.mac > autodicegamma.mac 
echo "/DetSys/gun/particle gamma" >> autodicegamma.mac 
echo "/DetSys/gun/efficiencyEnergy 511 keV" >> autodicegamma.mac 
echo "/DetSys/gun/SetBeta $B" >> autodicegamma.mac 
echo "/run/beamOn $NumberOfPoints" >> autodicegamma.mac 
./DICE10 autodicegamma.mac 
root -l -q DiceAutoSort/FlatOrangePostSort.C"(\"SumDice.root\",\"Gammas\",0,\"GammaSorted.root\")"
rm -rf autodicegamma.mac



hadd -f FullDiceSort.root SumDice.root DiceBetaSort.root GammaSorted.root DeltaSorted.root
