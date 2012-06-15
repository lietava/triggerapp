#include <iostream>
#include <TROOT.h>
#include "TTimer.h"
using namespace std;
class my1 : public TObject
{
 private:
 public:
         my1();
         virtual ~my1();
         virtual void call1();
         virtual void callshot();
 ClassDef(my1,1);
};

