void FlatOrangeScan(double Ein=400,double SimmN=1000000,const char *rootout = "flatorange.root", const char *rootin = "g4out.root") {
	
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
	TFile out(rootout,"RECREATE");
	out.cd();//cd into output file so histograms created in memory are associated with that file at creation rather than being manually saved there later.

    TH1F* E_RawSum=new TH1F("E_RawSum","E_RawSum",2000,0,2000);
    TH2F* E_RawSplit=new TH2F("E_RawSplit","E_RawSplit",2000,0,2000,20,50,2050);
    TH2F* E_RawSumChan=new TH2F("E_RawSumChan","E_RawSumChan",2000,0,2000,16,0,16);
    
    TH1F* E_AddbackSum=new TH1F("E_AddbackSum","E_AddbackSum",2000,0,2000);
    TH1F* E_AddbackVetoSum=new TH1F("E_AddbackVetoSum","E_AddbackVetoSum",2000,0,2000);
    
    TH1F* E_GoodSum=new TH1F("E_GoodSum","E_GoodSum",2000,0,2000);
    
    TH2F* E_Theta=new TH2F("E_Theta","E_Theta",180,0,3.14159,20,50,2050);
    
    TH1F* Efficiency=new TH1F("Efficiency","Efficiency",20,50,2050);
    TH1F* PeakTotal=new TH1F("PeakTotal","PeakTotal",20,50,2050);
    
    TH3F* AngleESeg=new TH3F("AngleESeg","AngleESeg",360,0,3.14159,20,50,2050,16,0,16);
    
    TH2F* AngleESegGrid=new TH2F("AngleESegGrid","AngleESegGrid",180*4,0,TMath::Pi()*4,15*4,50,6050);
    
	gROOT->cd();//cd back into main session memory 
	
	// The TTree newtree is the big list of energies and detector segments we need to load from
	// First we get the length of the list and output it
	long nentries = newtree->GetEntries();
	cout<<endl<<"nentries "<<nentries<<endl;

	// Next we create local variables into which we will read data from the list
	int EventID;
	int detNumber,cryNumber;
	double depEnergy,primaryTheta;
	
	// Define the link between the "columns" of the list and the local variables we want to read them into
	// See HistoManager.cc for a full list
	newtree->SetBranchAddress("eventNumber",&EventID);
	newtree->SetBranchAddress("detNumber",&detNumber);
	newtree->SetBranchAddress("cryNumber",&cryNumber);
	newtree->SetBranchAddress("depEnergy",&depEnergy);
	newtree->SetBranchAddress("primaryTheta",&primaryTheta);

	// As each detector within one physical event is stored as a new "line/row" in our list we have to collect them first
	// Thes variabls to help with assembling the events
	bool EndOfEvent=true;
	long CurrentEvent=0;

	vector<double> EventHolder(16,0); // A matrix to store the energies recorded in an event
    
    bool VETO1=false;
    bool VETO2=false;
    double Eaddback=0;
    double Mult=0;

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
                if(detNumber>0&&detNumber<17){//0 is gaurd ring
                    Eaddback+=e;
                    detNumber--; //Shift to zero index
                    EventHolder[detNumber]=e;
                    E_RawSum->Fill(e);
                    E_RawSplit->Fill(e,Ein);
                    E_RawSumChan->Fill(e,detNumber);
                }
                Mult++;
            }
            
		}else{
			CurrentEvent=EventID;
			EndOfEvent=true;
			jentry--;
		}

		// The loop only continues beyond here if we have collected all rows of an entire event
		if(jentry==(nentries-1))EndOfEvent=true;
		if(!EndOfEvent) continue; 
		
        /////////////////////////
        ////  Now we have collected a complete event and can analyse the event
        ////////////////////////
		
		if(Eaddback>30) {
        E_AddbackSum->Fill(Eaddback);
        if(!(VETO1||VETO2)){
        E_AddbackVetoSum->Fill(Eaddback);
        if(abs(Eaddback-Ein)<10){
        
            E_GoodSum->Fill(Eaddback);
            
            double Seg=0;
            for(unsigned int i=0;i<EventHolder.size();i++){
                double e=EventHolder[i];
                if(e>30){
                    Seg+=e*i;
                }
            }
            Seg/=Eaddback;
            
            E_Theta->Fill(primaryTheta,Ein);
        
            AngleESeg->Fill(primaryTheta,Ein,Seg);
            
            int X=(int)round(Seg)%4;
            int Y=Seg/4;
            
            AngleESegGrid->Fill(TMath::Pi()*Y+primaryTheta,X*1500+Ein);
        
        }}}    
        /////  Reset things ready for next event
// 		for(auto &i : EventHolder){
// 			std::fill(i.begin(), i.end(), 0);
// 		}
		
        std::fill(EventHolder.begin(), EventHolder.end(), 0);
		
		VETO1=false;
        VETO2=false;
        Eaddback=0;
        Mult=0;
    
	}
	
	Efficiency->Fill(Ein,100*E_GoodSum->Integral()/SimmN);
    PeakTotal->Fill(Ein,E_GoodSum->Integral()/E_AddbackVetoSum->Integral());
	
	// Save and close the output file
    out.Write();
    out.Close();	
    delete newfile; // Delete the variable holding the input file, which also closes it
    
}
