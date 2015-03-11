/*
 * timer.hpp
 *
 *  Created on: 04/11/2008
 *      Author: max
 *
 *
 *	Gera interrupcao de tempo em tempo
 */

#ifndef timer_HPP_
#define timer_HPP_

#include <systemc.h>

#include "timer_if.hpp"

SC_MODULE(timer)
{

public:
	sc_in_clk clock;
	sc_port<timer_if> interrupt_port;

	SC_HAS_PROCESS(timer);

	//simplescalar();
	timer(sc_module_name name_, int n_interrupt_)
	: sc_module(name_)
	{
		SC_METHOD(timer_count);
		dont_initialize();
		sensitive_neg << clock;
		count = 0;
		n_interrupt = n_interrupt_;
	}

private:
	void timer_count();
	unsigned int count;
	int n_interrupt;
};


#endif /* timer_HPP_ */
