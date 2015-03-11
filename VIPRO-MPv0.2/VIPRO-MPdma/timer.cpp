/*
 * int_time.cpp
 *
 *  Created on: 04/11/2008
 *      Author: max
 */

#include "timer.hpp"

void timer::timer_count(){

	count++;
	if ((count%10000 == 0)&&(n_interrupt > 0))
	{
		interrupt_port->setInterruptMode();
		n_interrupt--;
	}

}
