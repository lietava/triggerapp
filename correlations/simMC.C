#include <iostream>
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TRandom1.h"
#include "TMath.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TDecompSVD.h"
int simMC(){
 TFile *file;
 file = new TFile("stat.root","RECREATE","FILE");
 TH1F *h1= new TH1F("h1","Cor Coef",1000,0.,1.);
 TH2F *h2= new TH2F("h2","Coer Coef versus Dist",1000,0.,1.,15,0,15);
 //gROOT->Reset();
 TRandom1 rnd;
 Int_t NINP=4;
 Int_t NCOR=6;
 Int_t Nssm=1024;
 Double_t PINT=0.2;
 Double_t PBKG=0.8;
 Double_t* Prob = new Double_t[NINP];   // Probability of signal
 Double_t* Bckg = new Double_t[NINP];   // Probability of background
 Bool_t* BCmask = new Bool_t[Nssm]; 
 Int_t* Signals = new Int_t[NINP];
 //Double_t** Corr= new Double_t* [NINP];
 //for(Int_t i=0;i<NINP;i++) Corr[i] = new Double_t[NCOR];
 Bool_t** Inputs = new Bool_t* [NINP];
 for(Int_t i=0;i<NINP;i++) Inputs[i] = new Bool_t[Nssm];
 Bool_t* Interaction = new Bool_t [Nssm];
 Bool_t* Background = new Bool_t [Nssm];
 Double_t* rand = new Double_t[Nssm];
 Double_t* back = new Double_t[Nssm];
 // Create BCmask
 for(Int_t i=0;i<Nssm;i++)BCmask[i] = i%2;  // every second
 // Define probs
 Prob[0]=1.;
 Prob[1]=0.3;
 Prob[2]=0.4;
 Prob[3]=0.5;
 Bckg[0]=0.0;
 Bckg[1]=0.7;
 Bckg[2]=0.5;
 Bckg[3]=0.;
 for(Int_t III=0;III<100;III++){
 // Fill Inputs
 rnd.RndmArray(Nssm,rand);
 rnd.RndmArray(Nssm,back);
 for(Int_t i=0;i<Nssm;i++){
   if(rand[i]<PINT ) Interaction[i]=1;else Interaction[i]=0;
   if(back[i]<PBKG ) Background[i]=1;else Background[i]=0;
 }
 cout << III << " Number of signals: ";
 for(Int_t j=0;j<NINP;j++){
   rnd.RndmArray(Nssm,rand);
   rnd.RndmArray(Nssm,back);
   Int_t sum=0;
   for(Int_t i=0;i<Nssm;i++){
     Bool_t inter = rand[i]<Prob[j] && Interaction[i];
     Bool_t backg = back[i]<Bckg[j] && Background[i];
     Inputs[j][i]=inter || backg;
     sum += inter;
   }
   Signals[j]=sum;
   cout << sum << " ";
 }
 cout << endl;
 // Correlations
 Int_t* delay = new Int_t[NINP-1];
 Int_t Nscan=NCOR/2;
 Int_t Nall=(Int_t)TMath::Power((Double_t)NCOR+1,NINP-1);
 Double_t ddmin=15;
 Double_t ccmax=0;
 for(Int_t i=0;i<Nall;i++){
    //cout << "DELAYS: ";
    Int_t num=i;
    Int_t numlessdigit=0;
    for(Int_t j=0;j<NINP-1;j++){
       numlessdigit=num/(NCOR+1);
       delay[j]=num-numlessdigit*(NCOR+1)-Nscan;
       num=numlessdigit;
       //cout << delay[j] << " ";
    }
    //cout << endl;
    // Create vectors
    TVectorD cvec(NINP-1);
    TMatrixD var(NINP-1,NINP-1);
    for(Int_t j=1;j<NINP;j++){
       Int_t cor=0;
       for(Int_t k=0;k<Nssm;k++)cor += Inputs[0][k]&&Inputs[j][(k+delay[j-1])%Nssm];
       Double_t m0=Signals[0];
       Double_t mj=Signals[j];
       cvec[j-1] = (cor-m0*mj/Nssm)/TMath::Sqrt(m0*(1.-m0/Nssm)*mj*(1.-mj/Nssm)); 
       //cout << " cor=" << cor << endl;
       for(Int_t l=j;l<NINP;l++){
          cor=0;
          for(Int_t k=0;k<Nssm;k++)cor += Inputs[j][(k+delay[j-1])%Nssm]&&Inputs[l][(k+delay[l-1])%Nssm];
          //cout << "var cor=" << cor << " j=" << j << " l="<< l << endl;
                   mj=Signals[j];
          Double_t ml=Signals[l];
          var(j-1,l-1) = (cor-mj*ml/Nssm)/TMath::Sqrt(mj*(1.-mj/Nssm)*ml*(1.-ml/Nssm));
          var(l-1,j-1) = var(j-1,l-1); 
       } 
    }
    //var.Print();
    TDecompSVD svd(var,1e-10);
    //svd.Print();
    bool status;
    TMatrixD inv=svd.Invert(status);
    if(status==0){
      cout << "Inverting problem " << endl;
      return 0;
    }
    TVectorD cvect(cvec);
    cvec *= inv;
    Double_t cc=cvect*cvec;
    Double_t dd=0;
    for(Int_t j=0;j<NINP-1;j++)dd+=TMath::Abs(delay[j]);
    //cout << "dd= " << dd << endl;
    h1->Fill(cc);
    if(cc>ccmax){
      //if(cc==ccmax && dd<ddmin)ddmin=dd;
      ddmin=dd;
      ccmax=cc;
    }
 }
  //cout << ddmin << endl;
  h2->Fill(ccmax,ddmin);
 }
 h1->Write();
 h2->Write();
 file->Close();
 return 0;
}
