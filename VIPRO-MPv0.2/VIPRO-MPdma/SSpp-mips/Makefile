
CXX = g++-3.4
CC  = gcc-3.4
AFLAGS = -Wall -w -g
#CFLAGS =  
LFLAGS =   -L../systemc-2.0.1/lib-linux -Wall -w -lm -Xlinker -zmuldefs 
CXXFLAGS=  -I../systemc-2.0.1/include -DSYSTEMC 
MFLAGS = `./sysprobe -flags`
MLIBS  = `./sysprobe -libs` -lm
ENDIAN = `./sysprobe -s`
MAKE = make
AR = ar cr
AROPT =
RANLIB = ranlib
RM = rm -f
RMDIR = rm -r -f
LN = ln -s
LNDIR = ln -s
DIFF = diff
OEXT = o
LEXT = a
EEXT =
CS = ;
X=/

OBJS =	sim-outorder.o simplescalar.o syscall.o memory.o regs.o \
	    loader.o endian.o symbol.o global.o	\
	    eval.o options.o stats.o eio.o \
	    range.o misc.o power.o\
	    machine.o cache.o bpred.o resource.o ptrace.o \
	    libexo.o

all:	libmips.$(LEXT)

libmips.$(LEXT):	$(OBJS)
	$(RM) libmips.$(LEXT)
	$(AR) $(AROPT)libmips.$(LEXT) $(OBJS) libexo/exolex.o
#	$(RANLIB) libmips.$(LEXT)

.cxx.o:	$*.cpp
	$(CXX) $(CFLAGS) -c $*.cpp
	
clean:
	rm -f *.o *.a
