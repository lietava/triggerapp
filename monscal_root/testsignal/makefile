SDIR= .
ODIR= linux
LDGLAGS = -g -lpthread
CFLAGS = -g -Wall
ROOTCONFIG   := root-config
ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLDFLAGS  := $(shell $(ROOTCONFIG) --ldflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs)
ROOTGLIBS    := $(shell $(ROOTCONFIG) --glibs)


CFLAGS     += $(ROOTCFLAGS)
LDFLAGS    += $(ROOTLDFLAGS)
LIBS          = $(ROOTLIBS) $(SYSLIBS)
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)

LIBS += -L$(DIMDIR)/linux -ldim
CFLAGS += -I$(DIMDIR)/dim

CC=g++
SFX=cxx
#CFLAGS = -Wall -g3 -I$(DIMDIR)/dim -I$(VMEBDIR)/vmeblib  -I$(VMECFDIR)/ctp 
CFLAGS +=-DCPLUSPLUS


$(ODIR)/MonScal: $(ODIR)/MonScal.o $(ODIR)/File.o $(ODIR)/TrigConf.o $(ODIR)/guiDict.o \
	$(ODIR)/ActiveRun.o $(ODIR)/Counter.o $(ODIR)/Output.o $(ODIR)/Log.o \
	$(ODIR)/Dim.o $(ODIR)/main.o
	$(CC) $(LDFLAGS) $(ODIR)/MonScal.o $(ODIR)/File.o $(ODIR)/ActiveRun.o $(ODIR)/guiDict.o \
	$(ODIR)/TrigConf.o $(ODIR)/Counter.o $(ODIR)/Output.o $(ODIR)/Log.o \
	$(ODIR)/Dim.o  $(ODIR)/main.o \
	$(LIBS) $(GLIBS) -o $(ODIR)/MonScal
#	rm $(ODIR)/*.o


$(ODIR)/MonScal.o:$(SDIR)/MonScal.h $(SDIR)/MonScal.cxx
	$(CC) $(CFLAGS) -c -o $(ODIR)/MonScal.o MonScal.$(SFX)

$(ODIR)/File.o:$(SDIR)/File.h $(SDIR)/File.cxx  
	$(CC) $(CFLAGS) -c -o $(ODIR)/File.o File.cxx

$(ODIR)/Dim.o:$(SDIR)/Dim.h $(SDIR)/Dim.cxx
	$(CC) $(CFLAGS) -c -o $(ODIR)/Dim.o Dim.cxx

$(ODIR)/TrigConf.o:$(SDIR)/TrigConf.h $(SDIR)/TrigConf.cxx  
	$(CC) $(CFLAGS) -c -o $(ODIR)/TrigConf.o TrigConf.cxx

$(ODIR)/ActiveRun.o:$(SDIR)/ActiveRun.h $(SDIR)/ActiveRun.cxx  
	$(CC) $(CFLAGS) -c -o $(ODIR)/ActiveRun.o ActiveRun.cxx

$(ODIR)/Counter.o:$(SDIR)/Counter.h $(SDIR)/Counter.cxx  
	$(CC) $(CFLAGS) -c -o $(ODIR)/Counter.o Counter.cxx

$(ODIR)/Output.o:$(SDIR)/Output.h $(SDIR)/Output.cxx  
	$(CC) $(CFLAGS) -c -o $(ODIR)/Output.o Output.cxx


$(ODIR)/Log.o:$(SDIR)/Log.h $(SDIR)/Log.cxx  
	$(CC) $(CFLAGS) -c -o $(ODIR)/Log.o Log.cxx

$(ODIR)/guiDict.o:$(SDIR)/guiDict.h $(SDIR)/guiDict.cxx  
	$(CC) $(CFLAGS) -c -o $(ODIR)/guiDict.o guiDict.cxx

$(ODIR)/main.o:$(SDIR)/main.cxx  
	$(CC) $(CFLAGS) -c -o $(ODIR)/main.o main.cxx

clean:
	-rm $(ODIR)/*.o

