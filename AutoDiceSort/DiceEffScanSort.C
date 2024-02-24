void DiceEffScanSort(double Ein=400,double SimmN=1000000,string rootout = "", const char *rootin = "g4out.root") {
	
    double THETALIMDEG=10;//The limit on allowed theta std in a segment
    
    if(rootout.find(".root")>rootout.size()){
        stringstream ss;
        ss<<rootout<<"_flatorange"<< setw(4) << setfill('0')<<Ein<<".root";
        rootout=ss.str();
    }
    
	//// Random number generator to add electronic noise on to "perfect" geant4 detectors ///
	TRandom r;
	r.SetSeed();
	double noisefactor=2;
	// These factors should be tuned to data when possible 

	
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
    TH2F* E_RawSplit=new TH2F("E_RawSplit","RawHitsVsEmissionEnergy;Hit Energy (keV);Emission Energy (keV);Counts",2000,0,2000,20,50,2050);
    TH2F* E_RawSumChan=new TH2F("E_RawSumChan","RawHitEnergyChan;Electron Energy (keV);Hit Segment;Counts",2000,0,2000,16,0,16);
    
    TH1F* E_AddbackSum=new TH1F("E_AddbackSum","AddbackEnergy;Electron Energy (keV);Counts",2000,0,2000);
    TH1F* E_AddbackVetoSum=new TH1F("E_AddbackVetoSum","AddbackVetoEnergy;Electron Energy (keV);Counts",2000,0,2000);
    
    TH1F* E_GoodSum=new TH1F("E_GoodSum","EmissionEqualAddbackSelected;Electron Energy (keV);Counts",2000,0,2000);
    
    TH2F* E_GoodChan=new TH2F("E_GoodChan","FullEnergyVsSeg;Electron Energy (keV);Hit Segment;Counts",20,50,2050,16,0,16);
    
    TH2F* E_GoodChanThetaLim=new TH2F("E_GoodChanThetaLim","FullEnergyVsSegThetaLimited;Electron Energy (keV);Hit Segment;Counts",20,50,2050,16,0,16);
    TH2F* E_GoodChanThetaLimHalf=new TH2F("E_GoodChanThetaLimHalf","FullEnergyVsSegThetaLimitedHalf;Electron Energy (keV);Hit Segment;Counts",20,50,2050,16,0,16);
    
    TH1F* Efficiency=new TH1F("Efficiency","Efficiency;Emission Energy (keV);Full Energy Detection Efficiency (%)",20,50,2050);
    TH1F* EfficiencyCut=new TH1F("EfficiencyCut","EfficiencyThetaCut;Emission Energy (keV);Full Energy Detection Efficiency (%)",20,50,2050);
    TH1F* EfficiencyCutHalf=new TH1F("EfficiencyCutHalf","EfficiencyThetaCutHalf;Emission Energy (keV);Full Energy Detection Efficiency (%)",20,50,2050);
    TH1F* PeakTotal=new TH1F("PeakTotal","PeakToTotal;Emission Energy (keV);Detection PeakToTotal",20,50,2050);
    
	out.mkdir("Angles");
	out.cd("Angles");
    TH3F* AngleESeg=new TH3F("AngleESeg","EmissionAngleEnergyHitSegment;Emission Angle Theta #theta [Rad.];Emission Energy (keV);Hit Segment",360,0,3.14159,20,50,2050,16,0,16);
    TH2F* AngleESegGrid=new TH2F("AngleESegGrid","AngleESegGrid;Angle #theta [Rad.];Emission Energy (keV)",180*4,0,TMath::Pi()*4,15*4,50,6050);
    
        TH2F* SigmaEseg=new TH2F("SigmaEseg","ThetaSigmaESeg;Emission Energy (keV);Segment;S.t.d. Theta (deg.)",20,50,2050,16,0,16);
    
    
        TH2F* E_Theta=new TH2F("E_Theta","EmissionEnergyVsAngle;Emission Angle Theta #theta [Rad.];Emission Energy (keV)",180,0,3.14159,20,50,2050);
    
        TH2F *GridAngle[16];
        for(int i=0;i<16;i++){
            stringstream ss;ss<<"SegmentAngle"<< setw(2) << setfill('0')<<i;
            GridAngle[i] = new TH2F(ss.str().c_str(),(ss.str()+";Emission Angle Theta #theta [Rad.];Electron Energy (keV)").c_str(),360,0,3.14159,20,50,2050);
        }
        
        stringstream ss;ss<<"AngleSeg_"<< setw(4) << setfill('0')<<Ein<<"keV";
        TH2F* AngleSeg = new TH2F(ss.str().c_str(),(ss.str()+";Emission Angle Theta #theta [Rad.];Hit Segment").c_str(),360,0,3.14159,16,0,16);
	out.cd("");
        
    TGraph* HitMap=new TGraph();
    
	gROOT->cd();//cd back into main session memory 
	
	// The TTree newtree is the big list of energies and detector segments we need to load from
	// First we get the length of the list and output it
	long nentries = newtree->GetEntries();
	cout<<endl<<"nentries "<<nentries<<endl;

	// Next we create local variables into which we will read data from the list
	int EventID;
	int detNumber,cryNumber;
	double depEnergy,primaryTheta;
	double posx,posz;
	
	// Define the link between the "columns" of the list and the local variables we want to read them into
	// See HistoManager.cc for a full list
	newtree->SetBranchAddress("eventNumber",&EventID);
	newtree->SetBranchAddress("detNumber",&detNumber);
	newtree->SetBranchAddress("cryNumber",&cryNumber);
	newtree->SetBranchAddress("depEnergy",&depEnergy);
	newtree->SetBranchAddress("primaryTheta",&primaryTheta);
	newtree->SetBranchAddress("posx",&posx);
	newtree->SetBranchAddress("posz",&posz);

	// As each detector within one physical event is stored as a new "line/row" in our list we have to collect them first
	// Thes variabls to help with assembling the events
	bool EndOfEvent=true;
	long CurrentEvent=0;

	vector<double> EventHolder(16,0); // A matrix to store the energies recorded in an event
    
    bool VETO1=false;
    bool VETO2=false;
    double Eaddback=0;
    double Theta=0;
    double Mult=0;
    double TMult=0;
    
    double pX,pZ;

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
            if(cryNumber==0&&e>30){
                if(detNumber>0&&detNumber<17){//0 is guard ring
                    Eaddback+=e;
                    detNumber--; // Shift to zero index
                    EventHolder[detNumber]=e;
                    E_RawSum->Fill(e);
                    E_RawSplit->Fill(e,Ein);
                    E_RawSumChan->Fill(e,detNumber);
                    Mult++;
                    if(Mult==1){
                        pX=posx;
                        pZ=posz;
                    }
                }
            }
            Theta=primaryTheta;
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
		
        double Esumtmp=0;
        double Seg=0;
        for(unsigned int i=0;i<EventHolder.size();i++){
            double e=EventHolder[i];
            if(e>30){
                Esumtmp+=e;
                Seg+=e*i;
            }else if(Esumtmp>0){ // So only segments neigbouring will be summed;
                break;
            }
        }
        if(abs(Esumtmp-Eaddback)>5)Eaddback=0;
        
		if(Eaddback>30) {
        Seg/=Eaddback;
        E_AddbackSum->Fill(Eaddback);
        if(!(VETO1||VETO2)){
        E_AddbackVetoSum->Fill(Eaddback);
        if(abs(Eaddback-Ein)<10){
        
            E_GoodSum->Fill(Eaddback);
            
            E_GoodChan->Fill(Ein,Seg);
            E_GoodChanThetaLim->Fill(Ein,Seg);
            E_GoodChanThetaLimHalf->Fill(Ein,Seg);
            
            E_Theta->Fill(Theta,Ein);
        
            AngleESeg->Fill(Theta,Ein,Seg);
            
            if(abs(round(Seg))<16)GridAngle[(int)abs(round(Seg))]->Fill(Theta,Ein);
            
            AngleSeg->Fill(Theta,Seg);
            
            int X=(int)round(Seg)%4;
            int Y=Seg/4;
            AngleESegGrid->Fill(TMath::Pi()*Y+Theta,X*1500+Ein);
        
            if(HitMap->GetN()<100){
                HitMap->SetPoint(HitMap->GetN(),pZ,pX);
            }
            
        }}}    
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
    
	}
	
	
	Efficiency->Fill(Ein,100*E_GoodSum->Integral()/SimmN);
    PeakTotal->Fill(Ein,E_GoodSum->Integral()/E_AddbackVetoSum->Integral());
	
	TGraph AngleExpt[16][4];
    
    for(int b=1;b<17;b++){
		TH1* ht=AngleSeg->ProjectionX("_px",b,b);
        double widthsig=(ht->GetStdDev())*180.0/TMath::Pi();
        double thetamean=ht->GetMean();
        double thetamode=ht->GetXaxis()->GetBinCenter(ht->GetMaximumBin());
        if(widthsig<0)widthsig=0;
        SigmaEseg->SetBinContent(SigmaEseg->FindBin(Ein,b-1),widthsig);
        if(widthsig>THETALIMDEG||ht->Integral()<10){
			E_GoodChanThetaLim->SetBinContent(E_GoodChanThetaLim->FindBin(Ein,b-1),0);
			AngleExpt[b-1][0].SetPoint(0,Ein,-1);
			AngleExpt[b-1][1].SetPoint(0,Ein,-1);
		}else{
			AngleExpt[b-1][0].SetPoint(0,Ein,thetamean);
			AngleExpt[b-1][1].SetPoint(0,Ein,thetamode);
		}
        if(widthsig>THETALIMDEG*0.5||ht->Integral()<10){
			E_GoodChanThetaLimHalf->SetBinContent(E_GoodChanThetaLimHalf->FindBin(Ein,b-1),0);
			AngleExpt[b-1][2].SetPoint(0,Ein,-1);
			AngleExpt[b-1][3].SetPoint(0,Ein,-1);
		}else{
			AngleExpt[b-1][2].SetPoint(0,Ein,thetamean);
			AngleExpt[b-1][3].SetPoint(0,Ein,thetamode);
		}
    }
    
    // Simple Integral because the E_GoodChanThetaLim histogram is actually mostly empty, relying on hadd afterwards to fill
    EfficiencyCut->Fill(Ein,100*E_GoodChanThetaLim->Integral()/SimmN);
    EfficiencyCutHalf->Fill(Ein,100*E_GoodChanThetaLimHalf->Integral()/SimmN);
    
    
	// Save and close the output file
    
	out.mkdir("HitMaps");
	out.cd("HitMaps");
//         gStyle->SetPalette(kRust);
        double normalizedValue = (Ein - 100) / (1500 - 100);
        int color = gStyle->GetColorPalette(int(normalizedValue * 255));
        HitMap->SetMarkerColor(color);
    
        HitMap->SetMarkerStyle(8);
//         HitMap->SetMarkerSize(0.25);
//         HitMap->SetMarkerColor((int)(Ein/100));
        stringstream hh;hh<<"HitMap_"<< setw(4) << setfill('0')<<Ein<<"keV";
        HitMap->Write(ss.str().c_str());
	out.cd("");
	
	
	out.mkdir("AngleExpt");
	out.cd("AngleExpt");
    
    for(int b=0;b<16;b++){
		for(int m=0;m<4;m++){
			stringstream ss;
			ss<<"grid"<<b<<"_"<<m;
			AngleExpt[b][m].Write(ss.str().c_str());
		}
	}
	
	out.cd("");
    
	
	for(int x=1;x<E_GoodChan->GetNbinsX();x++){
	for(int y=1;y<E_GoodChan->GetNbinsY();y++){
		double z=E_GoodChan->GetBinContent(x,y);
		if(z<SimmN*0.001)E_GoodChan->SetBinContent(x,y,0);
		
	}}
	
	
    out.Write();
    out.Close();	
    delete newfile; // Delete the variable holding the input file, which also closes it
    
}

