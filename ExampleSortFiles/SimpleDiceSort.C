void SimpleDiceSort(const char *rootout = "dice.root", const char *rootin = "g4out.root") {
	
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
	
/// TH1F or TH1D where F/D is the float/double variable type
//// TH1D* VaraibleName= new TH1D("NameWithinRoot","TitleToDisplay",NumberOfBinsInteger,From,To);
//// TH2F* VaraibleName= new TH2F("NameWithinRoot","TitleToDisplay",NumberOfBinsXInteger,XFrom,XTo,NumberOfBinsYInteger,YFrom,YTo);
	
// 		TH2F* XYPos=new TH2F("OriginXY","XY Position Beam;X [mm];Y [mm]",20,-5,5,20,-5,5);
		TH1F* EnergySum=new TH1F("EnergySum","EnergySum;Energy [keV];Counts/keV",1000,0,2000);
		
		TH1F* EnergyN[10];
		for(int i=0;i<10;i++){
			stringstream ss;
			ss<<"EnergyDetector"<<i;
			EnergyN[i]=new TH1F(ss.str().c_str(),ss.str().c_str(),1000,0,1000);
		}
		
	gROOT->cd();//cd back into main session memory 
	
	
	// The TTree newtree is the big list of energies and detector segments we need to load from
	// First we get the length of the list and output it
	long nentries = newtree->GetEntries();
	cout<<endl<<"nentries "<<nentries<<endl;

	// Next we create local variables into which we will read data from the list
	int detNumber,cryNumber;
	double depEnergy;
	
	// Define the link between the "columns" of the list and the local variables we want to read them into
	// See HistoManager.cc for a full list
	newtree->SetBranchAddress("detNumber",&detNumber);
	newtree->SetBranchAddress("cryNumber",&cryNumber);
	newtree->SetBranchAddress("depEnergy",&depEnergy);

	// As each detector within one physical event is stored as a new "line/row" in our list we have to collect them first
	// Thes variabls to help with assembling the events

	///// Loop over every line of the input data in sequence ////
	for(long jentry=0;jentry<nentries;jentry++){
		
		// This line loads the data into the local variables we defined
		newtree->GetEntry(jentry);
		
		// Add randomised electronic noise
		depEnergy+=r.Gaus(0,noisefactor);

		///// Fill histograms ////
		
		EnergySum->Fill(depEnergy);
		
		if(detNumber<10){
			EnergyN[detNumber]->Fill(depEnergy);
		}
            
	}
	
	///// Draw histograms to screen /////
	// Create a new canvas (graphical windows)
	// ->cd to the canvas so that any drawing commands will appear in the desired window
	TCanvas *C1=new TCanvas();
	C1->cd();
	
	EnergySum->DrawCopy();
	// Using the DrawCopy() command, not Draw()  command, to create a copy.
	// Original histograms will disappear from memory when the file is closed and would not be visable
	
	
	TCanvas *C2=new TCanvas();
	C2->cd();
	EnergyN[0]->DrawCopy();
	
	///// The "same" option adds the histogram to the current window without overwritting what is already there
	EnergyN[1]->SetLineColor(2);
	EnergyN[1]->DrawCopy("same");

	EnergyN[2]->SetLineColor(3);
	EnergyN[2]->DrawCopy("same");
	
	C2->SetWindowPosition(600,50);
	
	/////// Analysis ////////
	
	// Peform some rudementry analysis on our histograms
	double cnt= EnergySum->Integral();
	
	cout<<endl<<" Recorded "<<cnt<<" counts in detectors";
	cout<<endl<<" Assuming 10000 events this is a TOTAL efficiency of "<<cnt/100.0<<" %"<<endl;
	
	double peak= EnergySum->Integral(EnergySum->FindBin(495),EnergySum->FindBin(505));
	cout<<endl<<" Recorded "<<peak<<" counts in detectors at ~500 keV";
	cout<<endl<<" Assuming 10000 events this is a 500 keV photo peak efficiency of "<<cnt/100.0<<" %"<<endl;
	
	/////// Save Files ////////

	cout<<endl;
	gStyle->SetLineScalePS(1.5);//default is 3, which looks crap
	C1->Print("EnergySum.pdf");
	
	// Save and close the output file
    out.Write();
    out.Close();
	
    delete newfile; // Delete the variable holding the input file, which also closes it
}
