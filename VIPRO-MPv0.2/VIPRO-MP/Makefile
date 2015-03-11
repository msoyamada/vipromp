
CXX = g++
CC  = gcc
AFLAGS = -Wall -w -g
#CFLAGS =  
LFLAGS =   -L./systemc-2.3.1/lib-linux -Wall -w -g -lm -Xlinker -zmuldefs -lpthread 
CXXFLAGS=-g -I./systemc-2.3.1/include -DSYSTEMC  -fpermissive
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


COMPONENTS = bus/libbus.$(LEXT) SSpp-mips/libmips.$(LEXT)
OBJS =	main_sc.o timer.o sharedmemory.o

all:	vipro-mp

vipro-mp:	$(OBJS) $(COMPONENTS) cacti/libcacti.a
	$(RM) vipro-mp
	$(CXX) $(LFLAGS) -o vipro-mp $(OBJS) $(COMPONENTS) cacti/libcacti.a systemc-2.3.1/lib-linux/libsystemc.a  -lpthread

cacti/libcacti.a: 
		cd cacti $(CS) \
		$(MAKE) "MAKE=$(MAKE)" "CC=$(CC)" "AR=$(AR)" "AROPT=$(AROPT)" "RANLIB=$(RANLIB)" "CFLAGS=$(FFLAGS) $(SAFEOFLAGS)" "OEXT=$(OEXT)" "LEXT=$(LEXT)" "EEXT=$(EEXT)" "X=$(X)" "RM=$(RM)" libcacti.$(LEXT)

.cxx.o:	$*.cpp
	$(CXX) $(CFLAGS) -c $*.cpp
		
clean:
	rm -f *.o vipro-mp
