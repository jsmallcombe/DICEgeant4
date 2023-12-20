energypoints="100 200 300 400 500 600 700 800 900 1000 1100 1200 1300 1400 1500"
# energypoints="400 600"


rm -rf flatorange*.root

NumberOfPoints=1000000

for E in $energypoints;
do

	echo "/run/initialize" > autodice.mac 
	echo "/DetSys/world/material Vacuum" >> autodice.mac 
	echo "/DetSys/gun/particle e-" >> autodice.mac 
	echo "/DetSys/gun/efficiencyEnergy $E keV" >> autodice.mac 
	echo "/DetSys/gun/BeamSpot 2 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceField 0.3 tesla" >> autodice.mac 
	echo "/DetSys/app/SetDiceArbA -7.50 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceArbB 55 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceArbC 15 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceArbD -15 mm" >> autodice.mac 
	echo "/DetSys/app/addDiceDetector 1" >> autodice.mac 

	echo "/DetSys/gun/position 0.0 0.0 0.0 mm " >> autodice.mac 
	echo "/DetSys/det/RecordGun 1" >> autodice.mac 

	echo "/run/beamOn $NumberOfPoints" >> autodice.mac 

	#running the sim with macro created here
	./DICE10 autodice.mac 

	root -l -q FlatOrangeScan.C"("$E","$NumberOfPoints",\"flatorange"$E".root\")"

done

hadd -f SumDice.root flatorange*.root
rm -rf flatorange*.root
rm -rf autodice.mac
