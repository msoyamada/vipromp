#include <systemc.h>
#include <unistd.h>
#include "SSpp-mips/simplescalar.hpp"
/*MSG*/
#include "sharedmemory.hpp"
#include "bus/my_bus.hpp"
#include "bus/simple_bus_arbiter.hpp"

/*MSK(12/06/2009) including the dma in jpeg process*/
#include "dma/dma.hpp"
#include "dma/simple_dma_arbiter.hpp"

/*MSO (06/06/2008) Variable declared to obtain the environ directly
  In SystemC programs we dont have the main, but sc_main*/
extern char **environ;


int sc_main(int argc, char *argv[]){

  sharedmemory 		*mshared  = new sharedmemory("mem", 0x80000000, 0x8fffffff);
  simple_bus_arbiter *arbiter = new simple_bus_arbiter("arbiter");
  my_bus 			*bus 	  = new my_bus("bus", false);

  simple_dma_arbiter *arb = new simple_dma_arbiter("arbiter_of_DMA");
  dma *mydma = new dma("dma", /*working*/false,/*base*/ 0x90000000,/*final*/ 0x90000103, /*latency*/ 25);

  //-----------------------------------------------------------------------------------
  const int argc1 = 14;
  char *argv1[argc1] = {argv[0],argv[1],argv[2],argv[3], argv[4], argv[5], argv[6],
		  argv[7],argv[8],argv[9],argv[10],argv[11],argv[12],argv[13]};

  //-----------------------------------------------------------------------------------

 FILE *fd= fopen(argv[14], "w+" );
 sc_signal<bool> reset;
 sc_clock clock("CLOCK", 10, 0.5, 0.0);

 bus->clock(clock);
 mshared->clock(clock);
 mydma->clock(clock);

 bus->slave_port(*mshared);
 bus->slave_port(*mydma);
 bus->arbiter_port(*arbiter);

 mydma->arbiter_port(*arb);
 mydma->bus_port(*bus);

 simplescalar scsp("PROC1", 1);
 scsp.setFd(fd);  // setting a file to dump the stats
 scsp.CLK(clock);
 scsp.bus_port(*bus);
 scsp.init(argc1, argv1, environ);
 scsp.reset(reset); // Ok, now this port is not used, processor starts immediatly

 mydma->aloc_sim(scsp.SIM, 1);

 cout << "Starting";
 sc_start(clock,2e10);
cout << "Finished SystemC simulation" << endl;
 sc_stop();

 //printf("\nMemoria gerada antes da copia\n");
 //mshared->print_mem(0x80001000, 0x80090000);
 ///printf("\nMemoria gerada apos copia\n");
 mshared->print_mem(0x80090000, 0x800b0000);

 return 0;
}
