#include <signal.h>
#include <iostream>
#include "TApplication.h"
#include "File.h"
#include "Dim.h"
#include "MonScal.h"
int main(int argc, char **argv) {
 VALIDLTUS ltus;
 if(ltus.readVALIDLTUS()){
  return 1;
 }
 //ltus.Print();
 //return 0;
 //File monscal("../../11.12.2009.rawcnts");
 //monscal.Plot(104800);
 OpenDim monscal(1,0);
 while(1) pause();
 return(0);
} 
