#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD

#include "legacyapps/aodvugd/aodvugd_routing_table.h"
//#include "legacyapps/aodvugd/aodvugd_message.h"
//#include "sys/simulation/simulation_controller.h"
//#include "sys/node.h"
#include <iostream>
#include <limits>
#include <cmath>
#include <sstream>
#include <map>

#include <algorithm> //std::max
//#include "legacyapps/aodvugd/aodvugd_processor.h"
#include <assert.h> //assert
#include <set> //set
#include <sstream> //tostring
#include "legacyapps/aodvugd/aodvugd_logger.h" //log

namespace aodvugd
{

// clase RouteEventTag----------------------------------------------------------------------
/*use como plantilla class RoutingEventTag*/
   /*es para tener informacion sobre el evento, de otra forma cuando ocurre el evento no voy a saber a que corresponde.
   cuando ocurra el evento voy a poder preguntar a que ruta corresponde el descubrimiento y ver si existe*/
	RouteEventTag:: //constructor
	RouteEventTag( std::string destino , std::string estado )
	:				//inicializacion
	destino_			{destino},
	estado_				{estado}

	{}
//----------------------------------------------------------------------
	//destructor
	RouteEventTag::
	~RouteEventTag(){}

	// get set----------------------------------------------------------------------
	inline std::string
	RouteEventTag::
	getDestino  (void) const	throw()
	{
		return destino_ ;
	}
	//----------------------------------------------------------------------
	inline std::string
	RouteEventTag::
	getEstado  (void) const	throw()
	{
		return estado_ ;
	}
//----------------------------------------------------------------------
// clase ListenerEstadoRuta----------------------------------------------------------------------
	ListenerEstadoRuta:: //constructor
	ListenerEstadoRuta(ObserverEstadoRuta * unObserverEstadoRuta , 
                       std::string destino , std::string estado )
	:				//inicializacion
	unObserverEstadoRuta_ {unObserverEstadoRuta},
	destino_			  {destino},
	estado_				  {estado}
	{}
//----------------------------------------------------------------------
	//destructor
	ListenerEstadoRuta::
	~ListenerEstadoRuta(){}

	// get set----------------------------------------------------------------------
	inline std::string	ListenerEstadoRuta::getDestino  () const throw()
	{return destino_ ;}
	//----------------------------------------------------------------------
	inline std::string	ListenerEstadoRuta::getEstado   () const throw()
	{return estado_ ;	}
	//----------------------------------------------------------------------
	inline ObserverEstadoRuta * ListenerEstadoRuta::getObservadorEstadoRuta () 
	const throw()
	{return unObserverEstadoRuta_ ;	}

// clase AodvUgdTableEntery----------------------------------------------------------------------
	AodvUgdTableEntery::	//constructor vacio
	AodvUgdTableEntery( )
	{}

	AodvUgdTableEntery::	//constructor
	AodvUgdTableEntery		( std::string dest_ , unsigned int destSequenceNumber_ ,
							bool validDestSeqNum_ , std::string state_ ,
							unsigned int hops_ , std::string nextHop_ ,
							double lifetime_ ,
							shawn::EventScheduler::EventHandle eventoRouteExpire_)
	:

							//inicializacion
							dest				{dest_} ,
							destSequenceNumber	{destSequenceNumber_} ,
							validDestSeqNum		{validDestSeqNum_} ,
							state				{state_} ,
							hops				{hops_} ,
							nextHop				{nextHop_} ,
							lifeTime			{lifetime_} ,
							eventoRouteExpire 	{eventoRouteExpire_}

	{}
// ----------------------------------------------------------------------
	AodvUgdTableEntery::	//destructor
	~AodvUgdTableEntery()
	{}
	// ----------------------------------------------------------------------
	// metodos----------------------------------------------------------------------

   // ----------------------------------------------------------------------
	void
	AodvUgdTableEntery::
	modificarEntradaDestino( unsigned int destSequenceNumber_ ,bool validDestSeqNum_ ,
							 unsigned int hops_ 			  , std::string nextHop_)
	{
		//estado y lifetime se modifican llamando a otros metodos

		destSequenceNumber=destSequenceNumber_;
		validDestSeqNum=validDestSeqNum_;
		hops=hops_;//(number of hops needed to reach destination)
		nextHop=nextHop_;
		//precursors;  por ahora no se modifican

	}

	// ----------------------------------------------------------------------
	void
	AodvUgdTableEntery::
	addPrecursor (  std::string direccionNodo )
	{
		//si el nodo ya existe no lo agrega
		precursors.insert (  direccionNodo );
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdTableEntery::
	deleteEventoRouteExpire () 
	{
		//seteo NULL porque como es un handle cuando no apunta a ningun lado se elimina
		eventoRouteExpire = NULL;
		//delete eventoRouteExpire;
	}

	// get set----------------------------------------------------------------------

	bool
	AodvUgdTableEntery::
	getIsActive() const
	{
		bool active=false;
		if(state=="active")
		{
			active=true;
		}
		return active;
	}
	// ----------------------------------------------------------------------
	std::string
	AodvUgdTableEntery::
	getDestino() const
	{
		return dest;
	}
	// ----------------------------------------------------------------------
	unsigned int
	AodvUgdTableEntery::
	getDestSequenceNumber() const
	{
		return destSequenceNumber;
	}
	// ----------------------------------------------------------------------
	bool
	AodvUgdTableEntery::
	getValidDestSeqNum() const
	{
		return validDestSeqNum;
	}
	// ----------------------------------------------------------------------
	double
	AodvUgdTableEntery::
	getLifeTime() const
	{
		return lifeTime;
	}
	// ----------------------------------------------------------------------
	std::string
	AodvUgdTableEntery::
	getState() const
	{
		return state;
	}
	// ----------------------------------------------------------------------
	unsigned int
	AodvUgdTableEntery::
	getHops() const
	{
		return hops;
	}
	// ----------------------------------------------------------------------
	std::string
	AodvUgdTableEntery::
	getNextHop() const
	{
		return nextHop;
	}
	// ----------------------------------------------------------------------
	shawn::EventScheduler::EventHandle
	AodvUgdTableEntery::
	getEventoRouteExpire () 
	{
		return eventoRouteExpire;
	}
	// ----------------------------------------------------------------------
	bool
	AodvUgdTableEntery::
	isEmptyPrecursors () const
	{
		return precursors.empty();
	}
	// ----------------------------------------------------------------------
	int
	AodvUgdTableEntery::
	getSizePrecursors () const
	{
		return precursors.size();
	}
	// ----------------------------------------------------------------------
	std::set<std::string>::const_iterator
	AodvUgdTableEntery::
	getItBeginPrecursors () const
	{
		return precursors.begin();
	}
		// ----------------------------------------------------------------------
	std::set<std::string>::const_iterator
	AodvUgdTableEntery::
	getItEndPrecursors () const
	{
		return precursors.end();
	}
	// set----------------------------------------------------------------------
	// ----------------------------------------------------------------------
	void
	AodvUgdTableEntery::
	setEventoRouteExpire (shawn::EventScheduler::EventHandle eventoRouteExpire_)
	{
		eventoRouteExpire= eventoRouteExpire_;
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdTableEntery::
	setState (std::string nuevoState)
	{
		state=nuevoState;
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdTableEntery::
	setLifetime (double nuevoLifetime)
	{
		lifeTime 	=	nuevoLifetime;
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdTableEntery::
	setDestSequenceNumber (unsigned int nuevoDestSeqNume)
	{
		destSequenceNumber 	=	nuevoDestSeqNume;
	}
	// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// clase AodvUgdRoutingTable----------------------------------------------------------------------
	AodvUgdRoutingTable::
	AodvUgdRoutingTable()
	{}
	// ----------------------------------------------------------------------
	AodvUgdRoutingTable::
	AodvUgdRoutingTable( const std::string direccionOwnerNode_, shawn::EventScheduler& pEventScheduler_,
						 const double DELETE_PERIOD_)
	:
		//inicializacion
		direccionOwnerNode				{ direccionOwnerNode_},
		pEventScheduler 				{ &pEventScheduler_},
		DELETE_PERIOD					{ DELETE_PERIOD_}
										
	{
		std::cout<< "pEventScheduler_ : " << &pEventScheduler_ <<  std::endl;
	}
	// ----------------------------------------------------------------------
	AodvUgdRoutingTable::
	~AodvUgdRoutingTable()
	{}

	// ----------------------------------------------------------------------
	// get set----------------------------------------------------------------------
	const std::string
	AodvUgdRoutingTable::
	getDireccionOwnerNode () const
	{
		
		return direccionOwnerNode;
	}
	// ----------------------------------------------------------------------
	const std::map<const std::string,AodvUgdTableEntery*>
	AodvUgdRoutingTable::
	getRoutingTableMap() const
	{
		return routingTableMap ;
	}
	// metodos----------------------------------------------------------------------

		// ----------------------------------------------------------------------
	/////////////////////////////////////////////////////////////////
	/***********************     Observer         ******************/
	/////////////////////////////////////////////////////////////////
	// ----------------------------------------------------------------------
	void 
	AodvUgdRoutingTable::
	observers_added( ListenerEstadoRuta& obs ) 
	throw()
	{
		//TODO algun log
	}
	// ----------------------------------------------------------------------
	void 
	AodvUgdRoutingTable::
	escucharUnEstadoRuta( ObserverEstadoRuta * unObserverEstadoRuta , 
                     	  std::string destino , std::string estado ) 
	throw()
	{
		observer_add(* new ListenerEstadoRuta(unObserverEstadoRuta,destino,estado)  );
	}
	// ----------------------------------------------------------------------
	void 
	AodvUgdRoutingTable::
	cancelarEscucharUnEstadoRuta( ObserverEstadoRuta * unObserverEstadoRuta , 
                     	  std::string destino , std::string estado ) 
	throw()
	{
	    ObserverIterator itSet;
	    for ( itSet=observers_begin() ; itSet!=observers_end(); ++itSet )
	   	{
	   		ListenerEstadoRuta *unListenerEstadoRuta = *itSet;
	   		if (  (unListenerEstadoRuta->getDestino() == destino)
	   			&&(unListenerEstadoRuta->getEstado () == estado )
	   			&&(unListenerEstadoRuta->getObservadorEstadoRuta () == unObserverEstadoRuta )
	   			  )
	   		{
		   		//quitar de los observadores
		   		observer_remove(*unListenerEstadoRuta);
	   		}
		}
	}	
	// ----------------------------------------------------------------------
	void
	AodvUgdRoutingTable::
	notifyChangeRouteState (std::string destino, std::string estado)
	throw()
	{
		//si el destino es de interes por algun proceso
		//con 
	    ObserverIterator itSet;
	    for ( itSet=observers_begin() ; itSet!=observers_end(); ++itSet )
	   	{
	   		ListenerEstadoRuta *unListenerEstadoRuta = *itSet;
	   		if (  (unListenerEstadoRuta->getDestino() == destino)
	   			&&(unListenerEstadoRuta->getEstado () == estado )  )
	   		{
		   		//avisar al proceso
		   		//se le avisa al observador que ocurrio un cambio en la ruta 
		   		//que estaba escuchando
		   		unListenerEstadoRuta->getObservadorEstadoRuta()->onEstadoRutaChange( destino , estado );	
	   		
	   			//luego de notificar los quito, por ahora lo hago aca..
/*	   			cancelarEscucharUnEstadoRuta(unListenerEstadoRuta->getObservadorEstadoRuta()
	   										 , destino ,estado);*/
		   		observer_remove(*unListenerEstadoRuta);
	   		}
		}
/*
		std::vector<std::list<int>::const_iterator> matches;
auto i = list.begin(), end = list.end();
while (i != end)
{
  i = std::find(i, end, findValue);
  if (i != end)
    matches.push_back(i++);
}
*/
	}
	/////////////////////////////////////////////////////////////////
	/***********************     Fin Observer     ******************/
	/////////////////////////////////////////////////////////////////

	// ----------------------------------------------------------------------
	void
	AodvUgdRoutingTable::
	crearNuevaEntradaTabla( std::string dest_ , unsigned int destSequenceNumber_ ,
							bool validDestSeqNum_ , std::string state_ ,
							unsigned int hops_ , std::string nextHop_ , double lifetime_)
	throw()
	{
/*		el algoritmo solo verifica que no reciba su propio rreq por el buffer, con esa medida 
		se pueden generar bucles al registrar su propia direccion.
		Mi idea es parchar al menos en las primeras pruebas.*/
		if(dest_ == direccionOwnerNode)
		{
			std::cout<< "ERROR: NODO REGISTRA SU PROPIA DIRECCION " <<	std::endl ;
			assert(false);
		}



		//creo el evento
/*		el lifeTime viene sumado en todos los casos con el tiempo actual, 
		el protocolo especifica donde hay que sumar asi que solo hay que respetar*/
		shawn::EventScheduler::EventHandle pEventoRouteExpire = crearEventoRouteExpire (
															    lifetime_ /*tiempoEvento */ ,
															    dest_, state_ );

		//creo la ruta con el evento de expiracion
		AodvUgdTableEntery *unaEntrada = new AodvUgdTableEntery ( dest_ , destSequenceNumber_ ,
																  validDestSeqNum_,	state_, hops_ , 
																  nextHop_ , lifetime_ , 
																  pEventoRouteExpire );

		routingTableMap.insert(std::pair<std::string,AodvUgdTableEntery*>( dest_,unaEntrada));

		notifyChangeRouteState(dest_, state_);
	}
	// ----------------------------------------------------------------------

	void
	AodvUgdRoutingTable::
	actualizrEntradaTablaToActive( std::string dest_     , unsigned int destSequenceNumber_ ,
								 bool validDestSeqNum_ , std::string state_ ,
								 unsigned int hops_    ,  std::string nextHop_ , double lifetime_)
	throw()
	{
		assert(state_=="active");

		std::map<std::string,AodvUgdTableEntery*>::iterator it;
		it=routingTableMap.find(dest_);
		AodvUgdTableEntery *unaEntrada=it->second; 

		//modifica el estado y el evento para que simpre sean choerentes
		establecerRutaActiva (*unaEntrada,lifetime_);

		unaEntrada->modificarEntradaDestino		(destSequenceNumber_ ,validDestSeqNum_ ,
				 	 	 	 	 	 	 	 	 hops_ , nextHop_ );
		notifyChangeRouteState(dest_, state_);
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdRoutingTable::
	avisoDiscoveryComplete	( std::string dest_ )
	throw()
	{
		notifyChangeRouteState(dest_, "active");
	}
	// ----------------------------------------------------------------------

	void
	AodvUgdRoutingTable::
	actualizrLifeTimeEntradaActiva( std::string dest_ , double lifetime_)
	throw()
	{

		std::map<std::string,AodvUgdTableEntery*>::iterator it;
		it=routingTableMap.find(dest_);
		AodvUgdTableEntery *unaEntrada=it->second; 

		//la entrada siempre tiene que estar activa
		assert(unaEntrada->getState() =="active");

		shawn::EventScheduler::EventHandle pEventoRouteExpire = 
													unaEntrada-> getEventoRouteExpire();
		moverEventoRouteExpire			( 	lifetime_ ,  
	 /*shawn::EventScheduler::EventHandle*/ pEventoRouteExpire );

		unaEntrada->setLifetime		( lifetime_ );
	}

	// ----------------------------------------------------------------------
	shawn::EventScheduler::EventHandle
	AodvUgdRoutingTable::
	crearEventoRouteExpire( double tiempoEvento , std::string destino , std::string estado )
							//pEventScheduler programador de Eventos
	{
		
		RouteEventTag* pRouteEventTag =	new RouteEventTag
											( destino ,estado  ) ;

		shawn::EventScheduler::EventHandle pEventHandle = pEventScheduler->new_event
												( *this , tiempoEvento ,
												 pRouteEventTag );

	return pEventHandle;
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdRoutingTable::
	moverEventoRouteExpire( double tiempoEvento ,  
							shawn::EventScheduler::EventHandle pEventHandle)
						
	{
		pEventScheduler->move_event ( pEventHandle , tiempoEvento );
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdRoutingTable::
	cancelarEventoRouteExpire(  AodvUgdTableEntery &unaEntrada   	)
						
	{
		shawn::EventScheduler::EventHandle pEventoRouteExpire = unaEntrada.getEventoRouteExpire();
		unaEntrada.deleteEventoRouteExpire (); //libero el handle 
		pEventScheduler->delete_event ( pEventoRouteExpire  );
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdRoutingTable::
	establecerRutaInactiva ( AodvUgdTableEntery &unaEntrada , double nuevoLifetime  )
						
	{
		unaEntrada.setState("inactive");
		unaEntrada.setLifetime (nuevoLifetime);
		shawn::EventScheduler::EventHandle pEventoRouteExpire = crearEventoRouteExpire (
															    nuevoLifetime /*tiempoEvento */ ,
															    unaEntrada.getDestino(),
															    unaEntrada.getState()  );
		unaEntrada.setEventoRouteExpire(pEventoRouteExpire);
	}
		// ----------------------------------------------------------------------
	void
	AodvUgdRoutingTable::
	establecerRutaActiva ( AodvUgdTableEntery &unaEntrada , double nuevoLifetime  )
						
	{
		//primero cancelo el evento de expiracion
		cancelarEventoRouteExpire(unaEntrada);

		unaEntrada.setState("active");
		unaEntrada.setLifetime (nuevoLifetime);
		
		//creo el nuevo envento de expiracion
		shawn::EventScheduler::EventHandle pEventoRouteExpire = crearEventoRouteExpire (
															    nuevoLifetime /*tiempoEvento */ ,
															    unaEntrada.getDestino(),
															    unaEntrada.getState()  );
		unaEntrada.setEventoRouteExpire(pEventoRouteExpire);
	}
		// ----------------------------------------------------------------------
	void
	AodvUgdRoutingTable::
	borrarEntradaTablaInactiva ( AodvUgdTableEntery *unaEntrada )
							
	{	
		routingTableMap.erase( unaEntrada->getDestino() );
		delete unaEntrada; 
		//si se consumio el evento por timeout NO hay que borrar el evento!!		
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdRoutingTable::
	handle_EventExpireRoute (const RouteEventTag &unRouteEventTag )
    throw()
	{		
		std::string dest_=	unRouteEventTag.getDestino();

		std::map<std::string,AodvUgdTableEntery*>::iterator it;
		it=routingTableMap.find(dest_);
		
		assert(it != routingTableMap.end());
		AodvUgdTableEntery *unaEntrada=it->second;
			
		assert (unRouteEventTag.getEstado() == unaEntrada->getState() );

		if ( unaEntrada->getState() == "active")
		{		
				double nuevoTiempoVida = pEventScheduler->current_time() + DELETE_PERIOD ;
				establecerRutaInactiva ( *unaEntrada , nuevoTiempoVida );
				return;
		}

		if ( unaEntrada->getState() == "inactive")
		{	
				borrarEntradaTablaInactiva ( unaEntrada );//se tiene que pasar el puntero para borrar
				return;
		}				
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdRoutingTable::
	timeout( shawn::EventScheduler&, shawn::EventScheduler::EventHandle,
	         double, shawn::EventScheduler::EventTagHandle& unEventTagHandle )
	      throw()
	   {
		   std::cout<< "TIMEOUT ruttttttttaaaaaaa: " <<	std::endl;

		   const RouteEventTag* unRouteEventTag =
				   dynamic_cast<const RouteEventTag*> ( unEventTagHandle.get() );

/*		   si se disparo el evento unEventTagHandle,
		   es porque expiro una ruta */
		   if ( unRouteEventTag != NULL){
			   std::cout<< "*************** TIMEOUT unRouteEventTag: " <<	std::endl;
			   handle_EventExpireRoute ( *unRouteEventTag );
		   }
	   }

	// ----------------------------------------------------------------------

	bool
	AodvUgdRoutingTable::
	existeEntradaDestino		(  std::string dest_) throw()
	{
		bool existe = false;
		std::map <std::string,AodvUgdTableEntery*> ::iterator it;
		it = routingTableMap.find( dest_ );
		 if ( it != routingTableMap.end() )
		 {
			 existe = true;
		 }
		 return existe;

	}
		// ----------------------------------------------------------------------

	bool
	AodvUgdRoutingTable::
	existeEntradaActiva		(  std::string dest_) throw()
	{
		//verifica que sxista una entrada y que tenga estado activo
		bool existeActiva = false;
		std::map <std::string,AodvUgdTableEntery*> ::iterator it;
		it = routingTableMap.find( dest_ );
		 if ( it != routingTableMap.end() )
		 {
		 	if(it->second->getState() == "active")
			  existeActiva = true;
		 }
		 return existeActiva;

	}
	// ----------------------------------------------------------------------
	unsigned int
	AodvUgdRoutingTable::
	devolverUltimoHopConocidoDestino(  std::string dest_) throw()
	{
		// si no se conoce el ultimo conteo de saltos se devuelve 0

		std::map <std::string,AodvUgdTableEntery*> ::iterator it;
		it = routingTableMap.find( dest_ );
		 if ( it != routingTableMap.end() )
		 {
			 AodvUgdTableEntery *unaEntrada=it->second; 
			 return  unaEntrada->getHops();
		 }
		 return 0;
	}

	// ----------------------------------------------------------------------

	bool
	AodvUgdRoutingTable::
	existeRutaActivaDestino		(  std::string dest_) throw()
	{
		bool existeEntadaDestinoActiva 	= false;
		bool existeEntadaNextHopActiva 	= false;

		existeEntadaDestinoActiva	=	existeEntradaActiva(dest_);
		if (existeEntadaDestinoActiva)
		{
			//ahora verifico que tenga NextHop activo
			const AodvUgdTableEntery* pEntradaDestino = devolverEntradaDestino(dest_);
			existeEntadaNextHopActiva	=	existeEntradaActiva( pEntradaDestino->getNextHop() );

			if (existeEntadaNextHopActiva)
			{
				return true;
			}

		}
		return false;
	}
	// ----------------------------------------------------------------------
	bool
	AodvUgdRoutingTable::
	formoParteDeRutaActiva	( ) throw()
	{
		//TODO analizar si para el rendimiento no es mejor mantener una lista de los vecinos.
		
		//a diferencia del resto de los simuladores que verifican si un nodo es vecino porque 
		//tiene un hop=1, yo verifico si el nextHop=destino.

		//es SUFICIENTE con que se encuentre que formo al menos una ruta activa.
		 for (auto it=routingTableMap.begin(); it!=routingTableMap.end(); ++it)
			{
				   AodvUgdTableEntery *unaEntrada=it->second;
				   if ( (  unaEntrada->getNextHop() == unaEntrada->getDestino() )//es vecino
				   && 	( !unaEntrada->isEmptyPrecursors() ) 				  //si tiene precursores forma una ruta
				   && 	(  unaEntrada->getState() == "active" )		    )     //la ruta esta activa
			   			return true;
			}
		return false;

	}
	// ----------------------------------------------------------------------
	std::string
	AodvUgdRoutingTable::
	devolverNextHop_a_Destino   ( std::string destino) throw()
	{
		//std::map<std::string,AodvUgdTableEntery*>::iterator it;
		//it=routingTableMap.find(destino);
		

		std::string nextHop =  routingTableMap.find(destino) ->second->getNextHop();
		
		//puede pasar que exista la ruta al destino pero no al NextHop
		//cuando se consulta el next hop a destino ya tendriamos que verificar que esta activo.
		//aca solo verifico para saber si se utilizaron los metodos correctos.
		std::string estadoNextHop = routingTableMap.find(nextHop) ->second->getState() ;
		assert(estadoNextHop == "active");

		return nextHop;
	}	
	// ----------------------------------------------------------------------
	const AodvUgdTableEntery *
	AodvUgdRoutingTable::
	devolverEntradaDestino		(  std::string dest_) throw()
	{
		std::map<std::string,AodvUgdTableEntery*>::iterator it;
		it=routingTableMap.find(dest_);
		AodvUgdTableEntery *unaEntrada=it->second;
		 return unaEntrada;
	}	
	// ----------------------------------------------------------------------
	AodvUgdTableEntery *
	AodvUgdRoutingTable::
	devolverEntradaDestino_w	(  std::string dest_) throw()
	{
		std::map<std::string,AodvUgdTableEntery*>::iterator it;
		it=routingTableMap.find(dest_);
		AodvUgdTableEntery *unaEntrada=it->second;
		 return unaEntrada;
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdRoutingTable::
	agregarPrecursorEntrada ( std::string dest_, std::string direccionPrecursor) throw()
	{
		std::map<std::string,AodvUgdTableEntery*>::iterator it;
				it=routingTableMap.find(dest_);
				AodvUgdTableEntery *unaEntrada=it->second;
				unaEntrada -> addPrecursor (direccionPrecursor );
	}
	// ----------------------------------------------------------------------
	mapTypeDestinosInvalidos*
	AodvUgdRoutingTable::
	armarListaMisDestinosAfectadosReenvioRERR ( const mapTypeDestinosInvalidos *mapDestinosInvalidos ,
												 mapTypeDestinosInvalidos* mapNuevosDestinosAfectados,
									 std::string transmitterRERR , double deletePeriod ) throw()
	{
	/* case (iii), the list should consist of those destinations in the RERR
   for which there exists a corresponding entry in the local routing
   table that has the transmitter of the received RERR as the next hop.*/

	/*   Some of the unreachable destinations in the list could be used by
   neighboring nodes, and it may therefore be necessary to send a (new)
   RERR.  The RERR should contain those destinations that are part of
   the created list of unreachable destinations and have a non-empty
   precursor list.*/

	    mapTypeDestinosInvalidos::const_iterator itMap;
	    //itero el mapa que llego en el RERR
    for ( itMap=mapDestinosInvalidos->begin() ; itMap!=mapDestinosInvalidos->end(); ++itMap )
    	{
         std::string  destinoInvalido 	 	 =  itMap->first  ;
         unsigned int destinoInvalidoSeqNum  =  itMap->second ;

         //traigo cada ruta que corresponde a los destinos
         std::map<std::string,AodvUgdTableEntery*>::iterator it;
		 it	=	routingTableMap.find ( destinoInvalido );	
			 if ( it != routingTableMap.end() ) //existe ruta
			 {
				AodvUgdTableEntery *unaEntrada=it->second;
				//has the transmitter of the received RERR as the next hop.
				//tengo que invalidar las  rutas que me afectan a mi.

	/*IMPORTANTE:  como el RERR vino desde un vecino, tengo que invalidar todas las 
				   rutas que enviaba por el como salida (NextHop). Despues recien agrego
				   a la lista NUEVA esas entradas pero que usaba algun vecino mio ruteando por mi,
				   es decir que tiene precursores!!
				   */
				if( unaEntrada->getNextHop() == transmitterRERR) 
				{
				    actualizarRutaAfectadaRERR(  destinoInvalidoSeqNum /*newSeqNum*/ , 
					  	 		 				 deletePeriod 	/*deletePeriod*/,
					  	 		 				 *unaEntrada );
				    /*The RERR should contain those destinations that are part of
		   			the created list of unreachable destinations and have a non-empty
		   			precursor list.*/
					if( !unaEntrada->isEmptyPrecursors() )
					    //solo voy a agregar la entradas que algun otro vecino la usaba..pero si actualizo 
		   				//arriba todas las que usaba yo	
						mapNuevosDestinosAfectados->insert ( *itMap );
				}
			 }
		}//fin for
	return mapNuevosDestinosAfectados;
	}
	// ----------------------------------------------------------------------
	mapTypeDestinosInvalidos*
	AodvUgdRoutingTable::
	armarListaDestinosAfectadosLinkBreak ( std::string destinoInvalido , double deletePeriod ) throw()
	{
/* For case (i), the node first makes a list of unreachable destinations
   consisting of the unreachable neighbor and any additional
   destinations (or subnets, see section 7) in the local routing table
   that use the unreachable neighbor as the next hop. */
   //el destinoInvalido se va a gregar solo porque itera su propia 
   //entrada, donde el va a ser su propio nextHop
	mapTypeDestinosInvalidos* mapNuevosDestinosAfectados;
	for( auto& itUnaEntradaTabla : routingTableMap ) 
	 {
	 	AodvUgdTableEntery *unaEntradaTabla =itUnaEntradaTabla.second;
		//buscar todos los destinos con nextHop == destinoInvalido
		if(   unaEntradaTabla->getNextHop() == destinoInvalido)
		{			
		   /*The RERR should contain those destinations that are part of
		   the created list of unreachable destinations and have a non-empty
		   precursor list.*/
		
		  //actualizar Entrada antes de agregar!!
		    actualizarRutaAfectadaRERR(  ( unaEntradaTabla->getDestSequenceNumber() )+1 /*newSeqNum*/ , 
							  	 		   deletePeriod 	/*deletePeriod*/,
							  	 		   *unaEntradaTabla );
		    //solo voy a agregar la entradas que algun otro vecino la usaba..pero si actualizo 
		    //arriba todas las que usaba yo
		   if ( !unaEntradaTabla->isEmptyPrecursors()  )         //filtro al armar la lista directamente
		   {
				mapNuevosDestinosAfectados->insert ( parDestinosInvalidos 
						( unaEntradaTabla->getDestino() , unaEntradaTabla->getDestSequenceNumber()  )  );
		   }
		}	
	 }
	return mapNuevosDestinosAfectados;
	}
	// ----------------------------------------------------------------------
	mapTypeDestinosInvalidos*
	AodvUgdRoutingTable::
	armarListaDestinosAfectadosSinRutaReenviar (  mapTypeDestinosInvalidos *mapNuevosDestinosAfectados,
												std::string destinoInvalido , double deletePeriod ) throw()
	//los nombres se hacen largo pero es la unica forma de describir el caso con exactitud
	{
	   /*For case (ii), there is only one unreachable destination, which is
   the destination of the data packet that cannot be delivered.*/

	//mapTypeDestinosInvalidos mapNuevosDestinosAfectados;
	AodvUgdTableEntery *unaEntradaTabla= devolverEntradaDestino_w ( destinoInvalido );
	
		//actualizar Entrada antes de agregar!!
		actualizarRutaAfectadaRERR(  unaEntradaTabla->getDestSequenceNumber()+1 /*newSeqNum*/ , 
							  	     deletePeriod 	/*deletePeriod*/,
							  	 	 *unaEntradaTabla);
	 /*The RERR should contain those destinations that are part of
	   the created list of unreachable destinations and have a non-empty
	   precursor list.*/
	if ( !unaEntradaTabla->isEmptyPrecursors() )	 
	 {
	 	 //solo voy a agregar la entradas que algun otro vecino la usaba..pero si actualizo 
		 //arriba todas las que usaba yo
	 	std::string  destino 	= unaEntradaTabla->getDestino();
	 	unsigned int destSeqNum = unaEntradaTabla->getDestSequenceNumber();

		mapNuevosDestinosAfectados->insert ( parDestinosInvalidos 
						( destino , destSeqNum  )  );
	 }	
	return mapNuevosDestinosAfectados;
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdRoutingTable::
	actualizarRutaAfectadaRERR ( double newSeqNum ,  double deletePeriod ,
								 AodvUgdTableEntery &unaEntrada ) throw()
	//se puede utilizar seqNum como opcional pero prefiero pasar siempre por ahora
	{
/*  Just before transmitting the RERR, certain updates are made on the
   routing table that may affect the destination sequence numbers for
   the unreachable destinations.  For each one of these destinations,
   the corresponding routing table entry is updated as follows:

   1. The destination sequence number of this routing entry, if it
      exists and is valid, is incremented for cases (i) and (ii) above,
      and copied from the incoming RERR in case (iii) above.
   2. The entry is invalidated by marking the route entry as invalid
   3. The Lifetime field is updated to current time plus DELETE_PERIOD.
      Before this time, the entry SHOULD NOT be deleted.*/

 /*    1.The destination sequence number of this routing entry, if it
	 	 exists and is valid, copied from the incoming RERR in case (iii) above*/
	    //se supone que todas las rutas en el map existen porque son mis destinos afectados..
        
        if ( unaEntrada.getValidDestSeqNum() )
		{
	         unaEntrada.setDestSequenceNumber (newSeqNum) ;
		}		

 /*    2.The entry is invalidated by marking the route entry as invalid 
	   3.The Lifetime field is updated to current time plus DELETE_PERIOD.
      	 Before this time, the entry SHOULD NOT be deleted.*/
			
			//tengo que cancelar el evento de expiracion de la ruta
			  cancelarEventoRouteExpire ( unaEntrada );
			//establecer la ruta incativa, lo que genera un nuevo evento, 
			// en la funcion realiza 2 y 3 (cuando invalida crea el evento)
			  const double newLifetime	= pEventScheduler->current_time() + deletePeriod	;
			  establecerRutaInactiva    ( unaEntrada , newLifetime );
	}
	
	// ----------------------------------------------------------------------

/*	const AodvUgdTableEntery &
	AodvUgdRoutingTable::
	devolverEntradaDestino		( const std::string dest_) throw()
	{
		std::map<std::string,AodvUgdTableEntery*>::iterator it;
		it=routingTableMap.find(dest_);
		AodvUgdTableEntery *unaEntrada=it->second;
		 return *unaEntrada;

	}*/

	// ----------------------------------------------------------------------
	std::string
	AodvUgdRoutingTable::
		toString(void)
		throw()

			{
		std::stringstream ss;
			   ss <<"\nTabla Nodo: " << direccionOwnerNode /**ownerNode->label()*/ << '\n' ;
			   ss <<  "	dest	|	nextHop		| hops | destSeqNum | validDestSeqNum |	 state	 | lifeTime |"	<< '\n' ;

			   for (auto it=routingTableMap.begin(); it!=routingTableMap.end(); ++it)
			   {
				   AodvUgdTableEntery *unaEntrada=it->second;
				   ss << unaEntrada->getDestino() 				<<"		"<<
						 unaEntrada->getNextHop() 				<<"	    "<<
						 unaEntrada->getHops() 					<<"		"<<
						 unaEntrada->getDestSequenceNumber() 	<<"		"<<
						 unaEntrada->getValidDestSeqNum() 		<<"		"<<
						 unaEntrada->getState() 				<<"		"<<
						 unaEntrada->getLifeTime() 				<<'\n';
			   }

			   return (ss.str());
			}

	// ----------------------------------------------------------------------

	void
	AodvUgdRoutingTable::
	show(void)
	throw()
	{
		if ( ! routingTableMap.empty() ) {// no mostrar tablas vacias
			std::cout << AodvUgdRoutingTable::toString()<< '\n';
		}
	}

	//----------------------------------------------------------------------
	// ----------------------------------------------------------------------

	void
	AodvUgdRoutingTable::
	log(void)
	throw()
	{
		if ( ! routingTableMap.empty() ) {// no mostrar tablas vacias
			   LoggerAodv::Instance()->logCsvRoutingTable_comoTabla(
			   							direccionOwnerNode , getRoutingTableMap() ) ;
		}
	}

	//----------------------------------------------------------------------

}
#endif
