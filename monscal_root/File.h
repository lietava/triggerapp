#ifndef _File_h_
#define _File_h
#include <fstream>
#include "Log.h"
#include "TString.h"
#include "MonScal.h"

typedef unsigned int w32;
//
class File : public MonScal
{
 private:
          int count;
          ifstream *file;
          void Run();
          int size;
          int getSize(){return size;};
          int* getData(){return buffer;};
          int buffer[970];          
 public:
   File(char* rawcntsfile);   
   void Plot(int run);
   void infoHandler();
};
#endif

