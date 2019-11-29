#ifndef __SHAWN_LEGACYAPPS_AODVUGD_PROCESSORHELLO_H
#define __SHAWN_LEGACYAPPS_AODVUGD_PROCESSORHELLO_H
#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD

#include <map>
#include <list>
#include <set>
#include <vector>
#include <limits>
#include "sys/processor.h"
#include "sys/event_scheduler.h"
//#include "legacyapps/aodvugd/aodvugd_message.h"
//#include "sys/simulation/simulation_controller.h"

namespace aodvugd
{
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------	
// clase HelloEscucharEventTag----------------------------------------------------------------------
	class HelloEscucharEventTag : public shawn::EventScheduler::EventTag
	{
	public:
		//constructor
		HelloEscucharEventTag(double startTime , double endTime , std::string vecino);
		//----------------------------------------------------------------------
		//destructor
		virtual ~HelloEscucharEventTag();
		// get set----------------------------------------------------------------------
		inline double 		getStartTime			(void) const	throw();
		inline double 		getEndTime  			(void) const	throw();
		inline std::string  getVecino 				(void) const	throw();
	
		// metodos----------------------------------------------------------------------
		void modificar (double startTime , double endTime ) 		throw()	;

	private:
	double 				startTime_;			
	double 				endTime_;	
	const std::string 	vecino_;			
	};
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// clase AodvUgdProcessorHello----------------------------------------------------------------------
	class AodvUgdProcessorHello

	 		: public shawn::Processor,  
	 		  public shawn::EventScheduler::EventHandler
	{
	public:
		AodvUgdProcessorHello();
		AodvUgdProcessorHello( double helloInterval , unsigned int allowedHelloLoss , 
						       double deletePeriod  , shawn::EventScheduler& pEventScheduler,
						       AodvUgdProcessor&  procesoAodv , AodvUgdRoutingTable& unaTablaRuteo );
		virtual ~AodvUgdProcessorHello();
		
		// processor----------------------------------------------------------------------
	  virtual void special_boot( void ) throw();
      virtual void boot( void ) throw();\
      virtual bool process_message( const shawn::ConstMessageHandle& ) throw();
      virtual void work( void ) throw();
		// inicializar----------------------------------------------------------------------
		void inicilizar( double helloInterval , unsigned int allowedHelloLoss , 
						 double deletePeriod  , shawn::EventScheduler& pEventScheduler,
						 AodvUgdProcessor&  procesoAodv , AodvUgdRoutingTable& unaTablaRuteo )
      					 throw();	

		// metodos----------------------------------------------------------------------

		void log 					  ( ) 	throw();
		void desactivarHello 		  ( ) 	throw();
		virtual void timeout( shawn::EventScheduler&,
	                          shawn::EventScheduler::EventHandle,
	                          double,
	                          shawn::EventScheduler::EventTagHandle& ) throw();

		//broadcast HELLO
	 	void nodoEnvioUnBroadcast 				( ) throw();
	 	void iniciarProcesoHello 				( ) throw();
	 	void crearNuevoProximoHelloBroadcast	( ) throw();
	 	void moverProximoHelloBroadcast			( )	throw(); // no se usa
	 	void enviarBroadcastHello 				( ) throw();
	 	void handle_EventExpireBroadcastTime 	( ) throw();

	 	//handle HELLO message
	 	void handle_Hello 				 ( const AodvUgdRREP& rrep ) 			throw();
	 	void handle_OtherMessage		 ( std::string vecino )  				throw();
	 	void handle_EventExpireEscucharVecino (std::string vecino )   			throw();
		bool existeVecino 				 (  std::string dest_) 					throw();
		void reniciarEventoEscucharVecino( std::string vecino , double timeout )throw();
		bool isHelloMessage  			 ( const AodvUgdRREP &unRREP )   		throw();
		void escucharNuevoVecino 		 ( std::string vecino ) 				throw();

		shawn::EventScheduler::EventHandle	 
		crearEventoEscucharVecino 	( std::string vecino , double timeout )	throw();
		
		shawn::EventScheduler::EventHandle	
		devolverEventoEscucharVecino (  std::string dest_) 					throw();

		void modificarEventoEscucharVecinoMap (  std::string dest_ , 
					shawn::EventScheduler::EventHandle unEventHandle  )     throw();

		void testCall();

	private:
		  unsigned int 			contadorBroadcast_ = 0;
		  double 	    		helloInterval_ = 0;
		  unsigned int 			allowedHelloLoss_ ;
		  double 				deletePeriod_;
		  double				timeOfLastBroadcast	= 0 ;

		  //eventoProximoBroadcast el evento para emitir el proximo hello hacia los vecinos
		  shawn::EventScheduler::EventHandle pProximoHelloBroadcast; 

		  //se guarda el programador de eventos para no tener que estar pasando
		  //por todos lados
		  shawn::EventScheduler* pEventScheduler_;
		  
		  AodvUgdProcessor*    procesoAodv_;
		  AodvUgdRoutingTable* unaTablaRuteo_;
		  //vecinos que se ecuchan con sus eventos
		   //typedef std::pair <const std::string , HelloEscucharEventTag > parListenNeighbors;
		   typedef std::map <const std::string , shawn::EventScheduler::EventHandle > mapTypelistenNeighbors;
		   					//vecino 		//evento delete period (HelloExpire)
		   typedef std::pair<const std::string , shawn::EventScheduler::EventHandle> parlistenNeighbors;
		   mapTypelistenNeighbors listenNeighbors_;
	// ----------------------------------------------------------------------
	};
	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------

}

#endif
#endif




