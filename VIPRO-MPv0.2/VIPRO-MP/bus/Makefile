
CXX = g++
CC  = gcc
AFLAGS = -Wall -w -g
LFLAGS =   -L../systemc-2.3.1/lib-linux -Wall -w -g -lm -Xlinker -zmuldefs 
CXXFLAGS=  -g -I../systemc-2.3.1/include -DSYSTEMC 
MFLAGS = `./sysprobe -flags`
MLIBS  = `./sysprobe -libs` -lm
ENDIAN = `./sysprobe -s`
MAKE = make
AR = ar qcv
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

OBJS = my_bus.o simple_bus_arbiter.o


all:	libbus.$(LEXT)

libbus.$(LEXT):	$(OBJS)
	$(RM) libbus.$(LEXT)
	$(AR) libbus.$(LEXT) $(OBJS) 
	$(RANLIB) libbus.$(LEXT)   


.cxx.o:	$*.cpp
	$(CXX) -c $*.cpp

clean:
	rm -f *.o *.a


