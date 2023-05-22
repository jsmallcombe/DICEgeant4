void SimpRecShadSort(const char *rootout = "diceshad.root", const char *rootin = "g4out.root") {
	
	//// Random number generator to add electronic noise on to "perfect" geant4 detectors ///
	TRandom r;
	r.SetSeed();
	double noisefactor=1;
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

    TH1F* Esumraw=new TH1F("Esumraw","Esumraw",2000,0,2000);
    TH1F* Esumvt1=new TH1F("Esumvt1","Esumvt1",2000,0,2000);
    TH1F* Esumvt2=new TH1F("Esumvt2","Esumvt2",2000,0,2000);
    TH1F* Esumvt3=new TH1F("Esumvt3","Esumvt3",2000,0,2000);
    
    
    TH2F* Eraw=new TH2F("Eraw","Eraw",2000,0,2000,16,0,16);
    TH2F* Ev1=new TH2F("Ev1","Ev1",2000,0,2000,16,0,16);
    TH2F* Ev2=new TH2F("Ev2","Ev2",2000,0,2000,16,0,16);
    TH2F* Ev3=new TH2F("Ev3","Ev3",2000,0,2000,16,0,16);


	gROOT->cd();//cd back into main session memory 
	
	
	// The TTree newtree is the big list of energies and detector segments we need to load from
	// First we get the length of the list and output it
	long nentries = newtree->GetEntries();
	cout<<endl<<"nentries "<<nentries<<endl;

	// Next we create local variables into which we will read data from the list
	int EventID;
	int detNumber,cryNumber;
	double depEnergy;
	
	// Define the link between the "columns" of the list and the local variables we want to read them into
	// See HistoManager.cc for a full list
	newtree->SetBranchAddress("eventNumber",&EventID);
	newtree->SetBranchAddress("detNumber",&detNumber);
	newtree->SetBranchAddress("cryNumber",&cryNumber);
	newtree->SetBranchAddress("depEnergy",&depEnergy);

	// As each detector within one physical event is stored as a new "line/row" in our list we have to collect them first
	// Thes variabls to help with assembling the events
	bool EndOfEvent=true;
	long CurrentEvent=0;

	vector<double> EventHolder(16,0); // A matrix to store the energies recorded in an event
    bool VETO1=false;
    bool VETO2=false;
    double Esum=0;
    double Mult=0;

	// A loop over every line of the input data in sequence
	for(long jentry=0;jentry<nentries;jentry++){
		
		// This line loads the data into the local variables we defined
		newtree->GetEntry(jentry);

// // 	Accumulate one event, detector data spread over several tree entries
		if(EventID==CurrentEvent){
			EndOfEvent=false;
            double e=depEnergy+r.Gaus(0,noisefactor);
			
            if(cryNumber==1&&e>100){
                VETO1=true;
            }
            if(cryNumber==2&&e>100){
                VETO2=true;
            }
            if(cryNumber==0&&e>30){
                Esum+=e;
                if(detNumber<16)EventHolder[detNumber]=e;
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
		
        Esumraw->Fill(Esum);
        if(!VETO1)Esumvt1->Fill(Esum);
        if(!VETO2)Esumvt2->Fill(Esum);
        if(!VETO2&&!VETO1)Esumvt3->Fill(Esum);
        
        if(Mult==1){
            for(unsigned int i=0;i<EventHolder.size();i++){
            
                double e=EventHolder[i];
                
                if(e>30){
                    Eraw->Fill(e,i);
                    if(!VETO1)Ev1->Fill(e,i);
                    if(!VETO2)Ev2->Fill(e,i);
                    if(!VETO2&&!VETO1)Ev3->Fill(e,i);
                    
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
        Esum=0;
        Mult=0;
    
	}
	
	// Save and close the output file
    out.Write();
    out.Close();
	
    delete newfile; // Delete the variable holding the input file, which also closes it
    
    
}
