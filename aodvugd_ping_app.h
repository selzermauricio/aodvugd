#ifndef __SHAWN_LEGACYAPPS_AODVUGD_PING_APP_H
#define __SHAWN_LEGACYAPPS_AODVUGD_PING_APP_H
#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD

#include <map>
#include <list>
#include <set>
#include <vector>
#include <limits>
#include "sys/processor.h"
#include "sys/event_scheduler.h"
#include "legacyapps/aodvugd/aodvugd_message.h"
#include "legacyapps/aodvugd/aodvugd_processor.h"
//#include "legacyapps/aodvugd/aodvugd_message.h"
//#include "sys/simulation/simulation_controller.h"

namespace aodvugd
{
class  InfoPingEnviado;
// ----------------------------------------------------------------------	
// clase PingProgramadoEventTag----------------------------------------------------------------------
	class PingProgramadoEventTag : public shawn::EventScheduler::EventTag
	{
	public:
		//constructor
		PingProgramadoEventTag( std::string destino , unsigned int  pingID);
		//----------------------------------------------------------------------
		//destructor
		virtual ~PingProgramadoEventTag();
		// get set----------------------------------------------------------------------
		inline std::string  getDestino 				(void) const	throw();
		inline unsigned int getPingID				(void) const	throw();
	
		// metodos----------------------------------------------------------------------


	private:
	const std::string 	destino_;
	const unsigned int  pingID_ ;			
	};
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// clase PingApp----------------------------------------------------------------------
	class PingApp

	 		: public shawn::Processor,  
	 		  public shawn::EventScheduler::EventHandler
	{
	public:
		PingApp();
		PingApp( shawn::EventScheduler& pEventScheduler,
		      	 AodvUgdProcessor&  procesoAodv );
		virtual ~PingApp();
		
		// processor----------------------------------------------------------------------
	  virtual void special_boot( void ) throw();
      virtual void boot( void ) throw();\
      virtual bool process_message( const shawn::ConstMessageHandle& ) throw();
      virtual void work( void ) throw();
		// inicializar----------------------------------------------------------------------
	  void inicilizar(   shawn::EventScheduler& pEventScheduler,
						 AodvUgdProcessor&  procesoAodv  )
      					 throw();	

		// metodos----------------------------------------------------------------------

		void log 					  ( ) 	throw();

		virtual void timeout( shawn::EventScheduler&,
	                          shawn::EventScheduler::EventHandle,
	                          double,
	                          shawn::EventScheduler::EventTagHandle& ) throw();

		void recibirMensajePing ( const PingMessage& unPingMessage  ) 	   throw();

		

        void programarPrimerPing( std::string destino     , unsigned int maxPings , 
								  bool pingInfinito       , double timeoutPing    ,
								  unsigned int ttl        , int size		      ,
								  double tiempoInicioPing   )  		   throw();	

    

	private:
		  //unsigned int 			contadorBroadcast_ = 0;
		  //par: destino , eventoPing
		  typedef std::pair<unsigned int , InfoPingEnviado*> ParPingEnviado;
          typedef std::map< unsigned int , InfoPingEnviado*> TipoPingsEnviados; 
          TipoPingsEnviados pingsEnviados;

		  shawn::EventScheduler* pEventScheduler_;
		  AodvUgdProcessor*    procesoAodv_;
		  unsigned int  pingID_ = 0;

// metodos privados----------------------------------------------------------------------	

    void enviarPingRequest (std::string destino , unsigned int ttl    , 
			  			    int size	   		, unsigned int pingID ,
			  			    unsigned int seqNum) throw();

    void enviarPingReply   (std::string destino , unsigned int ttl    , 
			  			    int size	   		, unsigned int pingID ,
			  			    unsigned int seqNum) throw();

    shawn::EventScheduler::EventHandle 
    generarEventoTiempoEspera ( double tiempoDeEspera , std::string destino ,
								unsigned int pingID  )	  				throw(); 

    void handle_EventPing   ( const PingProgramadoEventTag& unPingProgramadoEventTag  ) throw();
	void handle_pingReply   ( const PingMessage& unPingMessageReply )   throw();
	void handle_pingRequest ( const PingMessage& unPingMessageReply )   throw();
	bool existeIdPingEnviados ( unsigned int pingID	) 					throw();
	InfoPingEnviado* devolverInfoPingEnviado(  unsigned int idPing  )	throw();

	void cancelarTimeoutEventPing( shawn::EventScheduler::EventHandle pEventHandle  )
																		throw();
	void intentarEnviarPing ( InfoPingEnviado *unaInfoPingEnviado  ) 	throw();

	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------
	};


	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------
// class InfoPingEnviado ----------------------------------------------------------------------
	   class InfoPingEnviado
	   {
	   public:
		InfoPingEnviado ( unsigned int  max   , unsigned int pingID  ,
						  bool pingInfinito   , std::string  destino ,
						  shawn::EventScheduler::EventHandle eventoTiempoEspera ,
						  unsigned int ttl    , int size , double timeoutPing );
		//InfoPingEnviado();
	    virtual ~InfoPingEnviado();

	    // get ----------------------------------------------------------------------
	  const int 			getMax				     ( )   const throw();
	  const unsigned int	getPingID			     ( )   const throw();
	  const int				getSeqNum			  	 ( )   const throw();
	  const unsigned int	getTtl		    	     ( )   const throw();
	  const int 			getSize		    	     ( )   const throw();
	  const double 			getTimeoutPing    	     ( )   const throw();
	  const double 			getLastSendTime     	 ( )   const throw();
	  const double 			getStartSendTime  	     ( )   const throw();
	  const bool  			getPingInfinito		     ( )   const throw();
	  const std::string		getDestino			     ( )   const throw();

      shawn::EventScheduler::EventHandle getEventoTiempoEspera()
      											 			    throw();
      	//set
	    
	    // Metodos----------------------------------------------------------------------
      void  actualizarSeEnvioPing (   double lastSendTime  ,
							shawn::EventScheduler::EventHandle eventoTiempoEspera )  
																	throw();

	   private:
	      int			max_; 		      //cantidad max de pings permitidos.
	      int			seqNum_ = -1;     		  //equivale a pings enviados enviados.  -1 indica que no empezo
	      bool 			pingInfinito_;    //si es TRUE no hay cantidad max de pings.
	      std::string 	destino_;
	      unsigned int  pingID_;
	      unsigned int  ttl_;
	      int 			size_;
       	  double 		timeoutPing_ = 0; //el tiempo que ese espera para enviar un nuevo ping.
       	  double 		lastSendTime_= 0; //el tiempo en que se envio el ultimo ping.
       	  
       	  //el tiempo en que se comenzo a enviar la secuencia de ping 
       	  //puede ser util para calcular cuanto duro el ping por ej.
       	  double 		startSendTime_=0;

	      shawn::EventScheduler::EventHandle eventoTiempoEspera_;
	   };


}

#endif
#endif




