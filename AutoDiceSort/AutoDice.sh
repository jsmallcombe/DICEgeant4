# # # #  For real final experimental cals, use the target and K160 code AND use the full 4pi gun!
# # # #  Current settings are fast, but underestimate background
do_G4_part=true
do_vischeck=true

	energypoints="100 200 300 400 500 600 700 800 900 1000 1100 1200 1300 1400 1500 1600"
	dE=100
	NumberOfPoints=500000

	betapoints="0.2"
	NumberOfBetaPoints=50000

	do_extras=true

		EffPoints="5 10 15 20 30 40 50 75 100 150 200 300 500 700 1000 1500 2000"
		NumberOfEffPoints=100000

		NumGamPoints=2000000

		do_lifetime_part=false
			lifeepoints="500 1000"

mkdir FilesAutoSort

# ...do something interesting...
if [ "$do_G4_part" = true ] ; then

	echo "/run/initialize" > autodice.mac 
	echo "/DetSys/world/material Vacuum" >> autodice.mac 
	echo "/DetSys/gun/BeamSpot 0.5 mm" >> autodice.mac 
	echo "/DetSys/gun/position 0.0 0.0 0.0 mm " >> autodice.mac 

	echo "/DetSys/gun/particle e-" >> autodice.mac 

	echo "/DetSys/gun/coneMinPhi 210.0 deg" >> autodice.mac 
	echo "/DetSys/gun/coneMaxPhi 330.0 deg" >> autodice.mac 

	echo "/DetSys/world/TabMagneticField Field.txt" >> autodice.mac 
 	echo "/DetSys/world/SetFieldAntiMirror 3" >> autodice.mac 

 	echo "/DetSys/app/SetDiceLength MagThickA 5 mm" >> autodice.mac 
 	echo "/DetSys/app/SetDiceLength MagThickB 5 mm" >> autodice.mac 

 	cat autodice.mac > effonly.mac 
 	
	# # # # Set negative to add a target foil and ISOK160 chamber
	echo "/DetSys/app/addDiceDetector 1" >> autodice.mac 

	if [ "$do_vischeck" = true ] ; then
		cat autodice.mac > autovis.mac 
		cat AutoDiceSort/autovis.mac >> autovis.mac 
		./DICE10 -v autovis.mac 
		rm -rf autovis.mac
	fi
	
# # # 		Main

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
	
# # # 		Main

# # # 		Betas

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
	
# # # 		Betas

# # # 		Extras
	if [ "$do_extras" = true ] ; then
	
# # # 		Efficiency
		echo "/DetSys/gun/coneMinPhi 180.0 deg" >> effonly.mac 
		echo "/DetSys/gun/coneMaxPhi 360.0 deg" >> effonly.mac 
			
		rm -rf FilesAutoSort/EffTuple*.root
		for E in $EffPoints;
		do
			cat effonly.mac > effone.mac 
			cat effonly.mac > efftwo.mac 
			echo "/DetSys/app/SetDiceBool AddBlocker false" >> efftwo.mac 
			echo "/DetSys/app/addDiceDetector 1" >> effone.mac 
			echo "/DetSys/app/addDiceDetector 1" >> efftwo.mac 
			echo "/DetSys/gun/efficiencyEnergy $E keV" >> effone.mac 
			echo "/DetSys/gun/efficiencyEnergy $E keV" >> efftwo.mac 
			echo "/run/beamOn $NumberOfEffPoints" >> effone.mac 
			echo "/run/beamOn $NumberOfEffPoints" >> efftwo.mac 
			./DICE10 effone.mac 
			mv g4out.root FilesAutoSort/EffTupleOne$E.root
			./DICE10 efftwo.mac 
			mv g4out.root FilesAutoSort/EffTupleTwo$E.root
		done
		rm -rf eff*.mac
		
		hadd -f FilesAutoSort/EffSumTupleOne.root FilesAutoSort/EffTupleOne*.root
		hadd -f FilesAutoSort/EffSumTupleTwo.root FilesAutoSort/EffTupleTwo*.root

# # # 		Efficiency
# # # 		High E electrons (Betas)

		cat autodice.mac > autodicehigh.mac 
		echo "/DetSys/gun/efficiencyEnergy 3000 keV" >> autodicehigh.mac 
		echo "/run/beamOn $NumberOfPoints" >> autodicehigh.mac 
		./DICE10 autodicehigh.mac 
		mv g4out.root FilesAutoSort/highetup.root	
		rm -rf autodicehigh.mac

# # # 		High E electrons (Betas)		
# # # 		Gamma
		
		rm -rf	FilesAutoSort/gammatup*.root
		
		cat autodice.mac > autogam.mac 
		echo "/DetSys/gun/particle gamma" >> autogam.mac 
		echo "/DetSys/gun/position 0.0 5.0 0.0 mm " >> autogam.mac 
		echo "/DetSys/gun/BeamSpot 2.5 mm" >> autogam.mac 
		
		cat autogam.mac > autogamA.mac 
		cat autogam.mac > autogamB.mac 
		cat autogam.mac > autogamC.mac 
		
		echo "/DetSys/gun/efficiencyEnergy 10 keV" >> autogamA.mac 
		echo "/DetSys/gun/efficiencyEnergy 100 keV" >> autogamB.mac 
		echo "/DetSys/gun/efficiencyEnergy 511 keV" >> autogamC.mac 
		echo "/run/beamOn $NumGamPoints" >> autogamA.mac 
		echo "/run/beamOn $NumGamPoints" >> autogamB.mac 
		echo "/run/beamOn $NumGamPoints" >> autogamC.mac 
		
		./DICE10 autogamA.mac 
		mv g4out.root FilesAutoSort/gammatupA.root	
		./DICE10 autogamB.mac 
		mv g4out.root FilesAutoSort/gammatupB.root	
		./DICE10 autogamC.mac 
		mv g4out.root FilesAutoSort/gammatupC.root	
		rm -rf autoga*.mac
			
		hadd -f FilesAutoSort/gammatup.root FilesAutoSort/gammatup*.root

# # # 		Gamma
# # # 		Lifetime
		
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
# # # 		Lifetime

# # # 		Extras
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
else
	cp SumDice.root FullDiceSort.root
fi

if [ "$do_extras" = true ] ; then

	rm -rf FilesAutoSort/ExtraSorted.root

	root -l -q AutoDiceSort/EffOnly.C"(\"$EffPoints\","$NumberOfEffPoints*2",\"FullDiceSort.root\",\"FilesAutoSort/EffSumTupleOne.root\",\"Eff\")"
	root -l -q AutoDiceSort/EffOnly.C"(\"$EffPoints\","$NumberOfEffPoints*2",\"FullDiceSort.root\",\"FilesAutoSort/EffSumTupleTwo.root\",\"EffSansBlock\")"
	
	root -l -q AutoDiceSort/ExptEquivSort.C"(\"SumDice.root\",\"HighE\",0,\"FullDiceSort.root\",\"FilesAutoSort/highetup.root\")"
	root -l -q AutoDiceSort/ExptEquivSort.C"(\"SumDice.root\",\"Gammas\",0,\"FullDiceSort.root\",\"FilesAutoSort/gammatup.root\")"

	if [ "$do_lifetime_part" = true ] ; then
		rm -rf FilesAutoSort/Lifetime.root
		root -l -q AutoDiceSort/ExptEquivSort.C"(\"SumDice.root\",\"LifetimeA\",0.1,\"FullDiceSort.root\",\"FilesAutoSort/TupleLifeA.root\","$NumberOfBetaPoints*3")"
		root -l -q AutoDiceSort/ExptEquivSort.C"(\"SumDice.root\",\"LifetimeB\",0.1,\"FullDiceSort.root\",\"FilesAutoSort/TupleLifeB.root\","$NumberOfBetaPoints*3")"
	fi

fi








	
	
