energypoints="100 200 300 400 500 600 700 800 900 1000 1100 1200 1300 1400 1500 1600 1700"
dE=100

# energypoints="200 400 600 800 1000 1200 1400"
# dE=200

# energypoints="400 800 1200"
# dE=400

betapoints="0.05 0.1 0.2 .5"
# betapoints="0.1"

do_G4_part=false
NumberOfPoints=1000000
NumberOfBetaPoints=100000

mkdir AutoSortFiles

# ...do something interesting...
if [ "$do_G4_part" = true ] ; then

	echo "/run/initialize" > autodice.mac 
	echo "/DetSys/world/material Vacuum" >> autodice.mac 
	echo "/DetSys/gun/BeamSpot 1 mm" >> autodice.mac 
	echo "/DetSys/det/RecordGun 1" >> autodice.mac 
	echo "/DetSys/det/RecordPentration 1" >> autodice.mac 
	echo "/DetSys/gun/position 0.0 0.0 0.0 mm " >> autodice.mac 

	echo "/DetSys/gun/particle e-" >> autodice.mac 

	echo "/DetSys/gun/coneMinPhi 210.0 deg" >> autodice.mac 
	echo "/DetSys/gun/coneMaxPhi 330.0 deg" >> autodice.mac 

	echo "/DetSys/world/TabMagneticField Field_m10Pg14_9.txt" >> autodice.mac 
	echo "/DetSys/world/SetFieldOffset 0 25 0 mm" >> autodice.mac 
	echo "/DetSys/world/SetFieldAntiMirror 1" >> autodice.mac 
	echo "/DetSys/world/SetFieldMirror 2" >> autodice.mac 
	echo "/DetSys/world/SetFieldAntiMirror 3" >> autodice.mac 

	# # # # Will scale field if used with TabMagneticField
	# # # # echo "/DetSys/app/SetDiceField 0.18 tesla" >> autodice.mac 

	# # # # echo "DetSys/det/RemoveShield true" >> autodice.mac 
	echo "/DetSys/det/AddBlocker true" >> autodice.mac 
	
	echo "/DetSys/app/SetDiceLength BeamDetY 60 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceLength MagMidOffset 25 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceLength ShieldMidBeamSep 15 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceLength MagY -20 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceLength MagGapMinHalf 10 mm" >> autodice.mac 
	##echo "/DetSys/app/SetDiceLength PoleSepHalf 10 mm" >> autodice.mac 

	# # # # Set negative to add a target foil and ISOK160 chamber
	echo "/DetSys/app/addDiceDetector 1" >> autodice.mac 

	# # # # Must be included when using real field. Automatic step determination is wrong for the complex field. 
	# # # # StepLimit = 20 mm optimum speed. StepLimit<=2mm *tiny* difference, but massive computation cost. 
	echo "/DetSys/phys/SetStepper 1" >> autodice.mac 
	echo "/DetSys/world/StepLimit 20 mm" >> autodice.mac 

	rm -rf AutoSortFiles/Tuple*.root
	for E in $energypoints;
	do
		cat autodice.mac > autodice_main.mac 
		echo "/DetSys/gun/efficiencyEnergy $E keV" >> autodice_main.mac 
		echo "/run/beamOn $NumberOfPoints" >> autodice_main.mac 
	# # 	running the sim with macro created here
		./DICE10 autodice_main.mac 
		
		mv g4out.root AutoSortFiles/Tuple$E.root
	done
	rm -rf autodice_main.mac
	
	hadd -f AutoSortFiles/SumTuple.root AutoSortFiles/Tuple*.root

	rm -rf AutoSortFiles/betadata*.root
	for B in $betapoints;
	do
		for E in $energypoints;
		do
			cat autodice.mac > autodicebeta.mac 
			echo "/DetSys/gun/efficiencyEnergy $E keV" >> autodicebeta.mac 
			echo "/DetSys/gun/SetBeta $B" >> autodicebeta.mac 
			echo "/run/beamOn $NumberOfBetaPoints" >> autodicebeta.mac 
			./DICE10 autodicebeta.mac 
			mv g4out.root AutoSortFiles/betadata"$B"_"$E".root	
		done
		
		hadd -f AutoSortFiles/BetaData$B.root AutoSortFiles/betadata"$B"_*.root
	done

	cat autodice.mac > autodicedelta.mac 
	echo "/DetSys/gun/efficiencyEnergy 50 keV" >> autodicedelta.mac 
	echo "/run/beamOn $NumberOfPoints" >> autodicedelta.mac 
	./DICE10 autodicedelta.mac 
	mv g4out.root AutoSortFiles/deltatup.root	
	rm -rf autodicedelta.mac


	cat autodice.mac > autodicehigh.mac 
	echo "/DetSys/gun/efficiencyEnergy 3000 keV" >> autodicehigh.mac 
	echo "/run/beamOn $NumberOfPoints" >> autodicehigh.mac 
	./DICE10 autodicehigh.mac 
	mv g4out.root AutoSortFiles/highetup.root	
	rm -rf autodicehigh.mac

	cat autodice.mac > autodicegamma.mac 
	echo "/DetSys/gun/particle gamma" >> autodicegamma.mac 
	echo "/DetSys/gun/efficiencyEnergy 511 keV" >> autodicegamma.mac 
	echo "/run/beamOn $NumberOfPoints" >> autodicegamma.mac 
	./DICE10 autodicegamma.mac 
	mv g4out.root AutoSortFiles/gammatup.root	
	rm -rf autodicegamma.mac

	rm -rf autodice.mac

fi

root -l -q AutoDiceSort/DiceEffScanCombinedSort.C"("$NumberOfPoints*3",\"SumDice.root\",\"AutoSortFiles/SumTuple.root\","$dE")"

rm -rf AutoSortFiles/DiceBetaSort.root
for B in $betapoints;
do
	root -l -q AutoDiceSort/ExptEquivSort.C"(\"SumDice.root\",\"Beta"$B"\","$B",\"AutoSortFiles/DiceBetaSort.root\",\"AutoSortFiles/BetaData"$B".root\","$NumberOfBetaPoints*3")"
done

rm -rf AutoSortFiles/ExtraSorted.root
	
root -l -q AutoDiceSort/ExptEquivSort.C"(\"SumDice.root\",\"Deltas\",0,\"AutoSortFiles/ExtraSorted.root\",\"AutoSortFiles/deltatup.root\")"
root -l -q AutoDiceSort/ExptEquivSort.C"(\"SumDice.root\",\"HighE\",0,\"AutoSortFiles/ExtraSorted.root\",\"AutoSortFiles/highetup.root\")"
root -l -q AutoDiceSort/ExptEquivSort.C"(\"SumDice.root\",\"Gammas\",0,\"AutoSortFiles/ExtraSorted.root\",\"AutoSortFiles/gammatup.root\")"

hadd -f FullDiceSort.root SumDice.root AutoSortFiles/DiceBetaSort.root AutoSortFiles/ExtraSorted.root
