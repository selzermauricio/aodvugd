#ifndef __SHAWN_LEGACYAPPS_AODVUGD_ROUTE_DISCOVERY_H
#define __SHAWN_LEGACYAPPS_AODVUGD_ROUTE_DISCOVERY_H
#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD

#include <map>
//#include "sys/message.h"
//#include "sys/processor.h"
#include "sys/event_scheduler.h"
#include "legacyapps/aodvugd/aodvugd_observer_estado_ruta.h"

#include "legacyapps/aodvugd/aodvugd_message.h"

 
namespace aodvugd
{
class AodvUgdProcessor;
class AodvUgdTableEntery;
//class ObserverEstadoRuta;

enum class TipoLocalRepair;
/*use como plantilla class RoutingEventTag*/
   /*es para tener informacion sobre el evento, de otra forma cuando ocurre el evento no voy a saber a que corresponde.
   cuando ocurra el evento voy a poder preguntar a que ruta corresponde el descubrimiento y ver si existe*/
	class DiscoveryEventTag 
			: public shawn::EventScheduler::EventTag
	{
	public:
		//constructor
		DiscoveryEventTag( std::string destino, unsigned int ultimoTtl,
							unsigned int reintento );
		//----------------------------------------------------------------------
		//destructor
		virtual ~DiscoveryEventTag();
		// get set----------------------------------------------------------------------
		inline std::string getDestino () const throw();
		inline unsigned int getUltimoTtl () const	throw();
		inline const unsigned int getReintento() const throw();
		// metodos----------------------------------------------------------------------
		std::string	toString(void)	const		throw();
		void show			(void) 	const	throw();
		// ----------------------------------------------------------------------

	private:
		const std::string destino_;
		const unsigned int ultimoTtl_;
		const unsigned int reintento_;
	};
// class DiscoveryEventResetRREQ_limit----------------------------------------------------------------------
// ----------------------------------------------------------------------
	class DiscoveryEventResetRREQ_limit : public shawn::EventScheduler::EventTag
	{
	public:
		//constructor
		DiscoveryEventResetRREQ_limit( double ultimoReset );
		//----------------------------------------------------------------------
		//destructor
		virtual ~DiscoveryEventResetRREQ_limit();
		// get set----------------------------------------------------------------------
		inline const double getUltimoReset() const throw();
		// metodos----------------------------------------------------------------------
	private:
		const double ultimoReset_;
	};	
// ----------------------------------------------------------------------
// class ConfiguracionDiscovery ----------------------------------------------------------------------
		class ConfiguracionDiscovery	//unicamente los parametros de configuracion para el descubrimiento
		{
		public:
			//ConfiguracionDiscovery ( );
			ConfiguracionDiscovery( bool 		 useERS ,
									double 		 NET_TRAVERSAL_TIME ,
									unsigned int TTL_THRESHOLD ,
									unsigned int TTL_INCREMENT ,
									unsigned int NET_DIAMETER ,
									unsigned int RREQ_RETRIES ,
									unsigned int RREQ_RATELIMIT ,
									unsigned int LOCAL_ADD_TTL ,
									unsigned int TTL_START ,
									double 		 TIMEOUT_BUFFER ,
									double 		 NODE_TRAVERSAL_TIME ,
									bool   		 useGratuitousRREP ,
									bool 		 useDestOnly 	   );
			 ~ConfiguracionDiscovery();

			// get set----------------------------------------------------------------------
			 double 		getNET_TRAVERSAL_TIME	(void) const throw();
			 unsigned int  	getTTL_THRESHOLD 		(void) const throw();
			 unsigned int  	getTTL_INCREMENT 		(void) const throw();
			 unsigned int  	getNET_DIAMETER  		(void) const throw();
			 unsigned int	getRREQ_RETRIES	 		(void) const throw();
			 unsigned int   getRREQ_RATELIMIT       (void) const throw();
			 unsigned int   getLOCAL_ADD_TTL        (void) const throw();
			 unsigned int   getTTL_START	        (void) const throw();
			 bool			getUseERS		 		(void) const throw();
			 bool			getUseGratuitousRREP	(void) const throw();
			 bool			getUseDestOnly	 		(void) const throw();
			 bool			getUseLocalRepair	 	(void) const throw();

			// Metodos----------------------------------------------------------------------
			double calcularRING_TRAVERSAL_TIME (unsigned int ttlActual) const throw();
			std::string	toString(void)	throw();
			void show(void)	throw();


		private:

			 const bool 			useERS_;//ring;
			 const double 			NET_TRAVERSAL_TIME_;
			 const unsigned int  	TTL_THRESHOLD_;
			 const unsigned int 	TTL_INCREMENT_;
			 const unsigned int 	NET_DIAMETER_;
			 const double 			TIMEOUT_BUFFER_; //para calcular RING_TRAVERSAL_TIME
			 const double 			NODE_TRAVERSAL_TIME_; //para calcular RING_TRAVERSAL_TIME
			 const unsigned int 	RREQ_RETRIES_;
			 const bool   			useGratuitousRREP_;
			 const bool 		 	useDestOnly_; 	
			 const unsigned int  	RREQ_RATELIMIT_;	 
			 const unsigned int  	LOCAL_ADD_TTL_;	     
			 const unsigned int  	TTL_START_;	         

		};
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// class infoDiscoveryRoute ----------------------------------------------------------------------
	   class infoDiscoveryRoute
	   {
	   public:
		infoDiscoveryRoute ( const AodvUgdRREQ  &ultimoRREQ,
			      	  	  	 unsigned int	    RREQ_sent_by_this_discovery,
							 shawn::EventScheduler::EventHandle eventoTiempoEspera,
							 TipoLocalRepair unTipoLocalRepair);
		//infoDiscoveryRoute();
	    virtual ~infoDiscoveryRoute();

	    // get ----------------------------------------------------------------------
	  unsigned int 			getRREQ_sent_by_this_discovery ( ) throw();
	  const AodvUgdRREQ* 	getUltimoRREQ			(void) const throw();
      const TipoLocalRepair getTipoLocalRepair  	(void) const throw();

      shawn::EventScheduler::EventHandle getEventoTiempoEspera()
      											 			    throw();
      	//set
      void  setTipoLocalRepair (TipoLocalRepair unTipoLocalRepair) throw();

	     //rreq
	  const unsigned int    getHops 		    (void) const throw();
      const unsigned int    getRREQ_ID 		    (void) const throw();
      const std::string     getOrigen 		    (void) const /*throw()*/;
      const std::string     getDestino 		    (void) const throw();
      const unsigned int    getOrigenSequNumb   (void) const throw();
      const unsigned int    getDestSequNumb     (void) const throw();
      const bool		    getUnknownSequNum   (void) const throw();
      const bool		    getDestOnlyFlag     (void) const throw();
      const bool            getJoinFlag 	    (void) const throw();
      const bool            getRepairFlag  	    (void) const throw();
      const bool            getRREP_gFlag 	    (void) const throw();
      const unsigned int    getTtl 			    (void) const throw();



	    // Metodos----------------------------------------------------------------------


	   private:

	     // const AodvUgdRREQ *ultimoRREQ_;
	      unsigned int	RREQ_sent_by_this_discovery_;
	      shawn::EventScheduler::EventHandle eventoTiempoEspera_;

	//      rreq  no puedo persistir los mensajes!
	  const	std::string 	dest_;			  //The IP address of the destination
      const	std::string 	origen_;		  //The IP address of the Originator
      const unsigned int	destSequNumb_;			  //Destination Sequence Number
	  const unsigned int	origenSequNumb_;		  //Originator Sequence Number
	  const bool 			joinFlag_;                //Join flag; reserved for multicast.
      const bool 			repairFlag_;              //Repair flag; reserved for multicast.
      const bool 			rrep_gFlag_;              //Gratuitous RREP flag
      const bool 			destOnlyFlag_;            //Destination only flag;
      const bool 			UnknownSequNum_;          //Unknown sequence number; del destino
      //Reserved       Sent as 0; ignored on reception.
	  const unsigned int	hops_;      			  //The number of hops from the Originator IP Address
	  const unsigned int	rreqId_;			      //A sequence number uniquely identifying

	  //Ip parameter
	  const unsigned int 	ttl_;

	  //repair para saber que hacer si no se encuentra una ruta
	  TipoLocalRepair tipoLocalRepair_; 
	   };
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// class AodvUgdRouteDiscovery ----------------------------------------------------------------------
	   class AodvUgdRouteDiscovery
		       : //public shawn::Processor,
		         public shawn::EventScheduler::EventHandler
		        ,public ObserverEstadoRuta
	   {
	   public:
		   AodvUgdRouteDiscovery();
		   AodvUgdRouteDiscovery( unsigned int contadorRREQ_ID,
				   	   	   	   	  unsigned int	contadorRREQ_sent_seg,
								  ConfiguracionDiscovery  unaConfiguracionDiscovery,
								  AodvUgdProcessor&  procesoAodv );

		   virtual ~AodvUgdRouteDiscovery();

		    virtual void timeout( shawn::EventScheduler&,
		                          shawn::EventScheduler::EventHandle,
		                          double,
		                          shawn::EventScheduler::EventTagHandle& ) throw();

		 // get set----------------------------------------------------------------------
		      unsigned int getContadorRREQ_ID (void) throw();

	    // Metodos----------------------------------------------------------------------
			shawn::EventScheduler::EventHandle generarPrimerTiempoEspera (unsigned int ultimoTtl , double tiempoDeEspera ,
							   					std::string destino ) 				throw();

			shawn::EventScheduler::EventHandle	generarEventoTiempoEspera (
								unsigned int ultimoTtl 			 , double tiempoDeEspera ,
							   unsigned int contadorReintentos   , std::string destino   )
																					throw();

			void guardarPrimeraInfoDiscoveryRoute (const AodvUgdRREQ  &pUltimoRREQ ,
							   		 			   shawn::EventScheduler::EventHandle pEventHandle ,
							   		  			   const TipoLocalRepair  &unaTipoLocalRepair) throw();

			void guardarReintentoInfoDiscoveryRoute (const AodvUgdRREQ  &pUltimoRREQ ,
								   		    shawn::EventScheduler::EventHandle pEventHandle ,
								   		    unsigned int 		contadorReintentos ) 	 throw();
			//todo los rreq que son discovery se envian con este metodo, asi se suma el contador
			// y si se cambia la forma de enviar solo se modifica aca
			void sendDiscoveryRREQ ( AodvUgdRREQ* pRREQ);

			void modificarInfoDiscoveryRoute (const std::string dest ,
									infoDiscoveryRoute &pUnaInfoDiscoveryRoute ) throw();

			void modificarTipoRepairInfoDiscoveryRoute (const std::string dest ,
									 TipoLocalRepair unTipoLocalRepair ) throw();

			unsigned int obtenerTtlAnterrior ( std::string dest ) const throw();
			shawn::MessageHandle generarRREQ_reintento( std::string destino, 
														unsigned int newTtl )const throw();

			void         aumentarContadorRREQ_ID() throw();
			unsigned int obtenerRREQ_ReintentosSentDestino ( std::string dest ) throw();
		    bool 		 existeDescubrimientoIniciado 	   ( std::string dest ) throw();
			void         handle_EventExpireTimeDiscovery   ( std::string destino )  throw();
			bool 		 reintentarProcesoRuteDescovery    ( std::string destino )  throw();
			double 		 calcularTiempoEsperaERS (unsigned int rreqTtl , std::string destino) throw();

			void 		comenzarProcesoRuteDescovery   	   (std::string destino , std::string origen)
																					throw();
			void comenzarProcesoRuteDescoveryEntradaExistente (std::string destino , std::string origen,
												 		const AodvUgdTableEntery& unaEntrada)
    																				throw();
    																				
			AodvUgdRREQ* armarPrimerRREQRutaExistente(
								  std::string  dest         	, std::string  origen          ,
		               			  unsigned int rutaDestSequNumb , unsigned int  origenSequNumb ,
		               			  bool rutaValidDestSeqNum   	, unsigned int rutaDestHops    ,
		               			  unsigned int RREQ_ID       	)  				    throw();
			
			AodvUgdRREQ* armarRREQ_reintento( std::string destino, unsigned int newTtl ) throw();

			//se crea el evento que para que se dispare cada 1000ms
			void generarEventoResetContadorRREQ 		( ) throw();
			//se maneja el evento: se resetea y se crea un nuevo evento
			void handleEventoResetContadorRREQ 	        ( ) throw();
			bool verificarRateLimit						( ) throw();

	/***************************** AvisoEstadoRuta (observe)   ******************************/
			//hay que redefinir el metodo
			//maneja el aviso de una ruta de interes esta activa
			virtual void handleAvisoRutaActiva (std::string destino)	 throw();
			virtual void observarUnaRuta     (  std::string destino, std::string estado )throw();
			virtual void cancelarObservarUnaRuta(  std::string destino, std::string estado )throw();

			//se elimina el discovery y los eventos relacionados
			void cancelarDiscovery	   (std::string destino)     throw();
			//se cancela el evento del tiempo de espera
			void cancelarEventoTiempoEspera(  infoDiscoveryRoute &pUnaInfoDiscoveryRoute  )
																				   throw();	
	/***************************** FIN AvisoEstadoRuta (observer)   ******************************/

			
	/***************************** LocalRepair   ******************************/
			void comenzarProcesoRuteDescoveryLocalRepair(std::string destino , 
														 std::string origen ,
								 						 AodvUgdTableEntery *unaEntrada ,
								 						 unsigned int ttlInicialLocalRepair ,
								 						 const TipoLocalRepair &unTipoLocalRepair )
    																			 throw();
		  /*se devuelve el 1er RREQ cuando se comienza un discovery por querer reparar
		    la ruta por LocalRepair, siempre hay una ruta existente, la que se va a reparar*/
			AodvUgdRREQ* armarPrimerRREQRutaExistenteLocalRepair( 
							 std::string  dest         	   , std::string  origen         ,
	               			 unsigned int rutaDestSequNumb , unsigned int origenSequNumb ,
	               			 bool rutaValidDestSeqNum      , unsigned int rutaDestHops   ,
	               			 unsigned int RREQ_ID          , unsigned int ttlInicial     )
																				 throw();
			bool isDescubrimientoLocalRepair( std::string destino ) 			 throw();
			const TipoLocalRepair getTipoLocalRepairInfoDiscovery(std::string destino) throw();
			
	/***************************** FIN LocalRepair   ******************************/
	  private:
		  std::map<const std::string , infoDiscoveryRoute*> discoverysMap;
		   unsigned int contadorRREQ_ID_=1;
		   unsigned int	contadorRREQ_sent_seg_=0;
		   ConfiguracionDiscovery  unaConfiguracionDiscovery_;
		   AodvUgdProcessor* procesoAodv_;

	   };

}
#endif
#endif
