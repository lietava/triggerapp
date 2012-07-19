#include "extract.h"
#include "TStopwatch.h"
//-------------------------------------------------------------------
int main(int argc, char **argv) {
 if(argc != 2){
  cout << "One argument expected." << endl;
  return 1;
 }
 int ret;
 TStopwatch time,timetot;
 cout << argv[1] << endl;
 time.Start();
 timetot.Start();
 extractData a(argv[1]);
 //a.SetSkipNext(12);  // 0 for autocorrelations
 //a.SetNfiles(10);
 if((ret=a.readFileList())) return ret;
 a.chooseInputs();
 //a.checkAllSSM();
 //a.checkAllSSM1by1();
 //return 1;
 //
 time.Stop();
 cout << "CPU time after reading data: " << time.CpuTime() << endl;
 time.Start();
 a.removeEmptySSMs(0);
 a.extractAllSSM();
 //a.printData();
 //a.chooseInputs();
 time.Stop();
 cout << "CPU time for extracting data: " << time.CpuTime() << endl;
 //return 0;
 time.Start();
//----------------------------------Corr direct
  a.correlateAllSSM(0,10);  
  a.printCorrelations();
//-----------------------------------Corr 2 One
 cout << "Correlations2One======================================================================="<<endl;
 //a.correlate2One(0,5,"0TVX");
 //a.correlate2One(0,5);
//-----------------------------------Orbit
 //cout << "Correlations2Orbit====================================================================="<<endl;
 //a.distance2Orbit();// set in config.cfg
 //a.printDistance();
 //if((ret=a.normaliseDistance2Orbit())) return ret;
 //a.correlate2Orbit(0,5,"0TVX");
//----------------------------------Output
 //a.printAllHists();
 a.writeHists();
 time.Stop();
 cout << "CPU time for correlations: " << time.CpuTime() << endl;
 time.Start();
 //-----------------------------------------chi
 //a.checkDelaysCor(5); //chi2
 //a.checkDelaysOrb(5);
 time.Stop();
 cout << "CPU time for chi2: " << time.CpuTime() << endl;
 cout << "Finished---------------------------------------Total timr: "<< timetot.CpuTime() << endl;
}
