#include <systemc.h>
#include <unistd.h>
#include "simplescalar.hpp"
/*MSG*/
#include "memoryshared.hpp"
#include "bus/my_bus.hpp"
#include "bus/simple_bus_arbiter.hpp"

/*MSO (06/06/2008) Variable declared to obtain the environ directly
  In SystemC programs we dont have the main, but sc_main*/
extern char **environ;


int sc_main(int argc, char *argv[]){

  memshared 		*mshared  = new memshared("mem", 0x80000000, 0x8fffffff);
  simple_bus_arbiter *arbiter = new simple_bus_arbiter("arbiter");
  my_bus 			*bus 	  = new my_bus("bus", true);

  //-----------------------------------------------------------------------------------
  const int argc1 = 8;
  char *argv1[argc1] = {argv[0],argv[1],argv[2],argv[3], argv[4], argv[5],argv[6],
			argv[7]};

/*  const int argc2 = 8;
  char *argv2[argc2] = {argv[0],argv[1],argv[2],argv[3], argv[4], argv[5],argv[6],
  			argv[8]};
*/
  //-----------------------------------------------------------------------------------

 FILE *fd= fopen("stats.txt", "w+" );
 sc_signal<bool> reset;
 sc_clock clock("CLOCK", 10, 0.5, 0.0);

 bus->clock(clock);
 mshared->clock(clock);

 bus->slave_port(*mshared);
 bus->arbiter_port(*arbiter);

 simplescalar scsp("PROC1", 1);
 scsp.setFd(fd);  // setting a file to dump the stats
 scsp.CLK(clock);
 scsp.bus_port(*bus);
 scsp.init(argc1, argv1, environ);
 scsp.reset(reset); // Ok, now this port is not used, processor starts immediatly

/*
 simplescalar scsp2("PROC2", 2);
 scsp2.setFd(fd);  // setting a file to dump the stats
 scsp2.CLK(clock);
 scsp2.bus_port(*bus);
 scsp2.init(argc2, argv2, environ);
 scsp2.reset(reset); // Ok, now this port is not used, processor starts immediatly
*/

 cout << "Starting";
 sc_start(clock,1e7);
cout << "Finished SystemC simulation" << endl;
 sc_stop();

// mshared->print_mem(0x80000000, 0x8fffffff);

 return 0;
}
