
#include "my_bus.hpp"
#include "simple_bus_tools.hpp"

/*

void simple_bus::end_of_elaboration()
{
  // perform a static check for overlapping memory areas of the slaves
  bool no_overlap;
  for (int i = 1; i < slave_port.size(); ++i) {
    simple_bus_slave_if *slave1 = slave_port[i];
    for (int j = 0; j < i; ++j) {
      simple_bus_slave_if *slave2 = slave_port[j];
      no_overlap = ( slave1->end_address() < slave2->start_address() ) ||
               ( slave1->start_address() > slave2->end_address() );
      if ( !no_overlap ) {
        sb_fprintf(stdout,"Error: overlapping address spaces of 2 slaves : \n");
        sb_fprintf(stdout,"slave %i : %0X..%0X\n",i,slave1->start_address(),slave1->end_address());
        sb_fprintf(stdout,"slave %i : %0X..%0X\n",j,slave2->start_address(),slave2->end_address());
        exit(0);
      }
    }
  }
}

*/

//----------------------------------------------------------------------------
//-- process
//----------------------------------------------------------------------------

void my_bus::main_action()
{
  // m_current_request is cleared after the slave is done with a
  // single data transfer. Burst requests require the arbiter to
  // select the request again.

	if (!m_current_request)
		m_current_request = get_next_request();
	else
		// monitor slave wait states
		if (m_verbose)
			sb_fprintf(stdout, "%g SLV [%d]\n", sc_simulation_time(),
					m_current_request->address);
	if (m_current_request)
		handle_request();
	if (!m_current_request)
		clear_locks();

}


simple_bus_status my_bus::read_ss(unsigned int addr,				/* target address to access - word - unsigned int - 32bits*/
						void *host,									/* host memory address to access */
						unsigned int nbytes,
						unsigned int unique_priority,
						bool lock)
{
	if (m_verbose)
	    sb_fprintf(stdout, "%g %s : burst_read(%d) @ %x\n",
		       sc_simulation_time(), name(), unique_priority, addr);

	simple_bus_request *request = get_request(unique_priority);

	request->do_write           = false; // we are reading
	request->address            = addr;
	request->end_address        = addr + nbytes - 1;
	request->nbytes				= nbytes;
	request->data               = host;


	if (lock)
		request->lock = (request->lock == SIMPLE_BUS_LOCK_SET) ?
				SIMPLE_BUS_LOCK_GRANTED : SIMPLE_BUS_LOCK_SET;

	request->status = SIMPLE_BUS_REQUEST;
	wait(request->transfer_done);
	wait(clock->posedge_event());

	return request->status;

	// antigo
	//return slave_port->read(addr, host, nbytes);
}

simple_bus_status my_bus::write_ss(unsigned int addr,								/* target address to access - word - unsigned int - 32bits*/
						void *host,													/* host memory address to access */
						unsigned int nbytes,
						unsigned int unique_priority,
						bool lock)
{
	if (m_verbose)
	    sb_fprintf(stdout, "%g %s : burst_write(%d) @ %x\n",
		       sc_simulation_time(), name(), unique_priority, addr);

	simple_bus_request *request = get_request(unique_priority);

	request->do_write           = true; // we are writing
	request->address            = addr;
	request->end_address        = addr + nbytes - 1;
	request->nbytes				= nbytes;
	request->data               = host;


	if (lock)
		request->lock = (request->lock == SIMPLE_BUS_LOCK_SET) ?
				SIMPLE_BUS_LOCK_GRANTED : SIMPLE_BUS_LOCK_SET;

	request->status = SIMPLE_BUS_REQUEST;

	wait(request->transfer_done);
	wait(clock->posedge_event());
	return request->status;
}

//----------------------------------------------------------------------------
//-- BUS methods:
//
//     handle_request()   : performs atomic bus-to-slave request
//     get_request()      : BUS-interface: gets the request form of given
//                          priority
//     get_next_request() : returns a valid request out of the list of
//                          pending requests
//     clear_locks()      : downgrade the lock status of the requests once
//                          the transfer is done
//----------------------------------------------------------------------------

void my_bus::handle_request()
{
  if (m_verbose)
      sb_fprintf(stdout, "%g %s Handle Slave(%d)\n",
		 sc_simulation_time(), name(), m_current_request->priority);

  m_current_request->status = SIMPLE_BUS_WAIT;
  simple_bus_slave_if *slave = get_slave(m_current_request->address);

  /* address not word alligned */
  if (m_current_request->nbytes  > 1)
	 if (((m_current_request->nbytes == 2) && (m_current_request->address)%2 != 0) ||
		 ((m_current_request->nbytes > 2) && (m_current_request->address)%4 != 0)) {
			  sb_fprintf(stdout, "  BUS ERROR --> address %04X not word alligned\n",m_current_request->address);
			  m_current_request->status = SIMPLE_BUS_ERROR;
			  m_current_request = (simple_bus_request *)0;
			  return;
	  }

  if (!slave) {
    sb_fprintf(stdout, "  BUS ERROR --> no slave for address %04X \n",m_current_request->address);
    m_current_request->status = SIMPLE_BUS_ERROR;
    m_current_request = (simple_bus_request *)0;
    return;
  }

  simple_bus_status slave_status = SIMPLE_BUS_OK;
  if (m_current_request->do_write)
    slave_status = slave->write(m_current_request->data,
				m_current_request->address, m_current_request->nbytes);
  else
    slave_status = slave->read(m_current_request->data,
			       m_current_request->address, m_current_request->nbytes);

  if (m_verbose)
    sb_fprintf(stdout, "  --> status=(%s)\n", simple_bus_status_str[slave_status]);

  switch(slave_status)
  {
  case SIMPLE_BUS_ERROR:
	  m_current_request->status = SIMPLE_BUS_ERROR;
	  m_current_request->transfer_done.notify();
	  m_current_request = (simple_bus_request *)0;
	  break;
  case SIMPLE_BUS_OK:
	  //m_current_request->address+=4; //next word (byte addressing)
	  //printf("0x%X, 0x%X", m_current_request->address, m_current_request->end_address);

	  //if (m_current_request->address > m_current_request->end_address)
	  //{
		  // burst-transfer (or single transfer) completed
		  m_current_request->status = SIMPLE_BUS_OK;
		  m_current_request->transfer_done.notify();
		  m_current_request = (simple_bus_request *)0;
	  //}
	  //else
	  //{ // more data to transfer, but the (atomic) slave transfer is done
	  //	  byte_t *p = (byte_t*) m_current_request->data;
	  //	  m_current_request->data = &p[4];
	  //      m_current_request = (simple_bus_request *)0;
	  //}
	  break;
  case SIMPLE_BUS_WAIT:
	  // the slave is still processing: no clearance of the current request
	  break;
  default:
	  break;
  }
}

simple_bus_slave_if *my_bus::get_slave(unsigned int address)
{
	for (int i = 0; i < slave_port.size(); ++i)
	{
		simple_bus_slave_if *slave = slave_port[i];
		if ((slave->getLowLimit() <= address) &&
				(address <= slave->getHighLimit()))
			return slave;
	}
	return (simple_bus_slave_if *)0;
}

simple_bus_request * my_bus::get_request(unsigned int priority)
{
  simple_bus_request *request = (simple_bus_request *)0;
  for (int i = 0; i < m_requests.size(); ++i)
    {
      request = m_requests[i];
      if ((request) &&
	  (request->priority == priority))
	return request;
    }
  request = new simple_bus_request;
  request->priority = priority;
  m_requests.push_back(request);
  return request;
}

simple_bus_request * my_bus::get_next_request()
{
  // the slave is done with its action, m_current_request is
  // empty, so go over the bag of request-forms and compose
  // a set of likely requests. Pass it to the arbiter for the
  // final selection
  simple_bus_request_vec Q;
  for (int i = 0; i < m_requests.size(); ++i)
  {
	  simple_bus_request *request = m_requests[i];
	  if ((request->status == SIMPLE_BUS_REQUEST) ||
			  (request->status == SIMPLE_BUS_WAIT))
	  {
		  if (m_verbose)
			  sb_fprintf(stdout, "%g %s : request (%d) [%s]\n",
					  sc_simulation_time(), name(),
					  request->priority, simple_bus_status_str[request->status]);
		  Q.push_back(request);
	  }
  }
  if (Q.size() > 0)
    return arbiter_port->arbitrate(Q);

  return (simple_bus_request *)0;
}

void my_bus::clear_locks()
{
  for (int i = 0; i < m_requests.size(); ++i)
    if (m_requests[i]->lock == SIMPLE_BUS_LOCK_GRANTED)
      m_requests[i]->lock = SIMPLE_BUS_LOCK_SET;
    else
      m_requests[i]->lock = SIMPLE_BUS_LOCK_NO;
}

int my_bus::get_latency(unsigned int address)
{
	simple_bus_slave_if *slave = get_slave(address);
	if (slave)
		return slave->get_latency();
	return 0;
}


