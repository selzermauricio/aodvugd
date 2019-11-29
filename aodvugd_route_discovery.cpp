#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD
#include "legacyapps/aodvugd/aodvugd_logger.h"
//#include "legacyapps/aodvugd/aodvugd_route_discovery.h"

//#include "legacyapps/aodvugd/aodvugd_message.h"
#include "legacyapps/aodvugd/aodvugd_observer_estado_ruta.h"

//#include "legacyapps/aodvugd/aodvugd_processor.h"
//#include "legacyapps/aodvugd/aodvugd_message.h"
//#include "sys/simulation/simulation_controller.h"
//#include "sys/node.h"

#include <sstream> //tostring
#include <algorithm> //std::max
#include <math.h>  //round
namespace aodvugd
{
// clase AodvUgdRouteDiscovery----------------------------------------------------------------------
/*	AodvUgdRouteDiscovery::
	AodvUgdRouteDiscovery()
	{}*/
// ----------------------------------------------------------------------
	AodvUgdRouteDiscovery::
	   AodvUgdRouteDiscovery( unsigned int contadorRREQ_ID,
			   	   	   	   	  unsigned int	contadorRREQ_sent_seg,
							  ConfiguracionDiscovery  unaConfiguracionDiscovery,
							  AodvUgdProcessor&  procesoAodv )
	:
							   contadorRREQ_ID_				{contadorRREQ_ID},
							   contadorRREQ_sent_seg_		{contadorRREQ_sent_seg},
							   unaConfiguracionDiscovery_	{unaConfiguracionDiscovery},
							   procesoAodv_					{&procesoAodv}
	{

	}
// ----------------------------------------------------------------------
	AodvUgdRouteDiscovery::
	~AodvUgdRouteDiscovery() {}

// get set----------------------------------------------------------------------
	 unsigned int
	AodvUgdRouteDiscovery::
	getContadorRREQ_ID (void) throw()
	{
		return contadorRREQ_ID_;
	}
// metodos----------------------------------------------------------------------

	void
	   AodvUgdRouteDiscovery::
	   timeout( shawn::EventScheduler&, shawn::EventScheduler::EventHandle,
	            double, shawn::EventScheduler::EventTagHandle& unEventTagHandle )
	      throw()
	   {
	   //*********************************
	   //evento DiscoveryEvent
	   const DiscoveryEventTag* unDiscoveryEventTag =
		   dynamic_cast<const DiscoveryEventTag*> ( unEventTagHandle.get() );

/*		   si se disparo el evento unDiscoveryEventTag,
		   es porque expiro el tiempo de espera por un RREP*/
		   if ( unDiscoveryEventTag != NULL){
			   std::cout<< "*************** TIMEOUT unEventTagHandle: " <<	std::endl;
			   unDiscoveryEventTag->show();

			   //****************  LOG   *****************
			   std::ostringstream mensaje;
			   mensaje <<"Timeout:  "<< unDiscoveryEventTag->toString() ;
			   LoggerAodv::Instance()->logCsvPingDetalle( procesoAodv_->owner().label() ,
			                                 "" /*origen*/,unDiscoveryEventTag->getDestino() ,
			                                 "Discovery", mensaje.str() );
	   		   //**************** FIN LOG ****************

			   handle_EventExpireTimeDiscovery ( unDiscoveryEventTag->getDestino () );

		   }

		   
	   //*********************************
	   //evento ResetRREQ_limit
	   const DiscoveryEventResetRREQ_limit* unDiscoveryEventResetRREQ_limit =
		   dynamic_cast<const DiscoveryEventResetRREQ_limit*> ( unEventTagHandle.get() );
		   if ( unDiscoveryEventResetRREQ_limit != NULL)
		   		handleEventoResetContadorRREQ();


	   }
   // ----------------------------------------------------------------------
	void
	AodvUgdRouteDiscovery::
	aumentarContadorRREQ_ID() throw()
	{
			contadorRREQ_ID_++;
	}
	// ----------------------------------------------------------------------
	/////////////////////////////////////////////////////////////////
	/********************* AvisoEstadoRuta (observer)  *************/
	/////////////////////////////////////////////////////////////////
	// ----------------------------------------------------------------------
    void
    AodvUgdRouteDiscovery::
    observarUnaRuta(  std::string destino, std::string estado )
    throw()         
    {
   //****************  LOG   *****************
   std::ostringstream mensaje;
   mensaje <<"Escucha por Ruta: "<<estado<<".. Destino: "<<destino;
   LoggerAodv::Instance()->logCsvPingDetalle( procesoAodv_->owner().label() ,
                                 "" /*origen*/ , destino ,"Discovery", mensaje.str() );
   //**************** FIN LOG ****************

    	procesoAodv_->agregarObservadorTabla (  this , destino ,  estado );
    }
    // ----------------------------------------------------------------------
    void
    AodvUgdRouteDiscovery::
    cancelarObservarUnaRuta(  std::string destino, std::string estado )
    throw()         
    {
    	procesoAodv_->quitarObservadorTabla (  this , destino ,  estado );
    }
	// ----------------------------------------------------------------------
	void
	AodvUgdRouteDiscovery::
	handleAvisoRutaActiva ( std::string destino ) throw()
	{
	//****************  LOG   *****************
   std::ostringstream mensaje;
   mensaje <<"Conoce nueva Ruta Activa.. Destino: "<<destino;
   LoggerAodv::Instance()->logCsvPingDetalle(procesoAodv_->owner().label() ,
                                 "" /*origen*/ , destino  , "Discovery", mensaje.str() );
   //**************** FIN LOG ****************

		//verifico que este el decubrimiento todavia por las dudas
		if( existeDescubrimientoIniciado(destino) )
		{
			cancelarDiscovery(destino);
			//enviar paquetes pendientes a destino

		}
		else
		{
			//TODO avisar log
		}
	}

	// ----------------------------------------------------------------------
	void
	AodvUgdRouteDiscovery::
	cancelarDiscovery (std::string destino  ) throw()
	{
		std::map <std::string , infoDiscoveryRoute*> ::iterator it;
		it=discoverysMap.find( destino );
		infoDiscoveryRoute *pUnaInfoDiscoveryRoute = it->second;

		//cancelar Tiempo de espera
		cancelarEventoTiempoEspera(*pUnaInfoDiscoveryRoute);

		//elimino el info del discovery
		discoverysMap.erase (destino);
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdRouteDiscovery::
	cancelarEventoTiempoEspera(  infoDiscoveryRoute &pUnaInfoDiscoveryRoute  )
	throw()					
	{
		shawn::EventScheduler::EventHandle pEventHandle = 
								pUnaInfoDiscoveryRoute.getEventoTiempoEspera();

		procesoAodv_->owner_w().world_w().scheduler_w().delete_event ( pEventHandle );
	}
	/////////////////////////////////////////////////////////////////
	/***************** FIN AvisoEstadoRuta (observer)  *************/
	/////////////////////////////////////////////////////////////////
	// ----------------------------------------------------------------------
	bool
	AodvUgdRouteDiscovery::
	existeDescubrimientoIniciado	(  std::string dest_) throw()
	{
		bool existe = false;
		std::map<const std::string , infoDiscoveryRoute*> ::iterator it;
		it = discoverysMap.find( dest_ );
		 if ( it != discoverysMap.end() )
		 {
			 existe = true;
		 }
		 return existe;
	}
	// ----------------------------------------------------------------------
	unsigned int
	AodvUgdRouteDiscovery::
	obtenerRREQ_ReintentosSentDestino( std::string destino ) throw()
	{
		 unsigned int RREQ_Sent	=	0;
		 std::map<const std::string , infoDiscoveryRoute*> ::iterator it;
		 it = discoverysMap.find( destino );
		 //si existe el descubrimiento,
		 //no se  usa el metodo existeDescubrimientoIniciado para optimizar
		 if ( it != discoverysMap.end() )
		 {
			 RREQ_Sent	=	it -> second-> getRREQ_sent_by_this_discovery() ;
		 }
		 return RREQ_Sent;
	}
 	// ----------------------------------------------------------------------
	 AodvUgdRREQ*	
	AodvUgdRouteDiscovery::
	armarRREQ_reintento( std::string destino, unsigned int newTtl ) throw()
	{

		/*const_iterator si o si*/
		 std::map<const std::string , infoDiscoveryRoute*> ::const_iterator  it;
		  it = discoverysMap.find( destino );
		 //si existe el descubrimiento,
		 //no se  usa el metodo existeDescubrimientoIniciado para optimizar
		 if ( it != discoverysMap.end() )
		 {
			 //return it -> second->getUltimoRREQ() ;
			 const infoDiscoveryRoute *uninfoDiscoveryRoute	=	it->second;
			 return new AodvUgdRREQ( 
			 				  uninfoDiscoveryRoute->getDestino() ,
			    			  uninfoDiscoveryRoute->getOrigen() ,
			    			  uninfoDiscoveryRoute->getDestSequNumb() ,
			    			  uninfoDiscoveryRoute->getOrigenSequNumb() ,
			    			  uninfoDiscoveryRoute->getJoinFlag() ,
			    			  uninfoDiscoveryRoute->getRepairFlag() ,
			    			  uninfoDiscoveryRoute->getRREP_gFlag() ,
			    			  uninfoDiscoveryRoute->getDestOnlyFlag() ,
			    			  uninfoDiscoveryRoute->getUnknownSequNum() ,
			    			  uninfoDiscoveryRoute->getHops() ,
			    			  contadorRREQ_ID_ ,// esta sumado!!
			    			  procesoAodv_-> owner().label()  /*ipOrigen*/,
			    			  newTtl );
		 }
		 return NULL;
	}
	// ----------------------------------------------------------------------
	unsigned int
	AodvUgdRouteDiscovery::
	obtenerTtlAnterrior( std::string destino )const throw()
	{
		/*const_iterator si o si*/
		 std::map<const std::string , infoDiscoveryRoute*> ::const_iterator  it;
		  it = discoverysMap.find( destino );
		 //si existe el descubrimiento,
		 //no se  usa el metodo existeDescubrimientoIniciado para optimizar
		 if ( it != discoverysMap.end() )
		 {
			 const infoDiscoveryRoute *uninfoDiscoveryRoute	=	it->second;
			 return uninfoDiscoveryRoute->getTtl();
		 }
		 return 0;
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdRouteDiscovery::
	handle_EventExpireTimeDiscovery( std::string destino ) throw()
	{
/*		 si llega hasta aca es porque no se encontraron rutas activas todavia, porque
		 se monitorea todo el tiempo la ruta y si se encuentra se cancela el envento*/


		/*verificar por las dudas para hacer una assert que la ruta no existe*/

		  std::cout<< "Evento: Tiempo de espera sin encontrar Ruta " <<	std::endl;

		  /*TODO verificar en una funcion si se puede reintentar*/

		  /*TODO ver segun ERS o otro metodo activado*/


	bool seEnvioNuevoReintento         = reintentarProcesoRuteDescovery(destino);
   /*at  the end of the discovery period, the repairing node has not received
   a RREP (or other control message creating or updating the route) for
   that destination, it proceeds as described in Section 6.11 by
   transmitting a RERR message for that destination.*/
	
	//si por algun motivo no se realizan mas reintentos se supone que no se pudo
	//reparar la ruta..
	if (!seEnvioNuevoReintento)
		{
			bool descubrimientoLocalRepair    = isDescubrimientoLocalRepair (destino);
			TipoLocalRepair unTipoLocalRepair = getTipoLocalRepairInfoDiscovery (destino);

			//si el RREQ.repairFlag_ = false es porque el descubrimiento no usa LocalREpair
			assert (!descubrimientoLocalRepair && (unTipoLocalRepair == TipoLocalRepair::None) );
			//si el descubrimiento era por que se intentaba reparar la ruta, hay que manejar el error!
			if(descubrimientoLocalRepair) 
			{	
				//se intento reparar sin exito y se envia el RERR
				if(unTipoLocalRepair == TipoLocalRepair::LinkBreak)
					procesoAodv_-> rerrDetectesLinkBreak (destino);

				if(unTipoLocalRepair == TipoLocalRepair::PacketSinRutaActivaParaReenviar) 
					procesoAodv_->rerrDataPacketSinRutaActivaParaReenviar (destino);	  
				}
			//sin importar si se usa local repair o no..
			//si no hago mas reintentos hay que hacer drop de los mensajes pendientes en el buffer
			procesoAodv_->handleNotFindRoute(destino);
		}
	else
		{
			//TODO log no se encontro la ruta luego de todos los rintentos

		}	
	}
	// ----------------------------------------------------------------------
	bool
	AodvUgdRouteDiscovery::
	isDescubrimientoLocalRepair( std::string destino ) throw()
	{
		 bool repairFlag = false;
		 std::map<const std::string , infoDiscoveryRoute*> ::iterator it;
		 it = discoverysMap.find( destino );
		 //si existe el descubrimiento,
		 //no se  usa el metodo existeDescubrimientoIniciado para optimizar
		 if ( it != discoverysMap.end() )
		 {
			 repairFlag	=	it -> second-> getRREP_gFlag() ;
		 }
		 return repairFlag;
	}
		// ----------------------------------------------------------------------
	const TipoLocalRepair
	AodvUgdRouteDiscovery::
	getTipoLocalRepairInfoDiscovery( std::string destino ) throw()
	{
	
		 std::map<const std::string , infoDiscoveryRoute*> ::iterator it;
		 it = discoverysMap.find( destino );
		 //si existe el descubrimiento,
		 //no se  usa el metodo existeDescubrimientoIniciado para optimizar
		 if ( it != discoverysMap.end() )
		 {
			 return	it -> second-> getTipoLocalRepair() ;
		 }
		 return TipoLocalRepair::None ;
	}
		
	// ----------------------------------------------------------------------
	shawn::EventScheduler::EventHandle
	AodvUgdRouteDiscovery::
	generarPrimerTiempoEspera (unsigned int ultimoTtl , double tiempoDeEspera ,
							   std::string destino 	 ) 
	throw()
	{
		shawn::EventScheduler& pEventScheduler = procesoAodv_->owner_w().world_w().scheduler_w();

		DiscoveryEventTag* pDiscoveryEventTag=new DiscoveryEventTag
							(  destino , ultimoTtl, 0 /*reintento*/ );

		double tiempoEvento=tiempoDeEspera + procesoAodv_->owner().current_time();
		return pEventScheduler.new_event  ( *this , tiempoEvento , pDiscoveryEventTag );

		
		//**********										
		//LOGGGGG!									
/*		std::cout<< "Evento Tiempo Espera Route Dicovery en : " << procesoAodv_->owner().label()
						<<"  programado para:  "<< tiempoEvento
						<<	std::endl;

		std::stringstream mensaje;
      	mensaje <<"generarPrimerTiempoEspera programado: "<< tiempoEvento;
		LoggerAodv::Instance()->logCsvRREQ( pUltimoRREQ ,procesoAodv_->owner().label() , 
											mensaje.str()  ); 		*/			
	}
	// ----------------------------------------------------------------------
	shawn::EventScheduler::EventHandle
	AodvUgdRouteDiscovery::
	generarEventoTiempoEspera (unsigned int ultimoTtl 			 , double tiempoDeEspera ,
							   unsigned int contadorReintentos   , std::string destino   ) 
	throw()
	{
		shawn::EventScheduler& pEventScheduler = procesoAodv_->owner_w().world_w().scheduler_w();

		DiscoveryEventTag* pDiscoveryEventTag=new DiscoveryEventTag
											(  destino , ultimoTtl , contadorReintentos /*reintento*/);

		double tiempoEvento		=	tiempoDeEspera + procesoAodv_->owner().current_time();
		
		shawn::EventScheduler::EventHandle  pEventHandle= pEventScheduler.new_event  ( *this , tiempoEvento , pDiscoveryEventTag );
		return pEventHandle;
		
		 //*************
		 //LOGGGGGGGGGGG
/*		 		std::cout<< "Evento: Reintento-Tiempo Espera Route Dicovery en : "
						<< procesoAodv_->owner().label()
						<<"  programado para:  "<< tiempoEvento
						<<	std::endl;

		std::stringstream mensaje;
      	mensaje <<"generarUnNuevoEventoTiempoEsperaReintento programado: "<< tiempoEvento;
		LoggerAodv::Instance()->logCsvRREQ( pUltimoRREQ ,procesoAodv_->owner().label() ,
											 mensaje.str() ); */
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdRouteDiscovery::
	guardarPrimeraInfoDiscoveryRoute (const AodvUgdRREQ  &pUltimoRREQ ,
							   		  shawn::EventScheduler::EventHandle pEventHandle ,
							   		  const TipoLocalRepair	 &unaTipoLocalRepair) throw()
	{
								
		infoDiscoveryRoute* pUnaInfoDiscoveryRoute=new infoDiscoveryRoute
											(  pUltimoRREQ	/*&ultimoRREQ*/,
											   0	/*RREQ_sent_by_this_discovery*/,
											   pEventHandle	/*EventHandler &eventoTiempoEspera*/,
											   unaTipoLocalRepair 				);

		discoverysMap.insert(std::pair<std::string,infoDiscoveryRoute*>
		( pUltimoRREQ.getDestino() , pUnaInfoDiscoveryRoute));
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdRouteDiscovery::
	guardarReintentoInfoDiscoveryRoute (const AodvUgdRREQ  &pUltimoRREQ ,
							   		    shawn::EventScheduler::EventHandle pEventHandle ,
							   		    unsigned int 		contadorReintentos ) throw()
	{

		//tengo que traer el tipoLocalRepair anterrior
		TipoLocalRepair 	unaTipoLocalRepair	   = getTipoLocalRepairInfoDiscovery( pUltimoRREQ.getDestino()  );				
		infoDiscoveryRoute* pUnaInfoDiscoveryRoute = new infoDiscoveryRoute
											(  pUltimoRREQ	/*&ultimoRREQ*/,
											   contadorReintentos	/*RREQ_sent_by_this_discovery*/,
											   pEventHandle	/*EventHandler &eventoTiempoEspera*/,
											   unaTipoLocalRepair 				);

		modificarInfoDiscoveryRoute ( pUltimoRREQ.getDestino() , *pUnaInfoDiscoveryRoute );
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdRouteDiscovery::
	modificarInfoDiscoveryRoute (const std::string dest , infoDiscoveryRoute &pUnaInfoDiscoveryRoute ) throw()
	{
		std::map <std::string , infoDiscoveryRoute*> ::iterator it;
		it=discoverysMap.find( dest );
		it->second	=	&pUnaInfoDiscoveryRoute	; //utilizo un puntero para modificar directamente
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdRouteDiscovery::
	modificarTipoRepairInfoDiscoveryRoute (const std::string dest , TipoLocalRepair unTipoLocalRepair) throw()
	{
		std::map <std::string , infoDiscoveryRoute*> ::iterator it;
		it=discoverysMap.find( dest );
		it->second->setTipoLocalRepair(unTipoLocalRepair); 
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdRouteDiscovery::
	sendDiscoveryRREQ ( AodvUgdRREQ* pRREQ)
	//creo un metodo para enviar los dicoverys, asi se suma el contador y si
	//en algun momento cambio la forma de enviar modifico unicamente aca :)
	{
		contadorRREQ_sent_seg_++;
		//por ahora envio por el proceso principal
		procesoAodv_->sendRREQ (pRREQ) ;
	}
	// ----------------------------------------------------------------------
	void
	AodvUgdRouteDiscovery::
	comenzarProcesoRuteDescovery(std::string destino , std::string origen )
    throw()
    {
  /*se supone que antes de comenzar el descubrimiento se busco una ruta activa*/
  /*TODO existe un descubrimiento iniciado con el mismo destino?*/
    if (existeDescubrimientoIniciado (destino) )
	   /*TODO ver si se mueve algun evento*/
	   return;
  /*verificar si se supera RREQ_RATELIMIT, la cantidad de RREQ enviado en un seg*/
  /*A node SHOULD NOT originate more than RREQ_RATELIMIT RREQ messages
    per second.*/
	bool alcanzoRREQ_RATELIMIT = verificarRateLimit();
  /*un evento que cada segundo renicie el contador..*/
	if ( alcanzoRREQ_RATELIMIT )
	    return;

  /*The Originator Sequence Number in the RREQ message is the
    node's own sequence number, which is incremented prior to insertion
    in a RREQ.*/
    procesoAodv_->aumentarContadorNodoSeqNum() ;
    unsigned int nodoSeqNum = procesoAodv_-> getContadorNodoSeqNum();
  /*The RREQ ID field is incremented by one from the last
    RREQ ID used by the current node.*/
    contadorRREQ_ID_++;

  //Guarda en el buffer  rreqID+origen (su propia direccion)
  /*Before broadcasting the RREQ, the originating node buffers the RREQ
    ID and the Originator IP address (its own address) of the RREQ for
    PATH_DISCOVERY_TIME.  In this way, when the node receives the packet
    again from its neighbors, it will not reprocess and re-forward the
    packet*/
    procesoAodv_->agregarRREQ_Buffer ( contadorRREQ_ID_ ,  origen);
  /*TODO RREQ_sent_in_a_second+1  ver con un observe o verificando la variable nomas, 
    un observe que asigne 0 cada 1 segundo*/

    unsigned int ttlInicial   = unaConfiguracionDiscovery_.getTTL_START();
    AodvUgdRREQ *UnPrimerRREQ =
    				new AodvUgdRREQ (  destino         ,  origen        , 0     		  /*destSequNumb*/  , 
               						   nodoSeqNum  /*origenSequNumb*/   , false 		  /*joinFlag*/      ,  
               						   false       /*repairFlag*/                                           , 
               						   unaConfiguracionDiscovery_.getUseGratuitousRREP()   /*rrep_gFlag*/   ,
				       				   unaConfiguracionDiscovery_.getUseDestOnly()         /*destOnlyFlag*/ , 
               						   true 						                      /*UnknownSequNum*/,
				      				    0          /*hops*/             , contadorRREQ_ID_/*RREQ_ID*/       ,
    									procesoAodv_-> owner().label()					  /*ipOrigen*/		,
				      				    ttlInicial          /*ttl*/ );
    
  /*TODO si RREQ_sent_in_a_second> se puede enviar*/
  //en el primer RREQ no hay que verificar TTL_THRESHOLD
  /*If the RREQ times out
    without a corresponding RREP, the originator broadcasts the RREQ
    again with the TTL incremented by TTL_INCREMENT.  This continues
    until the TTL set in the RREQ reaches TTL_THRESHOLD, beyond which a
    TTL = NET_DIAMETER is used for each attempt.*/
	double tiempoDeEspera	=	unaConfiguracionDiscovery_.calcularRING_TRAVERSAL_TIME
									( ttlInicial );
  //calcularTiempoEsperaERS; no se usa en el primero
  /*programar evento: esperar TIMEOUT*/
	shawn::EventScheduler::EventHandle pEventHandle = generarEventoTiempoEspera (
										ttlInicial /*ultimoTtl*/ ,  
										tiempoDeEspera ,
										0 /*contadorReintentos*/ ,  destino 		);
  
  //guardar info del descubrimiento para el proximo reintento
	guardarPrimeraInfoDiscoveryRoute (*UnPrimerRREQ/*pUltimoRREQ*/ ,
							   		  pEventHandle ,
							   		  TipoLocalRepair::None );
  //observo la ruta para saber cuando esta activa, sea porque me responden el RREP
  //o porque recibo un mensaje que actualiza la tabla.
	observarUnaRuta    ( destino , "active" /*estado*/ );

	//****************  LOG   *****************
   std::ostringstream mensaje;
   mensaje <<"Busqueda: Nueva TTL: "<<UnPrimerRREQ->getTtl()<< "  Timeout: "<< tiempoDeEspera;
   LoggerAodv::Instance()->logCsvPingDetalle( procesoAodv_->owner().label()    ,
                                 UnPrimerRREQ->getOrigen() , UnPrimerRREQ->getDestino() ,
                                 "Discovery", mensaje.str() );
   //**************** FIN LOG ****************

  //sumo el contador global de RREQ enviados cada vez que llamo sendDiscoveryRREQ								
	sendDiscoveryRREQ  (UnPrimerRREQ);   
    }
// ----------------------------------------------------------------------
	void
	AodvUgdRouteDiscovery::
	comenzarProcesoRuteDescoveryEntradaExistente (std::string destino , std::string origen,
												  const AodvUgdTableEntery& unaEntrada)
    throw()
    {
  /*se supone que antes de comenzar el descubrimiento se busco una ruta activa*/
  /*TODO existe un descubrimiento iniciado con el mismo destino?*/
    if (existeDescubrimientoIniciado (destino) )
	   /*TODO ver si se mueve algun evento*/
	   return;
  /*verificar si se supera RREQ_RATELIMIT, la cantidad de RREQ enviado en un seg*/
  /*A node SHOULD NOT originate more than RREQ_RATELIMIT RREQ messages
    per second.*/
	bool alcanzoRREQ_RATELIMIT = verificarRateLimit();
  /*un evento que cada segundo renicie el contador..*/
	if ( alcanzoRREQ_RATELIMIT )
	    return;

  /*The Originator Sequence Number in the RREQ message is the
    node's own sequence number, which is incremented prior to insertion
    in a RREQ.*/
    procesoAodv_->aumentarContadorNodoSeqNum() ;
    unsigned int nodoSeqNum = procesoAodv_-> getContadorNodoSeqNum();
  /*The RREQ ID field is incremented by one from the last
    RREQ ID used by the current node.*/
    contadorRREQ_ID_++;

  //Guarda en el buffer  rreqID+origen (su propia direccion)
  /*Before broadcasting the RREQ, the originating node buffers the RREQ
    ID and the Originator IP address (its own address) of the RREQ for
    PATH_DISCOVERY_TIME.  In this way, when the node receives the packet
    again from its neighbors, it will not reprocess and re-forward the
    packet*/
    procesoAodv_->agregarRREQ_Buffer ( contadorRREQ_ID_ ,  origen);
  /*TODO RREQ_sent_in_a_second+1  ver con un observe o verificando la variable nomas, 
    un observe que asigne 0 cada 1 segundo*/

   //TODO solo cambia aca de route descovery se puede factorizar colocando en funciones
   //el resto de las funciones
    AodvUgdRREQ *UnPrimerRREQ =
	   	armarPrimerRREQRutaExistente(  destino          ,  origen         						,
			               			   unaEntrada.getDestSequenceNumber()/*rutaDestSequNumb*/   ,
			               			   nodoSeqNum 						 /*origenSequNumb*/ 	,
			               			   unaEntrada.getValidDestSeqNum()   /*rutaValidDestSeqNum*/,  
	   								   unaEntrada.getHops() 			 /*rutaDestHops*/  		,
			               			   contadorRREQ_ID_ 				 /*RREQ_ID*/       	);

  /*TODO si RREQ_sent_in_a_second> se puede enviar*/
  //en el primer RREQ no hay que verificar TTL_THRESHOLD
  /*If the RREQ times out
    without a corresponding RREP, the originator broadcasts the RREQ
    again with the TTL incremented by TTL_INCREMENT.  This continues
    until the TTL set in the RREQ reaches TTL_THRESHOLD, beyond which a
    TTL = NET_DIAMETER is used for each attempt.*/
	double tiempoDeEspera	=	unaConfiguracionDiscovery_.calcularRING_TRAVERSAL_TIME
									( UnPrimerRREQ->getTtl() );
  //calcularTiempoEsperaERS; no se usa en el primero
  /*programar evento: esperar TIMEOUT*/
	shawn::EventScheduler::EventHandle pEventHandle = generarEventoTiempoEspera (
										UnPrimerRREQ->getTtl() /*ultimoTtl*/ ,  
										tiempoDeEspera ,
										0 /*contadorReintentos*/ ,  destino 		);
  
  //guardar info del descubrimiento para el proximo reintento
	guardarPrimeraInfoDiscoveryRoute (*UnPrimerRREQ/*pUltimoRREQ*/ ,
							   		  pEventHandle ,
							   		  TipoLocalRepair::None );
  //observo la ruta para saber cuando esta activa, sea porque me responden el RREP
  //o porque recibo un mensaje que actualiza la tabla.
	observarUnaRuta    ( destino , "active" /*estado*/ );


   //****************  LOG   *****************
   std::ostringstream mensaje;
   mensaje <<"Busqueda: Comienzo TTL: "<<UnPrimerRREQ->getTtl()<< "  Timeout: "<< tiempoDeEspera;
   LoggerAodv::Instance()->logCsvPingDetalle( procesoAodv_->owner().label() ,
                                 UnPrimerRREQ->getOrigen() , UnPrimerRREQ->getDestino() ,
                                 "Discovery", mensaje.str() );
   //**************** FIN LOG ****************

  //sumo el contador global de RREQ enviados cada vez que llamo sendDiscoveryRREQ								
	sendDiscoveryRREQ  (UnPrimerRREQ);   

    }
   // ----------------------------------------------------------------------
	AodvUgdRREQ*	
	AodvUgdRouteDiscovery::
	armarPrimerRREQRutaExistente( std::string  dest         	, std::string  origen          ,
		               			  unsigned int rutaDestSequNumb , unsigned int  origenSequNumb ,
		               			  bool rutaValidDestSeqNum   	, unsigned int rutaDestHops    ,
		               			  unsigned int RREQ_ID       	)
    throw()
    {

	unsigned int ttlInicial =	rutaDestHops + unaConfiguracionDiscovery_.getTTL_INCREMENT();
  /*The Destination Sequence Number field in the RREQ message is the last
    known destination sequence number and is copied
    from the Destination Sequence Number field in the routing table.*/
	   
	unsigned int RREQ_DestSeqNum	=	0 ;	  
	   if ( rutaValidDestSeqNum )     	
		   RREQ_DestSeqNum	=	rutaDestSequNumb ;
			 
    return	new AodvUgdRREQ
			   	   ( dest            , origen          	  , RREQ_DestSeqNum     , 
               		 origenSequNumb  , false /*joinFlag*/ , false /*repairFlag*/, 
               		 unaConfiguracionDiscovery_.getUseGratuitousRREP() /*rrep_gFlag*/    	,
				     unaConfiguracionDiscovery_.getUseDestOnly() /*destOnlyFlag*/        	, 
                     ! rutaValidDestSeqNum /*UnknownSequNum*/                  	,
				     0 /*hops*/    , RREQ_ID   		  , 
			   	   	 procesoAodv_-> owner().label()  /*ipOrigen*/				, ttlInicial );

    }
    // ----------------------------------------------------------------------
	bool 	
	AodvUgdRouteDiscovery::
	verificarRateLimit()
	throw()
	{
		if (contadorRREQ_sent_seg_ > unaConfiguracionDiscovery_.getRREQ_RATELIMIT() )
			return true;
		return false;
	}
    // ----------------------------------------------------------------------
    void
	AodvUgdRouteDiscovery::
	comenzarProcesoRuteDescoveryLocalRepair(std::string destino , std::string origen,
								 			AodvUgdTableEntery *unaEntrada       ,
								 			unsigned int ttlInicialLocalRepair   			 ,
								 			const TipoLocalRepair &unTipoLocalRepair    )
    throw()
    {
  /*se supone que antes de comenzar el descubrimiento se busco una ruta activa*/
  /*TODO existe un descubrimiento iniciado con el mismo destino?*/
    if (existeDescubrimientoIniciado (destino) )
	   /*TODO ver si se mueve algun evento*/
	   return;
  /*verificar si se supera RREQ_RATELIMIT, la cantidad de RREQ enviado en un seg*/
  /*A node SHOULD NOT originate more than RREQ_RATELIMIT RREQ messages
    per second.*/
	bool alcanzoRREQ_RATELIMIT = verificarRateLimit();
/*	un evento que cada segundo renicie el contador..*/
	if ( alcanzoRREQ_RATELIMIT )
	    return;

  /*The Originator Sequence Number in the RREQ message is the
    node's own sequence number, which is incremented prior to insertion
    in a RREQ.*/
    procesoAodv_->aumentarContadorNodoSeqNum() ;
    unsigned int nodoSeqNum = procesoAodv_-> getContadorNodoSeqNum();

  /*The RREQ ID field is incremented by one from the last
    RREQ ID used by the current node.*/
    contadorRREQ_ID_++;

  //Guarda en el buffer  rreqID+origen (su propia direccion)
  /*Before broadcasting the RREQ, the originating node buffers the RREQ
    ID and the Originator IP address (its own address) of the RREQ for
    PATH_DISCOVERY_TIME.  In this way, when the node receives the packet
    again from its neighbors, it will not reprocess and re-forward the
    packet*/
    procesoAodv_->agregarRREQ_Buffer ( contadorRREQ_ID_ ,  origen);
  /*TODO RREQ_sent_in_a_second+1  ver con un observe o verificando la variable nomas, 
    un observe que asigne 0 cada 1 segundo*/

  /*To repair the link
    break, the node increments the sequence number for the destination
    and then broadcasts a RREQ for that destination.*/
    unsigned int nuevoSeqNumRuta = unaEntrada->getDestSequenceNumber() + 1;
    unaEntrada->setDestSequenceNumber ( nuevoSeqNumRuta );

	  AodvUgdRREQ *UnPrimerRREQ=
	   	armarPrimerRREQRutaExistenteLocalRepair
	   								(  destino         	,  origen          ,
			               			   unaEntrada -> getDestSequenceNumber() /*rutaDestSequNumb*/   ,
			               			   nodoSeqNum /*origenSequNumb*/ 	   ,
			               			   unaEntrada -> getValidDestSeqNum()    /*rutaValidDestSeqNum*/,  
	   								   unaEntrada -> getHops() 				 /*rutaDestHops*/  		,
			               			   contadorRREQ_ID_/*RREQ_ID*/       	, ttlInicialLocalRepair 			);
  
  /*TODO si RREQ_sent_in_a_second> se puede enviar*/
  //en el primer RREQ no hay que verificar TTL_THRESHOLD
  /*If the RREQ times out
    without a corresponding RREP, the originator broadcasts the RREQ
    again with the TTL incremented by TTL_INCREMENT.  This continues
    until the TTL set in the RREQ reaches TTL_THRESHOLD, beyond which a
    TTL = NET_DIAMETER is used for each attempt.*/
	double tiempoDeEspera	=	unaConfiguracionDiscovery_.calcularRING_TRAVERSAL_TIME
									( UnPrimerRREQ->getTtl() );

  //calcularTiempoEsperaERS; no se usa en el primero
  /*programar evento: esperar TIMEOUT*/
	shawn::EventScheduler::EventHandle pEventHandle = generarEventoTiempoEspera (
										UnPrimerRREQ->getTtl() /*ultimoTtl*/ ,
										tiempoDeEspera ,
										0 /*contadorReintentos*/ ,  destino 		);
  
  //guardar info del descubrimiento para el proximo reintento
	guardarPrimeraInfoDiscoveryRoute (*UnPrimerRREQ/*pUltimoRREQ*/ ,
							   		  pEventHandle ,
							   		  unTipoLocalRepair );

  //observo la ruta para saber cuando esta activa, sea porque me responden el RREP
  //o porque recibo un mensaje que actualiza la tabla.
	observarUnaRuta    ( destino , "active" /*estado*/ );

   //****************  LOG   *****************
   std::ostringstream mensaje;
   mensaje <<"Busqueda: Nueva Repair! TTL: "<<UnPrimerRREQ->getTtl()<< "  Timeout: "<< tiempoDeEspera;
   LoggerAodv::Instance()->logCsvPingDetalle( procesoAodv_->owner().label()    ,
                                 UnPrimerRREQ->getOrigen() , UnPrimerRREQ->getDestino() ,
                                 "Discovery", mensaje.str() );
   //**************** FIN LOG ****************

  //sumo el contador global de RREQ enviados cada vez que llamo sendDiscoveryRREQ							
	sendDiscoveryRREQ  (UnPrimerRREQ);   
    }
    // ----------------------------------------------------------------------
	AodvUgdRREQ*	
	AodvUgdRouteDiscovery::
	armarPrimerRREQRutaExistenteLocalRepair( std::string  dest         	   , std::string  origen         ,
					               			 unsigned int rutaDestSequNumb , unsigned int origenSequNumb ,
					               			 bool rutaValidDestSeqNum      , unsigned int rutaDestHops   ,
					               			 unsigned int RREQ_ID          , unsigned int ttlInicial     )
    throw()
    {
/* To repair the link
   break, the node increments the sequence number for the destination
   and then broadcasts a RREQ for that destination.  The TTL of the RREQ
   should initially be set to the following value:

      max(MIN_REPAIR_TTL, 0.5 * #hops) + LOCAL_ADD_TTL,*/


	
  /*The Destination Sequence Number field in the RREQ message is the last
    known destination sequence number and is copied
    from the Destination Sequence Number field in the routing table.*/
	   
	unsigned int RREQ_DestSeqNum	=	0 ;	  
	   if ( rutaValidDestSeqNum )     	
		   RREQ_DestSeqNum	=	rutaDestSequNumb ;
			 
    return	new AodvUgdRREQ
			   	   ( dest            , origen          	  , RREQ_DestSeqNum     , 
               		 origenSequNumb  , false /*joinFlag*/ , true /*repairFlag*/ , 
               		 unaConfiguracionDiscovery_.getUseGratuitousRREP() /*rrep_gFlag*/    	,
				     unaConfiguracionDiscovery_.getUseDestOnly() /*destOnlyFlag*/        	, 
                     !rutaValidDestSeqNum 	/*UnknownSequNum*/                 	,
				     0 /*hops*/    , RREQ_ID   		  , 
			   	   	 procesoAodv_->owner().label() /*ipOrigen*/ , ttlInicial );

    }
   	// ----------------------------------------------------------------------
	void
	AodvUgdRouteDiscovery::
	generarEventoResetContadorRREQ ( ) throw()
	{
		double tiempoDeEspera	=	1000; //cada 1000 milisegundos
		shawn::EventScheduler& pEventScheduler = procesoAodv_->owner_w().world_w().scheduler_w();

		DiscoveryEventResetRREQ_limit* unDiscoveryEventResetRREQ_limit=new DiscoveryEventResetRREQ_limit
									(  procesoAodv_->owner().current_time() /*ultimoReset*/ );

		double tiempoEvento=tiempoDeEspera + procesoAodv_->owner().current_time();
		shawn::EventScheduler::EventHandle pEventHandle = pEventScheduler.new_event
												( *this , tiempoEvento ,
												 unDiscoveryEventResetRREQ_limit );	
	}
   	// ----------------------------------------------------------------------
   	void
	AodvUgdRouteDiscovery::
	handleEventoResetContadorRREQ ( ) throw()
	{
		contadorRREQ_sent_seg_	=	0;
		generarEventoResetContadorRREQ();
	}
   	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------
	bool 
	AodvUgdRouteDiscovery::
	reintentarProcesoRuteDescovery
	( std::string destino  )
    throw()
	{		
	assert ( existeDescubrimientoIniciado (destino) );
  /*verificar si se supera RREQ_RATELIMIT, la cantidad de RREQ enviado en un seg*/
  /*A node SHOULD NOT originate more than RREQ_RATELIMIT RREQ messages
    per second.*/
	bool alcanzoRREQ_RATELIMIT = verificarRateLimit();
  /*un evento que cada segundo renicie el contador..*/
	if ( alcanzoRREQ_RATELIMIT )
	    return false;

	unsigned int reintentos				=	obtenerRREQ_ReintentosSentDestino ( destino );
	//const AodvUgdRREQ* pRREQ_anterrior 	= 	obtenerUltimoRREQ_Destino ( destino );
	unsigned int maxReintentos			=	unaConfiguracionDiscovery_.getRREQ_RETRIES();
	unsigned int maxTTL					=	unaConfiguracionDiscovery_.getTTL_THRESHOLD();
	unsigned int ttlincrement			=	unaConfiguracionDiscovery_.getTTL_INCREMENT();
	/*the originator broadcasts the RREQ
	again with the TTL incremented by TTL_INCREMENT.  This continues
	until the TTL set in the RREQ reaches TTL_THRESHOLD, beyond which a
	TTL = NET_DIAMETER is used for each attempt.  Each time, the timeout
	for receiving a RREP is RING_TRAVERSAL_TIME*/
	unsigned int newTTL	=	0;
	unsigned int oldTTL	=	obtenerTtlAnterrior(destino);
	//se puede simplificar mucho el if pero no quedan claras las condiciones
	if(maxReintentos>0) //se admiten reintentos
	{
		if ( (0 < reintentos) && (reintentos < maxReintentos) ){//comenzo los reintentos y todavia se pueden hacer mas
			reintentos++;
			newTTL	=	unaConfiguracionDiscovery_.getNET_DIAMETER ();}

		if ( (0 < reintentos) && (reintentos >= maxReintentos) ){ //comenzo los reintentos, pero se alcanzo el maxReintentos
			std::cout<< "se alcanzo maxReintentos: "<< maxReintentos
				<<"   contReintento: "<< reintentos <<std::endl;
			return false;}

		if ( reintentos==0  &&			         //no comenzo el proceso de reintentos
			oldTTL+ttlincrement < maxTTL )       //pero no hace falta enviar el 1er reintento
			newTTL	=	oldTTL + ttlincrement;

		if ( reintentos==0  &&			         //no comenzo el proceso de reintentos
			oldTTL + ttlincrement >= maxTTL ) {  //se realiza el 1er reintento
			reintentos++;
			newTTL	=	unaConfiguracionDiscovery_.getNET_DIAMETER ();}

	}else	//no se admiten reintentos maxReintentos == 0
	{
		if( oldTTL+ttlincrement < maxTTL )
			newTTL	=	oldTTL + ttlincrement;
		else{
			std::cout<< "se alcanzo maxTTl: "<< maxTTL
				<<"   y no se admiten reintentos: "<< reintentos <<std::endl;
			return false; }
	}
	/*Each new attempt MUST increment and update the RREQ ID.*/
	contadorRREQ_ID_++;
	procesoAodv_->agregarRREQ_Buffer ( contadorRREQ_ID_ ,  procesoAodv_->owner().label() );

	AodvUgdRREQ *pRREQ_Nuevo = armarRREQ_reintento( destino , newTTL );

	/*el valor de RING_TRAVERSAL_TIME depende del ttlActual y se tiene que calcular*/
	double tiempoDeEspera	= 	unaConfiguracionDiscovery_.
								calcularRING_TRAVERSAL_TIME ( newTTL );
  /*programar evento: esperar TIMEOUT*/
	shawn::EventScheduler::EventHandle pEventHandle = generarEventoTiempoEspera (
										newTTL /*ultimoTtl*/ 			  ,  tiempoDeEspera ,
										reintentos /*contadorReintentos*/ ,  destino 		);
  
  //guardar info del descubrimiento para el proximo reintento
	guardarReintentoInfoDiscoveryRoute (*pRREQ_Nuevo/*pUltimoRREQ*/ ,
							   		    pEventHandle ,
							   		    reintentos /*contadorReintentos*/ );
	
	LoggerAodv::Instance()->logCsvRREQ (*pRREQ_Nuevo, procesoAodv_->owner().label() , 
							 "reintentarProcesoRuteDescovery" );
  	LoggerAodv::Instance()->logCsvRREQ_Mensajes (*pRREQ_Nuevo,
                             "reintentarProcesoRuteDescovery" );

	   //****************  LOG   *****************
	   std::ostringstream mensaje;
	   mensaje <<"Busqueda: Reintento TTL: "<<pRREQ_Nuevo->getTtl()<< "  Timeout: "<< tiempoDeEspera;
	   LoggerAodv::Instance()->logCsvPingDetalle( procesoAodv_->owner().label() ,
	                                 			pRREQ_Nuevo->getOrigen() , pRREQ_Nuevo->getDestino() ,
	                                 			"Discovery", mensaje.str() );

	   LoggerAodv::Instance()->logCsvRoutingTable_comoTabla(procesoAodv_->owner().label() ,
	   									 procesoAodv_->unaTablaRuteo->getRoutingTableMap() );
	   //**************** FIN LOG ****************

  //sumo el contador global de RREQ enviados cada vez que llamo sendDiscoveryRREQ	
	sendDiscoveryRREQ (pRREQ_Nuevo);
 	return true;  //si se reaalizo el reintento aviso..
	}

	// ----------------------------------------------------------------------
/*	void
	AodvUgdRouteDiscovery::
    calcularTiempoEsperaERS( unsigned int rreqTtl , std::string destino	)
    throw()
    {
		double tiempo_RREQ_espera_un_RREP
		if( rreqTtl < TTL_THRESHOLD)
		{
			tiempo_RREQ_espera_un_RREP	=
					unaConfiguracionDiscovery.getRING_TRAVERSAL_TIME();
		}
		else
		{
			tiempo_RREQ_espera_un_RREP	=
					unaConfiguracionDiscovery.getNET_TRAVERSAL_TIME();
		}
		return	=	Tiempo_RREQ_espera_un_RREP;
    }*/
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
 // clase DiscoveryEventTag----------------------------------------------------------------------
	/*use como plantilla class RoutingEventTag*/
	//constructor
	DiscoveryEventTag::
	DiscoveryEventTag( std::string destino, unsigned int ultimoTtl,
						unsigned int reintento )
	//inicializacion
	:
			destino_			{destino},
			ultimoTtl_			{ultimoTtl},
			reintento_			{reintento}
			{}

	//----------------------------------------------------------------------
	//destructor
	DiscoveryEventTag::
	~DiscoveryEventTag(){}

	// get set----------------------------------------------------------------------
	/*unsigned int AodvUgdRREQ:: getHops ( void ) const throw()
	{	return hops_;}
	// ----------------------------------------------------------------------*/
	// ----------------------------------------------------------------------
	inline std::string
	DiscoveryEventTag::
	getDestino  (void) const	throw()
	{
		return destino_ ;
	}
	// ----------------------------------------------------------------------
	// metods----------------------------------------------------------------------
	std::string
	DiscoveryEventTag::
	toString(void) const
	throw()
		{
			{
				   std::stringstream ss;
				   ss <<
						   "destino: "		<<			destino_ 	<<' '<<
						   "ultimoTtl: "	<<			ultimoTtl_	<<' '<<
						   "reintento: "	<<			reintento_	<<' ';
				   return (ss.str());
			}
		}
	// ----------------------------------------------------------------------
	void
	DiscoveryEventTag::
	show  (void) const	throw()
	{
		std::cout << DiscoveryEventTag::toString()<< '\n';

	}
   // ----------------------------------------------------------------------
	 // clase DiscoveryEventResetRREQ_limit----------------------------------------------------------------------
	/*use como plantilla class RoutingEventTag*/
	//constructor
	DiscoveryEventResetRREQ_limit::
	DiscoveryEventResetRREQ_limit( double ultimoReset )
	//inicializacion
	:
			ultimoReset_			{ultimoReset}
			{}
	//----------------------------------------------------------------------
	//destructor
	DiscoveryEventResetRREQ_limit::
	~DiscoveryEventResetRREQ_limit(){}
	// get set----------------------------------------------------------------------
	inline const double 
	DiscoveryEventResetRREQ_limit::
	getUltimoReset  (void) const	throw()
	{		return ultimoReset_ ;	}
   // ----------------------------------------------------------------------
   // ----------------------------------------------------------------------
// clase infoDiscoveryRoute----------------------------------------------------------------------
/*	infoDiscoveryRoute::
	infoDiscoveryRoute()
	{}*/
	// ----------------------------------------------------------------------
	infoDiscoveryRoute::
	infoDiscoveryRoute (     const AodvUgdRREQ  	&ultimoRREQ,
			      	  	  	 unsigned int	        RREQ_sent_by_this_discovery,
							 shawn::EventScheduler::EventHandle eventoTiempoEspera,
							 TipoLocalRepair 	    unTipoLocalRepair )
	//inicializacion
	:
			//ultimoRREQ_						{&ultimoRREQ},
			RREQ_sent_by_this_discovery_	{RREQ_sent_by_this_discovery},
			eventoTiempoEspera_				{eventoTiempoEspera},

			dest_ 							{ ultimoRREQ.getDestino() },
			origen_ 						{ ultimoRREQ.getOrigen() },
			destSequNumb_ 					{ ultimoRREQ.getDestSequNumb() },
			origenSequNumb_ 				{ ultimoRREQ.getOrigenSequNumb() },
			joinFlag_ 						{ ultimoRREQ.getJoinFlag() },
			repairFlag_ 					{ ultimoRREQ.getRepairFlag() },
			rrep_gFlag_ 					{ ultimoRREQ.getRREP_gFlag() },
			destOnlyFlag_ 					{ ultimoRREQ.getDestOnlyFlag() },
			UnknownSequNum_ 				{ ultimoRREQ.getUnknownSequNum() },
			hops_ 							{ ultimoRREQ.getHops() },     			
	  		rreqId_ 						{ ultimoRREQ.getRREQ_ID() },			

	 		//Ip parameter
	  		ttl_ 							{ ultimoRREQ.getTtl()},
	  		//Local repair
	  		tipoLocalRepair_				{unTipoLocalRepair}
			{}

	// ----------------------------------------------------------------------
	infoDiscoveryRoute::
	~infoDiscoveryRoute() {}

	// get set----------------------------------------------------------------------
	unsigned int
	infoDiscoveryRoute::
	getRREQ_sent_by_this_discovery(void)
	throw(){
		return RREQ_sent_by_this_discovery_;
	}
	void
	infoDiscoveryRoute::
	setTipoLocalRepair (TipoLocalRepair unTipoLocalRepair)
	throw(){
		tipoLocalRepair_=unTipoLocalRepair;
	}
	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------
	// get set----------------------------------------------------------------------
	const unsigned int infoDiscoveryRoute:: getHops ( void ) const throw()
	{	return hops_;}
	// ----------------------------------------------------------------------
	const unsigned int infoDiscoveryRoute:: getRREQ_ID ( void ) const throw()
	{	return rreqId_;}
	// ----------------------------------------------------------------------
	const unsigned int infoDiscoveryRoute:: getOrigenSequNumb ( void ) const throw()
	{	return origenSequNumb_;}
	// ----------------------------------------------------------------------
	const unsigned int infoDiscoveryRoute:: getDestSequNumb ( void ) const throw()
	{	return destSequNumb_;}
	// ----------------------------------------------------------------------
	const bool infoDiscoveryRoute:: getUnknownSequNum ( void ) const throw()
	{	return UnknownSequNum_;}
	// ----------------------------------------------------------------------
	const bool infoDiscoveryRoute:: getDestOnlyFlag ( void ) const throw()
	{	return destOnlyFlag_;}
	// ----------------------------------------------------------------------
	const bool infoDiscoveryRoute:: getJoinFlag ( void ) const throw()
	{	return joinFlag_;}
	// ----------------------------------------------------------------------
	const bool infoDiscoveryRoute:: getRepairFlag ( void ) const throw()
	{	return repairFlag_;}
	// ----------------------------------------------------------------------
	const TipoLocalRepair infoDiscoveryRoute:: getTipoLocalRepair ( void ) const throw()
	{	return tipoLocalRepair_;}
	// ----------------------------------------------------------------------
	const bool infoDiscoveryRoute:: getRREP_gFlag ( void ) const throw()
	{	return rrep_gFlag_;}
	// ----------------------------------------------------------------------
	const std::string infoDiscoveryRoute:: getOrigen( void ) const /*throw()*/
	{
		assert(!origen_.empty());
		return origen_;
	}
	// ----------------------------------------------------------------------
	const std::string infoDiscoveryRoute:: getDestino( void ) const throw()
	{
		assert( !dest_.empty() );
		return dest_;
	}
	// ----------------------------------------------------------------------
	const unsigned int  infoDiscoveryRoute:: getTtl( void ) const throw()
	{
		return ttl_;
	}	
	// ----------------------------------------------------------------------
	shawn::EventScheduler::EventHandle  infoDiscoveryRoute:: getEventoTiempoEspera ( )  
	throw()
	{	return eventoTiempoEspera_;	}

	// metodos----------------------------------------------------------------------
	
	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------

// clase infoDiscoveryRoute----------------------------------------------------------------------
/*	ConfiguracionDiscovery::
	ConfiguracionDiscovery()
	{}*/
	// ----------------------------------------------------------------------
	ConfiguracionDiscovery::
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
							bool 		 useDestOnly )
	//inicializacion
	:
							useERS_ 				{useERS},
							NET_TRAVERSAL_TIME_ 	{NET_TRAVERSAL_TIME},
							TTL_THRESHOLD_			{TTL_THRESHOLD},
							TTL_INCREMENT_ 			{TTL_INCREMENT},
							NET_DIAMETER_		 	{NET_DIAMETER},
							RREQ_RETRIES_		 	{RREQ_RETRIES},
							RREQ_RATELIMIT_		 	{RREQ_RATELIMIT},
							LOCAL_ADD_TTL_		 	{LOCAL_ADD_TTL},
							TTL_START_		 		{TTL_START},
							TIMEOUT_BUFFER_			{TIMEOUT_BUFFER},
							NODE_TRAVERSAL_TIME_	{NODE_TRAVERSAL_TIME},
							useGratuitousRREP_		{useGratuitousRREP},
							useDestOnly_			{useDestOnly}

	{}

	// ----------------------------------------------------------------------
	ConfiguracionDiscovery::
	~ConfiguracionDiscovery() {}

	// get set----------------------------------------------------------------------

	double
	ConfiguracionDiscovery::
	getNET_TRAVERSAL_TIME(void) const	throw()
	{
		return NET_TRAVERSAL_TIME_;
	}
	// ----------------------------------------------------------------------
	unsigned int
	ConfiguracionDiscovery::
	getTTL_THRESHOLD(void) const	throw()
	{
		return TTL_THRESHOLD_;
	}
	// ----------------------------------------------------------------------
	unsigned int
	ConfiguracionDiscovery::
	getTTL_INCREMENT(void) const	throw()
	{
		return TTL_INCREMENT_;
	}
	// ----------------------------------------------------------------------
	bool
	ConfiguracionDiscovery::
	getUseERS(void) const	throw()
	{
		return useERS_;
	}
	// ----------------------------------------------------------------------
	bool
	ConfiguracionDiscovery::
	getUseGratuitousRREP(void) const	throw()
	{
		return useGratuitousRREP_;
	}
	// ----------------------------------------------------------------------
	bool
	ConfiguracionDiscovery::
	getUseDestOnly(void) const	throw()
	{
		return useDestOnly_;
	}
	// ----------------------------------------------------------------------
	unsigned int
	ConfiguracionDiscovery::
	getNET_DIAMETER(void) const	throw()
	{
		return NET_DIAMETER_;
	}
	// ----------------------------------------------------------------------
	unsigned int
	ConfiguracionDiscovery::
	getRREQ_RETRIES(void) const	throw()
	{
		return RREQ_RETRIES_;
	}
	// ----------------------------------------------------------------------
	unsigned int
	ConfiguracionDiscovery::
	getRREQ_RATELIMIT(void) const	throw()
	{
		return RREQ_RATELIMIT_;
	}
	// ----------------------------------------------------------------------
	unsigned int
	ConfiguracionDiscovery::
	getLOCAL_ADD_TTL(void)const throw(){
		return LOCAL_ADD_TTL_ ;
	}
	// ----------------------------------------------------------------------
	unsigned int ConfiguracionDiscovery::getTTL_START(void) const throw()
	{	return TTL_START_ ;	}
	// metodos----------------------------------------------------------------------
	double
	ConfiguracionDiscovery::
	calcularRING_TRAVERSAL_TIME (unsigned int ttlActual) const	throw()
	{
		  
		  std::cout<< "----------------------    2 * " <<NODE_TRAVERSAL_TIME_<<"* ( "<<ttlActual<<" + "<<
				  TIMEOUT_BUFFER_<<" )"<<
				  std::endl;
		return ( 2 * NODE_TRAVERSAL_TIME_ * ( ttlActual + TIMEOUT_BUFFER_ )  );
	}
	// ----------------------------------------------------------------------
	std::string
	ConfiguracionDiscovery::
	toString(void)
	throw()
	{
		{
			   std::stringstream ss;
			   ss <<
			     "useHelloMessages: "		<< 		  "" 	<< '\n'<<
			  	 "useERS: "					<< 			  useERS_		<<'\n'<<
			  	 "NET_DIAMETER: "			<<   NET_DIAMETER_			<<'\n'<<
			  	 "NODE_TRAVERSAL_TIME: "	<<   NODE_TRAVERSAL_TIME_	<<'\n'<<
			  	 "RING_TRAVERSAL_TIME: "    <<   ""	<<'\n'<<
			  	 "TIMEOUT_BUFFER: " 		<<   TIMEOUT_BUFFER_			<<'\n'<<
			  	 "TTL_INCREMENT: "			<<   TTL_INCREMENT_			<<'\n'<<
			  	 "TTL_THRESHOLD: "			<<   TTL_THRESHOLD_			<<'\n'<<

			  	 "NET_TRAVERSAL_TIME:"      <<  NET_TRAVERSAL_TIME_		<<'\n';
			   return (ss.str());
		}
	}

	// ----------------------------------------------------------------------
	void
	ConfiguracionDiscovery::
	show(void)
	throw()
	{
		std::cout << ConfiguracionDiscovery::toString()<< '\n';

	}
	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------
}
#endif
