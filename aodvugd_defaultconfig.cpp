#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD

#include "legacyapps/aodvugd/aodvugd_defaultconfig.h"
//#include "legacyapps/aodvugd/aodvugd_message.h"
//#include "sys/simulation/simulation_controller.h"
//#include "sys/node.h"
#include <iostream>
//#include <limits>
//#include <cmath>
#include <sstream> //tostring


#include <algorithm> //std::max
//#include "legacyapps/aodvugd/aodvugd_processor.h"

namespace aodvugd
{
	// ----------------------------------------------------------------------
	AodvUgdDefaultConfig::
	AodvUgdDefaultConfig()
	{}

	// ----------------------------------------------------------------------
	AodvUgdDefaultConfig::
	AodvUgdDefaultConfig(
		  	bool		 useHelloMessages_,
			bool		 useERS_,
        	bool	     useGratuitousRREP_,
        	bool    	 useDestOnly_,
        	bool 		 useLocalRepair_,
			double 		 ACTIVE_ROUTE_TIMEOUT_,
			unsigned int ALLOWED_HELLO_LOSS_,
			double 		 HELLO_INTERVAL_,
			unsigned int LOCAL_ADD_TTL_,
			unsigned int NET_DIAMETER_,
			double		 NODE_TRAVERSAL_TIME_,
			unsigned int RERR_RATELIMIT_,
			unsigned int RREQ_RETRIES_,
			unsigned int RREQ_RATELIMIT_,
			unsigned int TIMEOUT_BUFFER_,
			unsigned int TTL_START_,
			unsigned int TTL_INCREMENT_,
			unsigned int TTL_THRESHOLD_,
			unsigned int K_DELETE_PERIOD_
	)
	 :

	 //uso de funcionalidades
	 useHelloMessages		  {useHelloMessages_},
	 useERS					  {useERS_},
	 useGratuitousRREP		  {useGratuitousRREP_},
	 useDestOnly			  {useDestOnly_},
	 useLocalRepair			  {useLocalRepair_},

	 //Configuration Parameters
	 ACTIVE_ROUTE_TIMEOUT	  {ACTIVE_ROUTE_TIMEOUT_},//Milliseconds
	 ALLOWED_HELLO_LOSS	 	  {ALLOWED_HELLO_LOSS_},
	 HELLO_INTERVAL		  	  {HELLO_INTERVAL_}, //Milliseconds
	 LOCAL_ADD_TTL		  	  {LOCAL_ADD_TTL_},
   //MIN_REPAIR_TTL;          //see note below, es la ultima cantidad conocida de la ruta a reparar, cambia entre rutas
	 NET_DIAMETER			  {NET_DIAMETER_},
	 NODE_TRAVERSAL_TIME	  {NODE_TRAVERSAL_TIME_}, //milliseconds
	 RERR_RATELIMIT  		  {RERR_RATELIMIT_},
	 RREQ_RETRIES		      {RREQ_RETRIES_},
	 RREQ_RATELIMIT  		  {RREQ_RATELIMIT_},
	 TIMEOUT_BUFFER 		  {TIMEOUT_BUFFER_},
	 TTL_START 				  {corregirTtlStartPorDefecto(TTL_START_)}, //cambia si usa HELLO
	 TTL_INCREMENT			  {TTL_INCREMENT_},
	 TTL_THRESHOLD			  {TTL_THRESHOLD_},
  // TTL_VALUE    											see note below, el valor se encuentra en IP header
  // RING_TRAVERSAL_TIME      {2 * NODE_TRAVERSAL_TIME *    depende de TLL_VALUE
  //	 	 	 	 	 	 (TTL_VALUE + TIMEOUT_BUFFER)},

	 //dependen de otro valores
	 NET_TRAVERSAL_TIME       {2 * NODE_TRAVERSAL_TIME * NET_DIAMETER},
	 BLACKLIST_TIMEOUT  	  {devolverblackListTimeOutPorDefecto (TTL_THRESHOLD , TTL_START ,
		 	 	 	 	 			 	 	 	 	 	 	 	   TTL_INCREMENT , RREQ_RETRIES ,
																   NET_TRAVERSAL_TIME ,useERS )}, //cambia si se usa ERS
	 MAX_REPAIR_TTL           {0.3 * NET_DIAMETER},
	 MY_ROUTE_TIMEOUT         {2 * ACTIVE_ROUTE_TIMEOUT},
	 NEXT_HOP_WAIT            {NODE_TRAVERSAL_TIME + 10},
	 PATH_DISCOVERY_TIME      {2 * NET_TRAVERSAL_TIME},

	 // DELETE_PERIOD
	 K_DELETE_PERIOD		 {K_DELETE_PERIOD_},
	 DELETE_PERIOD 		 	 {devolverDeletePeriodPorDefecto	(K_DELETE_PERIOD_ , ACTIVE_ROUTE_TIMEOUT_ ,
			 	 	 	 	 	 	 	 	 	 	 	 	 	 HELLO_INTERVAL_)}


	{
		verificarActiveRouteTimeout();
	}

	// ----------------------------------------------------------------------
	AodvUgdDefaultConfig::
	~AodvUgdDefaultConfig()
	{}

	// get set----------------------------------------------------------------------
	bool
	AodvUgdDefaultConfig::
	getuseERS(void)
	throw(){
		return useERS;
	}
	// ----------------------------------------------------------------------
	bool
	AodvUgdDefaultConfig::
	getUseGratuitousRREP(void)
	throw(){
		return useGratuitousRREP;
	}
	// ----------------------------------------------------------------------
	bool
	AodvUgdDefaultConfig::
	getUseDestOnly(void)
	throw(){
		return useDestOnly;
	}
	// ----------------------------------------------------------------------
	bool
	AodvUgdDefaultConfig::
	getUseLocalRepair(void)
	throw(){
		return useLocalRepair;
	}
	// ----------------------------------------------------------------------
	double
	AodvUgdDefaultConfig::
	getACTIVE_ROUTE_TIMEOUT(void)
	throw(){
		return ACTIVE_ROUTE_TIMEOUT;
	}
	// ----------------------------------------------------------------------
	double
	AodvUgdDefaultConfig::
	getNODE_TRAVERSAL_TIME(void)
	throw(){
		return NODE_TRAVERSAL_TIME;
	}
	// ----------------------------------------------------------------------
	double
	AodvUgdDefaultConfig::
	getNET_TRAVERSAL_TIME(void)
	throw(){
		return NET_TRAVERSAL_TIME;
	}
	// ----------------------------------------------------------------------
	double
	AodvUgdDefaultConfig::
	getMY_ROUTE_TIMEOUT(void)
	throw(){
		return MY_ROUTE_TIMEOUT;
	}
	// ----------------------------------------------------------------------
	double
	AodvUgdDefaultConfig::
	getPATH_DISCOVERY_TIME(void)
	throw(){
		return PATH_DISCOVERY_TIME;
	}
	// ----------------------------------------------------------------------
	double
	AodvUgdDefaultConfig::
	getTIMEOUT_BUFFER(void)
	throw(){
		return TIMEOUT_BUFFER;
	}

	// ----------------------------------------------------------------------
	unsigned int
	AodvUgdDefaultConfig::
	getNET_DIAMETER(void)
	throw(){
		return NET_DIAMETER;
	}
	// ----------------------------------------------------------------------
	unsigned int
	AodvUgdDefaultConfig::
	getTTL_START(void)
	throw(){
		return TTL_START;
	}
	// ----------------------------------------------------------------------
	unsigned int
	AodvUgdDefaultConfig::
	getTTL_INCREMENT(void)
	throw(){
		return TTL_INCREMENT;
	}
	// ----------------------------------------------------------------------
	unsigned int
	AodvUgdDefaultConfig::
	getTTL_THRESHOLD(void)
	throw(){
		return TTL_THRESHOLD;
	}
	// ----------------------------------------------------------------------
	unsigned int
	AodvUgdDefaultConfig::
	getRREQ_RETRIES(void)
	throw(){
		return RREQ_RETRIES;
	}
	// ----------------------------------------------------------------------
	unsigned int
	AodvUgdDefaultConfig::
	getMAX_REPAIR_TTL(void)
	throw(){
		return MAX_REPAIR_TTL;
	}
	// ----------------------------------------------------------------------
	unsigned int
	AodvUgdDefaultConfig::
	getLOCAL_ADD_TTL(void)
	throw(){
		return LOCAL_ADD_TTL ;
	}
	// ----------------------------------------------------------------------
	unsigned int
	AodvUgdDefaultConfig::
	getRREQ_RATELIMIT(void)
	throw(){
		return RREQ_RATELIMIT ;
	}
	// ----------------------------------------------------------------------
	double
	AodvUgdDefaultConfig::
	getHELLO_INTERVAL(void)
	throw(){
		return HELLO_INTERVAL;
	}
	// ----------------------------------------------------------------------
	unsigned int
	AodvUgdDefaultConfig::
	getALLOWED_HELLO_LOSS(void)
	throw(){
		return ALLOWED_HELLO_LOSS;
	}
	// ----------------------------------------------------------------------
	double
	AodvUgdDefaultConfig::
	getDELETE_PERIOD(void)
	throw(){
		return DELETE_PERIOD;
	}
	// ----------------------------------------------------------------------
/*	double
	AodvUgdDefaultConfig::
	getACTIVE_ROUTE_TIMEOUT(void)
	throw(){
		return ACTIVE_ROUTE_TIMEOUT;
	}*/

	// ----------------------------------------------------------------------
	void
	AodvUgdDefaultConfig::
	verificarActiveRouteTimeout(void)
	throw(){
	//si se usa HELLO, ACTIVE_ROUTE_TIMEOUT >(ALLOWED_HELLO_LOSS * HELLO_INTERVAL)
	if (useHelloMessages) {
		if(!(ACTIVE_ROUTE_TIMEOUT >(ALLOWED_HELLO_LOSS * HELLO_INTERVAL))){
		//por defecto se cumple
		//					3000 >       2            *       1000

		//Warning, solo se advierte porque es posible que alguien quiera experimentar con otros valores
		std::cout<< "ACTIVE_ROUTE_TIMEOUT (" << ACTIVE_ROUTE_TIMEOUT <<
		")  parameter value MUST be more than the value (ALLOWED_HELLO_LOSS (" <<
		ALLOWED_HELLO_LOSS << " ) * HELLO_INTERVAL (" << HELLO_INTERVAL <<" ) " <<
		std::endl;
		}
	}   //despues hay que comparar al reparar
	}

	// ----------------------------------------------------------------------

//	 TTL_START should be
//	 set to at least 2 if Hello messages are used for local connectivity
//	 information.
	unsigned int
	AodvUgdDefaultConfig::
	corregirTtlStartPorDefecto(unsigned int ttlStart)
	throw()
	{
		unsigned int newTtlStart=ttlStart;
		if (useHelloMessages)
		{
		  if (ttlStart<2)
		  {
			newTtlStart=2;
			std::cout<< "TTL_START should be  set to at least 2 if Hello messages are used. TTL:  "<<
					 	 ttlStart  <<"   Change to TTL: "<< newTtlStart << std::endl;
		  }
		}
		return newTtlStart;
	}


	// ----------------------------------------------------------------------


//	 BLACKLIST_TIMEOUT should be suitably increased if
//	 an expanding ring search is used.  In such cases, it should be
//	 {[(TTL_THRESHOLD - TTL_START)/TTL_INCREMENT] + 1 + RREQ_RETRIES} *
//	 NET_TRAVERSAL_TIME.  This is to account for possible additional route
//	 discovery attempts.
	double
	AodvUgdDefaultConfig::
	devolverblackListTimeOutPorDefecto	(unsigned int TTL_THRESHOLD_ , unsigned int TTL_START_ ,
										 unsigned int TTL_INCREMENT_ , unsigned int RREQ_RETRIES_ ,
										 double		  NET_TRAVERSAL_TIME_ , bool useERS_ )
	throw(){
		std::cout<< " TTL_START:" <<TTL_START_	<< std::endl;
	double blackListTimeOut;
	if (useERS) {
		blackListTimeOut=(((TTL_THRESHOLD-TTL_START)/TTL_INCREMENT)+1+ RREQ_RETRIES)*NET_TRAVERSAL_TIME;
	}
	else {
		blackListTimeOut=RREQ_RETRIES * NET_TRAVERSAL_TIME;
	}
	return blackListTimeOut;
	}


	// ----------------------------------------------------------------------


	double
	AodvUgdDefaultConfig::
	devolverDeletePeriodPorDefecto	(unsigned int K_DELETE_PERIOD_ , double	ACTIVE_ROUTE_TIMEOUT_ ,
									 double		  HELLO_INTERVAL_)
	throw(){

	double	deletePeriod,max;
	max 			=	std::max (ACTIVE_ROUTE_TIMEOUT_, HELLO_INTERVAL_);
	deletePeriod	= 	K_DELETE_PERIOD_ * max;
	return deletePeriod;
	}


	// ----------------------------------------------------------------------


	std::string
	AodvUgdDefaultConfig::
	toString(void)
	throw()
	{
		{
		   std::stringstream ss;
		   ss <<
		     "useHelloMessages: "		<< 		  useHelloMessages 	<< '\n'<<
		  	 "useERS: "					<< 			  useERS		<<'\n'<<

		  	 //Configuration Parameters
		  	 "ACTIVE_ROUTE_TIMEOUT: "	<<   ACTIVE_ROUTE_TIMEOUT   <<'\n'<<
		  	 "ALLOWED_HELLO_LOSS:"		<< 	 ALLOWED_HELLO_LOSS		<<'\n'<<
		  	 "HELLO_INTERVAL: "	  	  	<<   HELLO_INTERVAL			<<'\n'<<
		  	 "LOCAL_ADD_TTL: "		  	<<   LOCAL_ADD_TTL			<<'\n'<<
		  	 "NET_DIAMETER: "			<<   NET_DIAMETER			<<'\n'<<
		  	 "NODE_TRAVERSAL_TIME: "	<<   NODE_TRAVERSAL_TIME	<<'\n'<<
		  	 "RERR_RATELIMIT: "  		<<   RERR_RATELIMIT			<<'\n'<<
		  	 "RING_TRAVERSAL_TIME: "    <<   RING_TRAVERSAL_TIME	<<'\n'<<
		  	 "RREQ_RETRIES: "		    <<   RREQ_RETRIES			<<'\n'<<
		  	 "RREQ_RATELIMIT: "  		<<	 RREQ_RATELIMIT			<<'\n'<<
		  	 "TIMEOUT_BUFFER: " 		<<   TIMEOUT_BUFFER			<<'\n'<<
		  	 "TTL_START: " 				<<   TTL_START				<<'\n'<<
		  	 "TTL_INCREMENT: "			<<   TTL_INCREMENT			<<'\n'<<
		  	 "TTL_THRESHOLD: "			<<   TTL_THRESHOLD			<<'\n'<<
		    // TTL_VALUE    see note below, el valor se encuentra en IP header

		  	 //dependen de otro valores
		  	 "NET_TRAVERSAL_TIME:"      <<  NET_TRAVERSAL_TIME		<<'\n'<<
			 "BLACKLIST_TIMEOUT: " 	  	<<	BLACKLIST_TIMEOUT 		<<'\n'<<
		  	 "MAX_REPAIR_TTL: "         <<  MAX_REPAIR_TTL			<<'\n'<<
		  	 "MY_ROUTE_TIMEOUT: "       <<  MY_ROUTE_TIMEOUT		<<'\n'<<
		  	 "NEXT_HOP_WAIT: "          <<  NEXT_HOP_WAIT			<<'\n'<<
		  	 "PATH_DISCOVERY_TIME: "      <<	PATH_DISCOVERY_TIME		<<'\n'<<

		  	 // DELETE_PERIOD
		  	 "K_DELETE_PERIOD: "		<<	 K_DELETE_PERIOD		<<'\n'<<
		  	 "DELETE_PERIOD: "			<< 	 DELETE_PERIOD			<<'\n';



		   return (ss.str());
		}
	}

	void
	AodvUgdDefaultConfig::
	show(void)
	throw()
	{
		std::cout << AodvUgdDefaultConfig::toString()<< '\n';

	}

}
#endif
