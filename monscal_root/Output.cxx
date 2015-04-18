#include "Output.h"
#include "ActiveRun.h"
#include <iostream>
#include <sstream>
#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TText.h"
#include "TMath.h"
#include "Dim.h"
//======================================================================
extern char machine;
ofstream* DisplaySCAL::fileSCAL=0;
DisplaySCAL::DisplaySCAL(ActiveRun& ar)
:
Log(),
runnum(0),
ninp(0),
nclust(0),
ivln(NITEMS),ivhn(NITEMS),
show(0),
npoints(20),
cpoints(0),
nnpoints(1)
{
 for(int i=0;i<NITEMS;i++){
   dataInp[i] = 0;
   itemnames[i] = 0;
 }
 for(int i=0;i<NCLUST;i++){
  dataL0[i]=0;
  dataL2[i]=0;
  strcpy(ClustNames[i],"");
 }
 cout << "Creating DisplaySCAL ver 1" << endl;
 runnum = ar.GetRunNumber();
 //if(runnum && (!TString(ar.GetName()).Contains("PHYSICS_1"))) return ;
 if(runnum == 0) goto run;
 if(runnum && (TString(ar.GetName()).Contains("PHYSICS_1"))) goto run ;
 if(runnum && (TString(ar.GetName()).Contains("PHYSICS_2"))) goto run ;
 if(runnum && (TString(ar.GetName()).Contains("PHYSICS_3"))) goto run ;
 if(runnum && (TString(ar.GetName()).Contains("PHYSICS_4"))) goto run ;
 if(runnum && (TString(ar.GetName()).Contains("PHYSICS_5"))) goto run ;
 if(runnum && (TString(ar.GetName()).Contains("PHYSICS_6"))) goto run ;
 return ;
 run:
 show = 1;
 cout << "Starting DisplaySCAL for run: " << runnum << endl;
 ar.Print();
 ninp = ar.Getninp();
 nclust = ar.Getnclust();
 inps = ar.GetInps();
 inter = ar.GetInter();
 clusts = ar.GetClusts();
 ar.Print();
 if(fileSCAL==0){
   char time[30];
   getdatetime(time);
   stringstream ss;
   ss << "MONSCAL/display" <<".log";
   //ss << "MONSCAL/dispdeb" <<".log";
   cout << "Opening " << ss.str() << endl;
   string text(ss.str());
   fileSCAL = new ofstream();
   fileSCAL->open(text.c_str());
 }
 SetGlobals();
 if(runnum){ // normal run
   for(int i=0;i<nclust;i++){ // loop over clusters
     CreateClassesofClust(i);
   }
 }else{      // inputs
   CreateInputs();
 }
 cout << "DisplayScal created for run " << runnum << endl;
}
DisplaySCAL::~DisplaySCAL()
{
 cout << "Stopping DisplaySCAL for run: " << runnum << endl;
 ClearData();
 for(int i=0;i<nclust;i++){
  stringstream ss;
  ss  << "rm " << ClustNames[i] << endl;
  cout << "Deleting "<< ClustNames[i] << " " << system(ss.str().c_str()) << endl;
 }
 cout << "DisplaySCAL for run: " << runnum << " stopped." << endl;
}
void DisplaySCAL::CheckIndex(int id,int index,int max)
{
 if(index >= max){
   cout << "ARRAY overflow ID= " << id << " index= "  << index << " max= " << max << endl; 
   exit(2);
 }
}
void DisplaySCAL::CreateInputs(int ninp,TriggerInputwCount** inps)
{
 this->ninp=ninp;
 this->inps=inps;
 CreateInputs();
}
void DisplaySCAL::CreateInputs()
{
// Allocating memory for input data
 dataInp[NTIME] = new double[npoints];
 for(int i=0;i<ninp;i++){
    int pos=inps[i]->GetPosition()-1;
    CheckIndex(1,pos,NITEMS);
    TString name(inps[i]->GetCounter()->GetName()); 
    if(pos<0){
       cout << "name = " << name << " pos " << pos << " skipped \n";
       continue;
    }
    dataInp[pos] = new double[npoints];
    SetItemName((inps[i]->GetCounter()->GetName()).c_str(),pos); 
    cout << "name = " << name << " pos " << pos << " created \n";
    //SetItemName(name.Data(),pos); 
    if(name.Contains("0VLN"))ivln=i;
    if(name.Contains("0VHN"))ivhn=i;
 }
 cout << "ivln= " << ivln << " ivhn=" << ivhn << endl;
 if(ivln<NITEMS && ivhn<NITEMS){
    dataInp[NLHRAT]=new double[npoints];
    SetItemName("VLN/VHN",NLHRAT);
 }
 if(inter){
   dataInp[NINT1] = new double[npoints];
   dataInp[NINT2] = new double[npoints];
   SetItemName(inter->GetName1().c_str(),NINT1); 
   SetItemName(inter->GetName2().c_str(),NINT2); 
   //SetItemName("IR1",NINT1); 
   //SetItemName("IR2",NINT2); 
 }
}
void DisplaySCAL::DisplayInputs()
{
 // Dealing with time
 if(inps[0]->GetCounter()->GetTimeSec() < 3.) return;  // skip 61 sec window, later with dim
 //dataInp[NITEMS][cpoints]=inps[0]->GetCounter()->GetTimeTotSec(); time from counters
 int tt;
 gettimeI(&tt);
 CheckIndex(2,cpoints,npoints);
 //cout << "tt= " << tt << endl;
 dataInp[NTIME][cpoints]=tt;
 // Dealing with interactions
 if(inter){
  dataInp[NINT1][cpoints] = inter->GetRate1();
  dataInp[NINT2][cpoints] = inter->GetRate2();
 }
 // Dealing with inputs
 for(int i=0;i<ninp;i++){
    int pos=inps[i]->GetPosition()-1;
    CheckIndex(3,pos,NITEMS);
    dataInp[pos][cpoints]=inps[i]->GetCounter()->GetRate();
 }
 //if(ivln<NLHRAT && ivhn<NLHRAT){
 if(ivln<NITEMS && ivhn<NITEMS){
   if(dataInp[ivhn][cpoints])dataInp[NLHRAT][cpoints]=dataInp[ivln][cpoints]/dataInp[ivhn][cpoints];
   //else dataInp[NLHRAT][cpoints]=0;
 }
 DrawCanvasInps();
 nnpoints++;
 if(nnpoints>npoints)nnpoints=npoints;
 cpoints = (cpoints+1) % nnpoints;
}
void DisplaySCAL::SetGlobals()
{
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
}
void DisplaySCAL::SetAxisInps(TGraphErrors* f1,int i, int ncd)
{
 f1->GetXaxis()->SetLabelSize(0.1);
 f1->GetYaxis()->SetLabelSize(0.1);
 f1->GetXaxis()->SetNdivisions(401,kFALSE);
 //f1->GetXaxis()->SetNdivisions(402,kFALSE);
 f1->GetYaxis()->SetNdivisions(-410,kFALSE);
 //f1->GetXaxis()->SetNoExponent();
 //f1->GetYaxis()->LabelsOption("v"); // doesn't work as labels "don't exist" for the graph y axis, only values
 f1->GetYaxis()->SetTimeDisplay(1);
 f1->GetYaxis()->SetTimeFormat("%H:%M");
 f1->SetMarkerColor(1+(ncd%4));
 f1->SetMarkerStyle(20+(ncd%4));
 f1->SetTitle(itemnames[i]);
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
}
void DisplaySCAL::SetAxisClass(TGraphErrors* f1,int item, int ncd,int level)
{
 //gStyle->SetTitleSize(0.8,"t");
 //gStyle->SetTitleFontSize(0.3);
 f1->GetXaxis()->SetLabelSize(0.14);
 f1->GetYaxis()->SetLabelSize(0.14);
 f1->GetYaxis()->SetNdivisions(402,kTRUE);
 f1->GetXaxis()->SetNdivisions(403,kTRUE);
 //f1->GetXaxis()->SetNdivisions(-405,kFALSE);
 f1->GetYaxis()->SetNoExponent();
 f1->GetXaxis()->SetTimeDisplay(1);
 f1->GetXaxis()->SetTimeFormat("%H:%M");
 //f1->GetXaxis()->SetLabelOffset(1);
 //
 f1->SetMarkerColor(1+(ncd%4));
 f1->SetMarkerStyle(20+(ncd%4));
 if(level == 0) f1->SetTitle(itemnames[item]);
 else if(level == 1) f1->SetTitle("L2 rate");
 else if(level == 2) f1->SetTitle("L2/L0");
 else{
   cout << "Wrong level " << level << endl;
 }
 if(item==0){
  //f1->GetYaxis()->SetLabelSize(0.12);
  //f1->GetYaxis()->SetLabelOffset(1);
  f1->GetYaxis()->SetTitle("Time(min)");
  f1->GetYaxis()->SetTitleSize(0.1);
  f1->GetYaxis()->SetTitleOffset(-4.75);
}
}
void DisplaySCAL::DrawCanvasClasses(int iclu)
{
  //int iclu=activeclust;
  if(!TString(ClustNames[iclu]).Contains("ALLNOTRD")){
  if(!TString(ClustNames[iclu]).Contains("ALL")) return ;
  }
  
  cout << "Drawing canvas classes " << ClustNames[iclu] << endl;
  if(!dataL0){
    cout << "Internal error, dataL0=0" << endl;
    return;
  }
  //
  TGraphErrors* graphs[NITEMS][3];
  for(int j=0;j<3;j++){
     for(int i=0;i<NITEMS;i++)graphs[i][j]=0;
  }
  int ncd=0;
  CheckIndex(4,iclu,NCLUST); 
  for(int i=0;i<NITEMS;i++){
    if(i==NTIME)continue;
    if(dataL0[iclu][i]){
    if(strncmp("CBEAMB",itemnames[i],5) == 0) continue;
    if(TString(itemnames[i]).Contains("-E")) continue;
    if(TString(itemnames[i]).Contains("-C")) continue;
    if(TString(itemnames[i]).Contains("-A")) continue;
    if(TString(itemnames[i]).Contains("-AC")) continue;
    if(TString(itemnames[i]).Contains("C0SMB")) continue;
    if(TString(itemnames[i]).Contains("C0SM5")) continue;
    if(TString(itemnames[i]).Contains("C0VBA")) continue;
    if(TString(itemnames[i]).Contains("C0VBC")) continue;
    cout << itemnames[i] << endl;
      //cout << "New " << iclu << " " << ClustNames[iclu] << " item= "  << i << endl;
// debug
      /*
      stringstream ss;
      //sprintf(ClustNames[iclu],"%i_%s.png",runnum,clusts[iclu]->GetNamechar());
      ss << ClustNames[iclu]<<"_"<<i<<".txt";
      cout << "Opening " << ss.str() << endl;
      string text(ss.str());
      ofstream* fileclust = new ofstream();
      fileclust->open(text.c_str());
      *fileclust << nnpoints << endl;
      for(int ii=0;ii<nnpoints;ii++){
        *fileclust << "L0:" << dataL0[iclu][i][ii] << " " << dataL0[iclu][NTIME][ii];
        *fileclust << " " << edatL0[iclu][i][ii] << " " << edatL0[iclu][NTIME][ii] << endl;
        *fileclust << "L2:" << dataL2[iclu][i][ii] << " " << dataL2[iclu][NTIME][ii];
        *fileclust << " " << edatL2[iclu][i][ii] << " " << edatL2[iclu][NTIME][ii] << endl;
        *fileclust << "RA:" << rati20[iclu][i][ii] << " " << rati20[iclu][NTIME][ii];
        *fileclust << " " << erat20[iclu][i][ii] << " " << erat20[iclu][NTIME][ii] << endl;
      }
      fileclust->close();
      delete fileclust;
      */
//

      //graphs[i][0] = new TGraphErrors(nnpoints,dataL0[iclu][i],dataL0[iclu][NTIME],edatL0[iclu][i],edatL0[iclu][NTIME]);
      //graphs[i][1] = new TGraphErrors(nnpoints,dataL2[iclu][i],dataL2[iclu][NTIME],edatL2[iclu][i],edatL2[iclu][NTIME]);
      //graphs[i][2] = new TGraphErrors(nnpoints,rati20[iclu][i],rati20[iclu][NTIME],erat20[iclu][i],erat20[iclu][NTIME]);
      graphs[i][0] = new TGraphErrors(nnpoints,dataL0[iclu][NTIME],dataL0[iclu][i],edatL0[iclu][NTIME],edatL0[iclu][i]);
      graphs[i][1] = new TGraphErrors(nnpoints,dataL2[iclu][NTIME],dataL2[iclu][i],edatL2[iclu][NTIME],edatL2[iclu][i]);
      graphs[i][2] = new TGraphErrors(nnpoints,rati20[iclu][NTIME],rati20[iclu][i],erat20[iclu][NTIME],erat20[iclu][i]);
      // need to modify names here
      //SetAxisClass(graphs[i][1],i,ncd,1);
      //SetAxisClass(graphs[i][2],i,ncd,2);
      ncd++;
    }
  }
  //cout << " 0 NAME of cluster: " << endl;
  TCanvas *fCanvas = new TCanvas("fc",ClustNames[iclu],1200,300);
  cout << "Canvas classes created" << endl;
  int NLEV=3;
  fCanvas->Divide(ncd,NLEV,0.0025,0.0025,0);
  int ncdmax=ncd;
  ncd=1;
  for(int i=0;i<NITEMS;i++){
     if(graphs[i][0]){
       for(int k=0;k<NLEV;k++){
          fCanvas->cd(ncd+ncdmax*k);
          fCanvas->GetPad(ncd+ncdmax*k)->SetGridx(1);
          SetAxisClass(graphs[i][k],i,ncd,k);
          graphs[i][k]->Draw("ap");
       }
       ncd++;
    }
  }
  // Copy to MONSCAL for html display
  cout << "Canvas before update" << endl;
  fCanvas->Update();
  cout << "Canvas before save" << endl;
  fCanvas->SaveAs(ClustNames[iclu]);
  cout << "Canvas class saved" << endl;
  if(machine == '7'){
    stringstream ss ;
    ss << "scp "<<ClustNames[iclu]<<" trigger@alidcscom835:v/vme/WORK/MONSCAL/."; 
    int rc=system((ss.str()).c_str());
    cout  << ss.str() << " rc= " << rc << endl;
  }else{
    cout << ClustNames[iclu] << " not copied to 026" << endl;
  }
  for(int j=0;j<NLEV;j++){
     for(int i=0;i<NITEMS;i++)if(graphs[i][j]) delete graphs[i][j];
  }
  fCanvas->Clear();
  delete fCanvas;
}
void DisplaySCAL::DrawCanvasInps()
{
  cout << "Drawing canvas Inputs" << endl;
  if(!dataInp){
    cout << "Internal error, dataInp=0" << endl;
    return;
  }
  //
  TGraphErrors* graphsInps[NITEMS];
  for(int i=0;i<NITEMS;i++)graphsInps[i]=0;
  int ncd=0;
  for(int i=0;i<NITEMS;i++){
    if(i==NTIME)continue;
    if(dataInp[i]){
      double ex[nnpoints],ey[nnpoints];
      for(int j=0;j<nnpoints;j++){
        CheckIndex(5,j,npoints);
	ey[j]=0.;
	if(dataInp[i][j] != 0.)ex[j]=dataInp[i][j]/10.;
        else ex[j]=1.;
      }
      TGraphErrors *f1 = new TGraphErrors(nnpoints,dataInp[i],dataInp[NTIME],ex,ey);
      SetAxisInps(f1,i,ncd);
      graphsInps[i] = f1;
      ncd++;
    }
  }
  TCanvas *fCanvas = new TCanvas("fc","INPUTS",1200,300);
  cout << "Canvas created " << endl;
  fCanvas->Clear();
  // Divide(nx,ny,xmargin,ymargin,color),
  fCanvas->Divide(ncd,1,0.0025,0.0025,0);
  ncd=0;
  char tt[64];
  gettime(tt);
  TText* timelabel = new TText(.5,0.5,tt);
  timelabel->SetNDC();
  //timelabel->SetTextAngle(90);
  for(int i=0;i<NITEMS;i++){
    if(graphsInps[i]){
      ncd++;
      fCanvas->GetPad(ncd)->SetGridy(1);
      fCanvas->cd(ncd);
      graphsInps[i]->Draw("ap");
      //timelabel->Draw();
    }
  }
  cout << "Canvas drawn " <<  endl;
  fCanvas->Update();
  // Copy to MONSCAL for html display
  fCanvas->SaveAs("inputs.png");
  cout << "Canvas saved" << endl;
  if(machine == '7'){
    stringstream ss ;
    ss << "scp inputs.png trigger@alidcscom835:v/vme/WORK/MONSCAL/."; 
    //ss << "scp inputs.png trigger@alidcscom835:/local/trigger/v/vme/CNTRRD/htmls/pngs/."; 
    int rc=system((ss.str()).c_str());
    cout  << ss.str() << " rc= " << rc << endl;
  }else
  cout << "inputs.png not copied to server" << endl;
  for(int i=0;i<NITEMS;i++)if(graphsInps[i]) delete graphsInps[i];
  delete timelabel;
  delete fCanvas;
}
void DisplaySCAL::CreateClassesofClust(int iclu)
{
  CheckIndex(6,iclu,NCLUST);
  sprintf(ClustNames[iclu],"%i_%s.png",runnum,clusts[iclu]->GetNamechar());
  dataL0[iclu] = new double* [NITEMS];
  edatL0[iclu] = new double* [NITEMS];
  dataL2[iclu] = new double* [NITEMS];
  edatL2[iclu] = new double* [NITEMS];
  rati20[iclu] = new double* [NITEMS];
  erat20[iclu] = new double* [NITEMS];
  for(int i=0;i<NITEMS;i++){
    dataL0[iclu][i]=0;
    edatL0[iclu][i]=0;
    dataL2[iclu][i]=0;
    edatL2[iclu][i]=0;
    rati20[iclu][i]=0;
    erat20[iclu][i]=0;
  }
  // Create time
  dataL0[iclu][NTIME]=new double [npoints];
  edatL0[iclu][NTIME]=new double [npoints];
  dataL2[iclu][NTIME]=new double [npoints];
  edatL2[iclu][NTIME]=new double [npoints];
  rati20[iclu][NTIME]=new double [npoints];
  erat20[iclu][NTIME]=new double [npoints];
  // Zero time  
  for(int i=0;i<npoints;i++){
     dataL0[iclu][NTIME][i]=0.;
     edatL0[iclu][NTIME][i]=0.;
     dataL2[iclu][NTIME][i]=0.;
     edatL2[iclu][NTIME][i]=0.;
     rati20[iclu][NTIME][i]=0.;
     erat20[iclu][NTIME][i]=0.;
  }
  // Create class clusters
  TriggerClasswCount* tcl=0;
  for(int j=0;j<clusts[iclu]->GetNumofClasses();j++){
     //cout << "Creating clust " << iclu << " class " << j ;
     if((tcl = clusts[iclu]->GetTriggerClass(j))){
       int pos = tcl->GetIndex()-1;
       CheckIndex(7,pos,NITEMS);
       SetItemName(tcl->GetShortName(),pos);
       dataL0[iclu][pos] = new double[npoints];
       edatL0[iclu][pos] = new double[npoints];
       dataL2[iclu][pos] = new double[npoints];
       edatL2[iclu][pos] = new double[npoints];
       rati20[iclu][pos] = new double[npoints];
       erat20[iclu][pos] = new double[npoints];
       // Zeroing here if necessaey
     }
  }
}
void DisplaySCAL::DisplayRun()
{
  //cout << "Debug " << runnum << " show=" << show << endl;
  if(!show) return;
  if(runnum==0){
    DisplayInputs();
    return ;
  }
  //cout << "Debug  2 " << runnum << " show=" << show << endl;
  // Normal run
  TriggerClasswCount* tcl=0;
  tcl=clusts[0]->GetTriggerClass(0);
  //cout << "Debug  2 " << runnum << " show=" << show << " time " <<tcl-> GetCounters()[0].GetTimeSec()  <<endl;
  //cout << "DisplayRun  ";
  //tcl->Print();
  if(tcl->GetCounters()[0].GetTimeSec() < 1.1) return;  // skip 1 sec window, later with dim
  int tt;
  gettimeI(&tt);
  //cout << "tt= " << tt << endl;
  //cout << "Debug  3 " << runnum << " show=" << show << endl;
  CheckIndex(8,cpoints,npoints);
  for(int i=0;i<nclust;i++){ // loop over clusters
     // Classes
     for(int j=0;j<clusts[i]->GetNumofClasses();j++){
        if((tcl = clusts[i]->GetTriggerClass(j))){
          int pos = tcl->GetIndex()-1;
          CheckIndex(9,pos,NITEMS);
          dataL0[i][NTIME][cpoints]=tt;
          dataL2[i][NTIME][cpoints]=tt;
          rati20[i][NTIME][cpoints]=tt;
          double rateL0= (tcl->GetCounters()[0]).GetRate();
          double rateL2= (tcl->GetCounters()[5]).GetRate();
          dataL0[i][pos][cpoints]= rateL0;
          edatL0[i][pos][cpoints]= TMath::Sqrt(rateL0);
          dataL2[i][pos][cpoints]= rateL2;
          edatL2[i][pos][cpoints]= TMath::Sqrt(rateL2);
          if(rateL0>0.)rati20[i][pos][cpoints]= rateL2/rateL0;
          else rati20[i][pos][cpoints]=0.;

          //error = sqrt(l2-l2*l2/l0)/l0;
          if(rateL2>0. && rateL0>0.){
            double v=rateL2*(1.-rateL2/rateL0);
            if(v>0.)erat20[i][pos][cpoints]= TMath::Sqrt(v)/rateL0;
            else erat20[i][pos][cpoints]= 0.;
          }else  erat20[i][pos][cpoints]= 0.;
        }
     }
     DrawCanvasClasses(i);
  }
  nnpoints++;
  if(nnpoints>npoints)nnpoints=npoints;
  cpoints = (cpoints+1) % nnpoints;
  return ;
}
void DisplaySCAL::ClearData()
{
 cout << "deleting inputs " << NITEMS << endl;
 for(int i=0;i<NITEMS;i++){
  if(dataInp[i] && runnum){
    cout << "Error: dataInp nonzero, not deleting" << endl;
  } else {
  if(dataInp[i]){
    cout << i << " " << dataInp[i] << endl;
    delete dataInp[i];
    dataInp[i]=0;
  }
  }
 }
 for(int i=0;i<nclust;i++){
  if(dataL0[i]){
   cout << "deleting dataL0 "<< i << endl;
   for(int j=0;j<NITEMS;j++)if(dataL0[i][j])delete dataL0[i][j];
   delete dataL0[i];
  }
  if(edatL0[i]){
   cout << "deleting edataL0 "<< i << endl;
   for(int j=0;j<NITEMS;j++)if(edatL0[i][j])delete edatL0[i][j];
   delete edatL0[i];
  }
  if(dataL2[i]){
   cout << "deleting dataL2 "<< i << endl;
   for(int j=0;j<NITEMS;j++)if(dataL2[i][j])delete dataL2[i][j];
   delete dataL2[i];
  }
  if(edatL2[i]){
   cout << "deleting edataL2 "<< i << endl;
   for(int j=0;j<NITEMS;j++)if(edatL2[i][j])delete edatL2[i][j];
   delete edatL2[i];
  }
  if(rati20[i]){
   cout << "deleting rati20 "<< i << endl;
   for(int j=0;j<NITEMS;j++)if(rati20[i][j])delete rati20[i][j];
   delete rati20[i];
  }
  if(erat20[i]){
   cout << "deleting erati20 "<< i << endl;
   for(int j=0;j<NITEMS;j++)if(erat20[i][j])delete erat20[i][j];
   delete erat20[i];
  }
 }
 cpoints=0;
 nnpoints=0;
 //inter=0;
 inps=0; 
 //clusts=0;
}
