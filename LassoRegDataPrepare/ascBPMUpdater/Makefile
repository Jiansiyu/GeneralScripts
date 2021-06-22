OS   := $(shell uname)
ARCH := $(shell arch)

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)

INCLUDES      = $(ROOTCFLAGS) $(addprefix -I, $(INCDIRS))

CXX           = g++
CXXFLAGS      = -O3
LD            = g++
LDFLAGS       = -O3 
DEFINES       = -DLINUXVERS -DHAS_SSTREAM
CXXFLAGS     += -Wall -Woverloaded-virtual -fPIC -fno-strict-aliasing

CXXFLAGS     += $(DEFINES) $(INCLUDES)
LIBS         += $(ROOTLIBS)
GLIBS        += $(ROOTGLIBS)

SRCDIR := src

all: trans

getlist: getlist.o
	$(LD) -g $(CXXFLAGS) -o $@ $< $(LIBS)

trans: trans.o
	$(LD) -g $(LDFLAGS) -o $@ $<

trans2: trans2.o
	$(LD) -g $(LDFLAGS) -o $@ $<

clean:
	rm -f *.o getlist trans

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -o $@ -c $<
