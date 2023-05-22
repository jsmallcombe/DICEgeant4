void HeliosSort(const char *rootout = "heliosdice.root", const char *rootin = "g4out.root") {
	
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

		TH1F* EnergySum=new TH1F("EnergySum","EnergySum;Energy [keV];Counts/keV",2000,0,2000);
		TH1F* EnergySingles=new TH1F("EnergySumRaw","EnergySumRaw;Energy [keV];Counts/keV",1000,0,2000);
		TH2F* NE=new TH2F("NE","NE;Detector;Energy [keV]",150,0,150,2000,0,2000);
		TH2F* NERaw=new TH2F("NEraw","NE;Detector;Energy [keV]",150,0,150,2000,0,2000);

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

	vector<pair<int,double>> EventHolder; // A matrix to store the energies recorded in an event
	vector<vector<pair<int,double>>> HitCollections;
    double EventSum;

	// A loop over every line of the input data in sequence
	for(long jentry=0;jentry<nentries;jentry++){
		
		// This line loads the data into the local variables we defined
		newtree->GetEntry(jentry);

// // 	Accumulate one event, detector data spread over several tree entries
		if(EventID==CurrentEvent){
			EndOfEvent=false;
            double E=depEnergy+r.Gaus(0,noisefactor);
            EventSum+=E;
            EnergySingles->Fill(E);
            NERaw->Fill(detNumber,E);
            pair<int,double> hit(detNumber,E);
			EventHolder.push_back(hit);
            
            bool merged=false;
			for(auto j : HitCollections){
				if(detNumber==j[0].first-1||detNumber==j[j.size()-1].first+1){
                    j.push_back(hit);
                    sort(j.begin(), j.end());
                    merged=true;
                    break;
                }
			}
			if(!merged){
//                 vector<pair<int,double>> NewHit={i};
                HitCollections.push_back(vector<pair<int,double>>{hit});
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
            
		
        vector<pair<int,double>> MergedHits;
        
        for(auto j : HitCollections){
            int i=j[0].first;
            double E=0;
            for(auto k : j){
                E+=k.second;
            }
            
            MergedHits.push_back(pair<int,double>(i,E));
            NE->Fill(i,E);
        }

		if(EventSum>0){
			EnergySum->Fill(EventSum);
		}
          
        /////  Reset things ready for next event
        EventHolder.clear();
        HitCollections.clear();
        EventSum=0;
	}
	
	//Before saving file draw histograms to screen
	
	TCanvas *C1=new TCanvas();
	C1->cd();
	NE->DrawCopy("col");
	// I use the DrawCopy not Draw  command, to create a copy because the histograms will disappear from memore when the file is closed and not be visable anymore
    
// 	TCanvas *C2=new TCanvas();
// 	C2->cd();
// 	EnergyN[0]->DrawCopy();
// 	
// 	EnergyN[1]->SetLineColor(2);
// 	EnergyN[1]->DrawCopy("same");
// 
// 	EnergyN[2]->SetLineColor(3);
// 	EnergyN[2]->DrawCopy("same");
// 	
// 	C2->SetWindowPosition(600,50);
// 	
// 	// Peform some rudementry analysis on our histograms
// 	double cnt= EnergySum->Integral();
// 	
// 	cout<<endl<<" Recorded "<<cnt<<" counts in detectors";
// 	cout<<endl<<" Assuming 10000 events this is a TOTAL efficiency "<<cnt/100.0<<" %";
// 	cout<<endl;
// 	
// 	gStyle->SetLineScalePS(1.5);//default is 3, which looks crap
// 	C1->Print("EnergySum.pdf");
	
	// Save and close the output file
    out.Write();
    out.Close();
	
    delete newfile; // Delete the variable holding the input file, which also closes it
}
