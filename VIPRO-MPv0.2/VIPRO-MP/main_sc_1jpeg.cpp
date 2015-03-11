#include <systemc.h>
#include <unistd.h>

/*MSG*/
#include "SSpp-mips/simplescalar.hpp"
#include "bus/my_bus.hpp"
#include "bus/simple_bus_arbiter.hpp"
#include "timer.hpp"
#include "sharedmemory.hpp"

/*MSO (06/06/2008) Variable declared to obtain the environ directly
  In SystemC programs we dont have the main, but sc_main*/
extern char **environ;


int sc_main(int argc, char *argv[]){

  sharedmemory 	  *sharedmem  = new sharedmemory("mem",/*base*/ 0x80000000,/*final*/ 0x8fffffff,/*latency*/ 6);
  simple_bus_arbiter *arbiter = new simple_bus_arbiter("arbiter");
  my_bus 			 *bus 	  = new my_bus("bus", /*mode verbose*/ false);
//  timer 	*int_generator    = new timer("int_generator", /*number of interrupt*/ 1);

  //-----------------------------------------------------------------------------------
  const int argc1 = 14;
  char *argv1[argc1] = {argv[0],argv[1],argv[2],argv[3], argv[4], argv[5], argv[6],
			argv[7],argv[8],argv[9],argv[10],argv[11],argv[12],argv[13]};

  //-----------------------------------------------------------------------------------

 FILE *fd= fopen(argv[14], "w+" );
 sc_signal<bool> reset;
 sc_clock clock("CLOCK", 10, 0.5, 0.0);


 bus->clock(clock);
 sharedmem->clock(clock);
// int_generator->clock(clock);
 bus->slave_port(*sharedmem);
 bus->arbiter_port(*arbiter);

 simplescalar scsp("PROC1", 1);
 scsp.setFd(fd);  // setting a file to dump the stats
 scsp.CLK(clock);
 scsp.bus_port(*bus);
// int_generator->interrupt_port(scsp);
 scsp.init(argc1, argv1, environ);
 scsp.reset(reset); // Ok, now this port is not used, processor starts immediatly

 cout << "Starting";
 sc_start(clock,2e8);
cout << "Finished SystemC simulation" << endl;
 sc_stop();

// sharedmem->print_mem(0x80000000, 0x8fffffff);

 return 0;
}
