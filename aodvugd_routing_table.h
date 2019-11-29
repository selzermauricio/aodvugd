#ifndef __SHAWN_LEGACYAPPS_AODVUGD_ROUTING_TABLE_H
#define __SHAWN_LEGACYAPPS_AODVUGD_ROUTING_TABLE_H
#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD
#include <map>
#include <set>
#include <vector>
#include <limits>
//#include "sys/processor.h"
#include "sys/event_scheduler.h"
#include "legacyapps/aodvugd/aodvugd_message.h"
#include "sys/misc/observable.h"//obsever
#include "legacyapps/aodvugd/aodvugd_observer_estado_ruta.h" //observador

namespace aodvugd
{



	class RouteEventTag;

// clase AodvUgdTableEntery----------------------------------------------------------------------
	class AodvUgdTableEntery
	{
	public:
		AodvUgdTableEntery();
		AodvUgdTableEntery		(	std::string dest_ , unsigned int destSequenceNumber_ ,
									bool validDestSeqNum_ , std::string state_ ,
									unsigned int hops_ , std::string nextHop_ ,
									double lifetime_ , 
									shawn::EventScheduler::EventHandle eventoRouteExpire_);
		virtual ~AodvUgdTableEntery();

		void modificarEntradaDestino(  unsigned int destSequenceNumber_ , bool validDestSeqNum_ ,
									   unsigned int hops_ 			    , std::string nextHop_);

		void addPrecursor			(  std::string direccionNodo );

		void deleteEventoRouteExpire (); 
		//get 
		std::string  	getDestino 					( ) const;
		bool 			getIsActive 				( ) const;
		unsigned int 	getDestSequenceNumber 		( ) const;
		bool		 	getValidDestSeqNum 			( ) const;
		double		 	getLifeTime 				( ) const;
		std::string  	getState 					( ) const;
		unsigned int 	getHops 					( ) const;
		std::string  	getNextHop 					( ) const;
		bool		 	isEmptyPrecursors 			( ) const;
		int	 		 	getSizePrecursors 			( ) const;
		std::set<std::string>::const_iterator getItBeginPrecursors () const;
		std::set<std::string>::const_iterator getItEndPrecursors   () const;

		shawn::EventScheduler::EventHandle getEventoRouteExpire();
 		//set
 		void setEventoRouteExpire(shawn::EventScheduler::EventHandle eventoRouteExpire_);
		void setState 				( std::string nuevoState );
		void setLifetime 			(double nuevoLifetime );
		void setDestSequenceNumber  (unsigned int nuevoDestSeqNum );



	private:

		std::string 				dest;
		unsigned int 				destSequenceNumber;
		bool 						validDestSeqNum;
		std::string 				state; //and routing flags (e.g., valid, invalid, repairable,being repaired)
		// Network Interface
		unsigned int 				hops;//(number of hops needed to reach destination)
		std::string					nextHop;
		double 						lifeTime;//(expiration or deletion time of the route)

		//set me parece una buena idea asi no se repiten los nodos y es facil (eficiente) buscar
		std::set  < std::string >  precursors;

		shawn::EventScheduler::EventHandle eventoRouteExpire;
													


	};
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------	
// clase RouteEventTag----------------------------------------------------------------------
/*use como plantilla class RoutingEventTag*/
   /*es para tener informacion sobre el evento, de otra forma cuando ocurre el evento no voy a saber a que corresponde.
   cuando ocurra el evento voy a poder preguntar a que ruta corresponde el descubrimiento y ver si existe*/
	class RouteEventTag 
		  : public shawn::EventScheduler::EventTag
		   
	{
	public:
		//constructor
		RouteEventTag( std::string destino , std::string estado );
		//----------------------------------------------------------------------
		//destructor
		virtual ~RouteEventTag();
		// get set----------------------------------------------------------------------
		inline std::string 	getDestino () const throw();
		inline std::string 	getEstado () const throw();

		// metodos----------------------------------------------------------------------
	
	private:
		const std::string destino_;	
		std::string estado_;
	};

// ----------------------------------------------------------------------
// class ListenerEstadoRuta--------------------------------------------------------------------
class ListenerEstadoRuta 
{
	//esta clase relacion al observador con la ruta y el estado que esta interesado
public:
  //constructor
  ListenerEstadoRuta(ObserverEstadoRuta * unObserverEstadoRuta , 
                     std::string destino , std::string estado );
  //----------------------------------------------------------------------
  //destructor
  virtual ~ListenerEstadoRuta();
  // get set----------------------------------------------------------------------
  inline std::string getDestino () const throw();
  inline std::string getEstado  () const throw();
  inline ObserverEstadoRuta* getObservadorEstadoRuta() const throw();
  // ----------------------------------------------------------------------
private:
    ObserverEstadoRuta * unObserverEstadoRuta_;
    std::string destino_;
    std::string estado_;
};
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// clase AodvUgdRoutingTable----------------------------------------------------------------------

	class AodvUgdRoutingTable

	 		: public shawn::EventScheduler::EventHandler,
	 		  public shawn::Observable <ListenerEstadoRuta>
	{
	public:
		AodvUgdRoutingTable();
		AodvUgdRoutingTable( const std::string direccionOwnerNode_, shawn::EventScheduler& pEventScheduler_,
							 const double DELETE_PERIOD);
		virtual ~AodvUgdRoutingTable();
		
		// get set----------------------------------------------------------------------
		const std::string	getDireccionOwnerNode ( ) const;
		const std::map<const std::string,AodvUgdTableEntery*> getRoutingTableMap() const;
		void	setOwnerNode( shawn::Node& nodo ) 	throw();

		// metodos----------------------------------------------------------------------

		bool existeEntradaDestino		(  std::string dest_) throw();
		bool existeEntradaActiva		(  std::string dest_) throw();

		void crearNuevaEntradaTabla 	(  std::string dest_ , unsigned int destSequenceNumber_ ,
										  bool validDestSeqNum_ , std::string state_ ,
										  unsigned int hops_ ,  std::string nextHop_ , double lifetime_)
										  throw();

		void actualizrEntradaTablaToActive ( std::string dest_ , unsigned int destSequenceNumber_ ,
										bool validDestSeqNum_ , std::string state_ ,
										unsigned int hops_ ,  std::string nextHop_ , double lifetime_)
										throw();
										
		void actualizrLifeTimeEntradaActiva ( std::string dest_ , double lifetime_)  throw();

		const AodvUgdTableEntery * devolverEntradaDestino   ( std::string dest_) throw();
		AodvUgdTableEntery * devolverEntradaDestino_w ( std::string dest_) throw();

	
		bool existeRutaActivaDestino		( std::string dest_) throw();
		void agregarPrecursorEntrada 		( std::string dest_, std::string direccionPrecursor) throw();
		std::string toString 				( ) throw();
		void show 							( ) throw();
		void log 							( ) throw();

		//route expire
		void handle_EventExpireRoute (const RouteEventTag &unRouteEventTag)throw();
		virtual void timeout( shawn::EventScheduler&,
		                          shawn::EventScheduler::EventHandle,
		                          double,
		                          shawn::EventScheduler::EventTagHandle& ) throw();

		shawn::EventScheduler::EventHandle crearEventoRouteExpire( 
									double tiempoEvento  ,
									std::string destino , 
									std::string estado  );	

		void moverEventoRouteExpire(double tiempoEvento  , 
									shawn::EventScheduler::EventHandle pEventHandle);
		
		void establecerRutaInactiva 	( AodvUgdTableEntery &unaEntrada , double nuevoLifetime);
		void establecerRutaActiva 		( AodvUgdTableEntery &unaEntrada , double nuevoLifetime);
		void borrarEntradaTablaInactiva (AodvUgdTableEntery *unaEntrada);

		//borra el evento de la entrada y desde el EventScheduler OJO donde usar!!
		//porque un evento por TIMEOUT se cancela solo.
		void cancelarEventoRouteExpire  ( AodvUgdTableEntery &unaEntrada);

		bool formoParteDeRutaActiva	( ) throw();

		//RERR
		mapTypeDestinosInvalidos* armarListaDestinosAfectadosSinRutaReenviar 
						(   mapTypeDestinosInvalidos *mapNuevosDestinosAfectados,
							std::string destinoInvalido , double deletePeriod ) throw();


		mapTypeDestinosInvalidos* armarListaDestinosAfectadosLinkBreak 
						( std::string destinoInvalido , double deletePeriod ) throw();

		mapTypeDestinosInvalidos* armarListaMisDestinosAfectadosReenvioRERR 
						( const mapTypeDestinosInvalidos *mapDestinosInvalidos , 
							 mapTypeDestinosInvalidos* mapNuevosDestinosAfectados,
						  std::string transmitterRERR , double deletePeriod ) throw();

		void actualizarRutaAfectadaRERR ( double newSeqNum ,  double deletePeriod ,
								 		  AodvUgdTableEntery &unaEntrada ) throw();

	    unsigned int devolverUltimoHopConocidoDestino(  std::string dest_) throw();

	/***********************     Observer         ******************/
		virtual void observers_added   (ListenerEstadoRuta& obs)   throw();
		void 	notifyChangeRouteState (std::string destino, std::string estado ) throw();
		void    escucharUnEstadoRuta 	      (ObserverEstadoRuta* unObserverEstadoRuta, 
                 			    	           std::string destino , std::string estado )  
																				throw();
		void    cancelarEscucharUnEstadoRuta (ObserverEstadoRuta * unObserverEstadoRuta, 
                 			    	  		  std::string destino , std::string estado )  
																				throw();
		void	avisoDiscoveryComplete			( std::string dest_ )	throw();
	/***********************     Fin Observer     ******************/

	/***********************     Routing         ******************/		
	std::string  devolverNextHop_a_Destino   ( std::string destino) throw();																
	/***********************     FIN Routing     ******************/																				
	private:
		   //shawn::Node*		ownerNode; //el dueño de la tabla
		  const std::string direccionOwnerNode; //el dueño de la tabla
		  std::map <const std::string,AodvUgdTableEntery*> routingTableMap;

		  //se guarda el programador de eventos para no tener que estar pasando
		  //por todos lados
		  shawn::EventScheduler* pEventScheduler;
		  double DELETE_PERIOD;
		  
		  //no hace falta mantener el handle, solo si hace falta
		  //al crear el evento se guarda en el setEventos en EventScheduler
		  //los eventos se guardan en cada entrada en este caso.
		  
		  //shawn::EventScheduler::EventHandle pEventHandle2;

		  //key destino + proceso (interesado en la ruta)
		  //en el set los valores son unicos-

	// ----------------------------------------------------------------------


	};
	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------

}

#endif
#endif




