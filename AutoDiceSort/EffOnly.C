void EffOnly(string Estring,int Nevent=0,const char * outfileName = "EffOnly.root", const char *ntuplefileName = "g4out.root"){
	
	stringstream pointsstream;
	pointsstream<<Estring;
	double Epoint;
	int N=0;
	vector<double> centroids;
	vector<double> binedge;
	while(pointsstream>>Epoint){centroids.push_back(Epoint);N++;}
	if(N<2)return;
	binedge.push_back(centroids[0]-0.5*(centroids[1]-centroids[0]));
	binedge.push_back(centroids[0]+0.5*(centroids[1]-centroids[0]));
	for(int i=1;i<N;i++)binedge.push_back(centroids[i]+0.5*(centroids[i]-centroids[i-1]));

	
    // Load the input file
    TFile *ntuplefile = new TFile(ntuplefileName,"READ");
	gROOT->cd();
    if(!ntuplefile->IsOpen())return;
    TTree *newtree = (TTree*)ntuplefile->Get("ntuple");
    if(newtree==nullptr)return;


	TFile out(outfileName,"UPDATE");
	
		TH1F* HitRaw=new TH1F("HitRaw","HitRaw;Electron Energy (keV);Counts",N,&binedge[0]);
		TH1F* Eff=new TH1F("EffRawSingle","EffRawSingle;Electron Energy (keV);Counts",N,&binedge[0]);
		TH1F* PeakTot=new TH1F("PeakTot","PeakTot;Electron Energy (keV);Counts",N,&binedge[0]);
		TH1F* ERaw=new TH1F("ERaw","ERaw;Electron Energy (keV);Counts",2000,0,2000);
		TGraph* EffG=new TGraph();

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
			HitRaw->Fill(primaryE);
		}
		if(cryNumber==0){
			if(detNumber>0&&detNumber<17){//0 is guard ring
				detNumber--; // Shift to zero index
				ERaw->Fill(depEnergy);
				if(primaryE-depEnergy<5){
					Eff->Fill(primaryE);
					PeakTot->Fill(primaryE);
				}
			}
		}

	}
	
	
	// Efficiency fitting
	if(Nevent>0){

		TAxis* ax=Eff->GetXaxis();
		for(int b=1;b<=ax->GetNbins();b++){
			Eff->SetBinContent(b,Eff->GetBinContent(b)/Nevent);
			double Tt=HitRaw->GetBinContent(b);
			if(Tt>0){
				PeakTot->SetBinContent(b,PeakTot->GetBinContent(b)/Tt);
			}
			
			EffG->SetPoint(b-1,centroids[b-1],Eff->GetBinContent(b)/Nevent);
		}
	}
	
	out.cd();
	EffG->Write("EffGraph");
    out.Write();
    out.Close();	
    delete ntuplefile; // Delete the variable holding the input file, which also closes it
    
}

