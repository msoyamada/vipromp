/*
 * int_mode_if.hpp
 *
 *  Created on: 04/11/2008
 *      Author: max
 */

#ifndef INT_MODE_HPP_
#define INT_MODE_HPP_

#include <systemc.h>



class timer_if
  : public virtual sc_interface
{
public:

	virtual void setInterruptMode(void) = 0;

};

#endif /* INT_TIME_HPP_ */
