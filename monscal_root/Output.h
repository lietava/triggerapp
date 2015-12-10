#ifndef _Output_h_
#define _Output_h_
#include "TGraphErrors.h"
#include "TrigConf.h"
#include "Log.h"
#include <string>
#include <fstream>

using namespace std;
class ActiveRun;
class DisplaySCAL : public Log
{
 private:
         //enum {NITEMS=54,NLHRAT=53,NTIME=52,NINT2=51,NINT1=50,NCLUST=6,NINP=60,NCLASS=100};
         enum {NITEMS=104,NLHRAT=103,NTIME=102,NINT2=101,NINT1=100,NCLUST=6,NINP=60,NCLASS=100};
         int runnum;
         int ninp;
         int nclust;
         int ivln,ivhn; // positions of vln and vhn inputs for ratio
         bool show;  // show partition
         TriggerInputwCount** inps;
         InteractionwCount* inter;
         TriggerClusterwCount** clusts;
         static ofstream *fileSCAL;
         double* dataInp[NITEMS];
         char* itemnames[NITEMS];
         double** dataL0[NCLUST];
         double** edatL0[NCLUST];
         double** dataL2[NCLUST];
         double** edatL2[NCLUST];
         double** rati20[NCLUST];
         double** erat20[NCLUST];
         char ClustNames[NCLUST][64];
         int npoints;
         int cpoints;   // pointer to one of npoints
         int nnpoints;  // <=npoints
         int count;
         float timelast;
         int activeclust;
         void DisplayInputs(const int ninp, TriggerInputwCount* inps[]);
         void DisplayInputs();
         void DrawCanvasInps();
         void DrawCanvasClasses(int iclu);
         void DrawCanvasClasses();
         void SetGlobals();
         void SetAxisInps(TGraphErrors* f1,int item, int ncd);
         void SetAxisClass(TGraphErrors* f1,int item, int ncd, int level);
         void SetItemName(const char* name,int pos){itemnames[pos]=const_cast<char *> (name);}
         void CreateClassesofClust(int i);
         void CreateInputs();
         void CheckIndex(int id, int index,int max);
 public:
 	//DisplaySCAL(const int runnum,const string& name);
 	DisplaySCAL(ActiveRun& ar);
 	virtual ~DisplaySCAL();
        static ofstream* GetfileSCAL(){return fileSCAL;};
        void DisplayRun();
        void ClearData();
        void CreateInputs(int ninp,TriggerInputwCount** inps);
};
#endif
