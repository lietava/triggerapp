#ifndef _Log_h_
#define _Log_h_
#include <string>
#include <fstream>

using namespace std;

class Log{
  private:
         static ofstream* log;
  public:
          Log();
          ~Log();
          ofstream* GetLog(){return log;};
          void getdatetime(char *time);
          void gettime(char *time);
          void gettimeI(int *time);
          void PrintLog(string& text);
          void PrintLog(const char* text);
          void PrintLog(char* text);
};
ostream &operator<<(ostream &  , Log  &);
#endif
