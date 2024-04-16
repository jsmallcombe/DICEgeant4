
energypoints="5 10 15 20 30 40 50 60 80 100 120 140 160 200 240 280 360 440 540 700 860 1020 1280 1500 1820"
NumberOfPoints=20000
do_G4_part=true

NumGamPoints=1000000


mkdir QuickAutoEff
if [ "$do_G4_part" = true ] ; then

	echo "/run/initialize" > autodice.mac 
	echo "/DetSys/world/material Vacuum" >> autodice.mac 
	echo "/DetSys/det/RecordGun 1" >> autodice.mac 
	echo "/DetSys/gun/BeamSpot 0.5 mm" >> autodice.mac 
	echo "/DetSys/gun/position 0.0 0.0 0.0 mm " >> autodice.mac 
	echo "/DetSys/gun/particle e-" >> autodice.mac 
	echo "/DetSys/gun/coneMinPhi 180.0 deg" >> autodice.mac 
	echo "/DetSys/gun/coneMaxPhi 360.0 deg" >> autodice.mac 

	echo "/DetSys/world/TabMagneticField Field.txt" >> autodice.mac 
	
	echo "/DetSys/world/SetFieldOffset 0 30 0 mm" >> autodice.mac 
	echo "/DetSys/world/SetFieldAntiMirror 1" >> autodice.mac 
	echo "/DetSys/world/SetFieldMirror 2" >> autodice.mac 
	echo "/DetSys/world/SetFieldAntiMirror 3" >> autodice.mac 
	# # # # echo "/DetSys/app/SetDiceField -0.5 tesla" >> autodice.mac 
	echo "/DetSys/det/AddBlocker true" >> autodice.mac 
	
	echo "/DetSys/app/SetDiceLength BeamDetY 60 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceLength MagMidOffset 30 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceLength ShieldMidBeamSep 20 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceLength MagY 20 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceLength MagGapMinHalf 10 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceLength PoleSepHalf 16 mm" >> autodice.mac 

	echo "/DetSys/app/addDiceDetector 1" >> autodice.mac 

	echo "/DetSys/phys/SetStepper 1" >> autodice.mac 
	echo "/DetSys/world/StepLimit 20 mm" >> autodice.mac 

	
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

	rm -rf QuickAutoEff/GamTuple*.root
	
	cat autodice.mac > autodice_gam.mac 
	echo "/DetSys/gun/particle gamma" >> autodice_gam.mac 
	echo "/DetSys/gun/efficiencyEnergy 10 keV" >> autodice_gam.mac 
	echo "/DetSys/gun/BeamSpot 0 mm" >> autodice_gam.mac 
	echo "/DetSys/gun/position 0.0 5.0 0.0 mm " >> autodice_gam.mac 
	echo "/run/beamOn $NumGamPoints" >> autodice_gam.mac 
	./DICE10 autodice_gam.mac 
	mv g4out.root QuickAutoEff/GamTuple10Miss.root
	
	cat autodice.mac > autodice_gam.mac 
	echo "/DetSys/gun/particle gamma" >> autodice_gam.mac 
	echo "/DetSys/gun/efficiencyEnergy 100 keV" >> autodice_gam.mac 
	echo "/DetSys/gun/BeamSpot 0 mm" >> autodice_gam.mac 
	echo "/DetSys/gun/position 0.0 5.0 0.0 mm " >> autodice_gam.mac 
	echo "/run/beamOn $NumGamPoints" >> autodice_gam.mac 
	./DICE10 autodice_gam.mac 
	mv g4out.root QuickAutoEff/GamTuple100Miss.root
	
	cat autodice.mac > autodice_gam.mac 
	echo "/DetSys/gun/particle gamma" >> autodice_gam.mac 
	echo "/DetSys/gun/efficiencyEnergy 10 keV" >> autodice_gam.mac 
	echo "/DetSys/gun/BeamSpot 2.5 mm" >> autodice_gam.mac 
	echo "/run/beamOn $NumGamPoints" >> autodice_gam.mac 
	./DICE10 autodice_gam.mac 
	mv g4out.root QuickAutoEff/GamTuple10Hit.root
	
	cat autodice.mac > autodice_gam.mac 
	echo "/DetSys/gun/particle gamma" >> autodice_gam.mac 
	echo "/DetSys/gun/efficiencyEnergy 100 keV" >> autodice_gam.mac 
	echo "/DetSys/gun/BeamSpot 2.5 mm" >> autodice_gam.mac 
	echo "/run/beamOn $NumGamPoints" >> autodice_gam.mac 
	./DICE10 autodice_gam.mac 
	mv g4out.root QuickAutoEff/GamTuple100Hit.root

	
	rm -rf autodice.mac

fi

rm -rf EffOnly.root
root -l -q AutoDiceSort/EffOnly.C"(\"$energypoints\","$NumberOfPoints*2",\"EffOnly.root\",\"QuickAutoEff/SumTuple.root\")"

hadd  -f tmp.root QuickAutoEff/GamTuple10Miss.root QuickAutoEff/GamTuple100Miss.root
root -l -q AutoDiceSort/RawSort.C"(\"EffOnly.root\",\"tmp.root\",\"XrayMiss\")"
hadd -f tmp.root QuickAutoEff/GamTuple10Hit.root QuickAutoEff/GamTuple100Hit.root
root -l -q AutoDiceSort/RawSort.C"(\"EffOnly.root\",\"tmp.root\",\"XrayHit\")"
