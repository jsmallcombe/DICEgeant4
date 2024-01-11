void AutoPostProcess(string rootout = "") {

TFile* inputFile = TFile::Open(rootout.c_str(),"UPDATE");

if (!inputFile || inputFile-> IsZombie ()) {
        std::cerr << "Error: Unable to open the ROOT file." << std::endl;
        return;
    }
    
    // Get the HitMap directory    
    TDirectory* hitMapDir = dynamic_cast <TDirectory*>(inputFile-> Get ( "HitMaps" ));

 // Check if the directory exists 
    if(!hitMapDir) {
        std::cerr << "Error: HitMap directory not found in the ROOT file." << std::endl;
        inputFile-> Close ();
        return;
    } 
    
    // Create a TMultiGraph to hold TGraphs 
    TMultiGraph* multiGraph = new TMultiGraph();

    TList* keys = hitMapDir->GetListOfKeys ();

 // Iterate over all objects in the directory 
    for ( int i = 0 ;i < keys-> GetSize (); ++i) {
        TKey* key = dynamic_cast <TKey*>(keys-> At (i));
        if (!key) continue ;
    // Check if the object is a TGraph 
        if(key-> GetClassName () == TString ( "TGraph" )) {
            TGraph* graph = dynamic_cast <TGraph*>(key-> ReadObj ());
            // Add the TGraph to TMultiGraph 
            multiGraph->Add(graph);
        }
    }

    
    gROOT->cd();
    
        // Create a TCanvas
    TCanvas* canvas = new TCanvas("HitMapCanvas");
    multiGraph->Draw("AP");

    // Create a box
    TBox* box = new TBox(-40, -20, 40, 20);
   box->SetFillStyle(3000);
    box->SetLineColor(1); // Set border width
    box->SetLineWidth(1); // Set border width
    box->Draw("l");

    for (double x = -35; x <= 35; x += 5) {
        TLine* verticalLine = new TLine(x, -20, x, 20);
        verticalLine->SetLineWidth(1);
        verticalLine->SetLineColor(kRed); // Set line color to red (you can choose any color)
        verticalLine->Draw();
    }

    canvas->Update();
    
    

	inputFile->cd("");
    canvas->Write("HitMapCanvas");
    multiGraph->Write("HitMap");
    inputFile->Close();
}
