#include <iostream>
#include <string>

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
 /*
 string aa("BCM1 aa dd aa iiiiii");
 cout << aa << endl;
 size_t ix;
 if((ix=aa.find("BCM")) != string::npos)cout << ix << endl;
 else cout << "not found" << endl;
  */
 /////////////////////////////////
 char* path = getenv("VMECFDIR");
 cout << path << endl;
 stringstream ss,pp;
 string frcfgfile;
 ifstream file;
 pp<<path;
 ss << pp.str() << "/CFG/ctp/monscal/inputs.mcfg";
 frcfgfile=ss.str();

 //frcfgfile= "/data/ClientCommonRootFs/usr/local/trigger/vd/vme/CFG/ctp/monscal/inputs.mcfg";

 file.open(frcfgfile.c_str());
 if(!file.is_open()){
  cout << (("ActiveRun: cannot open file: "+frcfgfile).c_str()) << endl;;
  return 1;
 }else{
  cout << (("ActiveRun: File: "+frcfgfile+" opened.").c_str()) << endl;;
 }

 return 0;
} 


