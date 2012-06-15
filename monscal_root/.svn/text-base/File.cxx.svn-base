#include "File.h"
#include "ctpcounters.h"
#include <iostream>
#include <sstream>
#include "TObjArray.h"
#include "TObjString.h"


using namespace std;
File::File(char* rawcntsfile)
:
MonScal(1,0),
count(0)
{
 file = new ifstream(rawcntsfile);
 Run();
}
void File::Run()
{
 TString strLine;
 int nlines=0;
 while (strLine.ReadLine(*file) && nlines<10) {
    //nlines++;
    TObjArray *tokens = strLine.Tokenize(" ");
    Int_t ntokens = tokens->GetEntriesFast();
    cout <<  ((TObjString*)tokens->At(1))->String() << " " << ntokens << endl;
    if(ntokens != 972){
      cout << "Unexpected number of counters: " << ntokens << endl;
      continue;
    }
    size=4*970;
    for(Int_t i=2;i<ntokens;i++){
      string cnt(((TObjString*)tokens->At(i))->String()); 
      stringstream convert(cnt); 
      UInt_t hcnt;
      convert >> std::hex >> hcnt;
      //cout << hcnt << endl;
      buffer[i-2]=hcnt;
    }
    infoHandler();
 }
}
void File::Plot(int run)
{
}
void File::infoHandler()
{
    int size=4*NCOUNTERS;
    if(size != 4*NCOUNTERS) {
      cout << "error in CTPCounters. size=" << size<<" instead of " << 4*NCOUNTERS << endl;
      return;
    };
    //cout << "Handler called "  << count++ << endl;
    SetBuffer((w32 *)getData());
    GetActiveRuns();
}

