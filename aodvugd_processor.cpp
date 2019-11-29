#include "_legacyapps_enable_cmake.h"

#ifdef ENABLE_AODVUGD
#include "legacyapps/aodvugd/aodvugd_logger.h"

#include "legacyapps/aodvugd/aodvugd_processor.h"
#include "legacyapps/aodvugd/aodvugd_message.h"
#include "legacyapps/aodvugd/aodvugd_ping_app.h"


//#include "legacyapps/aodvugd/aodvugd_observer_estado_ruta.h"
#include "sys/simulation/simulation_controller.h"
#include "sys/node.h"
#include <iostream>
#include <limits>
#include <cmath>


//#include "legacyapps/aodvugd/aodvugd_defaultconfig.h"
#include <utility>      // std::pair
//#include "legacyapps/aodvugd/aodvugd_route_discovery.h"
#include <sstream>   /* toString */
#include <string> /*to_string(int)*/
#include "sys/taggings/basic_tags.h" //int tag

//tarea ping nodos
#include "legacyapps/aodvugd/aodvugd_nodes_pings_task.h"  
#include "sys/simulation/simulation_task.h"
#include "sys/simulation/simulation_task_keeper.h"


namespace aodvugd
{
// clase BufferRREQ_EventTag----------------------------------------------------------------------
/*use como plantilla class RoutingEventTag*/
	//constructor
	BufferRREQ_EventTag::
	BufferRREQ_EventTag( std::string origino, unsigned int RREQ_ID )
	//inicializacion
	:
			origino_			{origino},
			RREQ_ID_			{RREQ_ID}
			{}
	//----------------------------------------------------------------------
	//destructor
	BufferRREQ_EventTag::
	~BufferRREQ_EventTag(){}


	//gets----------------------------------------------------------------------
	inline const std::string
	BufferRREQ_EventTag::
	getOrigino () const
	throw()
	{
		return origino_;
	}

	inline const unsigned int
	BufferRREQ_EventTag::
	getRREQ_ID () const
	throw()
	{
		return RREQ_ID_;
	}

// clase tagRREQ_stat----------------------------------------------------------------------
	//constructor
	tagRREQ_stat::
	tagRREQ_stat( )
	//inicializacion
	: Tag("tagRREQ_stat"/*n*/, false/*lock*/)
	{}
	//----------------------------------------------------------------------
	//destructor
	tagRREQ_stat::
	~tagRREQ_stat() {}
	//gets----------------------------------------------------------------------
	inline  unsigned int
	tagRREQ_stat::
	getContadorRenviosRREQ ()
	throw()
	{
		return contadorRenviosRREQ_;
	}
	//----------------------------------------------------------------------
	inline  unsigned int
	tagRREQ_stat::
	getContadorHandle_RREQ ()
	throw()
	{
		return contadorHandle_RREQ_;
	}
	//----------------------------------------------------------------------
	void
	tagRREQ_stat::
	setContadorRenviosRREQ (unsigned int newContador)
	throw()
	{
		contadorRenviosRREQ_=newContador;
	}
	//----------------------------------------------------------------------
	void
	tagRREQ_stat::
	setContadorHandle_RREQ (unsigned int newContador)
	throw()
	{
		contadorHandle_RREQ_=newContador;
	}
	// ----------------------------------------------------------------------
	const std::string&
	tagRREQ_stat::
	type_identifier( void )
	const throw()
	{
		return "tagRREQ_stat";
	}
	// ----------------------------------------------------------------------
	std::string
	tagRREQ_stat::
	encoded_content( void )
	const throw( std::runtime_error )
	{
/*		std::stringstream ss;
		ss << value();
		return ss.str();*/
		return " ";
	}
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// clase AodvUgdProcessor----------------------------------------------------------------------
   AodvUgdProcessor::
   AodvUgdProcessor()
/*      : connected_      ( false ),
        senderPing_     ( false ),
        hop_dist_       ( 0 ),
        send_period_    ( 1.2),
        predecessor_    ( NULL )*/

   	   //configuracion	( )
   	  // unaTablaRuteo	(  ) //cuando se crea new devuelve un puntero.. y para usar el contenido hay que usar *
   	  // procesoDiscovrey () /* {( new  AodvUgdRouteDiscovery () ) } */ 
   {}
   // ----------------------------------------------------------------------
   AodvUgdProcessor::
   ~AodvUgdProcessor()
   {}
   // ----------------------------------------------------------------------
   void
   AodvUgdProcessor::
   special_boot( void ) //solo un nodo utiliza el special boot. por ahora el que envia el ping
      throw()
   {
    //logger
    LoggerAodv::Instance()->setWorld( owner().world() );
	  
    //Tag para almacenar info stat de los nodos
    iniciarTagRREQ_stat();

      //llamar a la tarea nodes_ping_rask por unica vez para que configure 
      //los nodos envian  ping
     shawn::SimulationController& sc= owner_w().world_w().simulation_controller_w();

     shawn::SimulationTaskHandle unSimulationTaskHandle = 
                            sc.simulation_task_keeper_w().find_w( "nodes_pings" );

     unSimulationTaskHandle->run(sc);

   }


   // ----------------------------------------------------------------------
   void
   AodvUgdProcessor::
   boot( void )
      throw()
   {
/*	   const shawn::SimulationEnvironment& se = owner().world().
	   	                                      simulation_controller().environment();*/

      crearConfiguracionAodv();
      crearRouteTable();
      crearProcesoDiscovrey();
      crearProcesoHello();
      
   }


   // handle
   // ----------------------------------------------------------------------
   bool
   AodvUgdProcessor::
   process_message(  const shawn::ConstMessageHandle& mh )
      throw()
   {  
//no atiendo mis propios mensajes, es para controlar el nivel fisico, 
//TODO se puede redefinir el loop 
      if( ( owner() ) == ( mh->source() ) ) return true; 

      //****************************************************************************
      //RREQ
      /*el mensaje tiene que ser todo const porque si se modifica, tambien se 
      modifica el mensaje para otros nodos*/
      //obtiene un puntero al mensaje para no tener que crear otro
      const AodvUgdRREQ* rreq = dynamic_cast< const AodvUgdRREQ*> ( mh.get() );
      if ( rreq != NULL )
      {
        LoggerAodv::Instance()->logCsvRREQ( *rreq , owner() ,  "Nodo recibe RREQ");
    	  // *rreq  = es que pasa el valor de rreq
    	  // la funcion handle_RREQ( const AodvUgdRREQ& rreq )
    	  // se pasa por referencia para no tener que hacer una copia del objeto,
    	  // pero se asigna const para que no se pueda modificar
    	  handle_RREQ( *rreq );
     		return true;
     }
      //****************************************************************************
      std::string miDireccion    = owner().label();

      //RREP
      const AodvUgdRREP* rrep = dynamic_cast< const AodvUgdRREP*> ( mh.get() );
      if ( rrep != NULL )
      {
        //el RREP es enviado por unicast, se realiza la abstracion de la capa mac aca porque
        // todos los mensajes van por broadcast
        std::string unicastDelRREP = rrep->getIpDestino();
        if( miDireccion == unicastDelRREP ) //era para mi
        {
            LoggerAodv::Instance()-> logCsvRREP_Movimientos  ( *rrep , owner().label() ,"Nodo recibe RREP");
            handle_RREP( *rrep );
        }
        else if(unicastDelRREP == BROADCAST_ADDRESS) //HELLO
        {
          //es hello, pero se encarga el proceso HELLO
          //solo por las dudas, pero deberia agarar primero el proceso HELLO el mensaje
          return false;
          
        }
        return true;
      }
      //****************************************************************************
      //RERR
      const AodvUgdRERR* rerr = dynamic_cast< const AodvUgdRERR*> ( mh.get() );
      if ( rerr != NULL )
      {
        //el RERR es enviado por unicast, se realiza la abstracion de la capa mac aca porque
        // todos los mensajes van por broadcast
        std::string unicastDelRERR = rerr->getIpDestino();
        if( miDireccion == unicastDelRERR ) //era para mi
        {
            //LoggerAodv::Instance()-> logCsvRERR_Movimientos  ( *rerr , owner().label() ,"Nodo recibe RERR");
            handle_RERR( *rerr );
        }
        else if(unicastDelRERR == BROADCAST_ADDRESS) //RERR por broadcast
        {
            handle_RERR( *rerr );
        }
        return true;
      }
      //****************************************************************************
      //si es algun otro paquete IP que no sea de AODV
      const AodvUgdIpHeader* paqueteIP = dynamic_cast< const AodvUgdIpHeader*> ( mh.get() );
      if ( paqueteIP != NULL )
      {
        //solo recibo los que tengan mi "direccion MAC" como siguiente salto
        //si no es para mi ignoro
        if( miDireccion != paqueteIP->getMacDestino()   )
           return true;
          
        handlePaqueteIp(*paqueteIP);  
      }

      return true; //shawn::Processor::process_message( mh );
   }
      //****************************************************************************
      //Data
      /*const AodvUgdRERR* rerr = dynamic_cast< const AodvUgdRERR*> ( mh.get() );
      if ( rerr != NULL )
      {

      }*/
   // ----------------------------------------------------------------------

   void
   AodvUgdProcessor::
   work( void )
      throw()
   {
      unaTablaRuteo->log();


      if (owner().is_special_node() )
      {
         LoggerAodv::Instance()->logCsvRREQ_Buffer_comoTabla 
                              ( owner().label() , getBufferRREQ() ) ; 
      }

  /*  	  if(owner().current_time()==325){
    	  const AodvUgdRREQ* pRREQ_anterrior 	= 	procesoDiscovrey->obtenerUltimoRREQ_Destino ( testDestino );
    	  unsigned int oldTTL	=	pRREQ_anterrior->getTtl();
    	  std::cout<< "************************************ RREQQQQQQQQQQQQQQQQQ Ultimo TTL: " <<oldTTL<< std::endl;
    	  }*/
    	/*	std::cout<< " senderPing_" << std::endl;

         int min = std::numeric_limits<int>::max(); 
         int max = std::numeric_limits<int>::min();
         double avg = 0;
         
         for ( std::map<const shawn::Node*, int>::iterator
                  it = network_nodes_.begin();
                  it != network_nodes_.end();
                  ++it )
         {
            if ( it->second < min ) min = it->second;
            if ( it->second > max ) max = it->second;
            avg += it->second;
         }
         avg /= (double)network_nodes_.size();
         
         INFO( logger(), "Number of nodes known to gate:  " << network_nodes_.size() );
         INFO( logger(), "  Min hops:  " << min );
         INFO( logger(), "  Max hops:  " << max );
         INFO( logger(), "  Avg hops:  " << avg );*/
      
   }

   // ----------------------------------------------------------------------

   void 
   AodvUgdProcessor::
   timeout( shawn::EventScheduler&, shawn::EventScheduler::EventHandle, 
            double, shawn::EventScheduler::EventTagHandle& unEventTagHandle) 
      throw()
   {  

  
       const BufferRREQ_EventTag* unBufferRREQ_EventTag =
       dynamic_cast<const BufferRREQ_EventTag*> ( unEventTagHandle.get() );

       /*si se disparo el evento unEventTagHandle,
       es porque expiro el tiempo de espera de un registro del buffer*/
       if ( unBufferRREQ_EventTag != NULL){
         std::cout<< "*************** TIMEOUT unBufferRREQ_EventTag: " <<  std::endl;
         //por ahora solo borra del buffer
         handle_EventExpireTimeBufferRREQ ( unBufferRREQ_EventTag->getRREQ_ID(),
                                            unBufferRREQ_EventTag->getOrigino()  );

       }
   }

   // ----------------------------------------------------------------------
   // get set----------------------------------------------------------------------
   const unsigned int AodvUgdProcessor:: getNodoSeqNum ( void ) const throw()
   { return nodoSeqNum_;}

// metodos----------------------------------------------------------------------
// ----------------------------------------------------------------------
   void
   AodvUgdProcessor::
   handle_RREP( const AodvUgdRREP& rrep )
   throw()
{
  const std::string labelDest   = rrep.getDestino();
  const std::string labelOrigen = rrep.getOrigen();
  const std::string labelSource = rrep.source().label();
  const std::string miLabelNode = owner().label();
  /*check blacklist solo para RREQ*/
    //sumarContadorStatGlobalRREQ_Handle();

/*    std::stringstream mensaje;
    mensaje <<"Handle_RREQ contador RREQs: "<< 
              getContadorStatGlobalRREQ_Handle( );
     LoggerAodv::Instance()->logCsvRREQ(rreq , owner().label() ,mensaje.str() );
*/
  //************************************************************
  /*  first creates or updates a route to the previous hop
      se registra en la tabla la entrada PreviousHop (SOURCE)*/
  registrarRutaSource  ( labelSource /*direccion*/) ;

  //*************************************************************
  /*    forward route to the Originator of the RREP*/
  registrarRutaDestinoRREP (rrep ) ;
     
  //******************************************************************

   /*Si el nodo no es el origin de la solicitud de ruta, se reenvía
    el RREP hacia el origen*/
     /*verifico si soy el origen*/
  if( miLabelNode == labelOrigen )
  {
    /*soy el origen*/

/*  no hace falta hacer nada, en este punto se registro la entrada en la tabla
    la ruta hacia el destino, se le avisa al nodo cuando se regista. */

/*
    en el caso que el nextHop de la ruta  este invalido puede no avisar de cambios
    en la ruta hacia el destino si es que no hace falta actualizar.
    pero la ruta del source si puede cambiar

    como el source es el nextHop hacia la ruta..

    Data packets waiting for a route (i.e., waiting for a RREP after a
   RREQ has been sent) SHOULD be buffered.
    */
    unaTablaRuteo->avisoDiscoveryComplete(labelDest);
  }
  else
  {
    /*nodo intermedio*/
    bool existeEntrada = unaTablaRuteo->existeRutaActivaDestino(labelOrigen);
    if(existeEntrada)
    {     
        /*mandar el RREP desde nodo intermedio hacia el origen

        TODO If a node forwards a RREP
        over a link that is likely to have errors or be unidirectional, the
        node SHOULD set the 'A' flag to require that the recipient of the
        RREP acknowledge receipt of the RREP by sending a RREP-ACK message
       */
       reenviarRREP_Intermedio_a_Origen ( rrep );
        /*TODO registrar precursores*/
    }
    else 
    {
      /*TODO drop!!*/
           return;
    }

  }
  /*unaTablaRuteo->show() ;*/
}
// ----------------------------------------------------------------------
     void
     AodvUgdProcessor::
     reenviarRREP_Intermedio_a_Origen( const AodvUgdRREP &rrep)
     throw()
     {
        /*forwards the RREP towards the originator using the
        information in that route table entry.
  
        por ahora dice solo eso :(
        por ahora voy a sumar el hop y restar el TTl;*/


/*       When any node transmits a RREP, the precursor list for the
   corresponding destination node is updated by adding to it the next
   hop node to which the RREP is forwarded.  Also, at each node the
   (reverse) route used to forward a RREP has its lifetime changed to be
   the maximum of (existing-lifetime, (current time +
   ACTIVE_ROUTE_TIMEOUT).  Finally, the precursor list for the next hop
   towards the destination is updated to contain the next hop towards
   the source*/

//1
/*la lista de precursores para el nodo de destino correspondiente se
actualiza al agregarle el siguiente nodo de salto al que se reenvía el RREP.*/
     std::string  nextHopToOrigen  = unaTablaRuteo-> 
                                     devolverNextHop_a_Destino(rrep.getOrigen() ); //siguiente salto hacia la fuente
     
     unaTablaRuteo->agregarPrecursorEntrada ( rrep.getDestino() /*dest_*/ , nextHopToOrigen /*direccionPrecursor*/);
//2
/*Además, en cada nodo, la ruta (inversa) utilizada para reenviar un RREP ha cambiado su
vida útil para ser el MAX (existing-lifetime, (tiempo actual + ACTIVE_ROUTE_TIMEOUT).*/
     double tiempoActual    = owner().current_time();
     double lifetimeNuevo   = tiempoActual + configuracion.getACTIVE_ROUTE_TIMEOUT();
     double lifeTimeOrigen  = unaTablaRuteo->devolverEntradaDestino ( rrep.getOrigen() )->getLifeTime();
     double lifeTimeNextHop = unaTablaRuteo->devolverEntradaDestino ( nextHopToOrigen )->getLifeTime();
//para optimizar solo actualizo si es mayor pero sin asignar con MAX
     //std::max (  (tiempoActual + configuracion.getACTIVE_ROUTE_TIMEOUT()) )
    if ( lifeTimeOrigen < lifetimeNuevo)    
        unaTablaRuteo->actualizrLifeTimeEntradaActiva(  rrep.getOrigen() , lifetimeNuevo);

    if (lifeTimeNextHop < lifetimeNuevo)
        unaTablaRuteo->actualizrLifeTimeEntradaActiva(  nextHopToOrigen  , lifetimeNuevo);


//3
/*Finalmente, la lista de precursores para el próximo salto hacia el destino se 
actualiza para contener el siguiente salto hacia la fuente*/
    std::string  previousHop  = rrep.source().label(); //proximo salto hacia el destino 
    assert (previousHop==rrep.getIpOrigen()); //verifico por las dudas
    unaTablaRuteo->agregarPrecursorEntrada ( previousHop /*dest_*/ , nextHopToOrigen /*direccionPrecursor*/);

    unsigned int newHop       = rrep.getHops() + 1 ;   //se suma el HOP!
    unsigned int newTtl       = rrep.getTtl () + 1 ;   //se suma el TTL!

    sendRREP (  rrep.getDestino()  , rrep.getOrigen()     , rrep.getDestSequNumb() ,
                rrep.getAckFlag()  , rrep.getRepairFlag() , newHop                 ,
                rrep.getLifeTime() , newTtl               , nextHopToOrigen  ) ;
     }
// ----------------------------------------------------------------------
  /////////////////////////////////////////////////////////////////
  /********************* AvisoEstadoRuta (observer)  *************/
  /////////////////////////////////////////////////////////////////
  
  // ----------------------------------------------------------------------
  //el proceso general se encarga de agregar a los observadores
  void
  AodvUgdProcessor::
  agregarObservadorTabla (  ObserverEstadoRuta * unObserverEstadoRuta, 
                            std::string destino , std::string estado ) throw()
  {
     unaTablaRuteo->escucharUnEstadoRuta (unObserverEstadoRuta ,destino ,estado);
  }
  // ----------------------------------------------------------------------
  //el proceso general se encarga de quitar a los observadores
  void
  AodvUgdProcessor::
  quitarObservadorTabla (  ObserverEstadoRuta * unObserverEstadoRuta, 
                            std::string destino , std::string estado ) throw()
  {
     unaTablaRuteo->cancelarEscucharUnEstadoRuta( 
                                      unObserverEstadoRuta ,destino ,estado);
  }
  // ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  handleAvisoRutaActiva ( std::string destino ) throw()
  {
  //****************  LOG   *****************
   std::ostringstream mensaje;
   mensaje <<"Conoce nueva Ruta Activa.. Destino: "<<destino;
   LoggerAodv::Instance()->logCsvPingDetalle(
                                  owner().label() , 
                                  "" /*origen*/   ,   destino , "AODV",
                                  mensaje.str()                 );
   //**************** FIN LOG ****************


    //si estaba escuchando por una ruta activa es porque tenia mensajes para
    //enviar
    //puede ser que sea un mensaje propio o solo para enrutar.
    enviarPaqutesPendientesDestino(destino);
    
  }
  // ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  observarUnaRuta      (  std::string destino, std::string estado )
  throw()         
  {
  //****************  LOG   *****************
   std::ostringstream mensaje;
   mensaje <<"Escucha por Ruta: "<<estado<<".. Destino: "<<destino;
   LoggerAodv::Instance()->logCsvPingDetalle(   owner().label() ,
                                  "" /*origen*/, destino ,"AODV", mensaje.str() );
   //**************** FIN LOG ****************

    agregarObservadorTabla (  this , destino ,  estado );
  }
  // ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  cancelarObservarUnaRuta(  std::string destino, std::string estado )
  throw()         
  {
    quitarObservadorTabla (  this , destino ,  estado );
  }
  /////////////////////////////////////////////////////////////////
  /***************** FIN AvisoEstadoRuta (observer)  *************/
  /////////////////////////////////////////////////////////////////

  // ----------------------------------------------------------------------
   void
   AodvUgdProcessor::
   handle_RREQ( const AodvUgdRREQ& rreq )
   throw()
{
  const std::string labelDest   = rreq.getDestino();
  const std::string labelOrigen = rreq.getOrigen();
  const std::string labelSource = rreq.source().label();
  const std::string miLabelNode = owner().label();
	/*TODO check blacklist*/
   	sumarContadorStatGlobalRREQ_Handle();

    std::stringstream mensaje;
    mensaje <<"Handle_RREQ contador RREQs: "<< 
              getContadorStatGlobalRREQ_Handle( );
     LoggerAodv::Instance()->logCsvRREQ(rreq , owner().label() ,mensaje.str() );

	//************************************************************
  /*	first creates or updates a route to the previous hop
	    se registra en la tabla la entrada PreviousHop (SOURCE)*/
	registrarRutaSource  ( labelSource /*direccion*/) ;

	//*************************************************************
/*	 check buffer
 *   check  checks received a RREQ with the same Originator IP Address
	   and RREQ ID within at least the last PATH_DISCOVERY_TIME*/
	   //return; si recibio

	/*no se verifica si el mensaje es del propio nodo*/
	if( existeRREQ_Buffer ( rreq.getRREQ_ID() , labelOrigen  )   )
	{
    //Log
    LoggerAodv::Instance()->logCsvRREQ(rreq , owner().label() ,"Drop.. Existe en Buffer" );
		return;
	}
  else 
  {
    agregarRREQ_Buffer ( rreq.getRREQ_ID() , labelOrigen );
  }

	   //*************************************************************
/*	   reverse route to the Originator of the RREQ*/
	registrarRutaOrigenRREQ (rreq ) ;
	   // se pasa el lifeTime y seqNum del RREQ, pero en la funcion se asigna el mayor comparando la ruta

	   //******************************************************************

	 /*Si el nodo no puede responder con un RREP a la solicitud de ruta, se reenvía el RREQ*/
	 	 /*verifico si soy el destino*/

	if( miLabelNode == labelDest )
	{
		/*std::cout<< "soy el destino: " <<	miLabelNode <<" == " << labelDest << '\n'<< std::endl;
		std::cout<< "destino responde RREP........ " << '\n'<< std::endl;*/
		/*soy el destino, enviar un RREP que soy el destino*/
		respuestaDescubrimientoRREP_Destino( labelDest /*direccionDestino*/ , 
                                         labelOrigen /*direccionOrigen */, 
                                         rreq.getDestSequNumb()/* rreqDestSeqNum*/, 
                                         labelSource /*test!*/ );
	}
	else
	{
		/*std::cout<< "no soy el destino: " <<	miLabelNode <<" <> " << labelDest << '\n'<< std::endl;*/
		/*nodo intermedio*/
		bool intermedioRespondeRreq =
		verificarIntermedioRespondeRREQ( labelDest /*dest*/ , 
                                     rreq.getDestSequNumb() /*rreqDesSeqNum*/ ,
										                 rreq.getDestOnlyFlag () /*rreqDestOnlyFlag*/ );
		/*std::cout<< "intermedioRespondeRreq: " <<	intermedioRespondeRreq << '\n'<< std::endl;*/
		if(intermedioRespondeRreq)
		{
				/*mandar un RREP desde nodo intermedio, 
        TODO verificar si hay que mandar un RREP-G o algo mas*/
			 respuestaDescubrimientoRREP_Intermedio	( labelDest /*direccionDestino*/ , 
                                                labelOrigen/*direccionOrigen  */,
              					 	 	 	 	 	              labelSource /*direccionSource */);
		}
		else if ( rreq.getTtl() > 1) // se verifica >1 porque todavia no se resto
		{
			/*std::cout<< "reenviando RREQ... " <<	miLabelNode <<" => " << labelDest << '\n'<< std::endl;*/
			reenviarRREQ_Intermedio (rreq );
			/*no puedo responder con un RREP, no soy el destino / no tengo una ruta activa*/
		}
		else 
    {
      /*TODO drop!!*/
      std::stringstream mensaje;
      mensaje <<"Drop.. RREQ TTL: "<< rreq.getTtl()-1;
			LoggerAodv::Instance()->logCsvRREQ(rreq , miLabelNode ,mensaje.str() );
      return;
		}

	}
	/*unaTablaRuteo->show() ;*/
}

   // ----------------------------------------------------------------------
  void AodvUgdProcessor::registrarRutaSource( std::string source ) throw ()
  {
  double tiempoActual           =   owner().current_time(); //owner_w().world_w().current_time();
  double lifetimeSource         =   configuracion.getACTIVE_ROUTE_TIMEOUT() + tiempoActual;
  unsigned int sourceSeqNum     =   0; //xq se saca de la ip la informacion
  bool validDestSeqNum          =   false;
  std::string state             =   "active";
  unsigned int hops             =   1; //siempre uno porque llega desde el vecino

    	bool existeEntrada = unaTablaRuteo->existeEntradaDestino(source);

    	if (existeEntrada) { //actualizar
    		bool condicionActualizar = verificarActualizarTablaSource(source);
    		/*std::cout<< "Existe Entrada : " <<	direccion <<  std::endl;*/

    		if (condicionActualizar) {
    			 /*std::cout<< "Actualizar condicionActualizar: " <<	direccion <<  std::endl;*/

    			unaTablaRuteo->actualizrEntradaTablaToActive (source /*dest*/, sourceSeqNum,
    					validDestSeqNum, state, hops, source/*nextHopLabel*/, lifetimeSource);
    		}
    	} else {	//crear nueva ruta
    		/*std::cout<< "crear Entrada: " <<	direccion <<  std::endl;*/

    		unaTablaRuteo->crearNuevaEntradaTabla (source /*dest*/, sourceSeqNum,
    				validDestSeqNum, state, hops, source/*nextHopLabel*/, lifetimeSource);

    	}

  }
    // ----------------------------------------------------------------------
    void AodvUgdProcessor::registrarRutaOrigenRREQ	( const AodvUgdRREQ &rreq ) throw ()
    	{ //se usan solo 4 variables y se pasa el rreq, ver mas adelante

/*   When the reverse route is created or updated, the following actions on the route are also carried out:
   1. the Originator Sequence Number from the RREQ is compared to the
      corresponding destination sequence number in the route table entry
      and copied if greater than the existing value there
   2. the valid sequence number field is set to true;
   3. the next hop in the routing table becomes the node from which the
      RREQ was received (it is obtained from the source IP address in
      the IP header and is often not equal to the Originator IP Address
      field in the RREQ message);
   4. the hop count is copied from the Hop Count in the RREQ message;

   Whenever a RREQ message is received, the Lifetime of the reverse
   route entry for the Originator IP address is set to be the maximum of
   (ExistingLifetime, MinimalLifetime), where

      MinimalLifetime =    (current time + 2*NET_TRAVERSAL_TIME -
                           2*HopCount*NODE_TRAVERSAL_TIME).*/

/*  - Si se crea o se actualiza la ruta al origen se llevan a cabo las siguientes acciones:
  RutaOrigen.Activa=TRUE
  RutaOrigen.NextHop=ip.DireccionOrigen
  RutaOrigen.Hop= RREQ.Hop (fue aumentado en el paso 3)
  RutaOrigen.LifeTime=max(existingLifeTime, minimalLifeTime)
  -minimalLifeTime=(tiempoActual + 2*NET_TRAVERSAL_TIME - 2*HopCount*NODE_TRAVERSAL_TIME)
  RutaOrigen.DestSeqNum=max(RREQ.DestSeqNum, RutaOrigen.DestSeqNum)
  RutaOrigen.ValidDestSeqNum=TRUE*/
  double tiempoActual             = owner().current_time();
  const std::string sourceLabel   = rreq.source().label();
  std::string origen              = rreq.getOrigen();
  unsigned int origenSeqNum       = rreq.getOrigenSequNumb();
  bool validDestSeqNum            = true;
  std::string state               = "active";
  unsigned int hops               = rreq.getHops()+1;
  double minimalLifeTime          = (tiempoActual + 2 * configuracion.getNET_TRAVERSAL_TIME() -
                                     2 * hops * configuracion.getNODE_TRAVERSAL_TIME() );
               //(tiempoActual + 2 * NET_TRAVERSAL_TIME - 2 * HopCount * NODE_TRAVERSAL_TIME)
  bool existeEntrada = unaTablaRuteo->existeEntradaDestino(origen);

    	if (existeEntrada) { //actualizar
    		bool condicionActualizar = verificarActualizarTablaOrigenRREQ(origen ,
                                                                      origenSeqNum , 
                                                                      hops );

    		if (condicionActualizar) {
    			/*asigna el lifetime max*/
    			double existingLifeTime  = unaTablaRuteo->devolverEntradaDestino ( origen )->getLifeTime();
    			minimalLifeTime			= std::max ( existingLifeTime , minimalLifeTime );

    			/*si existe la entrada verifica el seqNum max*/
    			unsigned int entradaDestSeqNum	= unaTablaRuteo->devolverEntradaDestino ( origen )->getDestSequenceNumber();
    			origenSeqNum					= std::max ( entradaDestSeqNum , origenSeqNum );

    			unaTablaRuteo->actualizrEntradaTablaToActive ( origen /*dest*/, origenSeqNum,
    					                                  validDestSeqNum, state, hops, 
                                                sourceLabel/*nextHopLabel*/, 
                                                minimalLifeTime );
    		}
    	} else {	//crear nueva ruta
      		unaTablaRuteo->crearNuevaEntradaTabla ( origen /*dest*/, origenSeqNum,
    				                                     validDestSeqNum, state, hops, 
                                                 sourceLabel/*nextHopLabel*/, 
                                                 minimalLifeTime );
    	}
    }
     // ----------------------------------------------------------------------
    void AodvUgdProcessor::registrarRutaDestinoRREP  ( const AodvUgdRREP &rrep ) throw ()
      { //se usan 4 variables, ver mas adelante
//es la ruta hacia adelante (forward)

/*   If the route table entry to the destination is created or updated,
   then the following actions occur:
   (1)  the route is marked as active,
   (2)  the destination sequence number is marked as valid,
   (3)  the next hop in the route entry is assigned to be the node from
        which the RREP is received, which is indicated by the source IP
        address field in the IP header,
   (4)  the hop count is set to the value of the New Hop Count,
   (5)  the expiry time is set to the current time plus the value of the
        Lifetime in the RREP message,
   (6)  and the destination sequence number is the Destination Sequence
        Number in the RREP message.*/

/*  - Si se crea o se actualiza la ruta al destino se llevan a cabo las
      siguientes acciones:*/
          double tiempoActual            = owner().current_time();
          std::string destino            = rrep.getDestino();

  /*(1)*/ std::string state              = "active";
  /*(2)*/ bool validDestSeqNum           = true;
  /*(3)*/ const std::string sourceLabel  = rrep.source().label();
  /*(4)*/ unsigned int hops              = rrep.getHops()+1;
  /*(5)*/ double lifeTime                = ( tiempoActual + rrep.getLifeTime() );
  /*(6)*/ unsigned int destinoSeqNum     = rrep.getDestSequNumb();
          
      bool existeEntrada = unaTablaRuteo->existeEntradaDestino(destino);
      if (existeEntrada) { //actualizar
         bool condicionActualizar = verificarActualizaTablaDestinoRREP(
                                    destino ,destinoSeqNum , hops  );
         if (condicionActualizar) 
         {
              unaTablaRuteo->actualizrEntradaTablaToActive ( destino /*dest*/, destinoSeqNum,
                                                    validDestSeqNum, state, hops, 
                                                    sourceLabel/*nextHopLabel*/, 
                                                    lifeTime  );
         }
      }
      else {  //crear nueva ruta
              unaTablaRuteo->crearNuevaEntradaTabla ( destino /*dest*/, destinoSeqNum,
                                             validDestSeqNum, state, hops, 
                                             sourceLabel/*nextHopLabel*/, 
                                             lifeTime  );
      }
    }

   // ----------------------------------------------------------------------

   bool
   AodvUgdProcessor::
   verificarActualizarTablaSource( std::string source )
   throw()
   {
    /*
    NOTA: no se especifica que se actualiza directamente si la entrada es invalida,
    pero por ahora lo de esa forma debido a errores al rutear..*/
    bool actualizar=false;
    const AodvUgdTableEntery *unaEntrada= unaTablaRuteo->devolverEntradaDestino(source);

    if(! unaEntrada->getIsActive() )
      return true;




/*    6.2. Route Table Entries and Precursor Lists
   The route is only updated if
   the new sequence number is either
   (i)       higher than the destination sequence number in the route
             table, or 
             (no se verifica porque no hay DestSeqNum para el Source)

   (ii)      the sequence numbers are equal, but the hop count (of the
             new information) plus one, is smaller than the existing hop
             count in the routing table, or
            (no se verifica porque no hay DestSeqNum para el Source)

   (iii)     the sequence number is unknown.*/


/*	  Como no hay destSequencenumber, solo puedo actualizar:
      si la entrada tiene el un seqNumInvalido 
      seria no tengo mensajeDestSeqNum  para comparar */
	 		 
		   if(  !(unaEntrada->getValidDestSeqNum())  ) //invalid= unknown
			        actualizar=true;
	   
	   return actualizar;
   }
      // ----------------------------------------------------------------------

   bool
   AodvUgdProcessor::
   verificarActualizarTablaOrigenRREQ( std::string  direccion , 
                                     unsigned int mensajeDestSeqNum ,
                                     unsigned int mensajeHops)
   throw()
   {
/*
    NOTA: no se especifica que se actualiza directamente si la entrada es invalida,
    pero por ahora lo de esa forma debido a errores al rutear..*/

    bool actualizar=false;
    const AodvUgdTableEntery *unaEntrada= unaTablaRuteo->devolverEntradaDestino(direccion);

    if(! unaEntrada->getIsActive() )
      return true;

/*    6.2. Route Table Entries and Precursor Lists
   The route is only updated if
   the new sequence number is either
   (1)       higher than the destination sequence number in the route
             table, or 

   (2)      the sequence numbers are equal, but the hop count (of the
             new information) plus one, is smaller than the existing hop
             count in the routing table, or

   (3)     the sequence number is unknown.*/

    

/*(3)*/if(   !( unaEntrada->getValidDestSeqNum())  ) // invalid= unknown
                actualizar=true;
       
/*(1)*/else if( mensajeDestSeqNum > ( unaEntrada->getDestSequenceNumber() ) )// valid seqNum
                actualizar=true;
       
/*(2)*/else if( mensajeDestSeqNum == unaEntrada->getDestSequenceNumber()
             && mensajeHops< unaEntrada->getHops()  )  
                actualizar=true;

     return actualizar;
   }

         // ----------------------------------------------------------------------

   bool
   AodvUgdProcessor::
   verificarActualizaTablaDestinoRREP( std::string  direccion , 
                                       unsigned int mensajeDestSeqNum ,
                                       unsigned int mensajeHops)
   throw()
   {
/*  the existing entry is updated only in the following circumstances:
   (1)       the sequence number in the routing table is marked as
             invalid in route table entry.

   (2)      the Destination Sequence Number in the RREP is greater than
             the node's copy of the destination sequence number and the
             known value is valid, or

   (3)     the sequence numbers are the same, but the route is is
             marked as inactive, or

   (4)      the sequence numbers are the same, and the New Hop Count is
             smaller than the hop count in route table entry.*/

/*    muy parecido con la verificacion del RREQ pero ademas verifica si los 
    seqNum son iguales y la ruta esta inactiva*/

    bool actualizar=false;
    const AodvUgdTableEntery *unaEntrada= unaTablaRuteo->devolverEntradaDestino(direccion);

    if ( unaEntrada->getValidDestSeqNum() ) 
    {
    /*(2)*/ if( mensajeDestSeqNum > unaEntrada->getDestSequenceNumber() )
                actualizar = true;   
    }
    else        
    {           //seqNum invalido
    /*(1)*/     actualizar = true;
    }    


    if ( mensajeDestSeqNum == unaEntrada->getDestSequenceNumber() ) //seqNum iguales
    {
    /*(3)*/ if( !unaEntrada->getIsActive()  )// entrada inactiva
                actualizar = true;

    /*(4)*/ if( mensajeHops< unaEntrada->getHops()  )
                actualizar = true;
    }
    
    return actualizar;
   }
   // ----------------------------------------------------------------------

   void
   AodvUgdProcessor::
   respuestaDescubrimientoRREP_Destino(   std::string direccionDestino , std::string direccionOrigen ,
                             unsigned int rreqDestSeqNum, std::string labelSource /*test*/)
   throw()
   {
/*	   Si RREQ.DestSeqNum==Nodo.SeqNum se aumenta el número de secuencia del nodo (Nodo.SeqNum++)
	   RREP.DestSeqNum = Nodo.SeqNum
	   RREP.Hop=0
	   RREP.lifeTime=MY_ROUTE_TIMEOUT*/
	   double lifeTime 				=	configuracion.getMY_ROUTE_TIMEOUT();
	   bool ackFlag					  =	false; //TODO
	   bool repairFlag				=	false; //TODO
	   unsigned ttlRREP				=	configuracion.getNET_DIAMETER(); //TODO por ahora se utiliza esa valor, no aclara en el rfc

     /*section 6.1    TODO
      Immediately before a destination node originates a RREP in
      response to a RREQ, it MUST update its own sequence number to the
      maximum of its current sequence number and the destination
      sequence number in the RREQ packet.*/

   /*6.6.1. Route Reply Generation by the Destination
     If the generating node is the destination itself, it MUST increment
     its own sequence number by one if the sequence number in the RREQ
     packet is equal to that incremented value*/
	   if( nodoSeqNum_ == rreqDestSeqNum)
	   {
		   nodoSeqNum_++;
	   }
     //el RREP se envia por el NextHop hacia el origen
     std:: string direccionNextHop = unaTablaRuteo->devolverEntradaDestino(direccionOrigen)->getNextHop();
     // mi ip va en el encabezado
     std:: string ipOrigen         = owner().label();
     
     //verifico para hacer la abstraccion si siempre el NextHop es el Source
     assert( labelSource == direccionNextHop );//despues poner en un log para sacar stat
	   
     AodvUgdRREP* pRREP	=	new  AodvUgdRREP
			   	   	   	   	 ( direccionDestino , direccionOrigen , nodoSeqNum_ /*destSequNumb*/ ,
                         ackFlag ,  repairFlag ,
					   	   	       0 /*hops*/ , lifeTime , ttlRREP , direccionNextHop ,ipOrigen);
	   send ( pRREP ) ;
	   return ;
   }
   // ----------------------------------------------------------------------

     void
     AodvUgdProcessor::
     respuestaDescubrimientoRREP_Intermedio( std::string direccionDestino , std::string direccionOrigen ,std::string direccionSource )
     throw()
     {
 /*    1.RREP.DestSeqNum = RutaDestino.DestSeqNum
    	 2.RREP.HOP=RutaDestino.hop
    	 3.RREP.lifeTime=tiempoActual - RutaDestino.expirationTime
    	 4.RREP.ACK=FALSE  //solo aplica para el reenvio de RREP*/

  	   const AodvUgdTableEntery *unaEntradaDest	= unaTablaRuteo->devolverEntradaDestino(direccionDestino);
  	   /*1*/ unsigned int rutaDestSeqNum        =	unaEntradaDest -> getDestSequenceNumber();
  	   /*2*/ unsigned int hops			            =	unaEntradaDest -> getHops();
  	   /*3*/ double lifeTime 			              =	( ( unaEntradaDest -> getLifeTime() -  owner().current_time() ) );
             assert (lifeTime>0);
  	   /*4*/ bool ackFlag				                =	false; //TODO
  	         bool repairFlag		                =	false; //TODO
  	         unsigned ttlRREP		                =	configuracion.getNET_DIAMETER(); //TODO por ahora se utiliza esa valor, no aclara en el rfc


    
        // mi ip va en el encabezado
     std:: string ipOrigen         = owner().label();
        //el RREP se responde por PreviousHop hacia el origen
        // para simplificar enviamos por donde llego el RREQ
		AodvUgdRREP* pRREP = new AodvUgdRREP
							( direccionDestino , direccionOrigen , rutaDestSeqNum /*destSequNumb*/,
							  ackFlag , repairFlag , hops , lifeTime ,  ttlRREP , direccionSource , ipOrigen);

  	   /*Cuando se genera el RREP por el nodo intermedio se actualizan las listas de precursores*/
/*  	   The intermediate node updates the forward route entry by placing the
  	   last hop node (from which it received the RREQ, as indicated by the
  	   source IP address field in the IP header) into the precursor list for
  	   the forward route entry -- i.e., the entry for the Destination IP
  	   Address.  The intermediate node also updates its route table entry
  	   for the node originating the RREQ by placing the next hop towards the
  	   destination in the precursor list for the reverse route entry --
  	   i.e., the entry for the Originator IP Address field of the RREQ
  	   message data.*/

  	   /*los precursores son los vecinos del nodo intermedio   entonces...*/
  	   //la salida hacia el destino

	   //de donde llega el RREQ, si quiero enviar un mensaje al origen ocupo como salida
	   std::string	direccionPreviousHop	=	direccionSource;
    std::string  direccionNextHop    = unaEntradaDest->getNextHop();

  	   /* EntradaRutaDestino.addPrecursor	(RREQ.Source) //source por donde recibio el RREQ     */
	   unaTablaRuteo->agregarPrecursorEntrada (direccionDestino /*dest_*/ , direccionPreviousHop /*direccionPrecursor*/);

  	   /*EntradaRutaOrigen.addPrecursor	(EntradaRutaDestino.NextHop)*/
	   unaTablaRuteo->agregarPrecursorEntrada (direccionOrigen /*dest_*/ , direccionNextHop /*direccionPrecursor*/);

	   send ( pRREP ) ;
  	   return ;
     }
// ----------------------------------------------------------------------

  void
  AodvUgdProcessor::
  enviarRREP_G_Intermedio( std::string rreqDestino , std::string rreqOrigen ,
		  	  	  	  	  unsigned int rreqOrigenSeqNum  ) throw()
  {
/*
	  RREP-G.Destino = RREQ.Origen
	  RREP-G.Origen = RREQ.Destino

	  RREP-G.DestSeqNum = RREQ.OrigenSeqNum
	  RREP-G.Hop = RutaOrigen.Hop
	  RREP-G.LifeTime = RutaOrigen.LifeTime
*/
	   const AodvUgdTableEntery *unaEntradaDest	= 	unaTablaRuteo->devolverEntradaDestino ( rreqOrigen );
	   unsigned int hops			 =	unaEntradaDest -> getHops();
	   double lifeTime 				 =	( owner().current_time() ) - ( unaEntradaDest -> getLifeTime() );
	   bool ackFlag				 	   =	false; //TODO
	   bool repairFlag				 =	false; //TODO
	   unsigned ttlRREP				 =	configuracion.getNET_DIAMETER(); //TODO por ahora se utiliza esa valor, no aclara en el rfc

	   //destino y origen invertidos
     //el RREP-G se envia por el NextHop hacia el Destino
     std:: string direccionNextHop = unaTablaRuteo->devolverEntradaDestino(rreqDestino)->getNextHop();
               // mi ip va en el encabezado
     std:: string ipOrigen         = owner().label();   
  
		AodvUgdRREP* pRREP_G = new AodvUgdRREP
							( rreqOrigen  /*dest*/ ,rreqDestino  /*origen*/ , rreqOrigenSeqNum /*destSequNumb*/,
							  ackFlag , repairFlag , hops , lifeTime , ttlRREP,  direccionNextHop /*ipDestino*/ ,
                ipOrigen );

	   send ( pRREP_G ) ;
	   return ;
  }
     // ----------------------------------------------------------------------
         void
         AodvUgdProcessor::
         reenviarRREQ_Intermedio( const AodvUgdRREQ &rreq)
         throw()
		 {

	/*	 To update
		   the RREQ, the TTL or hop limit field in the outgoing IP header is
		   decreased by one, and the Hop Count field in the RREQ message is
		   incremented by one, to account for the new hop through the
		   intermediate node.  Lastly, the Destination Sequence number for the
		   requested destination is set to the maximum of the corresponding
		   value received in the RREQ message, and the destination sequence
		   value currently maintained by the node for the requested destination.*/

         unsigned int	newDestSeqNum	=	rreq.getDestSequNumb();
         bool existeRutaDestino		=	 unaTablaRuteo->existeEntradaDestino (rreq.getDestino());
         if (existeRutaDestino) {
        	  const AodvUgdTableEntery *unaEntradaDest	= 	unaTablaRuteo->devolverEntradaDestino( rreq.getDestino() );
        	  newDestSeqNum	=	std::max ( newDestSeqNum , unaEntradaDest -> getDestSequenceNumber() );
         }
         //al clonar TTL-- y HOP++ 
        
          AodvUgdRREQ* pRREQ_Reenvio =armarRREQ_Reenvio(rreq , newDestSeqNum);
         
         /*cuando se registra en la tabla no se resto al rreq, solo se paso una variable, al clonar se resta TTL y se suma los Hops*/
          sumarContadorStatGlobalRREQ_Renviados();//only for stat!
          
          std::stringstream mensaje;
          mensaje <<"-->>reenviarRREQ_Intermedio: "<<
                  getContadorStatGlobalRREQ_Renviados( )<<"  TTL: "<<
                  pRREQ_Reenvio->getTtl();
          
          LoggerAodv::Instance()->logCsvRREQ (*pRREQ_Reenvio, owner().label() , mensaje.str() );
          LoggerAodv::Instance()->logCsvRREQ_Mensajes (*pRREQ_Reenvio, mensaje.str());
          sendRREQ ( pRREQ_Reenvio ) ;
         }

   // ----------------------------------------------------------------------
  AodvUgdRREQ* 
  AodvUgdProcessor::
  armarRREQ_Reenvio  (const AodvUgdRREQ &rreq , unsigned int  newDestSeqNum)   
  throw()
  {
    //se arma un nuevo RREQ para reenviar, se suma el HOP y se resta el TTL
    const unsigned int newTtl   = rreq.getTtl()-1;
    const unsigned int newHops  = rreq.getHops()+1;
    // mi ip va en el encabezado
    std:: string ipOrigen       = owner().label();  

    return(
      new AodvUgdRREQ
         ( rreq.getDestino(), rreq.getOrigen() ,
           newDestSeqNum ,rreq.getOrigenSequNumb() ,
           rreq.getJoinFlag() , rreq.getRepairFlag() , rreq.getRREP_gFlag() ,
           rreq.getDestOnlyFlag() , rreq.getUnknownSequNum() , newHops ,
           rreq.getRREQ_ID() , ipOrigen, newTtl /*new*/  ) 
       );
  }
   // ----------------------------------------------------------------------
	void
	AodvUgdProcessor::
	sumarContadorStatGlobalRREQ_Renviados( )
	throw()
	{
		tagRREQ_stat* it 	=	devolverTagRREQ_stat();
		unsigned int contadorRREQ_renviados	=	it->getContadorRenviosRREQ();
		contadorRREQ_renviados++;
		it->setContadorRenviosRREQ (contadorRREQ_renviados);
	}
     // ----------------------------------------------------------------------
  unsigned int
  AodvUgdProcessor::
  getContadorStatGlobalRREQ_Renviados( )
  throw()
  {
    tagRREQ_stat* it  = devolverTagRREQ_stat();
    return it->getContadorRenviosRREQ();
  }
	// ----------------------------------------------------------------------
	void
	AodvUgdProcessor::
	sumarContadorStatGlobalRREQ_Handle( )
	throw()
	{
    tagRREQ_stat* it  = devolverTagRREQ_stat();
		unsigned int contadorHandle_RREQ =it->getContadorHandle_RREQ();
		contadorHandle_RREQ++;
		it->setContadorHandle_RREQ (contadorHandle_RREQ);
	}     
  // ----------------------------------------------------------------------
  unsigned int
  AodvUgdProcessor::
  getContadorStatGlobalRREQ_Handle( )
  throw()
  {
    tagRREQ_stat* it  = devolverTagRREQ_stat();
    return it->getContadorHandle_RREQ();
  }
    // ----------------------------------------------------------------------

   bool
   AodvUgdProcessor::
   verificarIntermedioRespondeRREQ( std::string dest, unsigned int rreqDesSeqNum, bool rreqDestOnlyFlag )
   throw()
   {
/*	   Si no es el destino se tienen que cumplir TODAS las siguientes condiciones para que pueda responder
	   1.RutaDestino.Ativa=TRUE
	   2.RutaDestino.DestSeqNum > = RREQ.DestSeqNum   y RutaDestino.validDestSeqNum=TRUE
	   3.RREQ.D=FALSE (bandera “Destination only” desactivada)*/

	   bool puedeResponder 					= 	false;
	   bool existeRuta						=	unaTablaRuteo->existeEntradaDestino(dest);
	   if (existeRuta) //se verifica al comienzo
	   {
		   const AodvUgdTableEntery *unaEntrada	= 	unaTablaRuteo->devolverEntradaDestino(dest);
	 /*1*/ bool rutaActiva						=	unaEntrada->getIsActive();
	 /*2*/ bool rutaValidDestSeqNum				=	unaEntrada->getValidDestSeqNum();
	 /*2*/ unsigned int rutaDestSeqNum			=	unaEntrada->getDestSequenceNumber();

		   if ( rutaActiva && !rreqDestOnlyFlag /*3*/ && rutaValidDestSeqNum)
		   {
			   if( rutaDestSeqNum >= rreqDesSeqNum ) /*2*/
			   {
				   puedeResponder =	true;
			   }
		   }
	   }
	   return puedeResponder;
   }
   // ----------------------------------------------------------------------
   // ----------------------------------------------------------------------
  const std::map< AodvUgdProcessor::par_Id_y_Direccion , shawn::EventScheduler::EventHandle> 
  AodvUgdProcessor:: getBufferRREQ ( void ) const throw()
  { return bufferRREQ;}
  // ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  handle_EventExpireTimeBufferRREQ(unsigned int RREQ_ID , std::string origen) 
  throw()
  {
  //borrar del buffer
  //el evento al disparar el timeout se borra solo..
/*    Removes the specified event. Must not be called from within
    timeout() (at least for the timeouting event)*/
    par_Id_y_Direccion Key_id_y_direccion = make_pair(RREQ_ID, origen);
    //map devuelve la cantidad de elementos borrados
    int borro = bufferRREQ.erase(Key_id_y_direccion); 
    assert(borro==1); 


    // bool encontro = false;
    // std::map < par_Id_y_Direccion , shawn::EventScheduler::EventHandle >::iterator it;
    // while(it != bufferRREQ.end() || encontro==false)
    // {
    //   if(it->second == unEventHandle)
    //   {
    //     encontro = true;
    //     bufferRREQ.erase (it); //borra el registro del map
    //   }
    //   it++;
    // }
    // assert(encontro);
  }
   // ----------------------------------------------------------------------
   void
   AodvUgdProcessor::
   agregarRREQ_Buffer(  unsigned int RREQ_ID, std::string origen  )
   throw()
   {
   //si existe en el buffer y queremos agregar es porque no se hizo el DROP 
   assert ( !existeRREQ_Buffer( RREQ_ID , origen ) ); 

	 double tiempoExpiracion= configuracion.getPATH_DISCOVERY_TIME() ;// PATH_DISCOVERY_TIME
	   //evento
	 shawn::EventScheduler& pEventScheduler = owner_w().world_w().scheduler_w();
	 	 //info del evento
	 BufferRREQ_EventTag* pBufferRREQ_EventTag=new BufferRREQ_EventTag(
			 	 	 	 	 	 	 	 	 	 	               origen , RREQ_ID );

	 shawn::EventScheduler::EventHandle pEventHandle = pEventScheduler.new_event
	 												( *this , tiempoExpiracion+owner().current_time() ,
	 														pBufferRREQ_EventTag );


	 //par_Id_y_Direccion id_y_direccion  = std::make_pair(RREQ_ID, origen);
   
	 bufferRREQ.insert({ { RREQ_ID, origen } , pEventHandle });
   }
	// ----------------------------------------------------------------------
	bool
	AodvUgdProcessor::
	existeRREQ_Buffer (unsigned int RREQ_ID, std::string origen) throw()
	{
		bool existe = false;
		par_Id_y_Direccion Key_id_y_direccion = make_pair(RREQ_ID, origen);
		std::map < par_Id_y_Direccion , shawn::EventScheduler::EventHandle >::iterator it;
		it = bufferRREQ.find (Key_id_y_direccion) ;
		if ( it != bufferRREQ.end() )
		 {
			 existe = true;
		 }
		return existe;
	}
   // ----------------------------------------------------------------------
   void
   AodvUgdProcessor::
   sendRREP  ( std::string dest , std::string origen , unsigned int destSequNumb ,
               bool ackFlag     , bool repairFlag    , unsigned int hops , 
               double lifeTime  , unsigned int ttl   , std::string  labelNodoNextHop  )   
   throw()
   {
       // mi ip va en el encabezado
    std:: string ipOrigen       = owner().label();  

//direccion de origen = destino , al menos por ahora
     AodvUgdRREP* pRREP = new AodvUgdRREP ( dest ,  origen , destSequNumb , ackFlag , 
                                            repairFlag ,  hops , lifeTime ,
                                            ttl ,  labelNodoNextHop  , ipOrigen ) ;

  //const AodvUgdRREP* pRREP_Nuevo = dynamic_cast< const AodvUgdRREP*> ( handleRREP_Nuevo.get() );
  send(pRREP);     
   }
   // ----------------------------------------------------------------------
   void
   AodvUgdProcessor::
   sendRREQ  ( AodvUgdRREQ* pRREQ)
   /*sendRREQ  ( std::string  dest         ,  std::string  origen         ,
               unsigned int destSequNumb , unsigned int  origenSequNumb ,
               bool joinFlag             , bool repairFlag              ,
               bool rrep_gFlag           , bool destOnlyFlag            ,
               bool UnknownSequNum       , unsigned int hops            ,
               unsigned int rreqId       , unsigned int ttl   )   */
   throw()
   {
   

/*    shawn::MessageHandle handleRREQ_Nuevo = new AodvUgdRREQ ( 
                                      dest           ,  origen     , destSequNumb , origenSequNumb , 
                                      joinFlag       ,  repairFlag , rrep_gFlag   , destOnlyFlag   ,
                                      UnknownSequNum ,  hops       , rreqId       , ttl            ) ;

  const AodvUgdRREQ* pRREQ_Nuevo = dynamic_cast< const AodvUgdRREQ*> ( handleRREQ_Nuevo.get() );*/
  //sumamos al contador global de RREQ enviados
  //send(handleRREQ_Nuevo);     
  send(pRREQ);
   }

/////////////////////////////////////////////////////////////////
/***********************     HANDLE RERR      ******************/
/////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------
  bool
  AodvUgdProcessor::
  localRepair ( std::string destinoInvalido , std::string sender ,
                TipoLocalRepair unTipoLocalRepair  )
  throw()
  {
    /*la funcion retorna verdadero si se comienza la reparacion local*/

  /*6.12. Local Repair
    When a link break in an active route occurs, the node upstream of
    that break MAY choose to repair the link locally if the destination
    was no farther than MAX_REPAIR_TTL hops away.*/

  /*1.the node increments the sequence number for the destination
      and then broadcasts a RREQ for that destination. 
    2.The TTL of the RREQ should initially be set to the following value:
      max(MIN_REPAIR_TTL, 0.5 * #hops) + LOCAL_ADD_TTL*/

  /* MIN_REPAIR_TTL = last hop count to the  destination
     #hops          = hops to the sender (originator) of the currently undeliverable packet*/

     AodvUgdTableEntery *unaEntrada =   unaTablaRuteo->devolverEntradaDestino_w(destinoInvalido);
    //intenta reparar
    if ( configuracion.getMAX_REPAIR_TTL() > unaEntrada->getHops() ) //abort
          return false;

    //verificar que no se esta intentando reparar...
    //pero si justo se esta haciendo un discovery comun sin repair??
    /*PARCHE! puedo avisar al proceso discovery que tambien quiero reparar esa ruta
    asi cambia el comportamiento*/
    bool descubrimientoIniciado = procesoDiscovrey->
                                  existeDescubrimientoIniciado( destinoInvalido );
    if(descubrimientoIniciado)
    {
        const TipoLocalRepair unaTipoLocalRepair = procesoDiscovrey->getTipoLocalRepairInfoDiscovery( destinoInvalido );
        if (unaTipoLocalRepair == TipoLocalRepair::None)
        {
    //asi si no encuentra la ruta genera el RERR
          procesoDiscovrey->modificarTipoRepairInfoDiscoveryRoute (destinoInvalido , unTipoLocalRepair);
        }
    }
    else
    {      
      //puedo inciar un LOCAL REPAIR.. si uso repair y cumple las condiciones: 
      //1. MAX_REPAIR_TTL 
      //2. descubrimiento no se incio

      //max(MIN_REPAIR_TTL, 0.5 * #hops) + LOCAL_ADD_TTL*/
      unsigned int ttlInicialLocalRepair  =   calcularTtlInicialLocalRepair(destinoInvalido,sender);
      procesoDiscovrey->comenzarProcesoRuteDescoveryLocalRepair(
                              destinoInvalido  /*destino*/    , owner().label() /*origen*/,
                              unaEntrada                     , ttlInicialLocalRepair                ,
                              unTipoLocalRepair          );
    }
    return true;  
  }
  // ----------------------------------------------------------------------
  unsigned int
  AodvUgdProcessor::
  calcularTtlInicialLocalRepair ( std::string  destino , std::string sender)
  throw()
  {
/* To repair the link
   break, the node increments the sequence number for the destination
   and then broadcasts a RREQ for that destination.  The TTL of the RREQ
   should initially be set to the following value:

    The MIN_REPAIR_TTL= should be the last known hop count to the   destination.
    #hops = hops to the sender (originator) of the currently undeliverable packet

    max(MIN_REPAIR_TTL, 0.5 * #hops) + LOCAL_ADD_TTL,*/
    unsigned int hopsToSender;
    unsigned int hopsToDest= unaTablaRuteo->devolverUltimoHopConocidoDestino(destino);

    //por ejemplo en el caso de hello yo soy el sender
    if(sender==owner().label())
      {
        hopsToSender=0;
      }
    else
      {
        hopsToSender = unaTablaRuteo->devolverUltimoHopConocidoDestino(sender);
      }

    unsigned int minRepairTtl      = hopsToDest;  
    unsigned int mitadHopsToSender = (unsigned int) round(0.5 * hopsToSender);
    unsigned int ttlInicial        = std::max ( minRepairTtl , mitadHopsToSender)   
                                        + configuracion.getLOCAL_ADD_TTL() ;
  return ttlInicial;
  }


  // ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  handeleDataPacketSinRutaActivaParaReenviar ( std::string  destinoInvalido , 
                                               std::string  sender      )
  throw()
  {
  /*6.11. Route Error (RERR) Messages, Route Expiry and Route Deletion
    case(ii) if it gets a data packet destined to a node for which it
    does not have an active route and is not repairing (if
    using local repair)*/
    
    //intenta reparar
    if(configuracion.getUseLocalRepair() )
    {
        bool comenzoLocalRepair = localRepair(destinoInvalido , sender , 
                                  TipoLocalRepair:: PacketSinRutaActivaParaReenviar);

      //si no se pudo comenzar la reparacion informo RERR.
      if(!comenzoLocalRepair)
        rerrDataPacketSinRutaActivaParaReenviar (destinoInvalido);
    }
    else
    {
      //no usa local repair, directamente envia un RERR!!
      rerrDataPacketSinRutaActivaParaReenviar (destinoInvalido);
      //algun LOG...
    }   
  }
  // ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  rerrDataPacketSinRutaActivaParaReenviar ( std::string destinoInvalido)
  throw()
  {
  /*6.11. Route Error (RERR) Messages, Route Expiry and Route Deletion
    case(ii) if it gets a data packet destined to a node for which it
             does not have an active route and is not repairing (if
             using local repair)*/

   /*For case (ii), there is only one unreachable destination, which is
   the destination of the data packet that cannot be delivered.*/

   //armo un map igual para seguir con la misma estructura,
   //pero tiene un elemento

/* Just before transmitting the RERR, certain updates are made on the
   routing table that may affect the destination sequence numbers for
   the unreachable destinations.*/
   //ACTUALIZO antes de armar el map!
   mapTypeDestinosInvalidos *mapMisDestinosAfectados = new mapTypeDestinosInvalidos;

   mapMisDestinosAfectados = unaTablaRuteo->armarListaDestinosAfectadosSinRutaReenviar(
                                        mapMisDestinosAfectados,
                                        destinoInvalido ,
                                        configuracion.getDELETE_PERIOD()     );
   
   //por mas que el destino invalido sea uno, tengo que fijarme si hago broadcast,
   //por que la idea es avisar a los precursores que usaban esa ruta!

   //TODO este codigo es candidado a factorizar en una nueva funcion, se repite mucho..
     if ( !mapMisDestinosAfectados->empty() ) //si hay destinos afectados
    {    //verifico si envio RERR por broadcast o unicast
        //por ahi en este caso no es tan necesario el broadcast
        std::string nextHop;
        bool enviarBroadcastRERR = verificarEnviarBroadcastRERR (*mapMisDestinosAfectados);
        if (enviarBroadcastRERR)
        {
          nextHop = BROADCAST_ADDRESS;
        }
        else
        {
          nextHop = devolverUnPrecursorEnviarUnicast(mapMisDestinosAfectados);
        }
        sendRERR( false/*noDeleteFlag*/ , mapMisDestinosAfectados ,nextHop);
    }

   //NOTA!: respete tal cual el párrafo.. asi que si la ruta afectada
   // NO tiene precursores, NO se envia el RERR
/* The RERR should contain those destinations that are part of
   the created list of unreachable destinations and have a non-empty
   precursor list.
   he neighboring node(s) that should receive the RERR are all those
   that belong to a precursor list of at least one of the unreachable
   destination(s) in the newly created RERR.
   */
  }
   // ----------------------------------------------------------------------
  std::string
  AodvUgdProcessor::
  devolverUnPrecursorEnviarUnicast ( const mapTypeDestinosInvalidos *mapDestinosInvalidos)
  throw()
  {
    mapTypeDestinosInvalidos::const_iterator it = mapDestinosInvalidos->begin();
    //ojo! el map puede tener varios destinosInvalidos, pero en este momento 
    //solo un precursor a enviar.
    //traigo cualquier destino total el precursor es el mismo
    std::string destinoInvalido = it->first; 
    
    const AodvUgdTableEntery *unaEntrada = 
                      unaTablaRuteo->devolverEntradaDestino ( destinoInvalido );
    std::string precursorEnviarUnicast = *(unaEntrada-> getItBeginPrecursors());

    //verifico si realmente hay un precursor
    assert ( unaEntrada-> getSizePrecursors() == 1 );

    return precursorEnviarUnicast;
    
  }
    // ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  helloInformaDetectesLinkBreak ( std::string destinoInvalido )
  throw()
  {
    handeleDetectesLinkBreakFromNextHop(destinoInvalido);
  }
  // ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  handeleDetectesLinkBreakFromNextHop ( std::string destinoInvalido )
  throw()
  {
  /*6.11. Route Error (RERR) Messages, Route Expiry and Route Deletion:

          if it detects a link break for the next hop of an active
          route in its routing table while transmitting data (and
          route repair, if attempted, was unsuccessful)*/

 //intenta reparar
    if(configuracion.getUseLocalRepair() )
    {

/*  The TTL of the RREQ should initially be set to the following value:
    max(MIN_REPAIR_TTL, 0.5 * #hops) + LOCAL_ADD_TTL
    MIN_REPAIR_TTL = last hop count to the  destination 
     #hops = hops to the sender (originator) of the currently undeliverable packet */

 //el local repair por ahora necesita un destino y un sender
 //cuando se detecta el link break desde el next hop el nodo es el sender
     bool comenzoLocalRepair  = localRepair(destinoInvalido , owner().label() /*sender*/, 
                                TipoLocalRepair::LinkBreak );
      //si no se pudo comenzar la reparacion informo RERR.
      if(!comenzoLocalRepair)
        rerrDetectesLinkBreak (destinoInvalido);

    }
    else
    {
      //no usa local repair, directamente envia un RERR!!
      rerrDetectesLinkBreak (destinoInvalido);
      //algun LOG...
    }   

  }
  // ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  rerrDetectesLinkBreak ( std::string destinoInvalido)
  throw()
  {
  /*6.11. Route Error (RERR) Messages, Route Expiry and Route Deletion:

          if it detects a link break for the next hop of an active
          route in its routing table while transmitting data (and
          route repair, if attempted, was unsuccessful)*/

/* For case (i), the node first makes a list of unreachable destinations
   consisting of the unreachable neighbor and any additional
   destinations (or subnets, see section 7) in the local routing table
   that use the unreachable neighbor as the next hop. */

/*    Just before transmitting the RERR, certain updates are made on the
   routing table that may affect the destination sequence numbers for
   the unreachable destinations.*/
   //ACTUALIZO antes de armar el map!

    //las entradas de ruta se actualizan antes de devolver el map.
     mapTypeDestinosInvalidos *mapMisDestinosAfectados = 
                    unaTablaRuteo->armarListaDestinosAfectadosLinkBreak(
                                        destinoInvalido ,
                                        configuracion.getDELETE_PERIOD()     );
   /*The neighboring node(s) that should receive the RERR are all those
   that belong to a precursor list of at least one of the unreachable
   destination(s) in the newly created RERR*/

    //supongo que solo se envia si hay percursores..en este caso siempre si tiene 
    //que cumplir porque si estaba enviando datos como intermedio yo formaba parte
    //de una ruta activa. verifico igual por las dudas..
    assert(!mapMisDestinosAfectados->empty());
    if ( !mapMisDestinosAfectados->empty() ) //si hay destinos afectados
    {    //verifico si envio RERR por broadcast o unicast
        //por ahi en este caso no es tan necesario el broadcast
        std::string nextHop;
        bool enviarBroadcastRERR = verificarEnviarBroadcastRERR (*mapMisDestinosAfectados);
        if (enviarBroadcastRERR)
        {
          nextHop= BROADCAST_ADDRESS;
        }
        else
        {
          nextHop = devolverUnPrecursorEnviarUnicast(mapMisDestinosAfectados);
        }
        sendRERR( false/*noDeleteFlag*/ , mapMisDestinosAfectados ,nextHop);
    }
  }
   // ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  handle_RERR ( const AodvUgdRERR& rerr)
  throw()
  {
  const std::string labelSource = rerr.source().label();
  //TODO registrar RERR source
  //TODO ver si hay que registrar el source
  //************************************************************
  /*  first creates or updates a route to the previous hop
      se registra en la tabla la entrada PreviousHop (SOURCE)*/
  registrarRutaSource  ( labelSource /*direccion*/) ;

  //no aclara si igual se verifica cuando no se usa LocalREpair
  //TODO esta funcion todavia no se implemento
  //TODO verificar RERR.n=true
  if ( ( rerr.getNoDeleteFlag() ) 
    && ( configuracion.getUseLocalRepair() )     )
  {

  }
  else  
  {
  //invalida las rutas del RERR antes de reenviar
      reenviarRERR (rerr.getMapDestinos() , rerr.source().label() );
  }
  }
  // ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  reenviarRERR ( const mapTypeDestinosInvalidos *mapDestinosInvalidosRERR, 
                 std::string transmitterRERR)
  throw()
//ESTA FUNCION SOLO CORRESPONDE AL case (iii):
// "if it receives a RERR from a neighbor for one or more active routes."

/* case (iii), the list should consist of those destinations in the RERR
   for which there exists a corresponding entry in the local routing
   table that has the transmitter of the received RERR as the next hop.*/


/*   Some of the unreachable destinations in the list could be used by
   neighboring nodes, and it may therefore be necessary to send a (new)
   RERR.  The RERR should contain those destinations that are part of
   the created list of unreachable destinations and have a non-empty
   precursor list.*/

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

  {
/*      invalido mis entradas (actualizarRutaAfectadaRERR) al armar la nueva lista, 
        si no hay que iterer el map de nuevo*/
    mapTypeDestinosInvalidos * mapMisDestinosAfectados =new mapTypeDestinosInvalidos;
    mapMisDestinosAfectados = unaTablaRuteo->armarListaMisDestinosAfectadosReenvioRERR (
                                                mapDestinosInvalidosRERR,
                                                mapMisDestinosAfectados, //mapa nuevo para devolver por referencia
                                                transmitterRERR,
                                                configuracion.getDELETE_PERIOD() );

      //assert(!mapMisDestinosAfectados->empty());
      if ( !mapMisDestinosAfectados->empty() ) //si hay destinos afectados
      {
        std::string nextHop;
          //verifico si envio RERR por broadcast o unicast
        bool enviarBroadcastRERR = verificarEnviarBroadcastRERR (*mapMisDestinosAfectados);
          if (enviarBroadcastRERR)
              nextHop= BROADCAST_ADDRESS;
          else
              nextHop = devolverUnPrecursorEnviarUnicast(mapMisDestinosAfectados);

        sendRERR( false/*noDeleteFlag*/ , mapMisDestinosAfectados ,nextHop);
      }
      else
      {
        //no tengo destinos afectados...
      }    
  }
    // ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  sendRERR (  bool noDeleteFlag    ,   
              mapTypeDestinosInvalidos *mapMisDestinosInvalidosRERR ,
              std::string labelNodoNextHop  )

  throw()
  {
    
    // mi ip va en el encabezado
    std:: string ipOrigen       = owner().label();  

    send( new AodvUgdRERR (noDeleteFlag , 
                           mapMisDestinosInvalidosRERR->size()  /*destCount*/ ,
                           mapMisDestinosInvalidosRERR ,    1  /*ttl*/       ,
                           labelNodoNextHop            , ipOrigen            )     );
  }
  // ----------------------------------------------------------------------
  bool
  AodvUgdProcessor::
  verificarEnviarBroadcastRERR ( mapTypeDestinosInvalidos &mapMisDestinosInvalidosRERR)
  throw()
  {
/*   The neighboring node(s) that should receive the RERR are all those
   that belong to a precursor list of at least one of the unreachable
   destination(s) in the newly created RERR.  In case there is only one
   unique neighbor that needs to receive the RERR, the RERR SHOULD be
   unicast toward that neighbor.  Otherwise the RERR is typically sent
   to the local broadcast address (Destination IP == 255.255.255.255,
   TTL == 1) with the unreachable destinations, and their corresponding
   destination sequence numbers, included in the packet.  The DestCount
   field of the RERR packet indicates the number of unreachable
   destinations included in the packet.
*/

    std::set < std::string> setTodosLosPrecursores;
    mapTypeDestinosInvalidos::iterator it;
    for ( it=mapMisDestinosInvalidosRERR.begin() ; it!=mapMisDestinosInvalidosRERR.end(); ++it )
    {
      std::string  destinoInvalido         =  it->first;
      const AodvUgdTableEntery *unaEntrada = 
                      unaTablaRuteo->devolverEntradaDestino ( destinoInvalido );
      //el map ya viene armado con que solo se agregan entradas con precursores
      assert (!unaEntrada->isEmptyPrecursors() );
      //si hay al menos 2 precursores distintos tengo que hacer broadcast

      //Inserting an Iterator Range into a Set
      setTodosLosPrecursores.insert (unaEntrada-> getItBeginPrecursors() , 
                                     unaEntrada-> getItEndPrecursors()    );
      //el set no agrega elementos duplicados, entonces.. 
      //voy uniendo los sets de precursores, cuando el tamaño es mayor a uno
      //tengo que hacer el broadcast
      if( setTodosLosPrecursores.size() >1 )
         return true;
    }
    return false;
  }

// FIN HANDLE RERR----------------------------------------------------------------------
// ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  aumentarContadorNodoSeqNum ()
  throw()
  {
    nodoSeqNum_++;
  }
  // ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  handleNotFindRoute (std::string destino)
  throw()
  {
    //cuando se termina con la busqueda sin exito..

    //drop de mensajes pendiente
    dropPaquetesPendientesDestino(destino);
    //cancelar observar ruta activa
    cancelarObservarUnaRuta (destino , "active");

    //Puede ser un LOG!
    
  }
// ----------------------------------------------------------------------
  unsigned int
  AodvUgdProcessor::
  getContadorNodoSeqNum ()
  throw()
  {
    return nodoSeqNum_;
  }
/////////////////////////////////////////////////////////////////
/***********************     Routing         ******************/
/////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  routePacketForward(  const AodvUgdIpHeader& unPaqueteIp )   throw()
  {
    std::string destino = unPaqueteIp.getIpDestino();
    std::string sender  = unPaqueteIp.getIpOrigen();

    bool rutaActiva  =  unaTablaRuteo->existeRutaActivaDestino (destino);
    if (rutaActiva)
    {
      //routing...
      //hay que reenviar un paquete nuevo
      AodvUgdIpHeader * unPaqueteIpNuevo = clonarUnPaqueteIP (unPaqueteIp);
      //diminuyo el TTL
      unPaqueteIpNuevo->disminuirTTL();
      
      if(unPaqueteIpNuevo->getTtl()>0)
      {

        //puedo reenviar
        sendToNextHop(*unPaqueteIpNuevo);
      }
      else
      {
         //****************  LOG   *****************
         std::ostringstream mensaje;
         mensaje <<"DROP: TTL=0   Destino: "<<destino;
         LoggerAodv::Instance()->logCsvPingDetalle( owner().label() , 
                                        unPaqueteIp.getIpOrigen() , destino , "AODV",
                                        mensaje.str()                 );
        //**************** FIN LOG ****************
        //drop
        return;
      }
    }
    else
    {
        //****************  LOG   *****************
         std::ostringstream mensaje;
         mensaje <<"No hay ruta activa para rutear Destino: "<<destino;
         LoggerAodv::Instance()->logCsvPingDetalle( owner().label() , 
                                        unPaqueteIp.getIpOrigen() , destino , "AODV",
                                        mensaje.str()                 );
        //**************** FIN LOG ****************
      handeleDataPacketSinRutaActivaParaReenviar(destino , sender); 
    }

  }
  // ----------------------------------------------------------------------
AodvUgdIpHeader*
AodvUgdProcessor::
clonarUnPaqueteIP ( const AodvUgdIpHeader& unPaqueteIp  )
throw()
{

    //equivale a quitar la cabecera IP
    const PingMessage *unPingMessage = dynamic_cast<  const PingMessage* > ( &unPaqueteIp );
    if( unPingMessage == NULL )
      assert(false);//no se pudo clonar paquete ip
    
  
        PingMessage * unNewPingMessage =new PingMessage( 
                              unPingMessage->size()         , unPingMessage->getUnTipoICMP(), 
                              unPingMessage->getPingID()    , unPingMessage->getSeqNum()    , 
                              unPingMessage->getTtl()       , 
                              unPingMessage->getIpDestino() , unPingMessage->getIpOrigen() );
         
    
        return dynamic_cast < AodvUgdIpHeader*> (unNewPingMessage );
}
// ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  dropPaquetesPendientesDestino (std::string destino)
  throw()
  {
     //itero el buffer para descartar los mensajes
     TipoSetMensajesPendientes::iterator itList;
    for (itList=bufferMensajesPendientes.begin(); itList != bufferMensajesPendientes.end(); ++itList)
    {
      //destino
      std::string destinoMensaje = itList->first;  
      if(destino == destinoMensaje )
      {
        AodvUgdIpHeader& unMensajeIp = itList->second;
        //descarto el mensaje 
       itList = bufferMensajesPendientes.erase (itList);
      }
    }

  }
// ----------------------------------------------------------------------
void
AodvUgdProcessor::
sendToNextHop (  AodvUgdIpHeader& unPaqueteIp  )
throw()
{ 

  std::string destino     = unPaqueteIp.getIpDestino();
  std::string source      = unPaqueteIp.getIpOrigen();
  std::string nextHop     = unaTablaRuteo->devolverNextHop_a_Destino(destino);
  
  //si soy el source no hay prepreviousHop
  std::string previousHop="";
  if( owner().label() != source) 
  {
    previousHop = unaTablaRuteo->devolverNextHop_a_Destino(source) ; 
  }


    //****************  LOG   *****************
   std::ostringstream mensaje;
   mensaje <<"Enviado paquete... Destino: "<<destino<<"  por NextHop: "<<nextHop;
   LoggerAodv::Instance()->logCsvPingDetalle(
                                  owner().label() , 
                                  "" /*origen*/   ,   destino , "AODV",
                                  mensaje.str()                 );
   //**************** FIN LOG ****************


//se escpecifica el destino y el mensaje a enviar, asi el proceso busca el 
//nextHop y envia el mensaje

//verifico los TTL del paquete por las dudas, en este puntos siempre tendria que ser mayor a 0
   assert(unPaqueteIp.getTtl()>0 );

//se utiliza directamente xq en la red no hay 2 IPs(label) iguales
  unPaqueteIp.setMacDestino(nextHop);

  sendPaqueteIP (unPaqueteIp);
  

  // cada vez que envio/ruteo un mensaje actualizo el lifetime de todas las rutas 
  // que corresponden al camino

/*   Each time a route is used to forward a data packet, its Active Route
   Lifetime field of the source, destination and the next hop on the
   path to the destination is updated to be no less than the current
   time plus ACTIVE_ROUTE_TIMEOUT.  Since the route between each
   originator and destination pair is expected to be symmetric, the
   Active Route Lifetime for the previous hop, along the reverse path
   back to the IP source, is also updated to be no less than the current
   time plus ACTIVE_ROUTE_TIMEOUT.  The lifetime for an Active Route is
   updated each time the route is used regardless of whether the
   destination is a single node or a subnet.*/

//cuando se esta ruteando el source es el originador del mensaje!
    actualizarPorUsarCamino( source , destino , nextHop , previousHop );
  //TODO
}
// ----------------------------------------------------------------------
void
AodvUgdProcessor::
handlePaqueteIp (  const AodvUgdIpHeader& unPaqueteIp  )
throw()
{

  std::string destino = unPaqueteIp.getIpDestino();
  //soy el destinatario final
  if(owner().label() == unPaqueteIp.getIpDestino() )
  {
   //****************  LOG   *****************
   std::ostringstream mensaje;
   mensaje <<"Llego un paquete para mi.. pasando app" ;
   LoggerAodv::Instance()->logCsvPingDetalle(
                                  owner().label() , 
                                  unPaqueteIp.getIpOrigen() ,   destino , "AODV",
                                  mensaje.str()                 );
   //**************** FIN LOG ****************

      pasarPaqueteIpToApp(unPaqueteIp);
  }
  else
  //hay que rutear al siguiente salto
  {
       //****************  LOG   *****************
   std::ostringstream mensaje;
   mensaje <<"Llego un paquete.. intento rutear.. Destino: "<<destino;
   LoggerAodv::Instance()->logCsvPingDetalle(
                                  owner().label() , 
                                  unPaqueteIp.getIpOrigen() , destino , "AODV",
                                  mensaje.str()                 );
   //**************** FIN LOG ****************


      routePacketForward(unPaqueteIp);
  }
}
// ----------------------------------------------------------------------
void
AodvUgdProcessor::
pasarPaqueteIpToApp ( const AodvUgdIpHeader& unPaqueteIp  )
throw()
{
    //equivale a quitar la cabecera IP
    const PingMessage *unPingMessage = dynamic_cast<  const PingMessage* > ( &unPaqueteIp );
    if( unPingMessage != NULL )
    {
    //por el momento solo hay una app ping
    //pasar el mensaje a Ping..
      PingApp *pPingApp = owner_w().get_processor_of_type_w <PingApp> ();
      pPingApp->recibirMensajePing (*unPingMessage);
    }
}
// ----------------------------------------------------------------------
void
AodvUgdProcessor::
recibirPaqueteDesdeApp (  AodvUgdIpHeader& unPaqueteIp  )
throw()
{
  //verifico por las dudas que sea mi paquete
  assert( owner().label() == unPaqueteIp.getIpOrigen() );
  
  sendMyOwnDataPacket ( unPaqueteIp  );
}
// ----------------------------------------------------------------------
void
AodvUgdProcessor::
sendMyOwnDataPacket (  AodvUgdIpHeader& unPaqueteIp  )
throw()
{
  std::string destino = unPaqueteIp.getIpDestino();

  //received Packet...
  //****************  LOG   *****************
  LoggerAodv::Instance()->logCsvPingDetalle( owner().label() ,
                             unPaqueteIp.getIpOrigen() , destino   , "AODV",
                             "Se quiere enviar unPaqueteIp");
  //**************** FIN LOG ****************

  //Finding route for destination
  bool existeRutaActivaDestino  = unaTablaRuteo->existeRutaActivaDestino(destino);
  if(existeRutaActivaDestino)
  {
    //routing 
    //el proceso va a buscar el nextHop y envia el mensaje

    //****************  LOG   *****************
      LoggerAodv::Instance()->logCsvPingDetalle( owner().label() ,
                             unPaqueteIp.getIpOrigen()   , destino     ,"AODV",
                             "Busca ruta activa... existe");
    //**************** FIN LOG ****************

    sendToNextHop( unPaqueteIp);
  }
  else
  {
        //****************  LOG   *****************
          LoggerAodv::Instance()->logCsvPingDetalle( owner().label() ,
                                  unPaqueteIp.getIpOrigen()      , destino ,"AODV",
                             "Busca ruta activa...no existe, buffer mensaje... llama discovery");
          //**************** FIN LOG ****************

    //Missing route for destination
    //Queuing datagram,destination
    bufferMensajesPendientes.push_back( ParSetMensajesPendientes (destino ,unPaqueteIp));

    //observar ruta, cuando hay una activa se van a transmitir los paquetes..
    observarUnaRuta(destino, "active");
    //Starting route discovery with originator 145.236.0.1 and destination 145.236.0.5
    llamarProcesoRouteDiscovery (destino);
  }
}
// ----------------------------------------------------------------------
void
AodvUgdProcessor::
enviarPaqutesPendientesDestino ( std::string destino ) throw()
{
    //****************  LOG   *****************
   std::ostringstream mensaje;
   mensaje <<"Buscando paquetes pendiente... Destino: "<<destino;
   LoggerAodv::Instance()->logCsvPingDetalle(
                                  owner().label() , 
                                  "" /*origen*/   ,   destino , "AODV",
                                  mensaje.str()                 );
   //**************** FIN LOG ****************



    //itero el buffer para enviar los mensajes
     TipoSetMensajesPendientes::iterator itList;
    for (itList=bufferMensajesPendientes.begin(); itList != bufferMensajesPendientes.end(); ++itList)
    {
      //destino
      std::string destinoMensaje = itList->first;  
      if(destino == destinoMensaje )
      {
        AodvUgdIpHeader& unMensajeIp = itList->second;
        sendToNextHop (unMensajeIp) ;
        //descarto el mensaje luego de enviar
       itList = bufferMensajesPendientes.erase (itList);
      }
    }
}
  // ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  llamarProcesoRouteDiscovery ( std::string destino)   throw()
  {
    if (unaTablaRuteo->existeEntradaDestino (destino ) )
    {
       const AodvUgdTableEntery *unaEntrada= unaTablaRuteo->devolverEntradaDestino(destino);

       //****************  LOG   *****************
       std::ostringstream mensaje;
       mensaje <<"Existe entrada antigua estado: "<< unaEntrada->getState();
       LoggerAodv::Instance()->logCsvPingDetalle( owner().label() ,
                                      "" /*origen*/ , destino , "AODV", mensaje.str() );
       //**************** FIN LOG ****************

       procesoDiscovrey->comenzarProcesoRuteDescoveryEntradaExistente(
                                               destino                   , 
                                               owner().label() /*origen*/,
                                               *unaEntrada                );
    }

    else
    {
       //****************  LOG   *****************
       std::ostringstream mensaje;
       mensaje <<"No existe entrada antigua";
       LoggerAodv::Instance()->logCsvPingDetalle( owner().label() ,
                                     "" /*origen*/, destino ,"AODV", mensaje.str() );
       //**************** FIN LOG ****************

      procesoDiscovrey->comenzarProcesoRuteDescovery (destino , owner().label() /*origen*/);
    }  
  }

    // ----------------------------------------------------------------------
  void
  AodvUgdProcessor::
  actualizarPorUsarCamino ( std::string source  , std::string destino , 
                            std::string nextHop , std::string previousHop     )  throw()
  {
/*     Each time a route is used to forward a data packet, its Active Route
   Lifetime field of the source, destination and the next hop on the
   path to the destination is updated to be no less than the current
   time plus ACTIVE_ROUTE_TIMEOUT.  Since the route between each
   originator and destination pair is expected to be symmetric, the
   Active Route Lifetime for the previous hop, along the reverse path
   back to the IP source, is also updated to be no less than the current
   time plus ACTIVE_ROUTE_TIMEOUT.  The lifetime for an Active Route is
   updated each time the route is used regardless of whether the
   destination is a single node or a subnet.*/
    double tiempoActual    = owner().current_time();
    double lifetimeNuevo   = tiempoActual + configuracion.getACTIVE_ROUTE_TIMEOUT();
    
    unaTablaRuteo->actualizrLifeTimeEntradaActiva (destino     , lifetimeNuevo);
    unaTablaRuteo->actualizrLifeTimeEntradaActiva (nextHop     , lifetimeNuevo);
    
/*    se podria crear una funcion aparte para el caso que se actualizan las rutas 
    cuando  el nodo que envia su propio paquete, pero preferimos que la actualizacion 
    se realice en una funcion    */

    /*es para el caso que el nodo envia su propio paquete.. no existe un salto anterior*/
    if(owner().label() != source)
    {
      unaTablaRuteo->actualizrLifeTimeEntradaActiva (source      , lifetimeNuevo);
      unaTablaRuteo->actualizrLifeTimeEntradaActiva (previousHop , lifetimeNuevo);  
    }

  }
   // ----------------------------------------------------------------------
   void
   AodvUgdProcessor::
   sendPaqueteIP  ( AodvUgdIpHeader& unPaqueteIp )   
   throw()
   {

  // shawn::MessageHandle *unMessageHandle =  dynamic_cast< shawn::MessageHandle> ( unPaqueteIp );
     send(&unPaqueteIp);     
   }
// FIN Routing----------------------------------------------------------------------
// ----------------------------------------------------------------------
   void
   AodvUgdProcessor::
   crearConfiguracionAodv( )
    throw(){
	   const shawn::SimulationEnvironment& se = owner().world().
	                                      simulation_controller().environment();

		//uso de funcionalidades
		bool useHelloMessages  =	se.optional_bool_param("Aodv_USE_HELLO", true);
		bool useERS            =	se.optional_bool_param("Aodv_USE_ERS", true);
    bool useGratuitousRREP =  se.optional_bool_param("Aodv_USE_RREP_G", false);
    bool useDestOnly       =  se.optional_bool_param("Aodv_USE_DEST_ONLY", false);
    bool useLocalRepair    =  se.optional_bool_param("Aodv_USE_LOCAL_REPAIR", false);

		//Configuration Parameters
		//TODO crear un archivo con los valores por defecto
		//https://www.codeproject.com/Articles/8342/CIniFile-Class-for-C-A-robust-cross-platform-INI-f
	     double   		ACTIVE_ROUTE_TIMEOUT =	se.optional_double_param("Aodv_ACTIVE_ROUTE_TIMEOUT",3000);	  //3000 Milliseconds
       unsigned int ALLOWED_HELLO_LOSS   =	se.optional_int_param("Aodv_ALLOWED_HELLO_LOSS",2);  	  //2;
	     double	      HELLO_INTERVAL       =	se.optional_int_param("Aodv_HELLO_INTERVAL",1000);		  //1000; Milliseconds
       unsigned int LOCAL_ADD_TTL        =	se.optional_int_param("Aodv_LOCAL_ADD_TTL",2);			  //2;
       unsigned int NET_DIAMETER         =	se.optional_int_param("Aodv_NET_DIAMETER",35);			  //35;
       unsigned int NODE_TRAVERSAL_TIME  =	se.optional_int_param("Aodv_NODE_TRAVERSAL_TIME",40);	  //40; //milliseconds
       unsigned int RERR_RATELIMIT       =	se.optional_int_param("Aodv_RERR_RATELIMIT",10);	      //10;
       unsigned int RREQ_RETRIES         =	se.optional_int_param("Aodv_RREQ_RETRIES",2);		      //2;
       unsigned int RREQ_RATELIMIT       =	se.optional_int_param("Aodv_RREQ_RATELIMIT",10);	      //10;
       unsigned int TIMEOUT_BUFFER       =	se.optional_int_param("Aodv_TIMEOUT_BUFFER",2);		  //2;
       unsigned int TTL_START            =	se.optional_int_param("Aodv_TTTL_START ",1);				  //1;			  //cambia si usa HELLO
       unsigned int TTL_INCREMENT        =	se.optional_int_param("Aodv_TTL_INCREMENT ",2);			  //2;
       unsigned int TTL_THRESHOLD        =	se.optional_int_param("Aodv_TTL_THRESHOLD ",7);			  //7;
       int          K_DELETE_PERIOD      =	se.optional_int_param("Aodv_K_DELETE_PERIOD ",5);


  	 //TTL_START 				  {se.optional_int_param("Aodv_TTTL_START ",AodvUgdDefaultConfig)}, //cambia si usa HELLO

       AodvUgdDefaultConfig unaConfiguracion(
    		 useHelloMessages,  
				 useERS,
         useGratuitousRREP,
         useDestOnly,
         useLocalRepair,

				 ACTIVE_ROUTE_TIMEOUT,
				 ALLOWED_HELLO_LOSS,
				 HELLO_INTERVAL,
				 LOCAL_ADD_TTL,
				 NET_DIAMETER,
				 NODE_TRAVERSAL_TIME,
				 RERR_RATELIMIT,
				 RREQ_RETRIES,
				 RREQ_RATELIMIT,
				 TIMEOUT_BUFFER,
				 TTL_START,
				 TTL_INCREMENT,
				 TTL_THRESHOLD,
				 K_DELETE_PERIOD
       );
       configuracion=unaConfiguracion;

   }

   // ----------------------------------------------------------------------
   void
   AodvUgdProcessor::
   crearProcesoDiscovrey( ) throw()  {

	   ConfiguracionDiscovery  unaConfiguracionDiscovery  =
			   ConfiguracionDiscovery(
				configuracion.getuseERS() ,
				configuracion.getNET_TRAVERSAL_TIME() ,
				configuracion.getTTL_THRESHOLD() ,
				configuracion.getTTL_INCREMENT() ,
				configuracion.getNET_DIAMETER() ,
				configuracion.getRREQ_RETRIES(),
        configuracion.getRREQ_RATELIMIT() ,
        configuracion.getLOCAL_ADD_TTL() ,
        configuracion.getTTL_START() ,
				configuracion.getTIMEOUT_BUFFER() ,
				configuracion.getNODE_TRAVERSAL_TIME(),
        configuracion.getUseGratuitousRREP(),
        configuracion.getUseDestOnly()         );


	   AodvUgdRouteDiscovery* newProcesoDiscovrey = new AodvUgdRouteDiscovery(
			   		   	     0 /*contadorRREQ_ID*/ ,
			   			       0 /*contadorRREQ_sent_seg*/,
						         unaConfiguracionDiscovery,
			   			       *this);
			
     procesoDiscovrey = newProcesoDiscovrey;


	   /*
	   AodvUgdRouteDiscovery UnProcesoDiscovrey = AodvUgdRouteDiscovery (
		   	   0 contadorRREQ_ID ,
			   0 contadorRREQ_sent_seg,
			   unaConfiguracionDiscovery,
			   *this);
	   procesoDiscovrey = UnProcesoDiscovrey;*/



	   std::cout<< "--------------------------procesoDiscovrey: adress " << &procesoDiscovrey<< std::endl;
	   std::cout<< "--------------------------nodo :  " << owner().label()<< std::endl;
	   std::cout<< "--------------------------UnProcesoDiscovrey: adress " << &newProcesoDiscovrey<< std::endl;
   }
   // ----------------------------------------------------------------------
   void
   AodvUgdProcessor::
   crearProcesoHello( ) throw()  
   {
    std::cout<< "--------------------------crearProcesoHello: nodo " << owner().id()<< std::endl;
    pAodvUgdProcessorHello = owner_w().get_processor_of_type_w <AodvUgdProcessorHello> ();
    // pAodvUgdProcessorHello   =   owner_w().get_processor_of_type_w <AodvUgdProcessorHello> ();
    pAodvUgdProcessorHello->
      inicilizar(  configuracion.getHELLO_INTERVAL (), 
                   configuracion.getALLOWED_HELLO_LOSS() , 
                   configuracion.getDELETE_PERIOD()  , 
                   owner_w().world_w().scheduler_w()  /*shawn::EventScheduler&*/ ,
                   *this /*AodvUgdProcessor&*/ , 
                   *unaTablaRuteo );
    assert ( pAodvUgdProcessorHello != NULL );

   }
   // ----------------------------------------------------------------------
   void
   AodvUgdProcessor::
   crearRouteTable( ) throw() 
   {
              //crearTabla
     const std::string direccionNodo = owner().label() ;
     //se le pasa el planificador de eventos
     shawn::EventScheduler& es = owner_w().world_w().scheduler_w();
     //se asigna despues y no se inicializa porque al momento de crear el proceso no tien planificador
      AodvUgdRoutingTable* newTablaRuteo = new AodvUgdRoutingTable( direccionNodo, es ,
                                                     configuracion.getDELETE_PERIOD() ) ;
      unaTablaRuteo=newTablaRuteo; 

      std::cout<< "es : " << &es <<  std::endl;
      std::cout<< "scheduler_w : " << &(owner_w().world_w().scheduler_w()) <<  std::endl;    
   }
   //----------------------------------------------------------------------
   void
   AodvUgdProcessor::
   iniciarTagRREQ_stat( ) throw()  {
	    tagRREQ_stat* it = new tagRREQ_stat( );
	    it->set_persistency(true);
	    owner_w().world_w().add_tag( it );
   }
   //----------------------------------------------------------------------
   tagRREQ_stat*
   AodvUgdProcessor::
   devolverTagRREQ_stat( ) throw()  {
	   shawn::TagHandle th = owner_w().world_w().find_tag_w("tagRREQ_stat");
	   tagRREQ_stat* it = dynamic_cast< tagRREQ_stat*>(th.get());
	   return it;
   }



//----------------------------------------------------------------------
/**************************************************************************
*********testing**********************************************************/

 void
  AodvUgdProcessor::
  testingRuteDescoveryFromNodeToNode( int idOrigen , int idDestino )   throw()
  {
    if( idOrigen ==owner().id() )// si soy el nodo que empieza el discovery
    {
      std::string direccionNodoDestino = owner().world().find_node_by_id ( idDestino )->label();
      llamarProcesoRouteDiscovery( direccionNodoDestino );
    }
  }
   // ----------------------------------------------------------------------

  void
  AodvUgdProcessor::
  testingRuteDescovery( )   throw()
  {
    //dummy route descovery, para pruebas unicamente
    std::string origen  =   owner().label();
    std::string destino ;
    int idOrigen    = owner().id();
    const shawn::Node* nodoDestino;
    bool encontroDestino = false;


    int nodesSize     = owner().world().node_id_space_size();
    int idDestino = 0;

    for ( int i = 0; (i < nodesSize) && (encontroDestino == false)  ; ++i)
    {
      if (idOrigen != i )
      {
        encontroDestino = true;
        idDestino = i;
      }
    }
    if (encontroDestino)
    {
      nodoDestino = owner().world().find_node_by_id ( idDestino );
      destino = nodoDestino->label();
      //test
      testDestino = destino;

      // std::stringstream mensaje;
      // mensaje << "Enviando RREQ Dummy para testing..  Origen: " << owner().id()
      //         << "  " << origen << "  ->   destino:" << idDestino << "   "  << destino;
      // LOG4CXX_DEBUG ( logTestRREQ, mensaje.str() );
      llamarProcesoRouteDiscovery( destino );
    }
    else
    {
      // std::stringstream mensaje;
      // mensaje << "No se pudo enviar RREQ Dummy para testing..  Origen: "
      //         << owner().id() << "  " << origen;
      // LOG4CXX_DEBUG ( logTestRREQ, mensaje.str() );
    }
  }


}
#endif





