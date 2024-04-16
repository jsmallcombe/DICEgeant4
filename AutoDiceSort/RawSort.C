void RawSort(const char * outfileName = "RawSort.root", const char *ntuplefileName = "g4out.root", string folder= ""){
	
    // Load the input file
    TFile *ntuplefile = new TFile(ntuplefileName,"READ");
	gROOT->cd();
    if(!ntuplefile->IsOpen())return;
    TTree *newtree = (TTree*)ntuplefile->Get("ntuple");
    if(newtree==nullptr)return;


	TFile out(outfileName,"UPDATE");
	if(folder.size()){
		out.mkdir(folder.c_str());
		out.cd(folder.c_str());
	}
	
		TH1F* HitSiRaw=new TH1F("HitSiRaw","HitSiRaw;Electron Energy (keV);Counts",2000,0,2000);
		TH1F* HitGuardRaw=new TH1F("HitGuard","HitGuard;Electron Energy (keV);Counts",2000,0,2000);
		TH1F* HitRawTot=new TH1F("HitRawTot","HitRawTot;Electron Energy (keV);Counts",2000,0,2000);
		TH2F* HitRawTotVTrue=new TH2F("ERawTrue","ERawTrue;Electron Energy (keV);Gun Energy",2000,0,2000,200,0,2000);

	gROOT->cd();//cd back into main session memory 
	
	// The TTree newtree is the big list of energies and detector segments we need to load from
	// First we get the length of the list and output it
	long nentries = newtree->GetEntries();
	cout<<endl<<"nentries "<<nentries<<endl;

	int detNumber,cryNumber;
	double depEnergy,primaryE;
	
	// Define the link between the "columns" of the list and the local variables we want to read them into
	// See HistoManager.cc for a full list
	newtree->SetBranchAddress("detNumber",&detNumber);
	newtree->SetBranchAddress("cryNumber",&cryNumber);
	newtree->SetBranchAddress("depEnergy",&depEnergy);
	newtree->SetBranchAddress("primaryE",&primaryE);

	// As each detector within one physical event is stored as a new "line/row" in our list we have to collect them first
	// Thes variabls to help with assembling the events

	// A loop over every line of the input data in sequence
	for(long jentry=0;jentry<nentries;jentry++){
		
		// This line loads the data into the local variables we defined
		newtree->GetEntry(jentry);

		if(cryNumber==4){
			HitRawTot->Fill(primaryE);
		}
		if(cryNumber==0){
			if(detNumber>0&&detNumber<17){//0 is guard ring
				HitSiRaw->Fill(depEnergy);
				HitRawTot->Fill(depEnergy);
				HitRawTotVTrue->Fill(depEnergy,primaryE);
			}else if(detNumber==0){
				HitGuardRaw->Fill(depEnergy);
				HitRawTot->Fill(depEnergy);
				HitRawTotVTrue->Fill(depEnergy,primaryE);
			}
		}

	}
	
    out.Write();
    out.Close();	
    delete ntuplefile; // Delete the variable holding the input file, which also closes it
    
}

