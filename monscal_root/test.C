{
 gROOT->Reset();
 gROOT->SetStyle("Plain");
  //gStyle->SetTitleFontSize(0.2);
  //gStyle->SetTitleSize(0.8,"t");
  gStyle->SetTitleH(0.22);
  gStyle->SetTitleW(0.4);
  gStyle->SetTitleX(0.25);
  gStyle->SetTitleY(1.020);
  gStyle->SetPadLeftMargin(0.2);
  gStyle->SetPadRightMargin(0.15);
  gStyle->SetPadTopMargin(0.18);
  gStyle->SetPadBottomMargin(0.2);
  //gStyle->SetLabelOffset(0.025,"Y");
  //gStyle->SetStripDecimals(kFALSE); 

 Int_t nn=20;
 TDatime tt;
 Int_t time=tt.GetTime();
 Double_t xx[nn],yy[nn],ex[nn],ey[nn];
 for(Int_t i=0;i<nn;i++){
    xx[i]=10000.+i*10;
    yy[i]=time+i*60;
    //yy[i]=i*60;
    ex[i]=xx[i]/10.;
    ey[i]=0;
 }
 Int_t ncd=1, i=1;
 Int_t NCD=7;
 TGraphErrors *f1= new TGraphErrors(nn,xx,yy,ex,ey);
 f1->GetXaxis()->SetLabelSize(0.1);
 f1->GetYaxis()->SetLabelSize(0.1);
 f1->GetXaxis()->SetNdivisions(401,kFALSE);
 f1->GetYaxis()->SetNdivisions(-410,kFALSE);
 //f1->GetXaxis()->SetNoExponent();
 //f1->GetYaxis()->LabelsOption("v"); // doesn't work as labels "don't exist" for the graph y axis, only values
 f1->GetYaxis()->SetTimeDisplay(1);
 f1->GetYaxis()->SetTimeFormat("%H:%M");
 f1->SetMarkerColor(1+(ncd%4));
 f1->SetMarkerStyle(20+(ncd%4));
 f1->SetTitle("TEST\n test ttt");
 f1->GetXaxis()->SetLabelOffset(0.001);
 if(ncd % 2)f1->GetXaxis()->SetLabelOffset(0.05);
 //f1->GetXaxis()->SetDecimals(0);
 if(i==0){
  //f1->GetYaxis()->SetLabelSize(0.12);
  //f1->GetYaxis()->SetLabelOffset(1);
  f1->GetYaxis()->SetTitle("Time(min)");
  f1->GetYaxis()->SetTitleSize(0.1);
  f1->GetYaxis()->SetTitleOffset(-4.75);
 }
 TCanvas *fCanvas = new TCanvas("fc","INPUTS",1200,300);
 fCanvas->Divide(NCD,1,0.0025,0.0025,0);
 ncd=2;
 fCanvas->GetPad(ncd)->SetGridy(1);
 fCanvas->cd(ncd);

 f1->Draw("ap");

 fCanvas->Update();
 // Copy to MONSCAL for html display
 fCanvas->SaveAs("test.png");

}

