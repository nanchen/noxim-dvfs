#-------------------------------------------------------
#              Unit Testing
#-------------------------------------------------------

TARGET_ARCH = linux
SYSTEMC = /home/nan/dev/systemc-2.2.0

CC     = g++
OPT    = -O0
DEBUG  = -g
OTHER  = -fpermissive # -Wall -Wno-deprecated
CFLAGS = $(OPT) $(OTHER) # $(DEBUG)

VPATH = ../tests:../src:.

SRCS := $(wildcard ../src/*.cpp)
OBJS = $(subst ../src/,,$(SRCS:.cpp=.o))

TEST_SRCS := $(wildcard ../tests/*.cpp)
TEST_OBJS = $(subst ../tests/,,$(TEST_SRCS:.cpp=.o))


INCDIR = -I. -I.. -I../src -I../tests -I$(SYSTEMC)/include 
LIBDIR = -L. -L.. -L../src -L$(SYSTEMC)/lib-$(TARGET_ARCH)

LIBS = -lsystemc -lm $(EXTRA_LIBS)

.SUFFIXES: .cpp .o 

runAll: runTests 


all: TestMain

runTests: TestMain
	./NoximQTestMain
	
TestMain: $(OBJS) $(TEST_OBJS) NoximQTestMain.o
	$(CC) $(CFLAGS) $(INCDIR) $(LIBDIR) -o $@ $(OBJS) $(TEST_OBJS) NoximQTestMain.o $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCDIR) -c -g -fpermissive  $<

%.d: %.cpp
		@set -e; rm -f $@; \
         $(CC) -M $(CFLAGS) $< >; $@.$$$$; \
         sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ >; $@; \
         rm -f $@.$$$$

clean:
	rm -f $(OBJS) *~ core *.o
