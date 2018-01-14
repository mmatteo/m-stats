{
   // some fancy changes to the default plotting style
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   gStyle->SetTitleStyle(0);
   gStyle->SetColorModelPS();
   gStyle->SetLineScalePS(1.5);

   // retrieve Ntuple from file
   TFile iFile ("tmp_1D_fitSingleSet.root");
   TTree* t = nullptr;
   iFile.GetObject("t",t);
   if(t==nullptr) {
      cerr << "ntuple not found. Aborting.\n";
      return;
   }
      
   // populate the canvas
   TCanvas c1;
   c1.Divide(3,3);
   c1.cd(1); t->Draw(          "global.h1","!minuitStatus");
   c1.cd(5); t->Draw(          "global.h2","!minuitStatus");              
   c1.cd(9); t->Draw(          "global.h3","!minuitStatus");              
   c1.cd(4); t->Draw("global.h2:global.h1","!minuitStatus","colz");
   c1.cd(7); t->Draw("global.h3:global.h1","!minuitStatus","colz");
   c1.cd(8); t->Draw("global.h3:global.h2","!minuitStatus","colz");

   // pre-draw canvas: needed to create the objects in the canvas
   // and retrieve them for further operations
   c1.Draw();

   // auto loop over the pads with the 1D hist
   for (auto i : {1, 5, 9} ) {
      c1.cd(i); 
      // retrieve from the pad the histogram. The object returned is a pointer
      // to a TObject and must be downcasted. For a save downcansting use
      // dynamic_cast. For more info about casting check here:
      // http://www.cplusplus.com/doc/tutorial/typecasting/
      auto hh = dynamic_cast<TH1F*>(gPad->GetListOfPrimitives()->At(1));
      if (hh == nullptr) continue;
      auto res = hh->Fit("gaus","S");
      cout << "Chi-square for " << hh->GetName() << " is " 
           << res->Chi2() << " / " << res->Ndf() << " = "
           << res->Chi2() / res->Ndf() <<  endl;
   }

   // auto loop over the pads with the 2D hist
   //  cout << "Correlation coefficients\n"
   for (auto i : {4, 7, 8} ) {
      c1.cd(i); 
      auto hh = dynamic_cast<TH2F*>(gPad->GetListOfPrimitives()->At(1));
      if (hh == nullptr) continue;
      cout << "Correlation factor for " << hh->GetName() << " is " 
           << hh->GetCorrelationFactor() << endl;
   }

   // Draw clone of canvas before c1 goes out of scope
   c1.DrawClone();
}
