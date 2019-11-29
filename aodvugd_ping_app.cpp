#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD

//#include "legacyapps/aodvugd/aodvugd_logger.h"
//#include "legacyapps/aodvugd/aodvugd_processor.h"
#include "legacyapps/aodvugd/aodvugd_ping_app.h"
#include "legacyapps/aodvugd/aodvugd_logger.h"
//#include "legacyapps/aodvugd/aodvugd_message.h"
//#include "sys/simulation/simulation_controller.h"
#include "sys/node.h"
#include <iostream>

#include <sstream>   /* toString */
#include <string> /*to_string(int)*/

#include <assert.h> //assert




namespace aodvugd
{
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// clase PingProgramadoEventTag----------------------------------------------------------------------
	PingProgramadoEventTag:: //constructor
	PingProgramadoEventTag( std::string destino ,  unsigned int pingID )
	:				//inicializacion
	destino_			{destino},
	pingID_ 			{pingID}
	{}
//----------------------------------------------------------------------
	//destructor
	PingProgramadoEventTag::~PingProgramadoEventTag(){}
	// get set---------------------------------------------------------------------
	inline std::string	PingProgramadoEventTag::getDestino  (void) const	throw()
	{		return destino_ ;	}
// ----------------------------------------------------------------------
	inline unsigned int	PingProgramadoEventTag::getPingID  (void) const	throw()
	{		return pingID_ ;	}
// ----------------------------------------------------------------------

// clase PingApp----------------------------------------------------------------------
	PingApp::
	PingApp()
	
	: 
		 pingID_ 						{ 0 }
	{}
// ----------------------------------------------------------------------
	PingApp:: 
	PingApp(  shawn::EventScheduler& pEventScheduler,
		      AodvUgdProcessor&  procesoAodv  )
	:
		//inicializacion

		pEventScheduler_ 				{ &pEventScheduler} ,
		procesoAodv_	 				{ &procesoAodv}     ,
		pingID_ 						{ 0 }

	{
		std::cout<< "pEventScheduler_PingApp : " << &pEventScheduler_ <<  std::endl;
	}
	// ----------------------------------------------------------------------
	PingApp::
	~PingApp()
	{}

// ----------------------------------------------------------------------
// get set----------------------------------------------------------------------
	void 
	PingApp::
	inicilizar( shawn::EventScheduler& pEventScheduler,
				AodvUgdProcessor&  procesoAodv )
      throw()	
	{
				//inicializacion

		pEventScheduler_ 				=  &pEventScheduler;
		procesoAodv_	 				=  &procesoAodv;
	}
// metodos----------------------------------------------------------------------
   void
   PingApp::
   special_boot( void ) //solo un nodo utiliza el special boot. 
      throw()
   {
   }
// ----------------------------------------------------------------------
   void
   PingApp::
   boot( void )
      throw()
   {
   	 std::cout<< "--------------------------boot PingApp " << owner().id()<< std::endl;                         
   }
// ----------------------------------------------------------------------
   bool
   PingApp::
   process_message(  const shawn::ConstMessageHandle& mh )
      throw()
   { 
      return false; 
   }
   // ----------------------------------------------------------------------
	void
	PingApp::
	timeout( shawn::EventScheduler&, shawn::EventScheduler::EventHandle unEventHandle,
	         double, shawn::EventScheduler::EventTagHandle& unEventTagHandle )
	      throw()
	   {

		  const PingProgramadoEventTag* unPingProgramadoEventTag =
				   dynamic_cast <const PingProgramadoEventTag*> ( unEventTagHandle.get() );

		  if ( unPingProgramadoEventTag != NULL)
		  	{  

			   std::cout<< "*************** TIMEOUT Llego un pedido de ping nodo: " <<	
			   owner().label()<<"  hacia: "<< unPingProgramadoEventTag->getDestino() <<
			   std::endl;


			   handle_EventPing ( *unPingProgramadoEventTag ); 
			   return;
			}	

	   }
   // ----------------------------------------------------------------------
   void
   PingApp::
   work( void )
      throw()
   {
      
   }
   // ----------------------------------------------------------------------
   void
   PingApp::
   handle_pingRequest( const PingMessage& unPingMessageRequest )
   throw()
	{
	//******************************  LOG   ******************************
		//se emula el ultimo salto porque como el mensaje que llega es constante no conviene
		//crear uno nuevo solo para disminuir el TTL.
		//TODO vmas adelante si hay que disminuir, 
		unsigned int TTL = unPingMessageRequest.getTtl()-1;

	 std::ostringstream mensaje;//log	
	 mensaje <<"PingApp Recibi Request ID: "<<unPingMessageRequest.getPingID() <<" SeqNum: "<< 
		   	     unPingMessageRequest.getSeqNum()<<" TTL: "<< TTL <<
		   		"  desde ipOrigen: " << unPingMessageRequest.getIpOrigen() ;

	 LoggerAodv::Instance()->logCsvPingDetalle( owner().label() /*nodo*/  ,
                             unPingMessageRequest.getIpOrigen()  /*origen*/,
			 				 unPingMessageRequest.getIpDestino() , "PingApp",
                             mensaje.str() );
	//***************************** FIN LOG ******************************

		//solo respondo por ahora
		enviarPingReply (      unPingMessageRequest.getIpOrigen() , 64 /*ttl*/ , 116/*size*/, 
		   					   unPingMessageRequest.getPingID() ,
				  			   unPingMessageRequest.getSeqNum()	);
	}
// ----------------------------------------------------------------------
   void
   PingApp::
   handle_pingReply( const PingMessage& unPingMessageReply )
   throw()
	{
		std::ostringstream mensaje;//log

		//se emula el ultimo salto porque como el mensaje que llega es constante no conviene
		//crear uno nuevo solo para disminuir el TTL.
		//TODO vmas adelante si hay que disminuir, 
		unsigned int TTL = unPingMessageReply.getTtl()-1;

		const unsigned int idPing   = unPingMessageReply.getPingID();
		  mensaje <<"Recibi Reply ID:"<<idPing<<" SeqNum:"<< 
			   	     unPingMessageReply.getSeqNum()<<" TTL: "<< TTL<<
			   		"  desde ipOrigen: " << unPingMessageReply.getIpOrigen() ;


		 //verifico si mande un ping al destino
		if ( existeIdPingEnviados (idPing) )
		{
			InfoPingEnviado *unaInfoPingEnviado  = devolverInfoPingEnviado (idPing) ; 	
		 //verifico si es la seq que espero
		 	if(unPingMessageReply.getSeqNum() == unaInfoPingEnviado->getSeqNum())
		 	{
		 	 //aviso que llego y sigo mandando
			 //****************  LOG   *****************
		 		double timeForReply = owner().world().current_time() - unaInfoPingEnviado->getLastSendTime();
			   mensaje <<" Time:" << timeForReply << " EXITO!";
			 //**************** FIN LOG ****************
			 //elimino evento timeout, como llego el reply no hay que esperar mas..
			    cancelarTimeoutEventPing( unaInfoPingEnviado->getEventoTiempoEspera() );
			 // envio mas pings si se puede
				intentarEnviarPing (unaInfoPingEnviado);
		 	}
		 	else
		 	{
			 //****************  LOG   *****************
			   mensaje<<" ERROR: otro SeqNum!!" ;
			 //**************** FIN LOG ****************
		 	}
		}
		else
		{
		 //llego un ping pero ya expiro el tiempo de espera	
			//****************  LOG   *****************
		   mensaje <<" ERROR: No espero reply!!" ;
		   //**************** FIN LOG ****************
		}	

	   LoggerAodv::Instance()->logCsvPingDetalle( owner().label() /*nodo*/  ,
                             unPingMessageReply.getIpOrigen()  /*origen*/,
			 				 unPingMessageReply.getIpDestino() , "PingApp",
                             mensaje.str() );
		
	}
	// ----------------------------------------------------------------------
	void
	PingApp::
	cancelarTimeoutEventPing( shawn::EventScheduler::EventHandle pEventHandle  )
	throw()					
	{
		owner_w().world_w().scheduler_w().delete_event ( pEventHandle );
	}

	// ----------------------------------------------------------------------
	InfoPingEnviado*
	PingApp::
	devolverInfoPingEnviado(  unsigned int idPing  )
	throw()					
	{
		TipoPingsEnviados::iterator it;
		it = pingsEnviados.find (idPing) ;
		return  it->second; 
	}
// ----------------------------------------------------------------------
	void
	PingApp::
	enviarPingRequest (std::string destino , unsigned int ttl    , 
		  			   int size	   		   , unsigned int pingID ,
		  			   unsigned int seqNum	) throw()
	{	
		PingMessage *unPingMessage	=  new PingMessage(
									       /*116*/ size , TipoICMP::EchoRequest,
   										   pingID       , seqNum ,
   										   /*64 */ ttl    		 ,
   				 						   destino /*ipDestino*/ , 
   				 						   owner().label() /*ipOrigen*/	 	);

   	//******************************  LOG   ******************************
	 std::ostringstream mensaje;//log	
	 mensaje <<"Envio Request ID:"<< unPingMessage->getPingID() <<" SeqNum:" << 
		   	     unPingMessage->getSeqNum()<<  " TTL:"<< unPingMessage->getTtl()  <<
		   		"  a ipDestino: "          << unPingMessage->getIpDestino() ;

	 LoggerAodv::Instance()->logCsvPingDetalle( owner().label() /*nodo*/  ,
                             unPingMessage->getIpOrigen()  /*origen*/,
			 				 unPingMessage->getIpDestino() , "PingApp",
                             mensaje.str() );
	//***************************** FIN LOG ******************************

		AodvUgdProcessor *procesoAodv =
				owner_w().get_processor_of_type_w <AodvUgdProcessor> ();

		procesoAodv->recibirPaqueteDesdeApp(*unPingMessage);
	}
	// ----------------------------------------------------------------------
	void
	PingApp::
	enviarPingReply (  std::string destino , unsigned int ttl    , 
		  			   int size	   		   , unsigned int pingID ,
		  			   unsigned int seqNum	) throw()
	{	
		PingMessage *unPingMessage	=  new PingMessage(
									       /*116*/ size , TipoICMP::EchoReply,
   										   pingID       , seqNum ,
   										   /*64 */ ttl    		 ,
   				 						   destino /*ipDestino*/ , 
   				 						   owner().label() /*ipOrigen*/	 	);

   	//******************************  LOG   ******************************
	 std::ostringstream mensaje;//log	
	 mensaje <<"Envio Reply ID:"  << unPingMessage->getPingID() <<" SeqNum:" << 
		   	     unPingMessage->getSeqNum()<<  " TTL: "<< unPingMessage->getTtl()  <<
		   		"  a ipDestino: "          << unPingMessage->getIpDestino() ;

	 LoggerAodv::Instance()->logCsvPingDetalle( owner().label() /*nodo*/  ,
                             unPingMessage->getIpOrigen()  /*origen*/,
			 				 unPingMessage->getIpDestino() , "PingApp",
                             mensaje.str() );
	//***************************** FIN LOG ******************************

		AodvUgdProcessor *procesoAodv =
				owner_w().get_processor_of_type_w <AodvUgdProcessor> ();

		procesoAodv->recibirPaqueteDesdeApp(*unPingMessage);
	}
	// ----------------------------------------------------------------------
	bool
	PingApp::
	existeIdPingEnviados ( unsigned int pingID	) throw()
	{
		TipoPingsEnviados::iterator it;
		it = pingsEnviados.find (pingID) ;
		 if ( it != pingsEnviados.end() )
		 {
			 return true;
		 }
		 return false;
	}
// ----------------------------------------------------------------------
	shawn::EventScheduler::EventHandle
	PingApp::
	generarEventoTiempoEspera ( double tiempoDeEspera , std::string destino ,
								unsigned int pingID  ) 
	throw()
	{
		shawn::EventScheduler& pEventScheduler = owner_w().world_w().scheduler_w();
	    double tiempoActual	=	pEventScheduler.current_time() ;														
		double tiempoEvento	=	tiempoActual + tiempoDeEspera ;

		PingProgramadoEventTag* unPingProgramadoEventTag =new PingProgramadoEventTag( 
																 destino , pingID ) ;
		shawn::EventScheduler::EventHandle  pEventHandle= pEventScheduler.new_event  (
											  *this , tiempoEvento , unPingProgramadoEventTag );
		return pEventHandle;
	}
// ----------------------------------------------------------------------
	void
	PingApp::
	recibirMensajePing ( const PingMessage& unPingMessage  ) throw()
	{	
		//se recibio un mensaje ping
		if (TipoICMP::EchoRequest == unPingMessage.getUnTipoICMP() )
		{
			handle_pingRequest (unPingMessage) ;
		}
		else if(TipoICMP::EchoReply == unPingMessage.getUnTipoICMP() )
		{
			handle_pingReply (unPingMessage) ;
		}
	}
// ----------------------------------------------------------------------
	void
	PingApp::
	handle_EventPing ( const PingProgramadoEventTag& unPingProgramadoEventTag  ) throw()
	{	
		//enviar ping
		std::string  destino 		= unPingProgramadoEventTag.getDestino();
		unsigned int idPingEvento   = unPingProgramadoEventTag.getPingID();
		InfoPingEnviado *unaInfoPingEnviado  = devolverInfoPingEnviado (idPingEvento) ; 

	   
		if(unaInfoPingEnviado->getSeqNum() != -1 )
		{
		   //****************  LOG   *****************
		   std::ostringstream mensaje;
		   mensaje <<"Unreachable timeout sin recibir Ping Reply ID:"<<idPingEvento <<
		   			  " SeqNum:" << unaInfoPingEnviado->getSeqNum();
		   LoggerAodv::Instance()->logCsvPingDetalle( owner().label() /*nodo*/,
		                                 owner().label() /*origen*/,destino , "PingApp",
		                                 mensaje.str() );
		   //**************** FIN LOG ****************
		}
		else
		{
		  //primer ping
		   //****************  LOG   *****************
		   std::ostringstream mensaje;
		   mensaje <<"primer Ping Request ID:"<<idPingEvento <<
		   			 " SeqNum:" << unaInfoPingEnviado->getSeqNum();
		   LoggerAodv::Instance()->logCsvPingDetalle( owner().label() /*nodo*/,
		                                 owner().label() /*origen*/,destino , "PingApp",
		                                 mensaje.str() );
		   //**************** FIN LOG ****************
		}
	  intentarEnviarPing (unaInfoPingEnviado);
	}
// ----------------------------------------------------------------------
    void	
	PingApp::
	intentarEnviarPing ( InfoPingEnviado *unaInfoPingEnviado  ) 
	throw()						
	{

		//verifico si se puede seguir enviando
        //si el ping es infinito o no se alcanzo los envios maximos.
		 if(  (unaInfoPingEnviado->getPingInfinito() )
        	|| (unaInfoPingEnviado-> getMax() > unaInfoPingEnviado->getSeqNum() )  )
        {
			if(unaInfoPingEnviado->getSeqNum() > 0)
        	{
        		//es un ping sequenciado..
        	}
        	else
        	{
        		//es el primer ping
        		//log
        	} 	
        	//genero el evento  por la espera de una repuesta al ping
			shawn::EventScheduler::EventHandle  pEventoTiempoEspera  = generarEventoTiempoEspera ( 
									 				 unaInfoPingEnviado->getTimeoutPing() ,
									 				 unaInfoPingEnviado->getDestino() , 
									 				 unaInfoPingEnviado->getPingID() ); 

			
			//actualiza el seq++ , asigna el tiempo de espera nuevo y el tiempo de envio del ultimo ping
        	unaInfoPingEnviado-> actualizarSeEnvioPing( owner().world().current_time() , pEventoTiempoEspera );

			enviarPingRequest( unaInfoPingEnviado->getDestino()  , unaInfoPingEnviado->getTtl()   ,
							   unaInfoPingEnviado->getSize   ()  , unaInfoPingEnviado->getPingID(),
							   unaInfoPingEnviado->getSeqNum()   );
			
        }
        else
        {

        	//se alcanzo la cantidad de envios de pings
        	//no se crean mas eventos
           //****************  LOG   *****************
		   std::ostringstream mensaje;
		   mensaje <<"Se finaliza el ping ID:"<<unaInfoPingEnviado->getPingID() ;
		   LoggerAodv::Instance()->logCsvPingDetalle( owner().label() /*nodo*/  ,
		                                 owner().label()  /*origen*/,
		   			 					 unaInfoPingEnviado->getDestino() , "PingApp",
		                                 mensaje.str() );
		   //**************** FIN LOG ****************

		   	//se cancela el ping!
		    //el evento se disparo o se cancelo antes!
        	//se borra de los pings enviados
        	pingsEnviados.erase ( unaInfoPingEnviado->getPingID() );
        }
	}

// ----------------------------------------------------------------------
    void	
	PingApp::
	programarPrimerPing ( std::string destino     , unsigned int maxPings , 
						  bool pingInfinito       , double timeoutPing   ,
						  unsigned int ttl        , int size		      ,
						  double tiempoInicioPing   ) 
	throw()						
	{
		
		//se programa el 1er ping
		shawn::EventScheduler::EventHandle  pEventHandle  = generarEventoTiempoEspera ( 
											 				 tiempoInicioPing ,  destino , pingID_); 

		InfoPingEnviado * unInfoPingEnviado = new InfoPingEnviado(    maxPings , pingID_ ,
												     pingInfinito  ,  destino  , 
												     pEventHandle  ,  ttl	   ,
												     size		   ,  timeoutPing); 

		pingsEnviados.insert ({ pingID_ , unInfoPingEnviado });
		pingID_++;
	}
// ----------------------------------------------------------------------
// clase InfoPingEnviado----------------------------------------------------------------------
/*	InfoPingEnviado::
	InfoPingEnviado()
	{}*/
	// ----------------------------------------------------------------------
	InfoPingEnviado::
	InfoPingEnviado (     unsigned int  max   , unsigned int pingID  ,
						  bool pingInfinito   , std::string  destino ,
						  shawn::EventScheduler::EventHandle eventoTiempoEspera ,
						  unsigned int ttl    , int size , double timeoutPing )
	//inicializacion
	:
			max_ 			    {max},
	      	seqNum_ 		    {-1},
	     	pingInfinito_ 	    {pingInfinito},
	      	destino_ 		    {destino},
	      	eventoTiempoEspera_ {eventoTiempoEspera},
	      	pingID_ 			{pingID},
	      	ttl_ 			    {ttl},
	      	size_ 				{size},
	      	timeoutPing_ 		{timeoutPing},

	      	//cuando se crea la info el primer evento determina cuando comienza el ping
	      	//entonces se puede usar para guardar el tiempo que se comienza el ping
	      	startSendTime_ 		{eventoTiempoEspera->time()}
			{}

	// ----------------------------------------------------------------------
	InfoPingEnviado::
	~InfoPingEnviado() {}
	// get set----------------------------------------------------------------------
	const  int InfoPingEnviado:: getMax      	 ( void ) const throw()
	{	return max_;}
	// ----------------------------------------------------------------------
	const unsigned int InfoPingEnviado:: getPingID       ( void ) const throw()
	{	return pingID_;}
	// ----------------------------------------------------------------------
	const unsigned int InfoPingEnviado:: getTtl      	 ( void ) const throw()
	{	return ttl_;}
	// ----------------------------------------------------------------------
	const int 		   InfoPingEnviado:: getSize      	 ( void ) const throw()
	{	return size_;}
	// ----------------------------------------------------------------------
	const double 	   InfoPingEnviado:: getTimeoutPing  ( void ) const throw()
	{	return timeoutPing_;}
	// ----------------------------------------------------------------------
	const double 	   InfoPingEnviado:: getLastSendTime ( void ) const throw()
	{	return lastSendTime_;}
	// ----------------------------------------------------------------------
	const double 	   InfoPingEnviado:: getStartSendTime( void ) const throw()
	{	return startSendTime_;}
	// ----------------------------------------------------------------------
	const int InfoPingEnviado:: getSeqNum 	 		    ( void ) const throw()
	{	return seqNum_;}
	// ----------------------------------------------------------------------
	const bool 		  InfoPingEnviado:: getPingInfinito  ( void ) const throw()
	{	return pingInfinito_;}
	// ----------------------------------------------------------------------
	const std::string InfoPingEnviado:: getDestino 		 ( void ) const throw()
	{	return destino_;}
	// ----------------------------------------------------------------------
	shawn::EventScheduler::EventHandle  InfoPingEnviado:: getEventoTiempoEspera ( )  
	throw()
	{	return eventoTiempoEspera_;	}
	// metodos----------------------------------------------------------------------
	// ----------------------------------------------------------------------
	void InfoPingEnviado:: actualizarSeEnvioPing (  double lastSendTime  ,
							shawn::EventScheduler::EventHandle eventoTiempoEspera )  
																	throw()
	{
		eventoTiempoEspera_ = 	eventoTiempoEspera;
		lastSendTime_		=   lastSendTime;
		seqNum_++;
	}
	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------





}
#endif
