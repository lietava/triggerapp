#ifndef _Output_h_
#define _Output_h_
#include "TGraphErrors.h"
#include "TrigConf.h"
#include "Log.h"
#include <string>
#include <fstream>
#include "TQObject.h"
#include "TObject.h"

using namespace std;
class ActiveRun;
class DisplaySCAL : public Log, public TObject
{
 private:
         enum {NITEMS=53,NTIME=52,NINT1=51,NINT2=50,NCLUST=6,NINP=60,NCLASS=50};
         int runnum;
         int ninp;
         int nclust;
         TriggerInputwCount** inps;
         InteractionwCount* inter;
         TriggerClusterwCount** clusts;
         static ofstream *fileSCAL;
         double* dataInp[NITEMS];
         char* itemnames[NITEMS];
         double** dataL0[NCLUST];
         double** dataL2[NCLUST];
         char ClustNames[NCLUST][64];
         int npoints;
         int cpoints;   // pointer to one of npoints
         int nnpoints;  // <=npoints
         int activeclust;
         void DisplayInputs(const int ninp, TriggerInputwCount* inps[]);
         void DisplayInputs();
         void DrawCanvasInps();
         //void DrawCanvasClasses(int iclu);
         void SetGlobals();
         void SetAxis(TGraphErrors* f1,int i, int ncd);
         void SetItemName(const char* name,int pos){itemnames[pos]=const_cast<char *> (name);}
         void CreateClassesofClust(int i);
         void CreateInputs();
 public:
 	//DisplaySCAL(const int runnum,const string& name);
        DisplaySCAL(){};
 	DisplaySCAL(ActiveRun& ar);
 	virtual ~DisplaySCAL();
        virtual void DrawCanvasClasses();
        static ofstream* GetfileSCAL(){return fileSCAL;};
        void DisplayRun();
        void ClearData();
        void CreateInputs(int ninp,TriggerInputwCount** inps);

        ClassDef(DisplaySCAL,1);
};

#endif
