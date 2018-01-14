{
   // create three components for the fit
   TH2D h1 ("h1","h1",100, 0, 10, 100, 0, 10);
   TH2D h2 ("h2","h2",100, 0, 10, 100, 0, 10);
   TH2D h3 ("h3","h3",100, 0, 10, 100, 0, 10);

   // fill pdf's
   for (int i = 0; i<1e8; i++) {
      h1.Fill(gRandom->Gaus(5,1),     gRandom->Gaus(3,.5));
      h2.Fill(gRandom->Uniform(0,10), gRandom->Uniform(0,10));
      h3.Fill(gRandom->Exp(2),        gRandom->Exp(1) );
   }

   // normalize pdf's
   h1.Scale(1./h1.Integral(0,10));
   h2.Scale(1./h2.Integral(0,10));
   h3.Scale(1./h3.Integral(0,10));

   // convert them into THn and store them into a file
   TFile output ("tmp_2D_pdfs.root", "recreate");
   output.cd();
   for (const auto& i : {h1, h2, h3}) {
      auto tmp = THn::CreateHn(i.GetName(), i.GetName(), &i);
      tmp->SetEntries(i.GetEntries());
      tmp->Write();
   }
   output.Close();
}
