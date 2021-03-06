#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TObjArray.h"
#include "TVectorD.h"
typedef unsigned int w32;
using namespace std;

class Hists
{
 private:
         TObjArray hists;  // correlations
         TObjArray hists2; // length and orbit
         TObjArray hists3; // orbit for given correlations
 public:
         //TH1F *hCorCoef;
         TH2F *hCorCoef;
         Hists();
	 ~Hists();
         int addHist(string const &name,int delta,int cordist);
         int addHist2(string const &name,int nbins,float x0, float xmax);
         int addHist3(string const &name,int delta);
         int fillHist(int i,int bin,float x);
         int fillHist2(int i,int bin,float x);
         int fillHist3(int i,int bin,float x);
   	 void entriesHist(int i,int sum){ ((TH1F*) hists[i])->SetEntries(sum);};
   	 void entriesHist2(int i,int sum){ ((TH1F*) hists2[i])->SetEntries(sum);};
   	 void entriesHist3(int i,int sum){ ((TH1F*) hists3[i])->SetEntries(sum);};
         int writeHists();
         void pdfAllHists();
         void pdfAllHists2();
};
//---------------------------------
class ssmpoint
{
 public:
 int issm,chanabs,chanrel,position;
 int iorbit; // position wrt to orbit
 ssmpoint(int is,int ch,int chr,int pos):
 issm(is),chanabs(ch),chanrel(chr),position(pos),iorbit(0)
 {};
 ssmpoint(int is,int ch,int chr,int pos,int iorb):
 issm(is),chanabs(ch),chanrel(chr),position(pos),iorbit(iorb)
 {};
 void Print();
 void addOrbit(int iorb){iorbit=iorb;};
};
//-------------------------------------------------------
class Input
{
 public:
         string name;
         int delay,index,level;
         int counts;
         int channel;
         int used;
         Input(string& name,int delay,int index);
         Input(string& name,int delay,int index,int level);
	 void SetCounts(int cnt){counts=cnt;};
	 void SetChannel(int chan){channel=chan;};
	 int GetCounts(){return counts;};
	 int GetLevel(){return level;};
	 int GetChannel(){return channel;};
	 void Print();
};
//---------------------------------------------------
class extractData : public Hists
{
 private:
        int checkInputs(int issm);
        void check1SSM(int issm);
        int extract1SSM(int issm);
        int extractBPX(int issm);
        int extract1SSMfilt1(int issm);
        int checkSSMforData(int issm);
        int fill(int chan1,int chan2,int dist);
        int fill(int chan1,int chan2,int dist,int orbit);
        int ParseVALIDCTPINPUTS();
	float chi2(int delta,int *delay1);
        int calculateMask(int* mask);
	int findInput(string& nameChosen, int& input);
	void normaliseCor();
	double calculateCorCoef(int delta,int* delay);
	void openRepFile();
	void printvar();
	void printcvec();
        vector<string> filelist;
        vector<string> ssmdumps;
        vector<ssmpoint> data;
        vector<ssmpoint> orbits;
        vector<ssmpoint> bpa;
        vector<ssmpoint> bpc;
        vector<string> inputNames;
        vector<Input> inputs;
        string FileList_d;
        string DataDir_d;
        string VALIDCTPINPUTS_d;
	string input2correlate_d;
        int cordist_d,delta_d;
        int ccordist_d,cdelta_d; // config values
        int nfiles;        // if < nlines only nfiles are read
        int nlines_d;      // # of lines in file list = # files (filelist)
        int nssms;         // # of non empty ssms= at least 2 signals (ssmsumps)
        int ninputs,ncorrelations;
	int chanbpa,chanpbc;  // channels of bpa,bpc
        int *(*correlations); // direct correlations between inputs
        float *(*correlaorbit); // correlations of 2 inputs's distance to orbit
        double *(*norcorrs);  // normalised correlationa
        double *(*varcorrs);  // variance of  correlationa
        int *(*orbit);       // distance to orbit 
        float *(*nororbit);  // normalised distance to orbit
        float *(*varorbit);  // normalised distance to orbit
        int *(*cororbit);  // distance of corrlation pairs to orbit
        ofstream repFile;
        int const Nchans;  // # of all chanels 24+24+12
        int *chan2inp;     // channel to input
        int *inp2chan;     // input to channel
        int *chans;        // # of signal
        int *chansd;       // # of signals closer than xxx
        int SkipNext;   // number of BCs to skip after first detected
	TVectorD *cvec;  // correlations to chosen
        TMatrixD *var;   // variance of unchosen
 public:
        //extractData(string const &FileList,string const &DataDir);
        extractData(char* config);
        ~extractData();
        int ReadConfig(char* config);
        void chooseInputs();
        int readFileList();
        int removeEmptySSMs(bool remove);
        void checkAllSSM();
        void extractAllSSM();
        void checkAllSSM1by1();
        int correlateAllSSM(int cordist,int delta);
	int correlate2OneAll(int cordist,int delta);
	int correlate2One(int cordist,int delta, string nameChosen);
	int correlate2One(int cordist,int delta){return correlate2One(cordist,delta,input2correlate_d);};
 	int correlate2Orbit(int cordist,int delta,string nameChosen);
 	int correlate2Orbit(int cordist,int delta){return correlate2Orbit(cordist,delta,input2correlate_d);};
	int checkDelaysCor(int delta);
	int checkDelaysOrb(int delta);
        int distance2Orbit();
	int normaliseDistance2Orbit();
        void printFileList();
        void printData();
        void printCorrelations();
        void printCorrelationOrbit();
	void printCorrelations2One(string& chname);
        void printDistance();
        void SetSkipNext(int skip){SkipNext=skip;}
        void SetNfiles(int n){nfiles=n;};
};
