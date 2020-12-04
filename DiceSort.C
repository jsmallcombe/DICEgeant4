void DiceSort(const char *rootout = "dice.root", const char *rootin = "g4out.root") {
	
    // get the merged root file
    TFile *newfile = new TFile(rootin);
	gROOT->cd();
    if(!newfile->IsOpen())return;
    TTree *newtree = (TTree*)newfile->Get("ntuple");
    if(newtree==nullptr)return;

	
	TFile out(rootout,"RECREATE");
	out.cd();	
		TH2F* XYPos=new TH2F("XYPos","XY Position;X [mm];Y [mm]",10,0,10,10,0,10);
	gROOT->cd();

	long nentries = newtree->GetEntries();
	
	cout<<endl<<"nentries "<<nentries<<endl;

	int EventID;
	int detNumber,cryNumber;
	double depEnergy;
    
	newtree->SetBranchAddress("eventNumber",&EventID);
	newtree->SetBranchAddress("detNumber",&detNumber);
	newtree->SetBranchAddress("cryNumber",&cryNumber);
	newtree->SetBranchAddress("depEnergy",&depEnergy);
    
//  TRandom r;
// 	r.SetSeed();
	
	bool EndOfEvent=true;
	long CurrentEvent=0;

	vector<vector<double>> EventHolder; // Holder for the event	
		
	for(long jentry=0;jentry<nentries;jentry++){
		newtree->GetEntry(jentry); 

// // 	Accumulate one event, detector data spread over several tree entries
		if(EventID==CurrentEvent){
			EndOfEvent=false;
			
			//// This is a little intensive and clunky, once detector is fixed replace
			while(EventHolder.size()<=detNumber)EventHolder.push_back(vector<double>());
			while(EventHolder[detNumber].size()<=cryNumber)EventHolder[detNumber].push_back(0);
			EventHolder[detNumber][cryNumber]=depEnergy;
			
			XYPos->Fill(detNumber,cryNumber);
			
		}else{
			CurrentEvent=EventID;
			EndOfEvent=true;
			jentry--;
		}
		
		if(jentry==(nentries-1))EndOfEvent=true;
		if(!EndOfEvent) continue;
		
        /////////////////////////
        ////  Now we have collected a complete event and can look at correlations
        ////////////////////////
            
        /////  Reset things ready for next event
		for(auto i : EventHolder){
			std::fill(i.begin(), i.end(), 0);
		}
	}
    
    XYPos->DrawCopy("col");
    
    out.Write();
    out.Close();
	
    delete newfile;
}
