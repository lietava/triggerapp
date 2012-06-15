#include "Output.h"
#include "ActiveRun.h"
#include <iostream>
#include <sstream>
#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TText.h"
#include "TTimer.h"
#include "Dim.h"

ClassImp(DisplaySCAL);

//======================================================================
extern char machine;
ofstream* DisplaySCAL::fileSCAL=0;
DisplaySCAL::DisplaySCAL(ActiveRun& ar)
:
Log(),
npoints(20),
cpoints(0),
nnpoints(0)
{
 runnum = ar.GetRunNumber();
 cout << "Starting DisplaySCAL for run: " << runnum << endl;
 ar.Print();
 cout << "run " << &ar << endl;
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
 for(int i=0;i<NCLUST;i++){
  dataL0[i]=0;
  dataL2[i]=0;
  strcpy(ClustNames[i],"");
 }
 for(int i=0;i<NITEMS;i++){
   dataInp[i] = 0;
   itemnames[i] = 0;
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
 ClearData();
 for(int i=0;i<nclust;i++){
  stringstream ss;
  ss  << "rm " << ClustNames[i] << endl;
  cout << "Deleting "<< ClustNames[i] << " " << system(ss.str().c_str()) << endl;
 }
 cout << "Stopping DisplaySCAL for run: " << runnum << endl;
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
    dataInp[pos] = new double[npoints];
    SetItemName((inps[i]->GetCounter()->GetName())->c_str(),pos); 
    //cout << "pos " << pos << " created \n";
 }
 if(inter){
   dataInp[NINT1] = new double[npoints];
   dataInp[NINT2] = new double[npoints];
   SetItemName("INT1",NINT1); 
   SetItemName("INT2",NINT2); 
 }
}
void DisplaySCAL::DisplayInputs()
{
 // Dealing with time
 //if(inps[0]->GetCounter()->GetTimeSec() < 9.) return;  // skip 61 sec window, later with dim
 //dataInp[NITEMS][cpoints]=inps[0]->GetCounter()->GetTimeTotSec(); time from counters
 int tt;
 gettimeI(&tt);
 cout << "tt= " << tt << endl;
 dataInp[NTIME][cpoints]=tt;
 // Dealing with interactions
 if(inter){
  dataInp[NINT1][cpoints] = inter->GetRate1();
  dataInp[NINT2][cpoints] = inter->GetRate2();
 }
 // Dealing with inputs
 for(int i=0;i<ninp;i++){
    int pos=inps[i]->GetPosition()-1;
    dataInp[pos][cpoints]=inps[i]->GetCounter()->GetRate();
 }
 cpoints = (cpoints+1) % npoints;
 DrawCanvasInps();
}
void DisplaySCAL::SetGlobals()
{
  gROOT->SetStyle("Plain");
  gStyle->SetLabelSize(0.08,"x");
  gStyle->SetTitleFontSize(0.2);
  gStyle->SetTitleSize(0.2);
  gStyle->SetTitleH(0.12);
  gStyle->SetTitleW(0.5);
  gStyle->SetTitleX(0.25);
  gStyle->SetTitleY(1.020);
  gStyle->SetLabelSize(0.075,"y");
  gStyle->SetPadLeftMargin(0.2);
  gStyle->SetPadRightMargin(0.15);
  gStyle->SetLabelOffset(0.025,"Y");
  //gStyle->SetStripDecimals(kFALSE); 
}
void DisplaySCAL::SetAxis(TGraphErrors* f1,int i, int ncd)
{
 f1->GetXaxis()->SetNdivisions(401,kTRUE);
 f1->GetYaxis()->SetNdivisions(-410,kFALSE);
 f1->GetXaxis()->SetNoExponent();
 //f1->GetYaxis()->LabelsOption("v"); // doesn't work as labels "don't exist" for the graph y axis, only values
 f1->GetYaxis()->SetTimeDisplay(1);
 f1->GetYaxis()->SetTimeFormat("%H\:%M");
 f1->SetMarkerColor(1+(ncd%4));
 f1->SetMarkerStyle(20+(ncd%4));
 f1->SetTitle(itemnames[i]);
 //f1->GetXaxis()->SetDecimals(0);
 if(i==0){
  //f1->GetYaxis()->SetLabelSize(0.12);
  //f1->GetYaxis()->SetLabelOffset(1);
  f1->GetYaxis()->SetTitle("Time(min)");
  f1->GetYaxis()->SetTitleSize(0.1);
  f1->GetYaxis()->SetTitleOffset(-4.75);
}
}
//void DisplaySCAL::DrawCanvasClasses(int iclu){};
void DisplaySCAL::DrawCanvasClasses()
{
  int iclu=activeclust;
  cout << "Drawing canvas classes" << endl;
  if(!dataL0){
    cout << "Internal error, dataL0=0" << endl;
    return;
  }
  //
  if(++nnpoints>npoints)nnpoints=npoints;
  double ex[nnpoints],ey[nnpoints];
  TGraphErrors* graphsL0[NITEMS];
  for(int i=0;i<NITEMS;i++)graphsL0[i]=0;
  int ncd=0; 
  for(int i=0;i<NITEMS;i++){
    if(i==NTIME)continue;
    if(dataL0[iclu][i]){
      for(int j=0;j<nnpoints;j++){
	ey[j]=0.;
	ex[j]=dataL0[iclu][i][j]/10.;
      }
      cout << "New " << iclu << " " << ClustNames[iclu] << " item= "  << i << endl;
      TGraphErrors *f1 = new TGraphErrors(nnpoints,dataL0[iclu][i],dataL0[iclu][NTIME],ex,ey);
      SetAxis(f1,i,ncd);
      graphsL0[i] = f1;
      ncd++;
    }
  }
  cout << " 0 NAME of cluster: " << endl;
  cout << ClustNames[iclu] << endl;
  TCanvas *fCanvas = new TCanvas("fc",ClustNames[iclu],1200,300);
  fCanvas->Divide(ncd,1,0.0025,0.0025,0);
  fCanvas->SetGridy(1);
  ncd=1;
  for(int i=0;i<NITEMS;i++){
    if(graphsL0[i]){
      fCanvas->cd(ncd);
      graphsL0[i]->Draw("ap");
      ncd++;
    }
  }
  // Copy to MONSCAL for html display
  cout << " NAME of cluster: " << endl;
  cout << ClustNames[iclu] << endl;
  fCanvas->Update();
  fCanvas->SaveAs(ClustNames[iclu]);
  if(machine == '7'){
    stringstream ss ;
    ss << "scp "<<ClustNames[iclu]<<" trigger@alidcscom026:v/vme/WORK/MONSCAL/."; 
    int rc=system((ss.str()).c_str());
    cout  << ss.str() << " rc= " << rc << endl;
  }else
  cout << ClustNames[iclu] << " not copied to 026" << endl;
  for(int i=0;i<NITEMS;i++)if(graphsL0[i]) delete graphsL0[i];
  fCanvas->Clear();
  delete fCanvas;
}
void DisplaySCAL::DrawCanvasInps()
{
  cout << "Drawing canvas" << endl;
  if(!dataInp){
    cout << "Internal error, dataInp=0" << endl;
    return;
  }
  //
  if(++nnpoints>npoints)nnpoints=npoints;
  TGraphErrors* graphsInps[NITEMS];
  for(int i=0;i<NITEMS;i++)graphsInps[i]=0;
  int ncd=0;
  for(int i=0;i<NITEMS;i++){
    if(i==NTIME)continue;
    if(dataInp[i]){
      double ex[nnpoints],ey[nnpoints];
      for(int j=0;j<nnpoints;j++){
	ey[j]=0.;
	ex[j]=dataInp[i][j]/10.;
      }
      TGraphErrors *f1 = new TGraphErrors(nnpoints,dataInp[i],dataInp[NTIME],ex,ey);
      SetAxis(f1,i,ncd);
      graphsInps[i] = f1;
      ncd++;
    }
  }
  TCanvas *fCanvas = new TCanvas("fc","INPUTS",1200,300);
  fCanvas->Clear();
  // Divide(nx,ny,xmargin,ymargin,color),
  fCanvas->Divide(ncd,1,0.0025,0.0025,0);
  for(int i=0; i<ncd; i++){
    fCanvas->GetPad(i+1)->SetGridy(1);
  }
  fCanvas->SetGridy(1);
  ncd=0;
  TText* timelabel = new TText(.050,0.5,"Time(s)");
  timelabel->SetNDC();
  timelabel->SetTextAngle(90);
  for(int i=0;i<NITEMS;i++){
    if(graphsInps[i]){
      ncd++;
      fCanvas->cd(ncd);
      //graphsInps[i]->Draw("alp");
      graphsInps[i]->Draw("ap");
    }
  }
  fCanvas->Update();
  // Copy to MONSCAL for html display
  fCanvas->SaveAs("inputs.png");
  if(machine == '7'){
    stringstream ss ;
    //ss << "scp inputs.png trigger@alidcscom026:v/vme/WORK/MONSCAL/."; 
    int rc=system((ss.str()).c_str());
    cout  << ss.str() << " rc= " << rc << endl;
  }else
  cout << "inputs.png not copied to 026" << endl;
  for(int i=0;i<NITEMS;i++)if(graphsInps[i]) delete graphsInps[i];
  delete timelabel;
  delete fCanvas;
}
void DisplaySCAL::CreateClassesofClust(int iclu)
{
  sprintf(ClustNames[iclu],"%i_%s.png",runnum,clusts[iclu]->GetNamechar());
  dataL0[iclu] = new double* [NITEMS];
  dataL2[iclu] = new double* [NITEMS];
  for(int i=0;i<NITEMS;i++){
    dataL0[iclu][i]=0;
    dataL2[iclu][i]=0;
  }
  dataL0[iclu][NTIME]=new double [npoints];
  dataL2[iclu][NTIME]=new double [npoints];
  TriggerClasswCount* tcl=0;
  for(int j=0;j<clusts[iclu]->GetNumofClasses();j++){
     cout << "Creating clust " << iclu << " class " << j ;
     if(tcl = clusts[iclu]->GetTriggerClass(j)){
       int pos = tcl->GetIndex()-1;
       cout << " pos = " << pos << endl;
       dataL0[iclu][pos] = new double[npoints];
       dataL2[iclu][pos] = new double[npoints];
       SetItemName(tcl->GetShortName(),pos);
     }
  }
}
void DisplaySCAL::DisplayRun()
{
  if(runnum==0){
    DisplayInputs();
    return ;
  }
  // Normal run
  TriggerClasswCount* tcl=0;
  tcl=clusts[0]->GetTriggerClass(0);
  //cout << "DisplayRun  ";
  //tcl->Print();
  //if(tcl->GetCounters()[0].GetTimeSec() < 3.) return;  // skip 1 sec window, later with dim
  int tt;
  gettimeI(&tt);
  cout << "tt= " << tt << endl;
  for(int i=0;i<nclust;i++){ // loop over clusters
     // Classes
     for(int j=0;j<clusts[i]->GetNumofClasses();j++){
        if(tcl = clusts[i]->GetTriggerClass(j)){
          int pos = tcl->GetIndex()-1;
          //cout << "npoints="<< npoints <<" cpoints= " << cpoints;
          //cout << " pos= " << pos << " i= "<< i << " j=" << j << endl;
          dataL0[i][NTIME][cpoints]=tt;
          dataL2[i][NTIME][cpoints]=tt;
          dataL0[i][pos][cpoints]= (tcl->GetCounters()[0]).GetRate();
          dataL2[i][pos][cpoints]= (tcl->GetCounters()[5]).GetRate();
        }
     }
     //DrawCanvasClasses(i);
     activeclust=i;
     TTimer::SingleShot(10, "DisplaySCAL", this, "DrawCanvasClasses()");
  }
  cpoints = (cpoints+1) % npoints;
  return ;
}
void DisplaySCAL::ClearData()
{
 for(int i=0;i<NITEMS;i++){
  if(dataInp[i]){
    delete dataInp[i];
    dataInp[i]=0;
  }
 }
 for(int i=0;i<nclust;i++){
  if(dataL0[i]){
   for(int j=0;j<NITEMS;j++)if(dataL0[i][j])delete dataL0[i][j];
   delete dataL0[i];
  }
  if(dataL2[i]){
   for(int j=0;j<NITEMS;j++)if(dataL2[i][j])delete dataL2[i][j];
   delete dataL2[i];
  }
 }
 cpoints=0;
 nnpoints=0;
 //inter=0;
 inps=0; 
 //clusts=0;
}
