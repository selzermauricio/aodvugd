#ifndef __SHAWN_LEGACYAPPS_AODVUGD_DEFAULTCONFIG_H
#define __SHAWN_LEGACYAPPS_AODVUGD_DEFAULTCONFIG_H
#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD

#include <string>
//#include <map>
//#include <limits>
//#include "sys/processor.h"
//#include "sys/event_scheduler.h"
//#include "legacyapps/aodvugd/aodvugd_message.h"


namespace aodvugd
{

   class AodvUgdDefaultConfig
   {
   public:
      ///@name Constructor/Destructor
      ///@{
	  AodvUgdDefaultConfig();
	  AodvUgdDefaultConfig(
			  bool		useHelloMessages_,
				bool		useERS_,
        bool    useGratuitousRREP_,
        bool    useDestOnly_,
        bool    useLocalRepair_,

				double 		   ACTIVE_ROUTE_TIMEOUT_,
				unsigned int ALLOWED_HELLO_LOSS_,
				double 		   HELLO_INTERVAL_,
				unsigned int LOCAL_ADD_TTL_,
				unsigned int NET_DIAMETER_,
				double		   NODE_TRAVERSAL_TIME_,
				unsigned int RERR_RATELIMIT_,
				unsigned int RREQ_RETRIES_,
				unsigned int RREQ_RATELIMIT_,
				unsigned int TIMEOUT_BUFFER_,
				unsigned int TTL_START_,
				unsigned int TTL_INCREMENT_,
				unsigned int TTL_THRESHOLD_,
				unsigned int K_DELETE_PERIOD_
		);
      ~AodvUgdDefaultConfig();  //destructor no necesita virtual
      ///@}

      //strings
      std::string	toString(void)	throw();
      void show(void)	throw();

  // get set ----------------------------------------------------------------------
      bool	       getuseERS               ( void ) throw();
      bool         getUseGratuitousRREP    ( void ) throw();
      bool         getUseDestOnly          ( void ) throw();
      bool         getUseLocalRepair       ( void ) throw();
      double       getACTIVE_ROUTE_TIMEOUT ( void ) throw();
      double       getNODE_TRAVERSAL_TIME  ( void )	throw();
      double       getNET_TRAVERSAL_TIME   ( void )	throw();
      double       getMY_ROUTE_TIMEOUT     ( void )	throw();
      double       getPATH_DISCOVERY_TIME  ( void )	throw();
      double       getTIMEOUT_BUFFER       ( void )	throw();
      double       getHELLO_INTERVAL       ( void ) throw();
      double       getDELETE_PERIOD        ( void ) throw();
      unsigned int getALLOWED_HELLO_LOSS   ( void )	throw();
      unsigned int getNET_DIAMETER         ( void ) throw();
      unsigned int getTTL_START            ( void )	throw();
      unsigned int getTTL_INCREMENT        ( void )	throw();
      unsigned int getTTL_THRESHOLD        ( void )	throw();
      unsigned int getRREQ_RETRIES         ( void )	throw();
      unsigned int getMAX_REPAIR_TTL       ( void ) throw();
      unsigned int getLOCAL_ADD_TTL        ( void ) throw();
      unsigned int getRREQ_RATELIMIT       ( void ) throw();
      //double getACTIVE_ROUTE_TIMEOUT(void)	throw();

   private:
      ///@name
      ///@{
      /**
       */
      void verificarActiveRouteTimeout(void) throw();
      unsigned int corregirTtlStartPorDefecto(unsigned int ttlStart) throw();

      double	  devolverblackListTimeOutPorDefecto
	  	  	  	  ( unsigned int TTL_THRESHOLD_ , unsigned int TTL_START_ ,
				          unsigned int TTL_INCREMENT_ , unsigned int RREQ_RETRIES_ ,
				          double  NET_TRAVERSAL_TIME_ , bool useERS_ ) throw();

      double
  	  devolverDeletePeriodPorDefecto	(unsigned int K_DELETE_PERIOD_ ,
                                       double	      ACTIVE_ROUTE_TIMEOUT_ ,
  			  	  	  	  	  	  	  	   double		    HELLO_INTERVAL_) throw();


      ///@}
            
      // --------------------------------------------------------------------


  	  //uso de funcionalidades
		bool          useHelloMessages;
		bool          useERS;
    bool          useGratuitousRREP;
    bool          useDestOnly;
    bool          useLocalRepair;

		//Configuration Parameters
		double 		    ACTIVE_ROUTE_TIMEOUT;	  //3000 Milliseconds
    unsigned int  ALLOWED_HELLO_LOSS;  	  //2;
    double		    DELETE_PERIOD;          //see note below,  se calcula abajo
		double 		    HELLO_INTERVAL;		      //1000; Milliseconds
    unsigned int  LOCAL_ADD_TTL;			    //2;
//		unsigned int MIN_REPAIR_TTL;        //see note below, es la ultima cantidad conocida de
											  //la ruta a reparar, cambia entre rutas
		unsigned int  NET_DIAMETER;			      //35;
		double		    NODE_TRAVERSAL_TIME;	  //40; //milliseconds
		unsigned int  RERR_RATELIMIT;	        //10;
		double		    RING_TRAVERSAL_TIME;    //2 * NODE_TRAVERSAL_TIME *
                               			      //(TTL_VALUE + TIMEOUT_BUFFER)
		unsigned int  RREQ_RETRIES;		        //2;
    unsigned int  RREQ_RATELIMIT;	        //10;
    unsigned int  TIMEOUT_BUFFER;		      //2;
    unsigned int  TTL_START;				      //1;			  //cambia si usa HELLO
    unsigned int  TTL_INCREMENT;			    //2;
    unsigned int  TTL_THRESHOLD;			    //7;
//      unsigned int TTL_VALUE                see note below, el valor se encuentra en IP header

		//dependen de otro valores
    double		    NET_TRAVERSAL_TIME;     //2 * NODE_TRAVERSAL_TIME * NET_DIAMETER;
		double		    BLACKLIST_TIMEOUT; 	    //RREQ_RETRIES * NET_TRAVERSAL_TIME; //cambia si se usa ERS
		unsigned int  MAX_REPAIR_TTL;         //0.3 * NET_DIAMETER;
		double		    MY_ROUTE_TIMEOUT;       //2 * ACTIVE_ROUTE_TIMEOUT;
		double		    NEXT_HOP_WAIT;          //NODE_TRAVERSAL_TIME + 10;
		double		    PATH_DISCOVERY_TIME;    //2 * NET_TRAVERSAL_TIME;

		//(K = 5 is recommended).
		int           K_DELETE_PERIOD;
	    //DELETE_PERIOD = K * max (ACTIVE_ROUTE_TIMEOUT, HELLO_INTERVAL),

   };
}

	//MIN_REPAIR_TTL
/* The MIN_REPAIR_TTL should be the last known hop count to the
 destination.  If Hello messages are used, then the
 ACTIVE_ROUTE_TIMEOUT parameter value MUST be more than the value
 (ALLOWED_HELLO_LOSS * HELLO_INTERVAL).  For a given
 ACTIVE_ROUTE_TIMEOUT value, this may require some adjustment to the
 value of the HELLO_INTERVAL, and consequently use of the Hello
 Interval Extension in the Hello messages.*/


 // no se implementa, se deja que el usuario defina si no quiere esperar. debe  poner en
 // TIMEOUT_BUFFER = 0.
/* TTL_VALUE is the value of the TTL field in the IP header while the
 expanding ring search is being performed.  This is described further
 in section 6.4.  The TIMEOUT_BUFFER is configurable.  Its purpose is
 to provide a buffer for the timeout so that if the RREP is delayed
 due to congestion, a timeout is less likely to occur while the RREP
 is still en route back to the source.  To omit this buffer, set
 TIMEOUT_BUFFER = 0.*/



 /*DELETE_PERIOD is intended to provide an upper bound on the time for
 which an upstream node A can have a neighbor B as an active next hop
 for destination D, while B has invalidated the route to D.  Beyond
 this time B can delete the (already invalidated) route to D.  The
 determination of the upper bound depends somewhat on the
 characteristics of the underlying link layer.  If Hello messages are
 used to determine the continued availability of links to next hop
 nodes, DELETE_PERIOD must be at least ALLOWED_HELLO_LOSS *
 HELLO_INTERVAL.  If the link layer feedback is used to detect loss of
 link, DELETE_PERIOD must be at least ACTIVE_ROUTE_TIMEOUT.  If hello
 messages are received from a neighbor but data packets to that
 neighbor are lost (e.g., due to temporary link asymmetry), we have to
 make more concrete assumptions about the underlying link layer. We
 assume that such asymmetry cannot persist beyond a certain time, say,
 a multiple K of HELLO_INTERVAL.  In other words, a node will
 invariably receive at least one out of K subsequent Hello messages
 from a neighbor if the link is working and the neighbor is sending no
 other traffic.  Covering all possibilities,
    DELETE_PERIOD = K * max (ACTIVE_ROUTE_TIMEOUT, HELLO_INTERVAL)
                       (K = 5 is recommended).
 NET_DIAMETER measures the maximum possible number of hops between two
 nodes in the network.  NODE_TRAVERSAL_TIME is a conservative estimate
 of the average one hop traversal time for packets and should include
 queuing delays, interrupt processing times and transfer times.
 ACTIVE_ROUTE_TIMEOUT SHOULD be set to a longer value (at least 10,000
 milliseconds) if link-layer indications are used to detect link
 breakages such as in IEEE 802.11 [5] standard.  TTL_START should be
 set to at least 2 if Hello messages are used for local connectivity
 information.  Performance of the AODV protocol is sensitive to the
 chosen values of these constants, which often depend on the
 characteristics of the underlying link layer protocol, radio
 technologies etc.  BLACKLIST_TIMEOUT should be suitably increased if
 an expanding ring search is used.  In such cases, it should be
 {[(TTL_THRESHOLD - TTL_START)/TTL_INCREMENT] + 1 + RREQ_RETRIES} *
 NET_TRAVERSAL_TIME.  This is to account for possible additional route
 discovery attempts.*/

#endif
#endif
