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
 
 
  sharedmemory 	  *sharedmem  = new sharedmemory("mem",/*base*/ 0x80000000,/*final*/ 0x8fffffff,/*latency*/ 25);
  simple_bus_arbiter *arbiter = new simple_bus_arbiter("arbiter");
  my_bus 			 *bus 	  = new my_bus("bus", /*mode verbose*/ false);
//  timer 	*int_generator    = new timer("int_generator", /*number of interrupt*/ 1);

	/* atribuindo os paramentro dos processadores */
	// PROC1
	const int argc1 = 14;
	char *argv1[argc1] = {argv[0],argv[1],argv[2],argv[3], argv[4], argv[5], argv[6],
			argv[7],argv[8],argv[9],argv[10],argv[11],argv[12], argv[14]};

	// PROC2
	const int argc2 = 13;
	char *argv2[argc2] = {argv[0],argv[1],argv[2],argv[3], argv[4], argv[5],argv[6],
			argv[13],argv[8],argv[9],argv[10],argv[11],argv[12]};

	FILE *fd= fopen(argv[15], "w+" );
	sc_signal<bool> reset;

	/* instanciando o clock da simulação */
	sc_clock clock("CLOCK", 10, 0.5, 0.0);

	/* ligando o sinal de clock na porta de clock dos componentes */
	bus->clock(clock);
	sharedmem->clock(clock);
//	int_generator->clock(clock);

	bus->slave_port(*sharedmem);
	bus->arbiter_port(*arbiter);

	/* Processador */
	simplescalar scsp("PROC1", 1);
	scsp.setFd(fd);  							// atribuindo o arquivo output
	scsp.CLK(clock);							// ligando o sinal de clock no procesador
	scsp.bus_port(*bus);						// ligando a porta do barramento com o barramento
//	int_generator->interrupt_port(scsp);		// ligando a porta da interrupcao no gerador de int.
	scsp.init(argc1, argv1, environ);			// passando os parametros de PROC1
	scsp.reset(reset);

	/* Processador */
	simplescalar scsp2("PROC2", 2);
	scsp2.setFd(fd);
	scsp2.CLK(clock);
	scsp2.bus_port(*bus);
//	int_generator->interrupt_port(scsp2);
	scsp2.init(argc2, argv2, environ);
	scsp2.reset(reset);

	cout << "Starting";
	sc_start(3e8, SC_NS);							// iniciando o clock e todas as threads
	cout << "Finished SystemC simulation" << endl;
	sc_stop();

	//mshared->print_mem(0x80090000, 0x8fffffff);	// imprime a memoria compartilhada, dado o intervalo

	return 0;
}
