energypoints="100 200 300 400 500 600 700 800 900 1000 1100 1200 1300 1400 1500"
# energypoints="100 300 500 700 900 1100 1300 1500"
#energypoints="400 800 1200"

rm -rf flatorange*.root

NumberOfPoints=1000000

for E in $energypoints;
do

	echo "/run/initialize" > autodice.mac 
	echo "/DetSys/world/material Vacuum" >> autodice.mac 
	echo "/DetSys/gun/BeamSpot 1 mm" >> autodice.mac 
	echo "/DetSys/det/RecordGun 1" >> autodice.mac 
	echo "/DetSys/gun/position 0.0 0.0 0.0 mm " >> autodice.mac 
	
	echo "/DetSys/gun/particle e-" >> autodice.mac 
# 	echo "/DetSys/gun/particle gamma" >> autodice.mac 
	echo "/DetSys/gun/efficiencyEnergy $E keV" >> autodice.mac 
	
#	echo "/DetSys/gun/coneMaxAngle 180.0 deg" >> autodice.mac 
#	echo "/DetSys/gun/coneMinAngle 90.0 deg" >> autodice.mac 
	echo "/DetSys/gun/coneMinPhi 210.0 deg" >> autodice.mac 
	echo "/DetSys/gun/coneMaxPhi 330.0 deg" >> autodice.mac 
	
	echo "/DetSys/app/SetDiceField 0.18 tesla" >> autodice.mac 
# 	echo "DetSys/det/RemoveShield true" >> autodice.mac 
	echo "/DetSys/det/AddBlocker true" >> autodice.mac 
	echo "/DetSys/app/SetDiceArbA 22.5 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceArbB 55 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceArbC 15 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceArbD 20 mm" >> autodice.mac 
	echo "/DetSys/app/addDiceDetector 1" >> autodice.mac 

	echo "/run/beamOn $NumberOfPoints" >> autodice.mac 

	#running the sim with macro created here
	./DICE10 autodice.mac 

	root -l -q FlatOrangeScan.C"("$E","$NumberOfPoints*3")"

done

hadd -f SumDice.root *flatorange*.root
rm -rf *flatorange*.root
rm -rf autodice.mac


root -l -q AutoPostProcess.C"(\"SumDice.root\")"
