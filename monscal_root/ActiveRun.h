#ifndef _ActiveRun_h_
#define _ActiveRun_h_
#include "Output.h"
#include "TrigConf.h"
#include <vector>
#include <list>
#include <string>
#include <sys/types.h>
typedef unsigned int w32;
using namespace std;
//---------------------------------------------------------------------------------------------------------
class ActiveRun : public Log
{
 private:
         enum {NCLASS=50, NCLUST=6,NINP=60, NDET=24};
         const int fRunNumber;
         string fname;
         string frcfgfile;
         string partifile;
         int ninp,nclass,nclust,ndet;
         vector<string> inputlist;
         bool copycounters2dcs;
         Counter L2a;
         DisplaySCAL *scal;
         TrigTimeCounters times;
         InteractionwCount* fINT;
         TriggerInputwCount* fTrigInputs[NINP];
         TriggerClasswCount* fClasses[NCLASS];
         TriggerClusterwCount* fClusters[NCLUST];
         DetectorwCount* fDetectors[NDET];
         void SetDefaultInputs();
         int ProcessCfgLine(const string& line,int& level);
         int ProcessInputLine(const string& line);
         int ProcessPartitionLine(const string& line,int& level);
         int ParseConfigFile(int runnum);
         int ParsePartitionFile(int runnum);
         int ParsePartitionClass(const string& classstring);
         int FindDetectors();
         int ParseInputsList();
         int OpenVALIDCTPINPUTS();
         int ParseValidCTPInputs();
 public:
         ActiveRun();
         ActiveRun(const int runnum);
         ~ActiveRun();
         int GetRunNumber(){return fRunNumber;};
         int Getninp(){return ninp;};
         int Getnclust(){return nclust;};
         string& GetName(){return fname;};
         TriggerClusterwCount** GetClusts(){return fClusters;};
         TriggerInputwCount** GetInps(){return fTrigInputs;};
         InteractionwCount* GetInter(){return fINT;};
         void AddClass(TriggerClasswCount* clss){fClasses[nclass++]=clss;};
         void AddCluster(TriggerClusterwCount* cls){fClusters[nclust++]=cls;};
         void AddInput(TriggerInputwCount* inp){fTrigInputs[ninp++]=inp;};
         void UpdateRunCounters(w32* buffer);
         void PrintInputs();
         void PrintClusters();
         void PrintClasses();
         void PrintDetectors();
         void Print(){printf("run: %i\n",fRunNumber);PrintInputs();PrintClusters();};
         //----------DisplayonScreen
         //void CreateDisplaySCAL(){scal = new DisplaySCAL(fRunNumber,fname);};
         void CreateDisplaySCAL(){scal = new DisplaySCAL(*this);};
         void DisplayRun(){scal->DisplayRun();DisplaySCAL::GetfileSCAL()->flush();};
         //----------CountersOCDB
         void CreateCountersOCDB(bool copy2dcs){};
         void WriteHeaderOCDB(){};
	 void WriteRecordOCDB(){};
         //-----------DAQlogbook
         void CreateDAQlogbook(int log=0){}
         void Write2DAQlogbook(){};
};
#endif
