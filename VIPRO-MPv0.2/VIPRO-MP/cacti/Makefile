## these are set in ../Makefile
## CC, AR, AROPT, CFLAGS, RANLIB
#
# all the sources
#

SRC	= main.c time.c area.c io.c leakage.c basic_circuit.c
HDR	= def.h areadef.h leakage.h basic_circuit.h io.h time.h cacti_interface.h
OEXT = o
#
# common objects
#
OBJ = time.$(OEXT) area.$(OEXT) io.$(OEXT) leakage.$(OEXT) basic_circuit.$(OEXT)
LEXT = a
AROPT= qcv 
FLAGS = -g
#
# all targets
#
all: libcacti.$(LEXT)
	@echo "my work is done here..."

libcacti.$(LEXT):	$(OBJ)
	$(RM) libcacti.$(LEXT)
	$(AR) $(AROPT) libcacti.$(LEXT) $(OBJ)
	$(RANLIB) libcacti.$(LEXT)
	
cacti : main.o time.o area.o io.o leakage.o basic_circuit.o
	  $(CC) $(FLAGS) $(OBJS) -o cacti $(LIBS)

main.o : main.c def.h areadef.h leakage.h basic_circuit.h
	  $(CC) $(FLAGS) -c main.c -o main.o

leakage.o : leakage.h leakage.c
	  $(CC) $(FLAGS) -c leakage.c -o leakage.o

time.o :  time.c def.h areadef.h leakage.h basic_circuit.h cacti_interface.h
	   $(CC) $(FLAGS) -c time.c -o time.o

area.o : area.c def.h areadef.h cacti_interface.h
	   $(CC) $(FLAGS) -c area.c -o area.o 

io.o : def.h io.c areadef.h cacti_interface.h
	  $(CC) $(FLAGS) -c io.c -o io.o

basic_circuit.o : basic_circuit.h basic_circuit.c
		   $(CC) $(FLAGS) -c basic_circuit.c -o basic_circuit.o 

.c.$(OEXT):
	$(CC) $(CFLAGS) -c $*.c

filelist:
	@echo $(SRC) $(HDR) Makefile

diffs:
	-rcsdiff RCS/*

clean:
	-$(RM) *.o *.obj core *~ Makefile.bak libcacti.a

unpure:
	rm -f sim.pure *pure*.o sim.pure.pure_hardlink sim.pure.pure_linkinfo

depend:
	makedepend.local -n -x $(SRC)


# DO NOT DELETE THIS LINE -- make depend depends on it.

cacti.$(OEXT): def.h
io.$(OEXT): def.h
 
