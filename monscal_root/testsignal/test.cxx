#include "test.h"
#include <TApplication.h>
ClassImp(my1)
my1::my1()
{
 cout << "my1 created " << endl;
}
my1::~my1()
{}
void my1::call1()
{
  cout << "I am called " << endl;
}
void my1::callshot()
{
 TTimer::SingleShot(10,"my1",this,"call1()");
 //TTimer *singleShotTimer = new TTimer(10);
 //TQObject::Connect(singleShotTimer, "Timeout()","my1", this, "call1()"); 
 
}
int main(int argc,char **argv){
 TROOT root("test","test");
 TApplication aa("aa",&argc,argv);
 my1 ss;
 ss.callshot();
 aa.Run();
}

