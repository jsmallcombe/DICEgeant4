void QuickPairsSort(const char * outfileName = "QuickSortOut.root", const char *ntuplefileName = "g4out.root") {
	    
	//// Random number generator to add electronic noise on to "perfect" geant4 detectors ///
	TRandom r;
	r.SetSeed();
	double noisefactor=2;
	// These factors should be tuned to data when possible 


    // Load the input file
    TFile *ntuplefile = new TFile(ntuplefileName,"READ");
	gROOT->cd();
    if(!ntuplefile->IsOpen())return;
    TTree *newtree = (TTree*)ntuplefile->Get("ntuple");
    if(newtree==nullptr)return;

	
	TFile out(outfileName,"UPDATE");

	TH1F* E_RawSum=new TH1F("Raw","RawHitEnergy;Electron Energy (keV);Counts",2000,0,2000);
	TH1F* E_AddbackSum=new TH1F("AddbackSum","AddbackSum;Electron Energy (keV);Counts",2000,0,2000);
	TH1F* E_AddbackVetoSum=new TH1F("AddbackVetoSum","AddbackVetoSum;Electron Energy (keV);Counts",2000,0,2000);
	TH1F* E_AddVetoSumMultTwo=new TH1F("E_AddVetoSumMultTwo","E_AddVetoSumMultTwo;Electron Energy (keV);Counts",2000,0,2000);
	TH1F* E_AVSMTP=new TH1F("E_AVSMTPair","E_AVSMTPair;Electron Energy (keV);Counts",2000,0,2000);

	gROOT->cd();
	
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
// 	newtree->SetBranchAddress("primaryTheta",&primaryTheta);
// 	newtree->SetBranchAddress("posx",&posx);
// 	newtree->SetBranchAddress("posz",&posz);

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
                    Mult++;
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

        int Gmin=16;
        int Gmax=0;
        for(unsigned int i=0;i<EventHolder.size();i++){
            if(EventHolder[i]>30){
				if(i>Gmax){Gmax=i;}
				if(i<Gmin){Gmin=i;}
            }
        }
		
		if(Eaddback>30) {

			E_AddbackSum->Fill(Eaddback);
			if(!(VETO1||VETO2)){
				E_AddbackVetoSum->Fill(Eaddback);
				if(Mult>1){
					E_AddVetoSumMultTwo->Fill(Eaddback);
					if(Gmax-Gmin>1){
						E_AVSMTP->Fill(Eaddback);
					}
				}
			}    
		}
			
		std::fill(EventHolder.begin(), EventHolder.end(), 0);
		
		VETO1=false;
		VETO2=false;
		Eaddback=0;
		Theta=0;
		Mult=0;
		TMult=0;
	
	}
	
    out.Write();
    out.Close();	
    delete ntuplefile; // Delete the variable holding the input file, which also closes it
    
}

