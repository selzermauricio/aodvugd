#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD

//#include "legacyapps/aodvugd/aodvugd_logger.h"
#include "legacyapps/aodvugd/aodvugd_processor.h"
#include "legacyapps/aodvugd/aodvugd_processorhello.h"
//#include "legacyapps/aodvugd/aodvugd_message.h"
//#include "sys/simulation/simulation_controller.h"
#include "sys/node.h"
#include <iostream>
#include <limits>
#include <cmath>


//#include "legacyapps/aodvugd/aodvugd_defaultconfig.h"
#include <utility>      // std::pair
//#include "legacyapps/aodvugd/aodvugd_route_discovery.h"
#include <sstream>   /* toString */
#include <string> /*to_string(int)*/



//#include "legacyapps/aodvugd/aodvugd_routing_table.h"
#include <map>
#include <algorithm> //std::max
#include <assert.h> //assert
#include <set> //set






namespace aodvugd
{

	//----------------------------------------------------------------------
// clase EscucharVecinoEventTag----------------------------------------------------------------------
	HelloEscucharEventTag:: //constructor
	HelloEscucharEventTag( double startTime , double endTime , std::string vecino )
	:				//inicializacion
	startTime_			{startTime},
	endTime_			{endTime},
	vecino_				{vecino}

	{}
//----------------------------------------------------------------------
	//destructor
	HelloEscucharEventTag::
	~HelloEscucharEventTag(){}

	// get set----------------------------------------------------------------------
	inline double
	HelloEscucharEventTag::
	getStartTime  (void) const	throw()
	{
		return startTime_ ;
	}
	//----------------------------------------------------------------------
	inline double
	HelloEscucharEventTag::
	getEndTime  (void) const	throw()
	{
		return endTime_ ;
	}
	//----------------------------------------------------------------------
	inline std::string
	HelloEscucharEventTag::
	getVecino  (void) const	throw()
	{
		return vecino_ ;
	}
		//----------------------------------------------------------------------
	void
	HelloEscucharEventTag::
	modificar (double startTime , double endTime ) 	throw()
	{
		startTime_	=	startTime ;
		endTime_ 	=	endTime   ;
	}
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// clase AodvUgdProcessorHello----------------------------------------------------------------------
	AodvUgdProcessorHello::
	AodvUgdProcessorHello()
	{}
	// ----------------------------------------------------------------------
	AodvUgdProcessorHello:: //por ahora no puedo ocupar porque se crean los procesos antes
	AodvUgdProcessorHello( double helloInterval , unsigned int allowedHelloLoss , 
						   double deletePeriod  , shawn::EventScheduler& pEventScheduler,
						   AodvUgdProcessor&  procesoAodv , AodvUgdRoutingTable& unaTablaRuteo )
	:
		//inicializacion
		helloInterval_ 					{  helloInterval },
		allowedHelloLoss_ 				{  allowedHelloLoss },
		deletePeriod_ 					{  deletePeriod },
		pEventScheduler_ 				{ &pEventScheduler} ,
		procesoAodv_	 				{ &procesoAodv},
		unaTablaRuteo_ 					{ &unaTablaRuteo}
	{
		std::cout<< "pEventScheduler_Hello : " << &pEventScheduler_ <<  std::endl;
	}
	// ----------------------------------------------------------------------
	AodvUgdProcessorHello::
	~AodvUgdProcessorHello()
	{}

	// ----------------------------------------------------------------------
	// get set----------------------------------------------------------------------
	void 
	AodvUgdProcessorHello::
	inicilizar( double helloInterval , unsigned int allowedHelloLoss , 
				double deletePeriod  , shawn::EventScheduler& pEventScheduler,
				AodvUgdProcessor&  procesoAodv , AodvUgdRoutingTable& unaTablaRuteo )
      throw()	
	{
				//inicializacion
		helloInterval_ 					=  helloInterval;
		allowedHelloLoss_ 				=  allowedHelloLoss;
		deletePeriod_ 					=  deletePeriod;
		pEventScheduler_ 				=  &pEventScheduler;
		procesoAodv_	 				=  &procesoAodv;
		unaTablaRuteo_ 					=  &unaTablaRuteo;

		iniciarProcesoHello();  
	}
	// metodos----------------------------------------------------------------------
	// ----------------------------------------------------------------------   
   void
   AodvUgdProcessorHello::
   special_boot( void ) //solo un nodo utiliza el special boot. 
      throw()
   {
 std::cout<< "--------------------------special_boot Hello " << owner().id()<< std::endl;
   }
   // ----------------------------------------------------------------------
   void
   AodvUgdProcessorHello::
   boot( void )
      throw()
   {
   	 std::cout<< "--------------------------boot Hello " << owner().id()<< std::endl;

	   const shawn::SimulationEnvironment& se = owner().world().
	   	                                      simulation_controller().environment();    


   }
   // handle
   // ----------------------------------------------------------------------
   bool
   AodvUgdProcessorHello::
   process_message(  const shawn::ConstMessageHandle& mh )
      throw()
   { 
   	//no atiendo mis propios mensajes, es para controlar el nivel fisico, 
	//TODO se puede redefinir el loop 
      if( owner() == mh->source() ) return true; //TODO ver que valor devolver

      const AodvUgdRREP* rrep = dynamic_cast< const AodvUgdRREP*> ( mh.get() );
/*hello*/ if (  ( rrep != NULL ) && ( isHelloMessage (*rrep)  )    )  
      	  {
      			handle_Hello( *rrep );
      	  }
      	  else 											
      	  {
      	  	//cualquier mensaje que me llega tiene que ser de mi vecino, 
      	  	//porque lo define el radio

/*      	 If, within the past DELETE_PERIOD, it has received
      	  	 a Hello message from a neighbor, and then for that neighbor does not
      	  	 receive any packets (Hello messages or otherwise) for more than
      	  	 ALLOWED_HELLO_LOSS * HELLO_INTERVAL milliseconds*/

/*      	 verifico! (Hello messages or otherwise) for more than
      	  	 ALLOWED_HELLO_LOSS * HELLO_INTERVAL milliseconds*/
/*otro*/     handle_OtherMessage ( mh.get()->source().label() );
      	  }
      //con valor en false pasa a otros procesos!
      //como los otros procesos necesitan el mensaje HELLO no confirma que recibio	  
      return false; 
   }

   // ----------------------------------------------------------------------
	void
	AodvUgdProcessorHello::
	timeout( shawn::EventScheduler&, shawn::EventScheduler::EventHandle unEventHandle,
	         double, shawn::EventScheduler::EventTagHandle& unEventTagHandle )
	      throw()
	   {
		   std::cout<< "TIMEOUT Hellloooo: " <<	std::endl;
		   //verifico si es el evento pProximoHelloBroadcast
		   if( unEventHandle == pProximoHelloBroadcast ) 
		   {
		   		handle_EventExpireBroadcastTime();
		   		return;
		   }

		   const HelloEscucharEventTag* unHelloEscucharEventTag =
				   dynamic_cast <const HelloEscucharEventTag*> ( unEventTagHandle.get() );
		 
		   if ( unHelloEscucharEventTag != NULL){  //link Break!!!
			   std::cout<< "*************** TIMEOUT unHelloEscucharEventTag: " <<	std::endl;
			   handle_EventExpireEscucharVecino ( unHelloEscucharEventTag->getVecino() ); 
			   return;
		   }
	   }
   // ----------------------------------------------------------------------
   void
   AodvUgdProcessorHello::
   work( void )
      throw()
   {
      
   }

  // ----------------------------------------------------------------------   

/////////////////////////////////////////////////////////////////
/*********************** HANDLE HELLO MESSAGE ******************/
/////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------
   void
   AodvUgdProcessorHello::
   handle_Hello( const AodvUgdRREP& rrep )
   throw()
	{
/*	A node MAY determine -connectivity by listening for packets from its
   set of neighbors.  If, within the past DELETE_PERIOD, it has received
   a Hello message from a neighbor, and then for that neighbor does not
   receive any packets (Hello messages or otherwise) for more than
   ALLOWED_HELLO_LOSS * HELLO_INTERVAL milliseconds, the node SHOULD
   assume that the link to this neighbor is currently lost.  When this
   happens, the node SHOULD proceed as in Section 6.11.*/

		std::string vecino 	=	rrep.source().label();
		//verifico si estoy ecuchando al vecino
		if ( existeVecino (vecino) )
		{
			//si me llego un Hello y todavia es mi vecino
			//reseteo para cumplir con "within the past DELETE_PERIOD"
			//entonces.. siempre que llega un hello despues espera por 
			//otro hello o algun otro mensaje
			reniciarEventoEscucharVecino (vecino , 
					deletePeriod_ + (allowedHelloLoss_ * helloInterval_) );
		}
		else //es el primer hello
		{
			//unicamente el mensaje hello comienza una nueva escucha
			escucharNuevoVecino (vecino);
		}
	}
// ----------------------------------------------------------------------
	bool
	AodvUgdProcessorHello::
	existeVecino (  std::string dest_) throw()
	{	
		mapTypelistenNeighbors ::iterator it;
		it = listenNeighbors_.find( dest_ );
		 if ( it != listenNeighbors_.end() )
		 {
			 return true;
		 }
		 return false;
	}
// ----------------------------------------------------------------------
	void
	AodvUgdProcessorHello::
	reniciarEventoEscucharVecino( std::string vecino , double timeout )
	throw()
							//pEventScheduler programador de Eventos
	{
		double tiempoActual	=	pEventScheduler_->current_time();
		double tiempoEvento	=	tiempoActual + timeout ;


		 shawn::EventScheduler::EventHandle pEventHandle = devolverEventoEscucharVecino (vecino); 
        //el tag es const asi que voy a crear de nuevo porque quiero info para el logger
		//creo un nuevo evento
		shawn::EventScheduler::EventHandle newEventHandle = crearEventoEscucharVecino (vecino , timeout);
		//modifico el map
		modificarEventoEscucharVecinoMap (vecino , newEventHandle);
		//borro despues para que se libere en handle
		pEventScheduler_->delete_event ( pEventHandle );
	}
// ----------------------------------------------------------------------
    shawn::EventScheduler::EventHandle	
	AodvUgdProcessorHello::
	crearEventoEscucharVecino ( std::string vecino , double timeout ) 
	throw()						//pEventScheduler programador de Eventos
	{
		double tiempoActual	=	pEventScheduler_->current_time() ;
		double tiempoEvento	=	tiempoActual + deletePeriod_ ;

		HelloEscucharEventTag* unHelloEscucharEventTag =new HelloEscucharEventTag
													(   tiempoActual /*startTime*/ , 
											    		tiempoEvento /*endTime*/ ,
											    		vecino ) ;

		return 
		pEventScheduler_->new_event  ( *this , tiempoEvento , unHelloEscucharEventTag );
	}
// ----------------------------------------------------------------------
   bool
   AodvUgdProcessorHello::
   isHelloMessage  (const AodvUgdRREP &unRREP )   
   throw()
   {
   if (unRREP.getIpDestino() == BROADCAST_ADDRESS )
      {
/*      	dos formas para saber si es HELLO
      	1. ipOrigen==ipDestino, cuando enviamos un HELLO no conocemos la direccion de origen
      	2. El RREP es un broadcast, es el unico RREP por broadcast.
      	otros simuladores verifican con el HOP, pero no abarca todos los casos.*/
   /*1*/assert ( unRREP.getOrigen() == unRREP.getDestino() );
   /*2*/assert ( unRREP.getIpDestino() == BROADCAST_ADDRESS );
      	//tiene que llegar desde un vecino
      	assert ( unRREP.getDestino() == unRREP.source().label() );
      	return true;
      }
   return false;
   }

	// ----------------------------------------------------------------------
   void
   AodvUgdProcessorHello::
   handle_OtherMessage( std::string vecino )
   throw()
	{
/*	A node MAY determine connectivity by listening for packets from its
   set of neighbors.  If, within the past DELETE_PERIOD, it has received
   a Hello message from a neighbor, and then for that neighbor does not
   receive any packets (Hello messages or otherwise) for more than
   ALLOWED_HELLO_LOSS * HELLO_INTERVAL milliseconds, the node SHOULD
   assume that the link to this neighbor is currently lost.  When this
   happens, the node SHOULD proceed as in Section 6.11.*/

	//manejo cualquier otro mensaje que llega desde el vecino
		//verifico si estoy ecuchando al vecino
		if ( existeVecino (vecino) ) //si existe es porque todavia esta el evento
		{
			reniciarEventoEscucharVecino (vecino , allowedHelloLoss_ * helloInterval_ );
		}
		//else no hago nada
	}
// ----------------------------------------------------------------------
	shawn::EventScheduler::EventHandle
	AodvUgdProcessorHello::
	devolverEventoEscucharVecino (  std::string dest_) throw()
	{	
		mapTypelistenNeighbors ::iterator it;
		it = listenNeighbors_.find( dest_ );
		return it->second;
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdProcessorHello::
	modificarEventoEscucharVecinoMap (  std::string dest_ , 
					shawn::EventScheduler::EventHandle unEventHandle  ) throw()
	{	
		mapTypelistenNeighbors ::iterator it;
		it = listenNeighbors_.find( dest_ );
		it->second  = unEventHandle;
	}
// ----------------------------------------------------------------------
	void
	AodvUgdProcessorHello::
	handle_EventExpireEscucharVecino (std::string vecino )
    throw()
	{	
		/*the node SHOULD assume that the link to this neighbor 
		is currently lost.  When this happens, the node SHOULD 
		proceed as in Section 6.11.*/
		
		//link break
		listenNeighbors_.erase (vecino);
		//TODO informar la perdida de conexion con el vecino para generar RERR
		procesoAodv_->helloInformaDetectesLinkBreak(vecino);
	}	
// ----------------------------------------------------------------------
    
    void
	AodvUgdProcessorHello::
	escucharNuevoVecino ( std::string vecino )
    throw()
    {
/* If, within the past DELETE_PERIOD, it has received
   a Hello message from a neighbor, and then for that neighbor does not
   receive any packets (Hello messages or otherwise) for more than
   ALLOWED_HELLO_LOSS * HELLO_INTERVAL milliseconds*/

    	//la primera vez se registra se utilizda DELETE_PERIOD porque llego un hello 
    	//desde el vecino.
    	
    	//cuando ocurra este evento se verifica si se recibio un hello u otro durante
    	//ALLOWED_HELLO_LOSS * HELLO_INTERVAL milliseconds
    	//en total cuando se recibe un hello se puede esperar mucho!
    	shawn::EventScheduler::EventHandle	pEventHandle 	= 
    	crearEventoEscucharVecino (vecino , deletePeriod_ + ( allowedHelloLoss_ * helloInterval_ ) );

    	listenNeighbors_.insert (  parlistenNeighbors (vecino ,pEventHandle)  );
    }
//----------------------------------------------------------------------
/////////////////////////////////////////////////////////////////
/***********************   BroadcastHello     ******************/
/////////////////////////////////////////////////////////////////
	void
	AodvUgdProcessorHello::
	nodoEnvioUnBroadcast () throw()
	{	
		//cada vez que se envia un broadcast se avisa por aca
		//TODO ver si hace falta hacer algo mas
		//moverProximoHelloBroadcast( ); es mas simple solo verifica periodicamente
		contadorBroadcast_ + 1;
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdProcessorHello::
	iniciarProcesoHello () throw()
	{	
		//TODO despertar proceso HELLO
		crearNuevoProximoHelloBroadcast();	
		contadorBroadcast_	=	0;
	}
	// ----------------------------------------------------------------------
	void	
	AodvUgdProcessorHello::
	crearNuevoProximoHelloBroadcast( )
	throw()
							//pEventScheduler programador de Eventos
	{
		//espero un tiempo para enviar el proximo hello
		double tiempoActual	=	pEventScheduler_->current_time();
		
		double tiempoEvento	=	tiempoActual + helloInterval_ ;
		//double tiempoEvento	= 2.00+ tiempoActual;
		//por ahora no hace falta tag
		pProximoHelloBroadcast	=	pEventScheduler_->new_event 
										( *this , tiempoEvento , NULL  );
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdProcessorHello::
	moverProximoHelloBroadcast( )
	throw()
	//por ahora no se usa, porque es mas simple, 
	//solo verifica en un intervalo si se envio almenos un broadcast
	{
		//espero un tiempo para enviar el proximo hello
		double tiempoActual	=	pEventScheduler_->current_time();
		double tiempoEvento	=	tiempoActual + helloInterval_ ;

		pEventScheduler_->move_event ( pProximoHelloBroadcast , tiempoEvento );
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdProcessorHello::
	enviarBroadcastHello ( )
    throw()
    {
/*    within the last HELLO_INTERVAL.  If it has not, it MAY
   broadcast a RREP with TTL = 1, called a Hello message, with the RREP
   message fields set as follows:
      Destination IP Address         The node's IP address.
      Destination Sequence Number    The node's latest sequence number.
      Hop Count                      0
      Lifetime                       ALLOWED_HELLO_LOSS * HELLO_INTERVAL
*/
   	 unsigned int 	ttl 	 	= 	 1;
   	 std::string direccionNodo 	=	 procesoAodv_->owner().label();
   	 unsigned int destSeqNum 	=	 procesoAodv_->getNodoSeqNum(); 
   	 double lifeTime 			= 	 allowedHelloLoss_ * helloInterval_;

//direccion de origen = destino , al menos por ahora
   	 procesoAodv_->sendRREP (	direccionNodo /*dest*/ 		, direccionNodo /*origen*/ ,
			     				destSeqNum /*destSequNumb*/ , false /*ackFlag*/ 	   ,  
			     				false /*repairFlag*/ 		, 0 /*hops */			   , 
			     				lifeTime /*lifeTime */		, ttl /*ttl*/ 			   ,
			     				BROADCAST_ADDRESS /*labelNodoNextHop*/ 				 ) ;

    }
    // ----------------------------------------------------------------------
	void
	AodvUgdProcessorHello::
	handle_EventExpireBroadcastTime ( )
    throw()
    {
    	/*		A node MAY offer connectivity information by broadcasting local Hello
   messages.  A node SHOULD only use hello messages if it is part of an
   active route.  Every HELLO_INTERVAL milliseconds, the node checks
   whether it has sent a broadcast (e.g., a RREQ or an appropriate layer
   2 message) within the last HELLO_INTERVAL.  If it has not, it MAY
   broadcast a RREP with TTL = 1,*/

    	//expiro el evento..hay que enviar el broadcast hello y 
    	//establecer el evento para el proximo broadcast
    	if ( (unaTablaRuteo_->formoParteDeRutaActiva () )  		//solo envio si formo parte de rutas activas
    	&&   (contadorBroadcast_ == 0 )					)   //si no se envio ningun broadcast
    	{
    		enviarBroadcastHello();	
    		contadorBroadcast_= 0;
		}
    	crearNuevoProximoHelloBroadcast(); //creo elevento para el proximo intervalo a verificar
    }
// FIN BroadcastHello!!----------------------------------------------------------------------
	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------

    // ----------------------------------------------------------------------

    // ----------------------------------------------------------------------
	void
	AodvUgdProcessorHello::
	desactivarHello (  )
    throw()
    {

    }
    // ----------------------------------------------------------------------
	void
	AodvUgdProcessorHello::
	testCall (  )
    {
    	  std::cout<< "testCall Hello : " << std::endl;
    }



}
#endif
