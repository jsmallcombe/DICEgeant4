
energypoints="5 10 15 20 30 40 50 60 80 100 120 140 160 200 240 280 360 440 540 700 860 1020 1280 1500 1820"
NumberOfPoints=20000
do_G4_part=true

mkdir QuickAutoEff
if [ "$do_G4_part" = true ] ; then

	echo "/run/initialize" > autodice.mac 
	echo "/DetSys/world/material Vacuum" >> autodice.mac 
# 	echo "/DetSys/det/RecordGun 1" >> autodice.mac 
# 	echo "/DetSys/gun/BeamSpot 0.5 mm" >> autodice.mac 
	echo "/DetSys/gun/position 0.0 0.0 0.0 mm " >> autodice.mac 
	echo "/DetSys/gun/particle e-" >> autodice.mac 
	echo "/DetSys/gun/coneMinPhi 180.0 deg" >> autodice.mac 
	echo "/DetSys/gun/coneMaxPhi 360.0 deg" >> autodice.mac 

	echo "/DetSys/world/TabMagneticField Field.txt" >> autodice.mac 
# 	echo "/DetSys/world/SetFieldAntiMirror 3" >> autodice.mac 
	# # # # echo "/DetSys/World/ScaleField -0.75 tesla" >> autodice.mac 

# 	echo "/DetSys/det/SetDiceBool AddBlocker false" >> autodice.mac 
# 	echo "/DetSys/app/SetDiceLength MagThickA 5 mm" >> autodice.mac 
# 	echo "/DetSys/app/SetDiceLength MagThickB 5 mm" >> autodice.mac 
# 	echo "/DetSys/app/SetDiceLength YokeDepth 8 mm" >> autodice.mac 

	echo "/DetSys/app/addDiceDetector 1" >> autodice.mac 

	rm -rf QuickAutoEff/Tuple*.root
	for E in $energypoints;
	do
		cat autodice.mac > autodice_main.mac 
		echo "/DetSys/gun/efficiencyEnergy $E keV" >> autodice_main.mac 
		echo "/run/beamOn $NumberOfPoints" >> autodice_main.mac 
		./DICE10 autodice_main.mac 
		mv g4out.root QuickAutoEff/Tuple$E.root
	done
	rm -rf autodice_main.mac
	
	hadd -f QuickAutoEff/SumTuple.root QuickAutoEff/Tuple*.root

	rm -rf autodice.mac

fi

rm -rf EffOnly.root
root -l -q AutoDiceSort/EffOnly.C"(\"$energypoints\","$NumberOfPoints*2",\"EffOnly.root\",\"QuickAutoEff/SumTuple.root\")"
