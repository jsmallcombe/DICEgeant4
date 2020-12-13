void DiceSort(const char *rootout = "dice.root", const char *rootin = "g4out.root") {
	
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
		TH2F* XYPos=new TH2F("OriginXY","XY Position Beam;X [mm];Y [mm]",20,-5,5,20,-5,5);
		TH1F* EnergySum=new TH1F("EnergySum","EnergySum;Energy [keV];Counts/keV",1000,0,1000);
		
		TH1F* EnergyN[3];
		for(int i=0;i<3;i++){
			stringstream ss;
			ss<<"Energy"<<i;
			EnergyN[i]=new TH1F(ss.str().c_str(),(ss.str()+"EnergySum;Energy [keV];Counts/keV").c_str(),1000,0,1000);
		}
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

	vector<vector<double>> EventHolder; // A matrix to store the energies recorded in an event
		

	// A loop over every line of the input data in sequence
	for(long jentry=0;jentry<nentries;jentry++){
		
		// This line loads the data into the local variables we defined
		newtree->GetEntry(jentry);

// // 	Accumulate one event, detector data spread over several tree entries
		if(EventID==CurrentEvent){
			EndOfEvent=false;
			
			// Expand EventHolder if needed and load the energy for that detector and the current event
			while(EventHolder.size()<=detNumber)EventHolder.push_back(vector<double>());
			while(EventHolder[detNumber].size()<=cryNumber)EventHolder[detNumber].push_back(0);
			EventHolder[detNumber][cryNumber]=depEnergy+r.Gaus(0,noisefactor);
			//// This is a little intensive and clunky, once detector is fixed replace
			
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
		
		//Iterate over the matrix (could also have made sum while building event)
		double Esum=0;
		for(auto i : EventHolder){
			for(auto j : i){
				Esum+=j;
			}
		}
		
		// Fill Histogram
		if(Esum>0){
			EnergySum->Fill(Esum);
		}
		
		// Loop to fill several histograms if there is data
		for(int i=0;i<3;i++){
			if(EventHolder.size()>i){
				if(EventHolder[i].size()>0){
				if(EventHolder[i][0]>0){
					EnergyN[i]->Fill(EventHolder[i][0]);				
				}}
			}
		}
            
        /////  Reset things ready for next event
		for(auto &i : EventHolder){
			std::fill(i.begin(), i.end(), 0);
		}
	}
	
	//Before saving file draw histograms to screen
	
	TCanvas *C1=new TCanvas();
	C1->cd();
	EnergySum->DrawCopy();
	// I use the DrawCopy not Draw  command, to create a copy because the histograms will disappear from memore when the file is closed and not be visable anymore
    
	TCanvas *C2=new TCanvas();
	C2->cd();
	EnergyN[0]->DrawCopy();
	EnergyN[1]->SetLineColor(2);
	EnergyN[1]->DrawCopy("same");
	EnergyN[2]->SetLineColor(3);
	EnergyN[2]->DrawCopy("same");
	
	C2->SetWindowPosition(600,0);
	
	// Peform some rudementry analysis on our histograms
	double cnt= EnergySum->Integral();
	
	cout<<endl<<" Recorded "<<cnt<<" counts in detectors";
	cout<<endl<<" Assuming 10000 events this is a TOTAL efficiency "<<cnt/100.0<<" %";
	cout<<endl;
	
	// Save and close the output file
    out.Write();
    out.Close();
	
    delete newfile; // Delete the variable holding the input file, which also closes it
}
