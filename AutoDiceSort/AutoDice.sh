# # # #  For real final experimental cals, use the target and K160 code AND use the full 4pi gun!
# # # #  Current settings are fast, but underestimate background

energypoints="100 200 300 400 500 600 700 800 900 1000 1100 1200 1300 1400 1500 1600 1700"
dE=100
# energypoints="200 400 600 800 1000 1200 1400"
# dE=200
# # energypoints="400 800 1200"
# # dE=400

# betapoints="0.05 0.1 0.2 .5"
betapoints="0.2"

lifeepoints="500 1000"

NumberOfPoints=500000
NumberOfBetaPoints=50000
do_G4_part=false
do_vischeck=true
do_extras=true
do_lifetime_part=true

deltapoints="5 10 15 20 25 30 35 40"
dd=5
# extras = gammas,betas,deltas, and lifetimes

mkdir FilesAutoSort

# ...do something interesting...
if [ "$do_G4_part" = true ] ; then

	echo "/run/initialize" > autodice.mac 
	echo "/DetSys/world/material Vacuum" >> autodice.mac 
	echo "/DetSys/gun/BeamSpot 0.5 mm" >> autodice.mac 
	echo "/DetSys/det/RecordGun 1" >> autodice.mac 
	echo "/DetSys/det/RecordPentration 1" >> autodice.mac 
	echo "/DetSys/gun/position 0.0 0.0 0.0 mm " >> autodice.mac 

	echo "/DetSys/gun/particle e-" >> autodice.mac 

	echo "/DetSys/gun/coneMinPhi 210.0 deg" >> autodice.mac 
	echo "/DetSys/gun/coneMaxPhi 330.0 deg" >> autodice.mac 

	echo "/DetSys/world/TabMagneticField Field.txt" >> autodice.mac 
	echo "/DetSys/world/SetFieldOffset 0 30 0 mm" >> autodice.mac 
	echo "/DetSys/world/SetFieldAntiMirror 1" >> autodice.mac 
	echo "/DetSys/world/SetFieldMirror 2" >> autodice.mac 
	echo "/DetSys/world/SetFieldAntiMirror 3" >> autodice.mac 

	# # # # Will scale field if used with TabMagneticField
	# # # # echo "/DetSys/app/SetDiceField 0.18 tesla" >> autodice.mac 

	# # # # echo "DetSys/det/RemoveShield true" >> autodice.mac 
	echo "/DetSys/det/AddBlocker true" >> autodice.mac 
	
	echo "/DetSys/app/SetDiceLength BeamDetY 60 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceLength MagMidOffset 30 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceLength ShieldMidBeamSep 20 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceLength MagY -20 mm" >> autodice.mac 
	echo "/DetSys/app/SetDiceLength MagGapMinHalf 10 mm" >> autodice.mac 
	##echo "/DetSys/app/SetDiceLength PoleSepHalf 10 mm" >> autodice.mac 

	# # # # Set negative to add a target foil and ISOK160 chamber
	echo "/DetSys/app/addDiceDetector 1" >> autodice.mac 

	# # # # Must be included when using real field. Automatic step determination is wrong for the complex field. 
	# # # # StepLimit = 20 mm optimum speed. StepLimit<=2mm *tiny* difference, but massive computation cost. 
	echo "/DetSys/phys/SetStepper 1" >> autodice.mac 
	echo "/DetSys/world/StepLimit 20 mm" >> autodice.mac 

	if [ "$do_vischeck" = true ] ; then
		cat autodice.mac > autovis.mac 
		cat AutoDiceSort/autovis.mac >> autovis.mac 
		./DICE10 -v autovis.mac 
		rm -rf autovis.mac
	fi
	
	# 	With the new thata/shield gap we dont get the full background but its twice as fast
	CorrectedN=$(($NumberOfPoints / 4))
	# 	Phi range = 1/3, Theta range = 1/2 (2x 1/4)
	#   For theta, number of events run actually fewer
	#   For phi, run all events, but scale efficiency 
	
	rm -rf FilesAutoSort/Tuple*.root
	for E in $energypoints;
	do
		cat autodice.mac > autodice_main.mac 
		echo "/DetSys/gun/efficiencyEnergy $E keV" >> autodice_main.mac 
		echo "/DetSys/gun/coneMaxAngle 60.0 deg" >> autodice_main.mac 
		echo "/run/beamOn $CorrectedN" >> autodice_main.mac 
		./DICE10 autodice_main.mac 
		mv g4out.root g4outA.root
		cat autodice.mac > autodice_main.mac 
		echo "/DetSys/gun/efficiencyEnergy $E keV" >> autodice_main.mac 
		echo "/DetSys/gun/coneMinAngle 120 deg" >> autodice_main.mac 
		echo "/run/beamOn $CorrectedN" >> autodice_main.mac 
		./DICE10 autodice_main.mac 
		hadd -f FilesAutoSort/Tuple$E.root g4out.root g4outA.root
# 		mv g4out.root FilesAutoSort/Tuple$E.root
	done
	rm -rf autodice_main.mac
	
	hadd -f FilesAutoSort/SumTuple.root FilesAutoSort/Tuple*.root

	rm -rf FilesAutoSort/betadata*.root
	for B in $betapoints;
	do
		for E in $energypoints;
		do
			cat autodice.mac > autodicebeta.mac 
			echo "/DetSys/gun/efficiencyEnergy $E keV" >> autodicebeta.mac 
			echo "/DetSys/gun/SetBeta $B" >> autodicebeta.mac 
			echo "/run/beamOn $NumberOfBetaPoints" >> autodicebeta.mac 
			./DICE10 autodicebeta.mac 
			mv g4out.root FilesAutoSort/betadata"$B"_"$E".root	
		done
		
		hadd -f FilesAutoSort/BetaData$B.root FilesAutoSort/betadata"$B"_*.root
	done

	
	if [ "$do_extras" = true ] ; then
	
		rm -rf FilesAutoSort/DeltaTuple*.root
		for E in $deltapoints;
		do
			cat autodice.mac > autodicedelta.mac 
			echo "/DetSys/gun/efficiencyEnergy $E keV" >> autodicedelta.mac 
				echo "/run/beamOn $NumberOfBetaPoints" >> autodicedelta.mac 
			./DICE10 autodicedelta.mac 
			mv g4out.root FilesAutoSort/DeltaTuple$E.root
		done
		rm -rf autodicedelta.mac
		
		hadd -f FilesAutoSort/deltatup.root FilesAutoSort/DeltaTuple*.root

		cat autodice.mac > autodicehigh.mac 
		echo "/DetSys/gun/efficiencyEnergy 3000 keV" >> autodicehigh.mac 
		echo "/run/beamOn $NumberOfPoints" >> autodicehigh.mac 
		./DICE10 autodicehigh.mac 
		mv g4out.root FilesAutoSort/highetup.root	
		rm -rf autodicehigh.mac

		cat autodice.mac > autodicegamma.mac 
		echo "/DetSys/gun/particle gamma" >> autodicegamma.mac 
		echo "/DetSys/gun/efficiencyEnergy 511 keV" >> autodicegamma.mac 
		echo "/run/beamOn $NumberOfPoints" >> autodicegamma.mac 
		./DICE10 autodicegamma.mac 
		mv g4out.root FilesAutoSort/gammatup.root	
		rm -rf autodicegamma.mac

		
		if [ "$do_lifetime_part" = true ] ; then
			rm -rf FilesAutoSort/TupleLife*.root 
			for E in $lifeepoints;
			do
				cat autodice.mac > autodicelife.mac 
				echo "/DetSys/gun/efficiencyEnergy $E keV" >> autodicelife.mac 
				echo "/DetSys/gun/SetBeta 0.1" >> autodicelife.mac 
				echo "/DetSys/gun/SetLifetime 1 ns" >> autodicelife.mac 
				echo "/run/beamOn $NumberOfBetaPoints" >> autodicelife.mac 
			# # 	running the sim with macro created here
				./DICE10 autodicelife.mac 
				mv g4out.root FilesAutoSort/TupleLifeA$E.root
				
				cat autodice.mac > autodicelife.mac 
				echo "/DetSys/gun/efficiencyEnergy $E keV" >> autodicelife.mac 
				echo "/DetSys/gun/SetBeta 0.02" >> autodicelife.mac 
				echo "/DetSys/gun/SetLifetime 10 ns" >> autodicelife.mac 
				echo "/run/beamOn $NumberOfBetaPoints" >> autodicelife.mac 
			# # 	running the sim with macro created here
				./DICE10 autodicelife.mac 
				mv g4out.root FilesAutoSort/TupleLifeB$E.root
			done
			hadd -f FilesAutoSort/TupleLifeA.root FilesAutoSort/TupleLifeA*.root
			hadd -f FilesAutoSort/TupleLifeB.root FilesAutoSort/TupleLifeB*.root
			rm -rf autodicelife.mac
		fi
	fi
	
	rm -rf autodice.mac

fi


root -l -q AutoDiceSort/DiceEffScanCombinedSort.C"("$NumberOfPoints*3",\"SumDice.root\",\"FilesAutoSort/SumTuple.root\","$dE")"

if [ ${#betapoints} -gt 0 ]; then
	rm -rf FilesAutoSort/DiceBetaSort.root
	for B in $betapoints;
	do
		root -l -q AutoDiceSort/ExptEquivSort.C"(\"SumDice.root\",\"Beta"$B"\","$B",\"FilesAutoSort/DiceBetaSort.root\",\"FilesAutoSort/BetaData"$B".root\","$NumberOfBetaPoints*3","$dE")"
	done
	
	hadd -f FullDiceSort.root SumDice.root FilesAutoSort/DiceBetaSort.root
fi

if [ "$do_extras" = true ] ; then

	rm -rf FilesAutoSort/ExtraSorted.root

	root -l -q AutoDiceSort/ExptEquivSort.C"(\"SumDice.root\",\"Deltas\",0,\"FullDiceSort.root\",\"FilesAutoSort/deltatup.root\","$NumberOfBetaPoints*3","$dE")"
	root -l -q AutoDiceSort/ExptEquivSort.C"(\"SumDice.root\",\"HighE\",0,\"FullDiceSort.root\",\"FilesAutoSort/highetup.root\")"
	root -l -q AutoDiceSort/ExptEquivSort.C"(\"SumDice.root\",\"Gammas\",0,\"FullDiceSort.root\",\"FilesAutoSort/gammatup.root\")"


	if [ "$do_lifetime_part" = true ] ; then
		rm -rf FilesAutoSort/Lifetime.root
		root -l -q AutoDiceSort/ExptEquivSort.C"(\"SumDice.root\",\"LifetimeA\",0.1,\"FullDiceSort.root\",\"FilesAutoSort/TupleLifeA.root\","$NumberOfBetaPoints*3")"
		root -l -q AutoDiceSort/ExptEquivSort.C"(\"SumDice.root\",\"LifetimeB\",0.1,\"FullDiceSort.root\",\"FilesAutoSort/TupleLifeB.root\","$NumberOfBetaPoints*3")"
	fi
fi

