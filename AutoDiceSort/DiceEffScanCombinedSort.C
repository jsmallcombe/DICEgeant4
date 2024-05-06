void DiceEffScanCombinedSort(double SimmN=1000000,string rootout = "", const char *rootin = "g4out.root",double dE=100,bool symmetrize=true) {
	
    double grid_eff_cut=0.0001; // Minimum fraction of good events to leave channel enabled
    
    double THETALIMDEG=10;//The limit on allowed theta std in a segment
    
	double noisefactor=2; 	// keV sigma. Factors should be tuned to data when possible 
    
    
//////////////////////////////////////////////////////////////////////////////// 
    
	int Ebin = round(2000/dE);
	double EbinMin=dE*0.5;
	double EbinMax=EbinMin+dE*Ebin;
	
    if(rootout.find(".root")>rootout.size()){
        stringstream ss;
        ss<<rootout<<"_flatorange.root";
        rootout=ss.str();
    }
    
	//// Random number generator to add electronic noise on to "perfect" geant4 detectors ///
	TRandom r;
	r.SetSeed();

    // Load the input file
    TFile *newfile = new TFile(rootin);
	gROOT->cd();
    if(!newfile->IsOpen())return;
    TTree *newtree = (TTree*)newfile->Get("ntuple");
    if(newtree==nullptr)return;

	// Create the output file and create the histograms we will fill
	TFile out(rootout.c_str(),"RECREATE");
	out.cd();//cd into output file so histograms created in memory are associated with that file at creation rather than being manually saved there later.

    TH1F* E_RawSum=new TH1F("E_RawSum","RawHitEnergy;Electron Energy (keV);Counts",2000,0,2000);
    TH2F* E_RawSplit=new TH2F("E_RawSplit","RawHitsVsEmissionEnergy;Hit Energy (keV);Emission Energy (keV);Counts",2000,0,2000,Ebin,EbinMin,EbinMax);
    TH2F* E_RawChan=new TH2F("E_RawChan","RawHitEnergyChan;Electron Energy (keV);Hit Segment;Counts",2000,0,2000,16,0,16);

    TH1F* E_AddbackSum=new TH1F("E_AddbackSum","AddbackEnergy;Electron Energy (keV);Counts",2000,0,2000);
    TH1F* E_AddbackVetoSum=new TH1F("E_AddbackVetoSum","AddbackVetoEnergy;Electron Energy (keV);Counts",2000,0,2000);
    
    TH2F* E_AVSplit=new TH2F("E_AVSplit","AVHitsVsEmissionEnergy;Hit Energy (keV);Emission Energy (keV);Counts",2000,0,2000,Ebin,EbinMin,EbinMax);
    TH2F* E_AVChan=new TH2F("E_AVChan","AVHitEnergyChan;Electron Energy (keV);Hit Segment;Counts",2000,0,2000,16,0,16);
    
    TH1F* E_AVGood=new TH1F("E_AVGood","EmissionEqualAddbackSelected;Electron Energy (keV);Counts",2000,0,2000);
    
    TH2F* E_Chan_GoodE=new TH2F("E_Chan_GoodE","FullEnergyVsSeg;Electron Energy (keV);Hit Segment;Counts",Ebin,EbinMin,EbinMax,16,0,16);
    TH2F* E_Chan_GoodEThetaLim=new TH2F("E_Chan_GoodEThetaLim","FullEnergyVsSegThetaLimited;Electron Energy (keV);Hit Segment;Counts",Ebin,EbinMin,EbinMax,16,0,16);
    TH2F* E_Chan_GoodEThetaLimHalf=new TH2F("E_Chan_GoodEThetaLimHalf","FullEnergyVsSegThetaLimitedHalf;Electron Energy (keV);Hit Segment;Counts",Ebin,EbinMin,EbinMax,16,0,16);
    
    TH1F* Efficiency=new TH1F("Efficiency","Efficiency;Emission Energy (keV);Full Energy Detection Efficiency (%)",Ebin,EbinMin,EbinMax);
    TH1F* PeakTotal=new TH1F("PeakTotal","PeakToTotal;Emission Energy (keV);Detection PeakToTotal",Ebin,EbinMin,EbinMax);
    
    TH1F* EfficiencyT=new TH1F("EfficiencyT","EfficiencyThetaCut;Emission Energy (keV);Full Energy Detection Efficiency (%)",Ebin,EbinMin,EbinMax);
    TH1F* EfficiencyTHalf=new TH1F("EfficiencyTHalf","EfficiencyThetaCutHalf;Emission Energy (keV);Full Energy Detection Efficiency (%)",Ebin,EbinMin,EbinMax);
    
    TH1F* GridSpreadA=new TH1F("GridSpreadA","GridSpreadA;Emission Energy (keV);N_{grid} > 10%  Events",Ebin,EbinMin,EbinMax);
    TH1F* GridSpreadB=new TH1F("GridSpreadB","GridSpreadB;Emission Energy (keV);N_{grid} > 5% Events",Ebin,EbinMin,EbinMax);
    
    
    // histograms which reject events which are rare (but not impossible) energy for for that channel
	out.mkdir("LowChanCut");
	out.cd("LowChanCut");
    
        TH1F* EfficiencyCut=new TH1F("EfficiencyCut","Efficiency;Emission Energy (keV);Full Energy Detection Efficiency (%)",Ebin,EbinMin,EbinMax);
        TH1F* PeakTotalCut=new TH1F("PeakTotalCut","PeakToTotal;Emission Energy (keV);Detection PeakToTotal",Ebin,EbinMin,EbinMax);

        TH1F* EfficiencyTCut=new TH1F("EfficiencyTCut","EfficiencyThetaCut;Emission Energy (keV);Full Energy Detection Efficiency (%)",Ebin,EbinMin,EbinMax);
        TH1F* EfficiencyTHalfCut=new TH1F("EfficiencyTHalfCut","EfficiencyThetaCutHalf;Emission Energy (keV);Full Energy Detection Efficiency (%)",Ebin,EbinMin,EbinMax);
	out.cd("");
	
    // Known emission angles
	out.mkdir("Angles");
	out.cd("Angles");
    TH3F* AngleESeg=new TH3F("AngleESeg","EmissionAngleEnergyHitSegment;Emission Angle Theta #theta [Rad.];Emission Energy (keV);Hit Segment",360,0,3.14159,Ebin,EbinMin,EbinMax,16,0,16);
    TH2F* AngleESegGrid=new TH2F("AngleESegGrid","AngleESegGrid;Angle #theta [Rad.];Emission Energy (keV)",180*4,0,TMath::Pi()*4,15*4,50,6050);
    
        TH2F* ThetaSigESeg=new TH2F("ThetaSigESeg","ThetaSigmaESeg;Emission Energy (keV);Segment;S.t.d. Theta (deg.)",Ebin,EbinMin,EbinMax,16,0,16);
    
    
        TH2F* E_Theta=new TH2F("E_Theta","EmissionEnergyVsAngle;Emission Angle Theta #theta [Rad.];Emission Energy (keV)",180,0,3.14159,Ebin,EbinMin,EbinMax);
    
        TH2F *GridAngle[16];
        for(int i=0;i<16;i++){
            stringstream ss;ss<<"SegmentAngle"<< setw(2) << setfill('0')<<i;
            GridAngle[i] = new TH2F(ss.str().c_str(),(ss.str()+";Emission Angle Theta #theta [Rad.];Electron Energy (keV)").c_str(),360,0,3.14159,Ebin,EbinMin,EbinMax);
        }
        
        TH2F* AngleSeg[20];
        for(int e=0;e<20;e++){
            stringstream ss;ss<<"AngleSeg_"<< setw(4) << setfill('0')<<(e+1)*dE<<"keV";
            AngleSeg[e] = new TH2F(ss.str().c_str(),(ss.str()+";Emission Angle Theta #theta [Rad.];Hit Segment").c_str(),360,0,3.14159,16,0,16);
        }
        
        out.mkdir("Angles/ThetaPhi");
        out.cd("Angles/ThetaPhi");
            
            TH3F* ChanThetaPhi[20];
            for(int e=0;e<20;e++){
                stringstream nn;
                nn<<dE*(e+1)<<"_ChanThetaPhi";
                ChanThetaPhi[e]=new TH3F(nn.str().c_str(),(nn.str()+";Emission Angle Theta #theta [Rad.];Emission Angle Phi #theta [Rad.];Hit Segment").c_str(),180,0,3.14159,180,0,3.14159,8,0,8);
            }
            TH3F* EThetaPhi[8];
            for(int e=0;e<8;e++){
                stringstream nn;
                nn<<"Chan"<<e<<"_EThetaPhi";
                EThetaPhi[e]=new TH3F(nn.str().c_str(),(nn.str()+";Emission Angle Theta #theta [Rad.];Emission Angle Phi #theta [Rad.];Beam Energy").c_str(),180,0,3.14159,180,0,3.14159,Ebin,EbinMin,EbinMax);
            }
            TH2F* ThetaPhi[20];
            for(int e=0;e<20;e++){
                stringstream nn;
                nn<<dE*(e+1)<<"_ThetaPhi";
                ThetaPhi[e]=new TH2F(nn.str().c_str(),(nn.str()+";Emission Angle Theta #theta [Rad.];Emission Angle Phi #theta [Rad.]").c_str(),180,0,3.14159,180,0,3.14159);
            }
            
        out.mkdir("Angles/PenAngle");
        out.cd("Angles/PenAngle");
	
			TH2F* PenThetaTot=new TH2F("PenThetaTot","PenThetaTot;Pen Angle Theta #theta [Rad.];Hit Segment",180,0,3.14159,16,0,16);
			TH2F* PenTheta[20];
            for(int e=0;e<20;e++){
                stringstream nn;
                nn<<dE*(e+1)<<"_ChanPenTheta";
                PenTheta[e]=new TH2F(nn.str().c_str(),(nn.str()+";Pen Angle Theta #theta [Rad.];Hit Segment").c_str(),180,0,3.14159,8,0,8);
            }
		
            
	out.cd("");
        
    TGraph* HitMap[20];
	out.mkdir("HitMaps");
	out.cd("HitMaps");
        TH2F* HitMapTot=new TH2F("HitMapTot","HitMapAll;Z [mm];X [mm]",200,-40,40,100,-20,20);
        for(int e=0;e<20;e++)HitMap[e]=new TGraph();
	out.cd("");
    
	
	gROOT->cd();//cd back into main session memory 
    
    ///// Histograms not kept
    
    
    TH3F* AVEECBin=new TH3F("AVEECBin","AVEECBin;Etrue;Eaddback,chan",Ebin,EbinMin,EbinMax,Ebin,EbinMin,EbinMax,16,0,16);
    
    
    /////// Finish declaring histograms
    
    
	// The TTree newtree is the big list of energies and detector segments we need to load from
	// First we get the length of the list and output it
	long nentries = newtree->GetEntries();
	cout<<endl<<"nentries "<<nentries<<endl;

	// Next we create local variables into which we will read data from the list
	int EventID;
	int detNumber,cryNumber;
	double depEnergy,primaryTheta,primaryPhi,primaryE;
	double posx,posz;
	double BB34PenDX,BB34PenDY,BB34PenDZ;
	
        
	// Define the link between the "columns" of the list and the local variables we want to read them into
	// See HistoManager.cc for a full list
	newtree->SetBranchAddress("eventNumber",&EventID);
	newtree->SetBranchAddress("detNumber",&detNumber);
	newtree->SetBranchAddress("cryNumber",&cryNumber);
	newtree->SetBranchAddress("depEnergy",&depEnergy);
	newtree->SetBranchAddress("posx",&posx);
	newtree->SetBranchAddress("posz",&posz);
	newtree->SetBranchAddress("primaryTheta",&primaryTheta);
	newtree->SetBranchAddress("primaryPhi",&primaryPhi);
	newtree->SetBranchAddress("primaryE",&primaryE);
	newtree->SetBranchAddress("BB34PenDX",&BB34PenDX);
	newtree->SetBranchAddress("BB34PenDY",&BB34PenDY);
	newtree->SetBranchAddress("BB34PenDZ",&BB34PenDZ);

	// As each detector within one physical event is stored as a new "line/row" in our list we have to collect them first
	// Thes variabls to help with assembling the events
	bool EndOfEvent=true;
	long CurrentEvent=0;

	vector<double> EventHolder(16,0); // A matrix to store the energies recorded in an event
    
    bool VETO1=false;
    bool VETO2=false;
    double Eaddback=0;
    double Theta=0;
    double Phi=0;
    double Mult=0;
    double TMult=0;
    
    double pX,pZ;

    double ThetaPen=0;
	
	// A loop over every line of the input data in sequence
	for(long jentry=0;jentry<nentries;jentry++){
		
		// This line loads the data into the local variables we defined
		newtree->GetEntry(jentry);

// // 	Accumulate one event, detector data spread over several tree entries
		if(EventID==CurrentEvent){
			EndOfEvent=false;
            double e=depEnergy+r.Gaus(0,noisefactor);
//             double e=depEnergy;
			
            if(cryNumber==1&&e>100){
                VETO1=true;
            }
            if(cryNumber==2&&e>100){
                VETO2=true;
            }
            if(cryNumber==0&&e>0.1){
                if(detNumber>0&&detNumber<17){//0 is guard ring
                    Eaddback+=e;
                    detNumber--; // Shift to zero index
                    EventHolder[detNumber]=e;
                    E_RawSum->Fill(e);
                    E_RawSplit->Fill(e,primaryE);
                    E_RawChan->Fill(e,detNumber);
                    Mult++;
                    if(Mult==1){
                        pX=posx;
                        pZ=posz;
                    }
                }
            }
            Theta=primaryTheta;
			ThetaPen=TVector3(BB34PenDX,BB34PenDY,BB34PenDZ).Theta();
            Phi=std::abs(primaryPhi);// -pi to pi
            TMult++;
            
		}else{
			CurrentEvent=EventID;
			EndOfEvent=true;
			jentry--;
            
//             newtree->GetEntry(jentry);//should be needed as we already loaded everything we needed from last event
		}

		// The loop only continues beyond here if we have collected all rows of an entire event
		if(jentry==(nentries-1))EndOfEvent=true;
		if(!EndOfEvent) continue; 
		
		
        /////////////////////////
        ////  Now we have collected a complete event and can analyse the event
        ////////////////////////
		
        // Very basic addback rule
        double Esumtmp=0;
        double Seg=0;
        for(unsigned int i=0;i<EventHolder.size();i++){
            double e=EventHolder[i];
            if(e>0.1){
                Esumtmp+=e;
                Seg+=e*i;
            }else if(Esumtmp>0){ // So only segments neighbouring will be summed;
                break;
            }
        }
        if(abs(Esumtmp-Eaddback)>5)Eaddback=0;
        
		
		if(Eaddback>0.1) {
			Seg/=Eaddback; // Taking an energy weighted segment position (consider alternates) 
            unsigned int chan=std::round(Seg);

			E_AddbackSum->Fill(Eaddback);
			
			if(!(VETO1||VETO2)){ // No scintillator triggers
				
				E_AddbackVetoSum->Fill(Eaddback);
				PeakTotal->Fill(primaryE);
                
                E_AVSplit->Fill(Eaddback,primaryE);
                E_AVChan->Fill(Eaddback,Seg);
                
                AVEECBin->Fill(primaryE,Eaddback,Seg);
				
				if(abs(Eaddback-primaryE)<10){ // If energy is good, no scattering
				
					E_AVGood->Fill(Eaddback);
					Efficiency->Fill(primaryE); // Fill and divide by N later
					
					E_Chan_GoodE->Fill(primaryE,Seg);
					E_Chan_GoodEThetaLim->Fill(primaryE,Seg); // Fill all bins and zero later when cut is applied
					E_Chan_GoodEThetaLimHalf->Fill(primaryE,Seg); // Fill all bins and zero later when cut is applied
					
					E_Theta->Fill(Theta,primaryE);
				
					AngleESeg->Fill(Theta,primaryE,Seg);
					
					if(chan<16)GridAngle[chan]->Fill(Theta,primaryE);
					
					int X=chan%4;
					int Y=chan/4;
					AngleESegGrid->Fill(TMath::Pi()*Y+Theta,X*1500+primaryE);
					
                    HitMapTot->Fill(pZ,pX);
                    
                    
					PenThetaTot->Fill(ThetaPen,chan);
					unsigned int TPchan=chan;
					double TPtheta=Theta;
					double TPthetapen=ThetaPen;
					if(TPchan>7){ //To double up the statistics for symetric configuration by flip across centre, irrespecive of symmetrize
						TPchan=15-TPchan;
					}else{
						TPtheta=TMath::Pi()-Theta;
						TPthetapen=TMath::Pi()-ThetaPen;
                    }
                    
					if(TPchan<8){
						EThetaPhi[TPchan]->Fill(TPtheta,Phi,primaryE);
                    }
                    
                    
					
                    unsigned int ei=round(primaryE/dE)-1; //Determine energy bin, dE is the steps in sim
                    if(ei<20){ // only histograms for first 20 E steps 
                        
                        AngleSeg[ei]->Fill(Theta,Seg);
                        
						PenTheta[ei]->Fill(TPthetapen,TPchan);
						
                        ChanThetaPhi[ei]->Fill(TPtheta,Phi,TPchan);
                        ThetaPhi[ei]->Fill(TPtheta,Phi);
                            
                        HitMap[ei]->SetPoint(HitMap[ei]->GetN(),pZ,pX);
                    }
				}
			}
		}    
		
        /////  Reset things ready for next event
// 		for(auto &i : EventHolder){
// 			std::fill(i.begin(), i.end(), 0);
// 		}
		
        std::fill(EventHolder.begin(), EventHolder.end(), 0);
		
		VETO1=false;
        VETO2=false;
        Eaddback=0;
        Theta=0;
        Mult=0;
        TMult=0;
    
	} // End of event loop


    
/////////////////////
//////////////////////////////////////////
//////////////////////////////////////////
//    End of sorting, process cumulative properties i.e angular distributions, efficiencies 
//////////////////////////////////////////
//////////////////////////////////////////
/////////////////////

//////////////////////////////////////////
//    If symetric setup mirror to double statistics of angular determination
//////////////////////////////////////////

	int SimmNC=SimmN;
	
	if(symmetrize){
        
		SimmNC*=2;
        
		for(int g=0;g<8;g++){
			TH2* h1=GridAngle[g];
			TH2* h2=GridAngle[15-g];
			for(int y=1;y<=h1->GetNbinsY();y++){
				int nX=h1->GetNbinsX();
				for(int x=1;x<=nX;x++){
					int x2=nX+1-x;
					double symsum=h1->GetBinContent(x,y)+h2->GetBinContent(x2,y);
					h1->SetBinContent(x,y,symsum);
					h2->SetBinContent(x2,y,symsum);
				}
			}
		}
		
		for(int g=0;g<20;g++){
			TH2* h1=AngleSeg[g];
            int nY=h1->GetNbinsY();
            for(int y=1;y<=nY/2;y++){
                int y2=nY+1-y;
				int nX=h1->GetNbinsX();
				for(int x=1;x<=nX;x++){
					int x2=nX+1-x;
					double symsum=h1->GetBinContent(x,y)+h1->GetBinContent(x2,y2);
					h1->SetBinContent(x,y,symsum);
					h1->SetBinContent(x2,y2,symsum);
				}
			}
		}
		
		
		TH2* Fix[3]={E_Chan_GoodE,E_Chan_GoodEThetaLim,E_Chan_GoodEThetaLimHalf};
		
		for(int g=0;g<3;g++){
			TH2* h1=Fix[g];
			for(int x=1;x<=h1->GetNbinsX();x++){
				int nY=h1->GetNbinsY();
				for(int y=1;y<=nY/2;y++){
					int y2=nY+1-y;
					double symsum=h1->GetBinContent(x,y)+h1->GetBinContent(x,y2);
					h1->SetBinContent(x,y,symsum);
					h1->SetBinContent(x,y2,symsum);
				}
			}
		}
		
		
	}
	
//////////////////////////////////////////
//    Determine the theta distribution for each channel, for each energy
//////////////////////////////////////////

  	out.mkdir("ThetaEval");
	out.cd("ThetaEval");
    
	TGraph AngleExpt[16][4];

    for(int g=0;g<16;g++){ // Channels
        TH2* h2=GridAngle[g];
        
        TAxis *ya=h2->GetYaxis();
        for(int b=1;b<=ya->GetNbins();b++){ // Energy bins
            double Ein=ya->GetBinCenter(b);
            TH1* ht=h2->ProjectionX("_px",b,b);
            
            // Determine theta distribution and centroid by 3 methods
            
            double widthsig=(ht->GetStdDev())*180.0/TMath::Pi();
            double thetamean=ht->GetMean();
            double thetamode=ht->GetXaxis()->GetBinCenter(ht->GetMaximumBin());
            if(widthsig<0)widthsig=0; //???
            
            // Record the values
        
            ThetaSigESeg->SetBinContent(ThetaSigESeg->FindBin(Ein,g),widthsig);
            
            // If the channel/E has a theta distribution that is acceptable record the value
            // There is a requirement for at least 10 counts so the numbers have any meaning, but this is sepate to the LowChanCut
            
            if(widthsig>THETALIMDEG||ht->Integral()<10){
                E_Chan_GoodEThetaLim->SetBinContent(E_Chan_GoodEThetaLim->FindBin(Ein,g),0);
//                 AngleExpt[g][0].SetPoint(b-1,Ein,-1);
//                 AngleExpt[g][1].SetPoint(b-1,Ein,-1);
            }else{
                AngleExpt[g][0].SetPoint(AngleExpt[g][1].GetN(),Ein,thetamean);
                AngleExpt[g][1].SetPoint(AngleExpt[g][1].GetN(),Ein,thetamode);
            }
            
            if(widthsig>THETALIMDEG*0.5||ht->Integral()<10){
                E_Chan_GoodEThetaLimHalf->SetBinContent(E_Chan_GoodEThetaLimHalf->FindBin(Ein,g),0);
//                 AngleExpt[g][2].SetPoint(b-1,Ein,-1);
//                 AngleExpt[g][3].SetPoint(b-1,Ein,-1);
            }else{
                AngleExpt[g][2].SetPoint(AngleExpt[g][3].GetN(),Ein,thetamean);
                AngleExpt[g][3].SetPoint(AngleExpt[g][3].GetN(),Ein,thetamode);
            }
            
            delete ht;
        }
        
        
		for(int m=0;m<4;m++){
            
            TGraph *adj=&AngleExpt[g][m];
            
            if(adj->GetN()==1){
                // If only one point, dublicate so extrapolation works as intended 
                double x,y;
                adj->GetPoint(0,x,y);
                adj->SetPoint(1,x+1,y);
            }            
            
            if(adj->GetN()){ // Only write the ones with data
                stringstream ss;
                ss<<"grid"<<g<<"_"<<m;
                adj->Write(ss.str().c_str(),TObject::kOverwrite);
            }
		}
        
    }
    
	gROOT->cd();
    
   
    
//////////////////////////////////////////
//    Convert counts in Efficiency hist to % efficiency and also do PeakTotal
//////////////////////////////////////////


    for(int b=1;b<=Efficiency->GetNbinsX();b++){
        double nn=Efficiency->GetBinContent(b);
        double NN=PeakTotal->GetBinContent(b);
        Efficiency->SetBinContent(b,100*nn/SimmN);
        if(NN>0){
            PeakTotal->SetBinContent(b,nn/NN);
        }
    }
    
//////////////////////////////////////////
//    Apply the minimum % cut to energy chan hists
//////////////////////////////////////////
    
    // Apply cuts to the "GoodE" histograms
    
	out.cd("LowChanCut");
        TH2F* E_Chan_GoodECut=(TH2F*)E_Chan_GoodE->Clone("E_Chan_GoodECut");
        TH2F* E_Chan_GoodEThetaLimCut=(TH2F*)E_Chan_GoodEThetaLim->Clone("E_Chan_GoodEThetaLimCut");
        TH2F* E_Chan_GoodEThetaLimHalfCut=(TH2F*)E_Chan_GoodEThetaLimHalf->Clone("E_Chan_GoodEThetaLimHalfCut");
	out.cd("");
    
	for(int x=1;x<=E_Chan_GoodE->GetNbinsX();x++){
	for(int y=1;y<=E_Chan_GoodE->GetNbinsY();y++){
		double z=E_Chan_GoodECut->GetBinContent(x,y);
		if(z<SimmNC*grid_eff_cut){
            E_Chan_GoodECut->SetBinContent(x,y,0);
            E_Chan_GoodEThetaLimCut->SetBinContent(x,y,0);
            E_Chan_GoodEThetaLimHalfCut->SetBinContent(x,y,0);
        }
	}}
	
	// Process a spectrum using E/chan cuts
	
    TH2* EChanTmp=(TH2*)E_AVChan->Clone("EChanTmp");

    TAxis* axA=E_Chan_GoodECut->GetXaxis();
    TAxis* axB=EChanTmp->GetXaxis();
    for(int x=1;x<=EChanTmp->GetNbinsX();x++){
	for(int y=1;y<=EChanTmp->GetNbinsY();y++){
        if(!(E_Chan_GoodECut->GetBinContent(axA->FindBin(axB->GetBinCenter(x)),y)>0)){
            EChanTmp->SetBinContent(x,y,0);
        }
	}}
	
	out.cd("LowChanCut");
        TH1* E_Cut=EChanTmp->ProjectionX("E_Cut");
    gROOT->cd();

	delete EChanTmp;
    
	// Process a peak to total using E/chan cuts
    
//   AVEECBin->Fill(primaryE,Eaddback,Seg);
    
	for(int y=1;y<=AVEECBin->GetNbinsY();y++){
	for(int z=1;z<=AVEECBin->GetNbinsZ();z++){
        if(!(E_Chan_GoodECut->GetBinContent(y,z)>0)){
            for(int x=1;x<=AVEECBin->GetNbinsX();x++){
                AVEECBin->SetBinContent(x,y,z,0);
            }
        }
    }}
    
//////////////////////////////////////////
//    Extract efficiencies from the % cut energy chan hists
//////////////////////////////////////////

    TH1* h0=AVEECBin->Project3D("x");
    TH1* h1=E_Chan_GoodECut->ProjectionX("A");
    TH1* h2=E_Chan_GoodEThetaLimCut->ProjectionX("B");
    TH1* h3=E_Chan_GoodEThetaLimHalfCut->ProjectionX("C");
    
    TH1* h4=E_Chan_GoodEThetaLim->ProjectionX("D");
    TH1* h5=E_Chan_GoodEThetaLimHalf->ProjectionX("E");
    
    for(int b=1;b<=Efficiency->GetNbinsX();b++){
    
        EfficiencyCut->SetBinContent(b,100*h1->GetBinContent(b)/SimmNC);
        if(h0->GetBinContent(b)>0)PeakTotalCut->SetBinContent(b,h1->GetBinContent(b)*0.5/h0->GetBinContent(b));
        
        EfficiencyTCut->SetBinContent(b,100*h2->GetBinContent(b)/SimmNC);
        EfficiencyTHalfCut->SetBinContent(b,100*h3->GetBinContent(b)/SimmNC);
        EfficiencyT->SetBinContent(b,100*h4->GetBinContent(b)/SimmNC);
        EfficiencyTHalf->SetBinContent(b,100*h5->GetBinContent(b)/SimmNC);
    }

    delete h1;
    delete h2;
    delete h3;
    delete h4;
    delete h5;
    
//////////////////////////////////////////
//    Pointless coloured hitmap with nice canvas
//////////////////////////////////////////
    
    // Create a TMultiGraph to hold TGraphs 
    TMultiGraph* multiGraph = new TMultiGraph();
    
    
    // Only store 100 points or graph unreadable
    // Incredibly inefficient, but because of the theta division of the geant part this was the best solution at the time
    for(int e=0;e<20;e++){
        int n=HitMap[e]->GetN()/100;
        if(n>1){
            TGraph *g=new TGraph;
            for(int i=0;i<HitMap[e]->GetN();i++){
                if(!(i%n)){
                    double x,y;
                    HitMap[e]->GetPoint(i,x,y);
                    g->SetPoint(g->GetN(),x,y);
                }
            }
            delete HitMap[e];
            HitMap[e]=g;
        }
        
        while(HitMap[e]->GetN()>100){
            HitMap[e]->RemovePoint((int)(r.Uniform()*HitMap[e]->GetN()));
        }
    }
    
	out.cd("HitMaps");
    
        for(int e=0;e<20;e++){
            double Ein=(e+1)*100;
            stringstream ss;ss<<"AngleSeg_"<< setw(4) << setfill('0')<<(e+1)*100<<"keV";
//         gStyle->SetPalette(kRust);
            double normalizedValue = (Ein - 100) / (1500 - 100);
            int color = gStyle->GetColorPalette(int(normalizedValue * 255));
            HitMap[e]->SetMarkerColor(color);
    
            HitMap[e]->SetMarkerStyle(8);
    //          HitMap[e]->SetMarkerSize(0.25);
    //          HitMap[e]->SetMarkerColor((int)(Ein/100));
            stringstream hh;hh<<"HitMap_"<< setw(4) << setfill('0')<<Ein<<"keV";
            if(HitMap[e]->GetN()) {
                HitMap[e]->Write(ss.str().c_str());
                multiGraph->Add(HitMap[e]);
            }
        }
	gROOT->cd();
	
    // Create a TCanvas
    TCanvas* canvas = new TCanvas("HitMapCanvas");
    multiGraph->Draw("AP");
    canvas->Update();

    // Create a box
    TBox* box = new TBox(-40, -20, 40, 20);
   box->SetFillStyle(3000);
    box->SetLineColor(1); // Set border width
    box->SetLineWidth(1); // Set border width
    box->Draw("l");

    for (double x = -35; x <= 35; x += 5) {
        TLine* verticalLine = new TLine(x, -20, x, 20);
        verticalLine->SetLineWidth(1);
        verticalLine->SetLineColor(kRed); // Set line color to red (you can choose any color)
        verticalLine->Draw();
    }

    canvas->Update();
	out.cd("");
        canvas->Write("HitMapCanvas",TObject::kOverwrite);
	out.cd("HitMaps");
        multiGraph->Write("HitMap",TObject::kOverwrite);
	gROOT->cd();
    
//////////////////////////////////////////
//    Calculate how many grids have a substantial fraction of each energies hits
//////////////////////////////////////////
	
	TAxis *ya=E_Chan_GoodE->GetXaxis();
	for(int b=1;b<=ya->GetNbins();b++){
		TH1* h=E_Chan_GoodE->ProjectionY("_py",b,b);
		int N=h->Integral();
		int NA=0,NB=0;
		for(int g=0;g<16;g++){
			double n=h->GetBinContent(g);
			if(n>N*0.1)NA++;
			if(n>N*0.05)NB++;
		}
		GridSpreadA->SetBinContent(b,NA);
		GridSpreadB->SetBinContent(b,NB);
	}
	
//////////////////////////////////////////
//    Dont save empty histograms. Delete them before writing the TFile
//////////////////////////////////////////

    for (int i = 0; i < 20; ++i) {
        if (ChanThetaPhi[i]->GetEntries() == 0) {
            delete ChanThetaPhi[i];
            ChanThetaPhi[i] = nullptr;  // Set the pointer to null
        }
        if (ThetaPhi[i]->GetEntries() == 0) {
            delete ThetaPhi[i];
            ThetaPhi[i] = nullptr;  // Set the pointer to null
        }
        if (AngleSeg[i]->GetEntries() == 0) {
            delete AngleSeg[i];
            AngleSeg[i] = nullptr;  // Set the pointer to null
        }
    }
    
    for (int i = 0; i < 8; ++i) {
        if (EThetaPhi[i]->GetEntries() == 0) {
            delete EThetaPhi[i];
            EThetaPhi[i] = nullptr;  // Set the pointer to null
        }
    }

/////////////////////
    

	// Save and close the output file
    out.Write();
    out.Close();	
    delete newfile; // Delete the variable holding the input file, which also closes it
    
}

