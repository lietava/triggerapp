#include "extract.h"
#include <climits> 
#include <sstream>
#include "TH2F.h"
#include "TObjArray.h"
#include "TCanvas.h"
#include "TDecompSVD.h"
#include "TMath.h"
#define Mega 1024*1024
typedef unsigned int w32;
#define ORBIT 3564
//-----------------------------------------------------------------------
void splitstring(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ")
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}
//-------------------------------------------------------
Hists::Hists(){
//  hCorCoef = new TH1F("hCorCoef", "Multi Correlation coeficients",10000,0.,1.);
  hCorCoef = new TH2F("hCorCoef", "Multi Correlation coeficients",10000,0.,1.,30,0.,30.);
}
Hists::~Hists(){
  delete hCorCoef;
}
int Hists::addHist(string const &name,int delta,int cordist){
 int nbins=2*delta+1;
 float x0=cordist-delta-0.5;
 float xm=cordist+delta+0.5;
 TH1F *h = new TH1F(name.c_str(),name.c_str(),nbins,x0,xm);
 hists.Add(h);
 return 0;
}
int Hists::addHist2(string const &name,int nbins,float x0,float xmax){
 TH1F *h = new TH1F(name.c_str(),name.c_str(),nbins,x0,xmax);
 hists2.Add(h);
 return 0;
}
int Hists::addHist3(string const &name,int delta){
 int nbins=delta;
 float x0=0;
 float xm=delta+0.5;
 TH1F *h = new TH1F(name.c_str(),name.c_str(),nbins,x0,xm);
 hists3.Add(h);
 return 0;
}
int Hists::fillHist(int i,int bin,float x){
 TH1F *h = (TH1F*) hists[i];
 //cout << "i= "<< i << ":" << h->GetName() << endl;
 h->SetBinContent(bin,x);
 //h->AddBinContent(bin,x);
 return 0;
}
int Hists::fillHist2(int i,int bin,float x){
 TH1F *h = (TH1F*) hists2[i];
 //cout << "i= "<< i << ":" << h->GetName() << endl;
 h->SetBinContent(bin,x);
 return 0;
}
int Hists::fillHist3(int i,int bin,float x){
 TH1F *h = (TH1F*) hists3[i];
 //cout << "i= "<< i << ":" << h->GetName() << endl;
 h->SetBinContent(bin,x);
 return 0;
}
int Hists::writeHists(){
  TFile *file;
  file = new TFile("pdf/Histos.root","RECREATE","FILE");
  hists.Write();
  hists2.Write();
  hists3.Write();
  hCorCoef->Write();
  file->Close();
 return 0;
}
void Hists::pdfAllHists(){
 int n=hists.GetEntriesFast();
 cout << "pdfAllHists: # of hists: " << n << endl;
 TCanvas c1("c1","c1",900,20,540,550);
 for(int i=0;i<n;i++){
   hists[i]->Draw(); 
   cout << (hists[i])->GetName() << " saved" << endl;
   string ext(".pdf");
   string pdf("pdf/");
   string file(pdf+hists[i]->GetName()+ext);
   c1.SaveAs(file.c_str());
 }
}
void Hists::pdfAllHists2(){
 int n=hists2.GetEntriesFast();
 cout << "pdfiAllHists2: # of hists: " << n << endl;
 TCanvas c1("c1","c1",900,20,540,550);
 for(int i=0;i<n;i++){
   hists2[i]->Draw(); 
   cout << (hists2[i])->GetName() << " saved" << endl;
   string ext(".pdf");
   string pdf("pdf/");
   string file(pdf+hists2[i]->GetName()+ext);
   c1.SaveAs(file.c_str());
 }
}
//=======================================================================================ssmpoint
void ssmpoint::Print(){
  cout << "ssm="<< issm << " ch=" << chanabs<<" ch2inp=" << chanrel << " pos=" << position << endl;
}
//=======================================================================================Input
Input::Input(string &name,int delay,int index,int level)
:
name(name),delay(delay),index(index),level(level),counts(1),used(-1)
{
 channel=index+7;  // for L0
}
void Input::Print()
{
 cout << name << " level: " << level << " index: " << index<< " channel: "<< channel << " used: " << used << endl;
}
//========================================================================================extractData
//---------------------------------------------------
extractData::extractData(char* config):
cordist_d(0),delta_d(0),nfiles(0),nssms(0),Nchans(60)
{
 //FileList_d=FileList;
 //DataDir_d=DataDir;
 openRepFile();
 if(ReadConfig(config)) exit(1);
 chan2inp = new int[Nchans];  // this has be modified for l1,l2 boards
 inp2chan = new int[Nchans];
 chans = new int[Nchans];
 chansd = new int[Nchans];
 for(int i=0;i<Nchans;i++){
    chan2inp[i] = -1;
    inp2chan[i] = -1;
    chans[i]=0;
    chansd[i]=0;
 }
}
extractData::~extractData()
{
 //correlations = new int*[ncorrelations];
 //cororbit     = new int*[ncorrelations];
 for (int i=0;i<ncorrelations;i++){
   //correlations[i]=new int[2*delta+1];
   delete [] correlations[i];
   //cororbit[i] = new int[ORBIT];
 }
 delete [] correlations; 
// repFile.close();
}
//-----------------------------------------------------------
void extractData::openRepFile()
{
  repFile.open("report.txt",ios_base::app);

}
//------------------------------------------------------
int extractData::ReadConfig(char* config)
{
 ifstream in;
 //string config("config.txt");
 char file[256];
 sprintf(file,"%s.txt",config);
 in.open(file);
  if(!in.is_open()){
   cout << "ReadConfig: File not found: "<< config << endl;
   return 1;
  }
  int nlines=0;
  string line;
  while (getline(in,line)) {
      //cout << nlines << " " << line << endl;
      if(line[0]=='#') continue;
      if(nlines==0){
       VALIDCTPINPUTS_d=line;
       if(ParseVALIDCTPINPUTS()) return 1;
       nlines++;
      }else if(nlines==1){
       DataDir_d=line;
       nlines++;
      } else if(nlines==2){
       FileList_d=line;
       nlines++;
      }else if(nlines==3){
       vector<string> items;
       splitstring(line,items," ");
       unsigned int nit=items.size();
       if(nit == 0){
         cout << "Error in config file, 0 inputs selected." << endl;
         return 1;
       }
       if(items[0].find("ALL") != string::npos){
         cout << "ALL inputs are to be used." << endl;
         unsigned int j=0;
         ninputs=0;
         while(j < inputs.size()){
           //cout << inputs.at(j).name << " " << inputs.at(j).level << endl;
           bool nadd=0; 
           nadd += inputs.at(j).name.find("0BP") != string::npos;
           nadd += inputs.at(j).name.find("0LSR") != string::npos;
           if(!nadd){
             if((inputs.at(j)).level==0)inputs.at(j).used=ninputs++;
  	   }
           j++;
         }
       }else{
         ninputs=0;
         for(unsigned int i=0;i<nit;i++){
           unsigned int j=0;
           while(j < inputs.size() && (inputs.at(j)).name.find(items[i]) == string::npos)j++;
           if(j==inputs.size()){
            cout << "Input " << items[i] << " not found in VALID.CTPINPUTS" << endl;
            return 1;
           }
           //cout << "Choosing input: " << items[i] << endl;
           inputs.at(j).used=ninputs++;
        }
       }
       nlines++;
      }else if(nlines==4){
        SetSkipNext(atoi(line.c_str()));  // 0 for autocorrelations
        nlines++;
      }else if(nlines==5){
        SetNfiles(atoi(line.c_str()));
        cout << "# of files: " << nfiles << endl;
        nlines++;
      }else if(nlines==6){
         // correlation distance and center
         vector<string> items;
         splitstring(line,items," ");
         if(items.size() != 2){
          cout << "confif.txt: wrong syntax in line:" << endl;
          cout << line << endl;
          return 1;
         }
         ccordist_d=atoi(items[0].c_str());
         cdelta_d=atoi(items[1].c_str());
         cout << "config.txt: FROM SIZE dist delta" << ccordist_d << " " << cdelta_d << endl;
         nlines++;
      }else if(nlines==7){
         // chose input to correlate to
         input2correlate_d=line;
         unsigned int j=0;
         while(j < inputs.size() && (inputs.at(j)).name.find(line) == string::npos)j++;
         cout << "j=" << j << endl;
         if(j==inputs.size() || inputs.at(j).used<0){
           cout << "Chosen input " << line << " not found." << endl;
           return 1; 
         }
         cout << "Chosen input: " << line << endl;
      }else{
       cout << "config.txt: Unexpected number of lines." << endl;
       return 1;
      }
  }
  return 0;
}
//------------------------------------------------------
int extractData::ParseVALIDCTPINPUTS()
{
  ifstream in;
  in.open(VALIDCTPINPUTS_d.c_str());
  if(!in.is_open()){
   cout << "ParseVALIDCTPINPUTS: File not found: "<< VALIDCTPINPUTS_d   << endl;
   return 1;
  }
  string line;
  int nlines=0;
  while (getline(in,line)) {
      //cout << line << endl;
      if(line[0]=='#') continue;  
      if(line[0]=='l') continue;
      vector<string> items;
      splitstring(line,items," =");
      //cout << items[0] << " " << items[4] << " " << items[8] << endl;
      //cout << items.size() << endl;
      if(items.size() < 11){
       cout << "Wrong syntax in " << VALIDCTPINPUTS_d << endl;
       cout << line << endl;
       return 1;
      }
      int index=atoi(items[4].c_str());
      int level=atoi(items[2].c_str()); 
      vector<Input>::iterator i= inputs.begin();
      while(i != inputs.end() && (i->index<index))i++;
      inputs.insert(i,Input(items[0],atoi(items[8].c_str()),index,level));
      nlines++;
   }
   in.close();
 return 0;
}
//------------------------------------------------------
void extractData::chooseInputs(){
 for(int i=0;i<Nchans;i++){
    chan2inp[i] = -1;
    inp2chan[i] = -1;
 }
 inputNames.clear();
 //
 unsigned int inppos[Nchans]; // intermediate variable
 cout << "Adding inputs:" << endl;
 for(unsigned int i=0;i<inputs.size();i++){
  if(inputs.at(i).used >= 0 ){
   int index=inputs.at(i).index;
   chan2inp[index+7]=inputs.at(i).used;
   inp2chan[inputs.at(i).used]=index+7;
   inppos[inputs.at(i).used] = i;
   //ninputs++;
   //cout << "Adding input: " << inputs.at(i).name << " index=" << inputs.at(i).index << " i=" << i << endl;
   inputs.at(i).Print();
  }
 }
 cout << "ninputs=" << ninputs <<  endl;
 for(int i=0;i<Nchans;i++){
  if(inp2chan[i] != -1){
    inputNames.push_back(inputs.at(inppos[i]).name);
    }
 }
 /*
 for(int i=0;i<Nchans;i++){
 if(chan2inp[i] != -1){
    cout << "inp= "<< i-7 << " chan= " << i << " " << inputNames[chan2inp[i]] << endl;
   }
 }
 */
}
//------------------------------------------------------------
int extractData::extractBPX(int issm){
 ifstream in;
 string file(DataDir_d+ssmdumps[issm]); // can be parameter
 in.open(file.c_str(),ios::in | ios::binary);
 if(!in.is_open()){
    cout << "extract1SSMfilt1: File not found: "<<  file  << endl;
    return 1;
 }
 cout << file << " OK" << endl;
 w32 ssm[Mega];
 in.read((char*)ssm,sizeof(ssm));
 in.close();
 for (int i=0;i<Mega;i++){
  if(!ssm[i]) continue;
  // najdi bpx not finished
  if(ssm[i] & (1<<chanbpa)){
      ssmpoint p(issm,chanbpa,chanbpa-7,i);
      //p.Print();
      data.push_back(p);
  }
 }
 return 0;
}
//------------------------------------------------------------
int extractData::extract1SSMfilt1(int issm){
// del : correlation only to the edge del[i]>0
//       del[i]=0 no limit
 ifstream in;
 string file(DataDir_d+ssmdumps[issm]);
 in.open(file.c_str(),ios::in | ios::binary);
 if(!in.is_open()){
    cout << "extract1SSMfilt1: File not found: "<<  file  << endl;
    return 1;
 }
 cout << file << " OK" << endl;
 int del[Nchans];
 for(int i=0;i<Nchans;i++)del[i]=0;
 int delorbit=0;
 w32 ssm[Mega];
 in.read((char*)ssm,sizeof(ssm));
 in.close();
 for (int i=0;i<Mega;i++){
  if(!ssm[i]) continue;
  for(int j=0;j<Nchans;j++){
   if(j==0){
    if((delorbit--)<=0 && (ssm[i] & 1)){
      ssmpoint o(issm,j,0,i);
      //p.Print();
      orbits.push_back(o);
      delorbit=41;
    }
   }
   if(chan2inp[j] != -1){
    if((del[j]--)<=0 && (ssm[i] & (1<<j))){
      ssmpoint p(issm,j,chan2inp[j],i);
      //p.Print();
      data.push_back(p);
      del[j]=SkipNext;
      chans[j]++;
      //if(j==10)cout << "j==10 " << file  << file << endl;
    }
   }
  }
 }
 return 0;
}
//------------------------------------------------------------
int extractData::extract1SSM(int issm){
 ifstream in;
 string file(DataDir_d+ssmdumps[issm]);
 in.open(file.c_str(),ios::in | ios::binary);
 if(!in.is_open()){
    cout << "extract1SSM: File not found: "<<  ssmdumps[issm]  << endl;
    return 1;
 }
 cout << file << " OK" << endl;
 w32 ssm[Mega];
 in.read((char*)ssm,sizeof(ssm));
 in.close();
 for (int i=0;i<Mega;i++){
  if(!ssm[i]) continue;
  for(int j=0;j<Nchans;j++){
   if(chan2inp[j] != -1){
    if(ssm[i] & (1<<j)){
      ssmpoint p(issm,j,chan2inp[j],i);
      //p.Print();
      data.push_back(p);
    }
   }
  }
 }
 return 0;
}
//-----------------------------------------------------------
int extractData::readFileList(){
// Read the list of ssm files
  ifstream in;
  in.open(FileList_d.c_str());
  if(!in.is_open()){
   cout << "readFileList: File not found: "<<  FileList_d  << endl;
   return 1;
  }
  cout << "readFileList: nfiles= " << nfiles << endl;
  nlines_d=0;
  while (1) {
      string line;
      in >> line;
      if (!in.good()) break;
      //cout << line << endl;
      if(line[0]=='#') continue;
      bool dump = (line.find(".dmp") == string::npos);
      dump *= (line.find(".dump") == string::npos);
      if(dump) continue;
      filelist.push_back(line); 
      nlines_d++;
      if(nfiles && nlines_d>=nfiles)break;
   }
   in.close();
   cout << " # found  lines " << nlines_d << endl;
   printFileList();
   cout << " # files read ok: " << nlines_d << endl;
   return 0;
}
//--------------------------------------------------------------
void extractData::extractAllSSM(){
 //for(int i=0;i<nssms;i++)extract1SSM(i);
 for(int i=0;i<nssms;i++){
    extract1SSMfilt1(i);
    for(int i=0;i<ninputs;i++){
       cout << " inp= " << inputNames[i] << " #signals: " << chans[inp2chan[i]];
    }
    cout << endl;
 }
 /*
 Attempt to remove empty inputs - not finished
 for(unsigned int i=0;i<inputs.size();i++){
   if(inputs.at(i).used > 0){
     if(chans[inputs.at(i).GetChannel()]==0)inputs.at(i).used=-1;
   }
 }
 */
}
//---------------------------------------------------------------
void extractData::checkAllSSM(){
 cout << "CheckALL SSM starts:" << endl;
 for(int j=0;j<Nchans;j++)chans[j]=0;
 for(int j=0;j<nlines_d;j++)checkInputs(j);
 for(int j=0;j<Nchans;j++){
  if(chans[j])
  {
    if(chan2inp[j] != -1){
     cout << "Channel: "<< j << " " <<inputNames[chan2inp[j]] << " "<< chans[j] << endl;
    }
    else cout << "Channel: "<< j << "    " << chans[j] << endl;
  }
 }
 cout << "CheckALL SSM finished." << endl;
}
//---------------------------------------------------------------
void extractData::check1SSM(int issm){
 for(int j=0;j<Nchans;j++)chans[j]=0;
 checkInputs(issm);
 for(int j=0;j<Nchans;j++){
  if((chan2inp[j] != -1) && chans[j]>0){
    if(j)cout << j-7 << " " << chans[j] << " " << chansd[0]<<endl;
    else cout << "ORBIT  " << chans[0] << " " << chansd[0] << endl;
  }
 }
}
//-------------------------------------------------------------------
void extractData::checkAllSSM1by1(){
 for(int i=0;i<nlines_d;i++)check1SSM(i);
}
//-------------------------------------------------------------------
int extractData::removeEmptySSMs(bool remove){
 int ret;
 nssms=0;
 if(!remove){
  for(int i=0;i<nlines_d;i++){
    ssmdumps.push_back(filelist[i]); 
    nssms++; 
  }
  return 0;
 }
 for(int i=0;i<nlines_d;i++){
    if(!(ret=checkSSMforData(i))){
      ssmdumps.push_back(filelist[i]); 
      nssms++; 
    }else{
     if(ret==2) return 1;
    }
 }
 cout << "# of dumps after removing empty: " << nssms << endl;
 return 0;
}
//-------------------------------------------------------------------
int extractData::checkSSMforData(int issm){
 ifstream in;
 string file(DataDir_d+filelist[issm]);
 in.open(file.c_str(),ios::in | ios::binary);
 if(!in.is_open()){
    cout << "checkSSMforData: File not found: "<<  file.c_str()  << endl;
    return 2;
 }
 cout << file << " OK" << endl;
 w32 ssm[Mega];
 in.read((char*)ssm,sizeof(ssm));
 in.close();
 for(int i=0;i<Mega;i++){
  int nsig=0;
  for(int j=0;j<Nchans;j++){
   if(chan2inp[j] != -1){
     if(ssm[i]&(1<<j)){
       nsig++;
       if(nsig>0){
         cout << file << " first non zero at " << i << " " << j << endl;
         return 0;
       }
     }
   }
  }
 }
 cout << file << " removed" << endl;
 return 1; 
}
//-------------------------------------------------------------------
// Checking input by opening file
int extractData::checkInputs(int issm){
 ifstream in;
 string file(DataDir_d+filelist[issm]);
 in.open(file.c_str(),ios::in | ios::binary);
 if(!in.is_open()){
    cout << "checkInputs: File not found: "<<  ssmdumps[issm]  << endl;
    return 1;
 }
 int last[Nchans];
 for(int i=0;i<Nchans;i++){last[i]=0;chansd[i]=0;} // set variable later
 last[24]=Mega;
 cout << file << " ----------------------------------OK" << endl;
 w32 ssm[Mega];
 in.read((char*)ssm,sizeof(ssm));
 in.close();
 for (int i=0;i<Mega;i++){
  if(!ssm[i]) continue;
  for(int j=8;j<Nchans;j++){
   if(ssm[i] & (1<<j)){
     chans[j]++;
     last[j]=i;
   }
  }
  if(abs(last[21]-last[24])<10)chansd[0]++;
 }
 return 0;
}
//-----------------------------------------------------------
int extractData::fill(int chan1,int chan2,int dist){
 if(chan1>chan2){
  //return 0; // double counting
  dist=delta_d-dist;
  int ch=chan1; chan1=chan2;chan2=ch;
 } else dist=delta_d+dist;
 int i=(2*ninputs-chan1-1)*chan1/2+chan2;
 correlations[i][dist]++;
 return 0;
}
//-----------------------------------------------------------
// fill also position in orbit. 
// Since inputs are correlated one orbit position is enough
int extractData::fill(int chan1,int chan2,int dist,int iorbit){
 if(chan1>chan2){
  //return 0; // double counting
  dist=delta_d-dist;
  int ch=chan1; chan1=chan2;chan2=ch;
 } else dist=delta_d+dist;
 int i=(2*ninputs-chan1-1)*chan1/2+chan2;
 correlations[i][dist]=correlations[i][dist]+1;
 cororbit[i][iorbit]++; 
 return 0;
}
//-----------------------------------------------------------
int extractData::distance2Orbit(){
 orbit = new int*[ninputs];
 int delta=ORBIT;
 int cordist=0;
 for (int i=0;i<ninputs;i++){
   orbit[i]=new int[delta];
   for(int j=0;j<delta;j++)orbit[i][j]=0;
 }
 int ndata=data.size();
 int norbit=orbits.size();
 cout << "# of orbit points= " << norbit << endl;
 for(int i=0;i < ndata; i++){
   ssmpoint *a = &data[i];
   int issm=a->issm;
   int pos=a->position;
   int chan=a->chanrel;
   int j=0;
   while((j<norbit)){
     //cout << i << " " << j << " " << pos << " " << orbits[j].position << " ";
     //cout << pos- orbits[j].position -cordist <<  endl;
     if(
        (issm == orbits[j].issm) &&
        ((pos - orbits[j].position - cordist) > 0) &&
        ((pos - orbits[j].position - cordist) < delta))
     {
      //cout << delta << " " << ((pos - orbits[j].position - cordist) < delta) << endl;
      //cout << i << " chan=" << chan << " " << pos- orbits[j].position - cordist << endl;
      int iorb=pos- orbits[j].position - cordist;
      orbit[chan][iorb]++;
      a->addOrbit(iorb);
      break;
     }
    j++;
   }
 }
 return 0;
}
//--------------------------------------------------------------
// Multiple correlation coefficient analysis
int extractData::correlate2OneAll(int cordist,int delta)
{
 repFile << "Correlations to input: " << input2correlate_d << endl;
 // create TVectors
 cvec = new TVectorD(ninputs-1);
 var = new TMatrixD(ninputs-1,ninputs-1);
 ncorrelations=ninputs*(ninputs+1)/2;
 correlations = new int*[ncorrelations];
 for (int i=0;i<ncorrelations;i++){
   correlations[i]=new int[2*delta+1];
   for(int j=0;j<2*delta+1;j++)correlations[i][j]=0;
 }
 delta_d=delta;
 delta=delta/2;
 cordist_d=cordist;
 correlateAllSSM(cordist,delta_d);
 //-----------------------------------------------------------------------------------------
 // normalise (like in printCorrel)
 //normaliseCor(); - done in printCorr - needst ot be cleaned
 printCorrelations();
 // calculate multicorrelation  for different delays
 int npow=1;
 int delaymax[ninputs];
 for(int i=0;i<ninputs-1;i++)npow=npow*(2*delta+1);
 double ddmax=DBL_MIN,dd=DBL_MIN,distmin=DBL_MAX;
 //cout << "DELAYS:" << endl;
 for(int i=0;i<npow;i++){
   int delay[ninputs];
   delay[ninputs-1]=0;
   int num=i;
   int numlessdigit; // This is based on idea that i is number in (2*delta+1) base 
   for(int j=0;j<ninputs-1;j++){
      numlessdigit=num/(2*delta+1);
      delay[j]=num-numlessdigit*(2*delta+1)-delta;
      num=numlessdigit;
      //cout << delay[j] << " ";
   }
   //cout << endl;
   dd=calculateCorCoef(delta,delay);  // Chosen inputs has to be first/
   double dist=0;
   for(int i=0;i<ninputs-1;i++)dist+=TMath::Abs(delay[i]);
   hCorCoef->Fill(dd,dist);
   if(dd>ddmax){ 
     ddmax=dd;
     distmin=dist;
     for(int j=0;j<ninputs-1;j++)delaymax[j]=delay[j];
   }else if(dd==ddmax){
     if(distmin>dist){
       distmin=dist;
       for(int j=0;j<ninputs-1;j++)delaymax[j]=delay[j];      
     }
   }
   //cout << "dd= "<< dd << " ddmax= " << ddmax << endl;
 }
 if(distmin) 
    repFile << "\033[1;32m>>> " <<" MAX="<< distmin  << "\033[0m\n" << endl;
 else
    repFile << " MAX= " << distmin << endl;
 cout << "ddmax= " << ddmax << " delays: ";
 for(int j=0;j<ninputs-1;j++)cout << delaymax[j] << " ";
 cout << endl; 
 return 0;
}
//-----------------------------------------------------------
int extractData::correlateAllSSM(int cordist,int delta){
 ncorrelations=ninputs*(ninputs+1)/2;
 correlations = new int*[ncorrelations];
 cororbit     = new int*[ncorrelations];
 if(delta==0){
  // use config values
  delta_d=cdelta_d;
  delta=delta_d;
  cordist_d=ccordist_d; 
 }else{
  delta_d=delta;
  cordist_d=cordist;
 }
 printf("ncorrelations=%i 2*delta+1=%i \n",ncorrelations,2*delta+1);
 for (int i=0;i<ncorrelations;i++){
   correlations[i]=new int[2*delta+1];
   for(int j=0;j<2*delta+1;j++)correlations[i][j]=0;
   cororbit[i] = new int[ORBIT];
   for(int j=0;j<ORBIT;j++)cororbit[i][j]=0;
 }
 int ndata=data.size();
 cout << "# of data points= " << ndata << endl;
 for(int i=0;i < ndata; i++){
   ssmpoint *a = &data[i];
   int issm=a->issm;
   int pos=a->position;
   int chan=a->chanrel;
   int j=i;
   while((j<ndata) &&
         (issm == data[j].issm) && 
         //(chan <= data[j].chanrel) &&
         (abs(pos - data[j].position - cordist) <= delta)
   ){
    fill(chan,data[j].chanrel,pos-data[j].position,a->iorbit);
    j++;
   }
   /*
   j=i-1;
   while((j>=0) &&
         (issm == data[j].issm) && 
         //(chan <= data[j].chanrel) &&
         (abs(pos - data[j].position - cordist) <= delta)
   ){
    fill(chan,data[j].chanrel,pos-data[j].position);
    j--;
   }
  */
 }
 return 0;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void extractData::printFileList(){
 for(int i=0;i<nlines_d;i++) cout << filelist[i] << endl;
 //for(
 //    vector<string>::iterator i=ssmdumps.begin();
 //    i != ssmdumps.end();
 //    ++i) cout << ssmdumps[j] << endl;
}
//------------------------------------------------------------------
void extractData::printData(){
 for(
     vector<ssmpoint>::iterator i=data.begin();i !=
     data.end();++i){
     i->Print();
     }
}
//------------------------------------------------------------------
float extractData::chi2(int delta,int* delay){
 float chi=0.;
 for(int m=0;m<ninputs;m++){
  for(int n=m;n<ninputs;n++){
   if( (chans[inp2chan[m]]==0) || (chans[inp2chan[n]]==0) ) continue;
   int k1=(2*ninputs-m-1)*m/2+n;
   for(int i=0;i<ninputs;i++){
    for(int j=i;j<ninputs;j++){
     int k2=(2*ninputs-i-1)*i/2+j;
     if(k2<=k1) continue;
     if( (chans[inp2chan[i]]==0) || (chans[inp2chan[j]]==0) ) continue;
     //cout << "k1,k2= " << k1 << " " << k2 << endl;
     for(int l=2*delta;l<2*delta_d+1-2*delta;l++){
        //fillHist(k,l+1,correlations[k][l]);
        int delay1=delay[m]-delay[n];
        int delay2=delay[i]-delay[j];
        float del = norcorrs[k1][l+delay1]-norcorrs[k2][l+delay2];
        float var = varcorrs[k1][l+delay1]+varcorrs[k2][l+delay2];
        //var=1.;
        //cout << del << " del,var " << var << endl;
        //if(del) cout << "del=" << del << endl;
        if(var) chi += del*del/var;
     }
    }
   }
  }
 }
 return chi;
}
//------------------------------------------------------------------
// To be used with correlationst agains 0BPX
int extractData::calculateMask(int* mask){
  unsigned int j=0;
  while(j < inputs.size() && (inputs.at(j)).name.find("0BPA") == string::npos)j++;
  if(j==inputs.size()){
     cout << "Input 0BPA not found in VALID.CTPINPUTS" << endl;
     return 1;
    }
    cout << "Found 0BPA" << endl;
  j=0;
  while(j < inputs.size() && (inputs.at(j)).name.find("0BPC") == string::npos)j++;
  if(j==inputs.size()){
     cout << "Input 0BPC not found in VALID.CTPINPUTS" << endl;
     return 1;
    }
    cout << "Found 0BPC" << endl;
  return 0;
}
//------------------------------------------------------------------
// Find position in ninputs, NOT chan
int extractData::findInput(string& nameChosen, int& input){
 int iChosen=-1;
 for(int i=0;i<ninputs;i++){
    string name(inputNames[i]);
    if(name.find(nameChosen) != string::npos){
      iChosen=i;
      break;
    }
 }
 if(iChosen==-1){
   cout << "correlate2Orbit:: chosen input not found: " << nameChosen << endl;
   return 1;
 }
 input=iChosen;
 return 0;
}
//------------------------------------------------------------------
// calculate correlations from orbit distance to chosen input
//  
int extractData::correlate2Orbit(int cordist,int delta, string nameChosen)
{
 //ncorrelations=ninputs*(ninputs+1)/2;
 ncorrelations=ninputs;
 correlaorbit = new float*[ncorrelations];
 for (int i=0;i<ncorrelations;i++){
   correlaorbit[i]=new float[2*delta+1];
   for(int j=0;j<2*delta+1;j++)correlaorbit[i][j]=0;
 }
 delta_d=delta;   // this is to be cleaned - delta_d inputs on severa; places
 cordist_d=cordist;
 // find input
 //string nameChosen("0TVX");
 int iChosen;
 findInput(nameChosen,iChosen);
 cout << " Chosen: " << nameChosen << " pos=" << iChosen << endl;
 // Calculate chi2
 for(int k=0;k<ninputs;k++){
    float min=FLT_MAX; 
    int imin=INT_MAX;
    cout << inputNames[k];
    for(int i=-delta;i<delta+1;i++){
       float sum=0.;
       for(int j=0;j<cdelta_d;j++){
          float diff=nororbit[iChosen][j]-nororbit[k][(j+i)%cdelta_d];
          float var1=varorbit[k][(j+i)%cdelta_d];
          float var2=varorbit[iChosen][j];
          if(var1 || var2)sum += diff*diff/(var1+var2);  // add error
       }
       correlaorbit[k][i+delta]=sum;
       if(sum<min){
         min=sum;imin=i;
       }
       cout << " " << sum;
    }
    cout << " MIN= " << imin << endl;
 } 
 return 0;
}
//---------------------------------------------------------------------------------
int extractData::normaliseDistance2Orbit()
{
 float norm[ninputs];
 for(int i=0;i<ninputs;i++){
   float sum=0;
   for(int j=0;j<cdelta_d;j++) sum += orbit[i][j];
   norm[i]=sum;
   //cout << i << " norm " << norm[i] << endl;
 }
 nororbit = new float*[ninputs];
 varorbit = new float*[ninputs];
 for (int i=0;i<ninputs;i++){
   nororbit[i]=new float[cdelta_d];
   varorbit[i]=new float[cdelta_d];
   for(int j=0;j<cdelta_d;j++){
      nororbit[i][j]=0;
      varorbit[i][j]=0;
      if(norm[i]){
        nororbit[i][j]=((float)orbit[i][j])/norm[i];
        varorbit[i][j]=((float)orbit[i][j])/norm[i]/norm[i];
        //cout << nororbit[i][j] << endl;
      }
   }
 }
 return 0;
}
//---------------------------------------------------------------------------------
// delta here and checkallSSM can be different
int extractData::checkDelaysOrb(int delta){
 // calculate mask
 //int mask[ddelta_d];
 //calculateMask(mask);
 // normalise orbit
 return 0;
}
//------------------------------------------------------------------
// chi2 everybody with everybody
// delta here and checkallSSM can be different
int extractData::checkDelaysCor(int delta){
 // normalise
 float norm[ncorrelations];
 for(int i=0;i<ncorrelations;i++){
   float sum=0;
   for(int j=0;j<2*delta_d+1;j++) sum += correlations[i][j];
   norm[i]=sum;
   cout << i << " norm " << norm[i] << endl;
 }
 norcorrs = new double*[ncorrelations];
 varcorrs = new double*[ncorrelations];
 for(int i=0;i<ncorrelations;i++){
   norcorrs[i]=new double[2*delta_d+1];
   varcorrs[i]=new double[2*delta_d+1];
   for(int j=0;j<2*delta_d+1;j++){
      if(norm[i]){
        norcorrs[i][j]= (float) correlations[i][j]/norm[i];
        varcorrs[i][j] = norcorrs[i][j]/norm[i];
      }else{
        norcorrs[i][j] = 0.; 
        varcorrs[i][j] = 0.;
      }
      //cout << i << " " << j << " " << norcorrs[i][j] << endl;
   }
 }
 // calculate chi for different delays
 int npow=1;
 float chimin=FLT_MAX;
 int delaymin[ninputs];
 for(int i=0;i<ninputs-1;i++)npow=npow*(2*delta+1);
 for(int i=0;i<npow;i++){
   int delay[ninputs];
   delay[ninputs-1]=0;
   int num=i;
   int numlessdigit; 
   for(int j=0;j<ninputs-1;j++){
      numlessdigit=num/(2*delta+1);
      delay[j]=num-numlessdigit*(2*delta+1)-delta;
      num=numlessdigit;
   }
   float chi=chi2(delta,delay);
   if(chi<chimin){
     chimin=chi;
     for(int j=0;j<ninputs;j++)delaymin[j]=delay[j];
   }
   /*
   if(chi){
     cout << "Inputs: ";
     for(int k=0;k<ninputs;k++) cout << delay[k] << " ";
     cout << " chi2: "<< chi << endl;
   }*/
 }
 cout << "Minimum chi2: " << chimin << endl;
 cout << "Estimated Input Delays wrt to delays in VALIDCTP.INPUTS:" << endl;
 for(int i=0;i<ninputs;i++){
  if(chans[inp2chan[i]]) cout << inputNames[i] << ": " << delaymin[i] << endl;
  else cout << inputNames[i] << " not available- no signal recorded" << endl;
 }
 return 0;
}
//------------------------------------------------------------------
// create array of pointers in the form d = cT V^-1 c
// c= correlations against e.g. TVX =(TVXAxVBA, TVXAxVBC)
// V=(vbaxvba,vbaxvbc,vbaxvba,vbcxvbx)
double extractData::calculateCorCoef(int delta,int* delay)
{
 // C vector
 int l = 1;
 for(int i=1;i<ninputs;i++){
    //int n = l % ninputs;
    l += 1;
    l += ((l%ninputs)==0) * (l/ninputs);
    int del=delta_d-delay[i-1];
    //cout << "i= " << i << " del= " << del << " l=" << l << " n="<< n <<endl;
    (*cvec)[i-1]=norcorrs[i][del];
 }
 int dim=ninputs-1;
 int ll=0;
 for(int i=ninputs;i<ncorrelations;i++){
        int m = l/ninputs;
        int n = l % ninputs;
	int j = ll/dim;
        int k = ll % dim;
        l += 1;
        l += ((l%ninputs)==0) * (l/ninputs);
        ll += 1;
        ll += ((ll%dim)==0) * (ll/dim);
        //cout << "m,n,k " << inputNames[m] << " " << inputNames[n] << " " << i  << endl;
        int del=delta_d+delay[m-1]-delay[n-1];
        //cout << "delta= " << del << " d[m]= " << delay[m-1] << " d[n]=" << delay[n-1] << endl;
	//cout << "m= " << m << " n=" << n << " del= " << del << " j= " << j << " k= " << k << endl;
	(*var)[j][k]=norcorrs[i][del];
	(*var)[k][j]=norcorrs[i][del];
 }
 /*
 TDecompSVD svd(*var);
 //svd.SetTol(svd.GetTol()/1000.);
 //cout << "Tolerance: " << svd.GetTol() << endl;
 bool status;
 TMatrixD inv=svd.Invert(status);
 // Unit matrix=just scalar product
 //TMatrixD inv(*var);
 //inv.UnitMatrix();
 //inv.Print();
 //printcvec();
 //printvar();
 //(*var).Print();
 //(*cvec).Print();
 if(status==0){
   return DBL_MAX; 
 }
 else{
   TVectorD vartimecvec(*cvec);
   vartimecvec *= inv;
   double dd= (*cvec)*vartimecvec;
   return dd;
 }
 */
 return 0;
}
//---------------------------------------------------------------
void extractData::printcvec()
{
 cout << "CVEC:";
 for(int i=0;i<ninputs-1;i++)cout << " " << (*cvec)[i];
 cout << endl;
}
//-----------------------------------------------------------------
void extractData::printvar()
{
 for(int i=0;i<ninputs-1;i++){
    for(int j=0;j<ninputs-1;j++){
       cout << (*var)[i][j] << " ";
    }
    cout << endl;
 }
}
//------------------------------------------------------------------
// Normalisation to proper correlations with sigmas in denominator
void extractData::normaliseCor()
{
 norcorrs = new double*[ncorrelations];
 for(int i=0;i<ncorrelations;i++)norcorrs[i]=new double[2*delta_d+1];
 cout << "norcorrs created with size: " << 2*delta_d+1 << endl;
 for(int i=0;i<ninputs;i++){
  for(int j=i;j<ninputs;j++){
    int k=(2*ninputs-i-1)*i/2+j;
    for(int l=0;l<2*delta_d+1;l++){
     float m1=chans[inp2chan[i]];
     float m2=chans[inp2chan[j]];
     float N=nssms*Mega;
     float cor=correlations[k][l]-m1*m2/N;
     cor=cor/TMath::Sqrt(m1*(1-m1/N)*m2*(1-m2/N));
     norcorrs[k][l]=cor;
     //norcorrs[k][l]=correlations[k][l];
    }
    if(i==j){ //symmetrise cor
      for(int l=0;l<delta_d;l++){
         correlations[k][2*delta_d-l]=correlations[k][l];
         norcorrs[k][2*delta_d-l]=norcorrs[k][l];
      }
    }
  }  
 }
}
//---------------------------------------------------------------------
void extractData::printDistance()
{
 int cordist=0;
 int delta=ORBIT;
 for(int i=0;i<ninputs;i++){
  string name(inputNames[i]);
  cout << name << endl;
  addHist2(name,delta,cordist,cordist+delta);
  int sum=0;
  for(int j=0;j<delta;j++){
     fillHist2(i,j,orbit[i][j]);
     sum += orbit[i][j];
     //cout << " " << orbit[i][j];
  }
  entriesHist2(i,sum);
  //cout << endl;
 }
}
//-------------------------------------------------------------------
void extractData::printCorrelations(){
 normaliseCor();
 for(int i=0;i<ninputs;i++){
  for(int j=i;j<ninputs;j++){
    std::stringstream ss;
    ss << i << "x" << j;
    string name("");
    name=inputNames[i]+"x"+inputNames[j];
    addHist(name,delta_d,cordist_d);
    int k=(2*ninputs-i-1)*i/2+j;
    cout << name << ":";
    cout << i << "x" << j << ": ";
    int sum=0;
    for(int l=0;l<2*delta_d+1;l++){
     fillHist(k,l+1,correlations[k][l]);
     sum += correlations[k][l];
     //fillHist(k,l+1,norcorrs[k][l]);
     //sum += norcorrs[k][l];
     //cout << norcorrs[k][l] << " ";
    }
    entriesHist(k,sum);
    //cout << endl;
  }
 }
}
void extractData::printCorrelationOrbit()
{
 for(int i=0;i<ninputs;i++){
  for(int j=i;j<ninputs;j++){
    std::stringstream ss;
    ss <<"ORBIT " <<  i << "x" << j;
    string name("");
    name="ORBIT " + inputNames[i]+"x"+inputNames[j];
    addHist3(name,ORBIT);
    int k=(2*ninputs-i-1)*i/2+j;
    cout << name << ":";
    cout << i << "x" << j << ": ";
    int sum=0;
    for(int l=0;l<ORBIT;l++){
     fillHist3(k,l+1,cororbit[k][l]);
     sum += cororbit[k][l];
     //cout << cororbit[k][l] << " ";
    }
    entriesHist3(k,sum);
    //cout << endl;
  }
 }

}
//-------------------------------------------------------------------
void extractData::printCorrelations2One(string& chname){
 cout << "Correlations to input: " << input2correlate_d << endl;
 repFile << "Correlations to input: " << input2correlate_d << endl;
 int ihist=0;
 for(int i=0;i<ninputs;i++){
  for(int j=i;j<ninputs;j++){
    std::stringstream ss;
    ss << i << "x" << j;
    string name("");
    name=inputNames[i]+"x"+inputNames[j];
    if(name.find(chname) == string::npos) continue;;
    addHist(name,delta_d,cordist_d);
    int k=(2*ninputs-i-1)*i/2+j;
    //cout << name << ":";
    //cout << i << "x" << j << ": ";
    //int peak=0,offpeak=0;
    int max=INT_MIN;
    int imax=INT_MIN;
    for(int l=0;l<2*delta_d+1;l++){
     //cout << correlations[k][l] << " ";
     fillHist(ihist,l+1,correlations[k][l]);
     if(correlations[k][l]>max){
       max=correlations[k][l];
       imax=l-delta_d;
     }
     //if(l==delta_d)peak = correlations[k][l];
     //else offpeak=offpeak+correlations[k][l];
    }
    ihist++;
    cout << name << " MAX= " << imax << endl;
    if(imax) 
    repFile << "\033[1;32m>>> "<<name<<" MAX="<< imax  << "\033[0m\n";
    else
    repFile << name << " MAX= " << imax << endl;
    //cout << peak << " off: "<< offpeak << endl;
  }
 }
}

