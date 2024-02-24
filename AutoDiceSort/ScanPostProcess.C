void ScanPostProcess(string rootout = "") {

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
    canvas->Write("HitMapCanvas",TObject::kOverwrite);
    multiGraph->Write("HitMap",TObject::kOverwrite);
	
	
	
	inputFile->mkdir("CorrAngles");
	gROOT->cd();
	
    for(int b=0;b<16;b++){
		for(int m=0;m<4;m++){
			stringstream ss;
			ss<<"AngleExpt/grid"<<b<<"_"<<m;
			TGraph *grf = (TGraph*)inputFile->Get(ss.str().c_str());
			
			if(grf){
				TGraph *adj=new TGraph();
				double xpre,ypre,x,y;
				
				
				// Old version were cut was done in angles
// // 				grf->GetPoint(0,xpre,ypre);
// // 				adj->SetPoint(0,0,-1);
// // 				adj->SetPoint(1,xpre,ypre);
// // 				for(int p=1;p<grf->GetN();p++){
// // 					grf->GetPoint(p,x,y);
// // 					
// // 					if(ypre*y<0){
// // 						double dX=x-xpre;
// // 						adj->SetPoint(adj->GetN(),xpre+dX*0.49,ypre);
// // 						adj->SetPoint(adj->GetN(),xpre+dX*0.51,y);
// // 					}
// // 					adj->SetPoint(adj->GetN(),x,y);
// // 					
// // 					xpre=x;
// // 					ypre=y;
// // 				}
				
				// New version; cut is done before angle, so leave zeros out, for interpolation
				
				for(int p=0;p<grf->GetN();p++){
					grf->GetPoint(p,x,y);
					
					if(y>0){
						adj->SetPoint(adj->GetN(),x,y);
					}
				}
				
				
				inputFile->cd("CorrAngles");
					stringstream SS;
					SS<<"grid"<<b<<"_"<<m;
					adj->Write(SS.str().c_str(),TObject::kOverwrite);
				gROOT->cd();
				
			}else{
				cout<<endl<<"NO GRAPH"<<endl;
				return;
			}
		}
	}
	
	
	
	
	
    inputFile->Close();
}
