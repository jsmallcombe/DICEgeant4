void ShoeSort(const char *rootout = "shoedice.root", const char *rootin = "g4out.root") {
	
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

		TH1F* EnergySum[2];
		EnergySum[0]=new TH1F("EnergySumA","EnergySumA;Energy [keV];Counts/keV",2000,0,2000);
		EnergySum[1]=new TH1F("EnergySumB","EnergySumB;Energy [keV];Counts/keV",2000,0,2000);
        
		TH1F* EnergySingles[2];
		EnergySingles[0]=new TH1F("EnergySumRawA","EnergySumRawA;Energy [keV];Counts/keV",1000,0,2000);
		EnergySingles[1]=new TH1F("EnergySumRawB","EnergySumRawB;Energy [keV];Counts/keV",1000,0,2000);
        
		TH2F* NE[2];
        NE[0]=new TH2F("NEA","NEA;Detector;Energy [keV]",160,0,160,2000,0,2000);
        NE[1]=new TH2F("NEB","NEB;Detector;Energy [keV]",160,0,160,2000,0,2000);
        
		TH2F* NEx[2];
        NEx[0]=new TH2F("NExA","NExA;DetectorX;Energy [keV]",20,0,20,2000,0,2000);
        NEx[1]=new TH2F("NExB","NExB;DetectorX;Energy [keV]",20,0,20,2000,0,2000);

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

	vector<pair<int,double>> EventHolder[2];
    double EventSum;

	// A loop over every line of the input data in sequence
	for(long jentry=0;jentry<nentries;jentry++){
		
		// This line loads the data into the local variables we defined
		newtree->GetEntry(jentry);

// // 	Accumulate one event, detector data spread over several tree entries
		if(EventID==CurrentEvent){
			EndOfEvent=false;
            double E=depEnergy+r.Gaus(0,noisefactor);
//             NERaw->Fill(detNumber,E);
            pair<int,double> hit(detNumber,E);
            
            
            if(cryNumber>1||cryNumber<0)continue;
            
            EventHolder[cryNumber].push_back(hit);
            EventSum+=E;
            if(detNumber>0)EnergySingles[cryNumber]->Fill(E);
                
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
            
//         cout<<"Event "<<jentry<<endl;
//         cout<<"EventHolder Sizes "<<EventHolder[0].size()<<" "<<EventHolder[1].size()<<endl;
        
        
		if(!(EventHolder[0].size()*EventHolder[1].size())){
            int Det=0;
            if(EventHolder[1].size())Det=1;
            sort(EventHolder[Det].begin(),EventHolder[Det].end());
        
//             cout<<"Det "<<Det<<endl;
        
            if(EventHolder[Det].size()<3&&EventHolder[Det].size()>0){
//                 cout<<"[0].first "<<EventHolder[Det][0].first<<endl;
                if(EventHolder[Det][0].first!=0){
                    int x=(EventHolder[Det][0].first-1)%8;
                    int y=(EventHolder[Det][0].first-1)/8;

//                     cout<<"xy "<<x<<" "<<y<<endl;
                    
                    if(EventSum>0){
                        EnergySum[Det]->Fill(EventSum);
                        
                        NE[Det]->Fill(y+20*x,EventSum);
                        NEx[Det]->Fill(y,EventSum);
                    }
                }
            }
        }
//         cout<<endl;
          
        /////  Reset things ready for next event
        EventHolder[0].clear();
        EventHolder[1].clear();
        EventSum=0;
	}
	
	//Before saving file draw histograms to screen
	
	TCanvas *C1=new TCanvas();
	C1->cd();
	NE[0]->DrawCopy("col");
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
