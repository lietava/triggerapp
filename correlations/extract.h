#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TObjArray.h"
typedef unsigned int w32;
using namespace std;

class Hists
{
 private:
         TObjArray hists;  // correlations
         TObjArray hists2; // length and orbit
 public:
         Hists();
	 ~Hists();
         int addHist(string const &name,int delta,int cordist);
         int addHist2(string const &name,int nbins,float x0, float xmax);
         int fillHist(int i,int bin,float x);
         int fillHist2(int i,int bin,float x);
         int writeHists();
         void printAllHists();
};
//---------------------------------
class ssmpoint
{
 public:
 int issm,chanabs,chanrel,position;
 ssmpoint(int is,int ch,int chr,int pos):
 issm(is),chanabs(ch),chanrel(chr),position(pos)
 {};
 void Print();
};
//-------------------------------------------------------
class Input
{
 public:
         string name;
         int delay,index,level;
         int counts;
         int channel;
         bool used;
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
        int ParseVALIDCTPINPUTS();
	float chi2(int delta,int *delay1);
        int calculateMask(int* mask);
	int findInput(string& nameChosen, int& input);
	void openRepFile();
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
        int dcordist_d,ddelta_d;
        int nfiles;        // if < nlines only nfiles are read
        int nlines_d;      // # of lines in file list = # files (filelist)
        int nssms;         // # of non empty ssms= at least 2 signals (ssmsumps)
        int ninputs,ncorrelations;
	int chanbpa,chanpbc;  // channels of bpa,bpc
        int *(*correlations); // direct correlations between inputs
        float *(*correlaorbit); // correlations of 2 inputs's distance to orbit
        float *(*norcorrs);  // normalised correlationa
        float *(*varcorrs);  // variance of  correlationa
        int *(*orbit);       // distance to orbit 
        float *(*nororbit);  // normalised distance to orbit
        float *(*varorbit);  // normalised distance to orbit
        ofstream repFile;
        int const Nchans;  // # of all chanels 24+24+12
        int *chan2inp;     // channel to input
        int *inp2chan;     // input to channel
        int *chans;        // # of signal
        int *chansd;       // # of signals closer than xxx
        int SkipNext;   // number of BCs to skip after first detected
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
	void printCorrelations2One(string& chname);
        void printDistance();
        void SetSkipNext(int skip){SkipNext=skip;}
        void SetNfiles(int n){nfiles=n;};
};
