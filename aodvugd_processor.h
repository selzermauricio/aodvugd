
#ifndef __SHAWN_LEGACYAPPS_AODVUGD_PROCESSOR_H
#define __SHAWN_LEGACYAPPS_AODVUGD_PROCESSOR_H
#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD

#include <map>
#include <limits>
#include "sys/processor.h"
#include "sys/event_scheduler.h"
#include "legacyapps/aodvugd/aodvugd_message.h"
#include "legacyapps/aodvugd/aodvugd_observer_estado_ruta.h"

#include "sys/simulation/simulation_controller.h"
#include "legacyapps/aodvugd/aodvugd_defaultconfig.h"
#include "legacyapps/aodvugd/aodvugd_routing_table.h"
#include "legacyapps/aodvugd/aodvugd_route_discovery.h"
#include "legacyapps/aodvugd/aodvugd_processorhello.h"
#include "legacyapps/aodvugd/aodvugd_types.h" //tipos definidos
#include "sys/tag.h" //tag


namespace aodvugd
{
//using mapTypeDestinosInvalidos = std::map <const std::string , const unsigned int> ;
//using parDestinosInvalidos     = std::pair<const std::string , const unsigned int> ; 


inline const std::string BROADCAST_ADDRESS = "broadcast";

enum class TipoLocalRepair 
    { None=0 , PacketSinRutaActivaParaReenviar=1 , LinkBreak=2 };


// --------------------------------------------------------------------
// --------------------------------------------------------------------
// class tagRREQ_stat--------------------------------------------------------------------
class tagRREQ_stat : public shawn::Tag
{
public:
	//constructor
	tagRREQ_stat( );
	//----------------------------------------------------------------------
	//destructor
	virtual ~tagRREQ_stat();
	// get set----------------------------------------------------------------------
	inline unsigned int getContadorRenviosRREQ () throw();
	inline unsigned int getContadorHandle_RREQ () throw();
	inline void setContadorRenviosRREQ (unsigned int newContador) throw();
	inline void setContadorHandle_RREQ (unsigned int newContador) throw();
	// ----------------------------------------------------------------------
    virtual std::string encoded_content( void ) const throw( std::runtime_error  );
    virtual const std::string& type_identifier( void ) const throw();

private:
	unsigned int contadorRenviosRREQ_ = 0;
	unsigned int contadorHandle_RREQ_ = 0;
	unsigned int contadorPropios_RREQ_ = 0;
};
class AodvUgdRouteDiscovery;

// --------------------------------------------------------------------
// --------------------------------------------------------------------
// class AodvUgdProcessor--------------------------------------------------------------------
   /**
    * la clase hereda desde  Processor y EventHandler con accesso publico
    * lo que significa que va heredar los miembros tal cual como estan definidos en la clase madre
    */
   class AodvUgdProcessor
       : public shawn::Processor, 
         public shawn::EventScheduler::EventHandler,
         public ObserverEstadoRuta
   {
   public:
      ///@name Constructor/Destructor
      ///@{
      AodvUgdProcessor();
      virtual ~AodvUgdProcessor();
      ///@}
      friend class AodvUgdRouteDiscovery;
      ///@name Inherited from Processor
      ///@{
      /**
       */
      virtual void special_boot( void ) throw();
      /**
       */
      virtual void boot( void ) throw();\
      /**
       */
      virtual bool process_message( const shawn::ConstMessageHandle& ) throw();
      /**
       */
      virtual void work( void ) throw();
      ///@}
      
      ///@name Inherited from EventScheduler::EventHandler
      ///@{
      /**
       */
      virtual void timeout( shawn::EventScheduler&, 
                            shawn::EventScheduler::EventHandle, 
                            double, 
                            shawn::EventScheduler::EventTagHandle& ) throw();
      ///@}
      //el subProceso discovery se comunica con el proceso principal para reenviar RREQ
	  void	 renviarRRRQ_SubProcesoDiscovery (  const AodvUgdRREQ& pRREQ ) throw();
    void   sendRREP  ( std::string dest , std::string origen , unsigned int destSequNumb ,
                       bool ackFlag     , bool repairFlag    , unsigned int hops , 
                       double lifeTime  , unsigned int ttl   , std::string  labelNodoNextHop  )   
                       throw();
      //discovery avisa qdestino no encontrada
    void handleNotFindRoute (std::string destino)throw();

    void helloInformaDetectesLinkBreak ( std::string destinoInvalido )
                                                                        throw();

    void   aumentarContadorNodoSeqNum ()  throw();
    //get set
    unsigned int getContadorNodoSeqNum ()  throw();

    //typeDef
    typedef std::pair <unsigned int, std::string> par_Id_y_Direccion;

    const std::map< par_Id_y_Direccion , shawn::EventScheduler::EventHandle> 
    getBufferRREQ() const throw();
    const unsigned int getNodoSeqNum ( void ) const throw();

        //sirve de interfaz para que usen las app
    void recibirPaqueteDesdeApp (AodvUgdIpHeader& unPaqueteIp)          throw();


   private:
            
      // metodos privados--------------------------------------------------------------------
    //unsigned int getNodoSeqNum() const throw();
    
    void handle_RREP(  const AodvUgdRREP& rrep )   throw(); 
    void handle_RREQ(  const AodvUgdRREQ& rreq )  throw();
    bool verificarIntermedioRespondeRREQ( std::string dest, unsigned int rreqDesSeqNum,
    		  	  	  	  	  	  	  	  	bool rreqDestOnlyFlag ) throw();

   // bool armarRepuestaRrepDestino	( shawn::Node& dest , shawn::Node& origen ) throw();

    void respuestaDescubrimientoRREP_Destino 	( std::string direccionDestino , 
                                                std::string direccionOrigen ,
    		  	  	  	  	  	  	                unsigned int rreqDestSeqNum ,
                                                std::string source /*test*/) throw();

    void respuestaDescubrimientoRREP_Intermedio	( std::string direccionDestino , 
                                                  std::string direccionOrigen ,
    		  	  	  	  	  	  	                  std::string direccionSource ) throw();

    
    //RREQ
    void reenviarRREQ_Intermedio(const AodvUgdRREQ &rreq) throw();
    //se arma un nuevo RREQ para reenviar, se suma el HOP y se resta el TTL
    AodvUgdRREQ* armarRREQ_Reenvio  (const AodvUgdRREQ &rreq , unsigned int  newDestSeqNum)   
                                    throw();

    //registrarRuta SOURCE
    bool verificarActualizarTablaSource  ( std::string source ) throw ();
    void registrarRutaSource  ( std::string source ) throw ();

    //registrarRuta RREQ
	  void registrarRutaOrigenRREQ ( const AodvUgdRREQ &rreq ) throw ();
    bool verificarActualizarTablaOrigenRREQ( std::string direccion ,
                                             unsigned int mensajeDestSeqNum ,
                                             unsigned int mensajeHops) throw ();

    //registrarRuta RREP
    void registrarRutaDestinoRREP  ( const AodvUgdRREP &rrep ) throw ();
    bool verificarActualizaTablaDestinoRREP( std::string  direccion , 
                                             unsigned int mensajeDestSeqNum ,
                                             unsigned int mensajeHops) throw();

    void reenviarRREP_Intermedio_a_Origen( const AodvUgdRREP &rrep)
                                           throw();
    
	  void enviarRREP_G_Intermedio( std::string rreqDestino , std::string rreqOrigen ,
				  	  	  	  	  unsigned int rreqOrigenSeqNum ) throw();
    //buffer RREQ
	  void agregarRREQ_Buffer ( unsigned int RREQ_ID , std::string origen ) throw();
	  bool existeRREQ_Buffer  ( unsigned int RREQ_ID , std::string origen ) throw();
    void handle_EventExpireTimeBufferRREQ( unsigned int RREQ_ID , std::string origen ) 
                                          throw();


	  void  crearConfiguracionAodv                     ( ) throw();
	  void  crearProcesoDiscovrey                      ( ) throw();
    void  crearProcesoHello                          ( ) throw();
    void  crearRouteTable                            ( ) throw();
	  //tag
	  void iniciarTagRREQ_stat                         ( ) throw();
	  tagRREQ_stat* devolverTagRREQ_stat               ( ) throw();
	  void sumarContadorStatGlobalRREQ_Renviados       ( ) throw();
	  void sumarContadorStatGlobalRREQ_Handle          ( ) throw();
    unsigned int getContadorStatGlobalRREQ_Renviados ( ) throw();
    unsigned int getContadorStatGlobalRREQ_Handle    ( ) throw();

    bool         isHelloMessage  ( AodvUgdRREP &unRREP ) throw();
    //bool puedoProcesarRREQ () throw ()://es handleable?
	

  /***********************     HANDLE RERR (LocalRepair)   ******************/
  /*se emaneja la llegada de un RERR*/
    void handle_RERR ( const AodvUgdRERR& rerr)                      throw();

    bool localRepair ( std::string destinoInvalido , std::string sender ,
                     TipoLocalRepair unTipoLocalRepair  )            throw();
    
    unsigned int calcularTtlInicialLocalRepair ( std::string  destino , std::string sender)
                                                                     throw();

  /*se encarga de manejar el caso de datos a reenviar sin ruta, verifica si se 
    puede hacer un local repair antes de enviar el RERR*/
    void handeleDataPacketSinRutaActivaParaReenviar ( std::string  destinoInvalido , 
                                                      std::string  sender )
                                                                        throw();
  
  /*envia RERR en el caso de recibir paquete a reenviar y no se tiene una ruta activa*/
    void rerrDataPacketSinRutaActivaParaReenviar ( std::string destinoInvalido)
                                                                        throw();

  /*se encarga de manejar el caso de linkBreak, verifica si se 
    puede hacer un local repair antes de enviar el RERR*/
    void handeleDetectesLinkBreakFromNextHop ( std::string destinoInvalido )
                                                                        throw();

  /*envia RERR en el caso de detectar LinkBreak*/
    void rerrDetectesLinkBreak ( std::string destinoInvalido)           throw();

  /*envia RERR en el caso que llega un RERR desde un vecino */
    void reenviarRERR ( const mapTypeDestinosInvalidos *mapDestinosInvalidosRERR ,
                        std::string transmitterRERR )
                                                                        throw();                                                                    
    void sendRERR ( bool noDeleteFlag ,   
                    mapTypeDestinosInvalidos *mapMisDestinosInvalidosRERR ,
                    std::string labelNodoNextHop  )                     throw();

  /*verifica si se envia el RERR por broadcast/unicast, depende de si hay
    precursores en los destinos invalidos*/
    bool verificarEnviarBroadcastRERR ( mapTypeDestinosInvalidos &mapMisDestinosInvalidosRERR)
                                                                        throw(); 
    
    std::string devolverUnPrecursorEnviarUnicast ( const mapTypeDestinosInvalidos *mapDestinosInvalidos)
                                                                        throw();                                                                     
  /*********************** FIN HANDLE RERR (LocalRepair)   ******************/
 
  /***********************            Routing              ******************/
    void  routePacketForward (  const AodvUgdIpHeader& unPaqueteIp )          throw();
    void  sendMyOwnDataPacket(  AodvUgdIpHeader& unPaqueteIp )          throw();
    void  llamarProcesoRouteDiscovery ( std::string destino  )          throw();
    //se escpecifica el destino y el mensaje a enviar, asi el proceso busca el 
    //nextHop y envia el mensaje
    //importante! en esta funcion se actualiza la ruta cada vez que se envia un mensaje
    void  sendToNextHop      ( AodvUgdIpHeader& unPaqueteIp  )          throw();
    void  sendPaqueteIP      ( AodvUgdIpHeader& unPaqueteIp  )          throw();
    void  enviarPaqutesPendientesDestino ( std::string destino )        throw();
    void  dropPaquetesPendientesDestino ( std::string destino )        throw();
    
    void  actualizarPorUsarCamino ( std::string source  , std::string destino , 
                                    std::string nextHop , std::string previousHop )  
                                                                        throw();

    void handlePaqueteIp     (const AodvUgdIpHeader& unPaqueteIp)             throw();
    void pasarPaqueteIpToApp (const AodvUgdIpHeader& unPaqueteIp)             throw();
    AodvUgdIpHeader* clonarUnPaqueteIP   (const AodvUgdIpHeader& unPaqueteIp)             throw();




  /***********************        FIN   Routing            ******************/

  /***************************** AvisoEstadoRuta (observe)   ******************************/
      //hay que redefinir el metodo
      //maneja el aviso de una ruta de interes esta activa

      //onEstadoRutaChange ()
      //esta definido en la clase abstracta ObserverEstadoRuta donde
      //se llama al diferente handle dependiendo el estado de la ruta
      virtual void handleAvisoRutaActiva (std::string destino)   throw();
      virtual void observarUnaRuta    (  std::string destino, std::string estado )throw();
      virtual void cancelarObservarUnaRuta    (  std::string destino, std::string estado )throw();

      //se elimina el discovery y los eventos relacionados
      void cancelarDiscovery     (std::string destino)     throw();
      //se cancela el evento del tiempo de espera
      void cancelarEventoTiempoEspera(  infoDiscoveryRoute &pUnaInfoDiscoveryRoute  )
                                           throw(); 

      //el proceso general se encarga de agregar a los observadores
      void agregarObservadorTabla  (  ObserverEstadoRuta * unObserverEstadoRuta, 
                                      std::string destino , std::string estado)    throw();
      void quitarObservadorTabla  (  ObserverEstadoRuta * unObserverEstadoRuta, 
                                      std::string destino , std::string estado)    throw();
  /***************************** FIN AvisoEstadoRuta (observer)   ******************************/
    void sendRREQ  ( AodvUgdRREQ* pRREQ) throw() ;
	  

    //test! only for test
    void testingRuteDescovery( ) throw();
	  void testingRuteDescoveryFromNodeToNode( int idOrigen , int idDestino )throw();
    // --------------------------------------------------------------------



      /// defines whether a node is connected to the routing tree (and thus
      /// knows a valid predecessor)
      //bool connected_;
      /// just set to true when 'special node' 
     // bool senderPing_;
      /// current hop distance to sink
      //int hop_dist_;
      /** Used by ordinary nodes to send actual values like temperature or 
       *  hop_count. Set parameter \c state_period. 
       * 
       *  On gate, it defines the refreshing interval for flooding the 
       *  network. Set parameter \c flood_period.
       */
     //int send_period_;
      /// Used for routing messages to sink. They are always sent/forwarded
      /// to the predecessor (if not null)
     // const shawn::Node* predecessor_;
      /// Storing at gate: nodes and their hopcount
      //std::map<const shawn::Node*, int> network_nodes_;
      /// Storing return value when registering an event
      //shawn::EventScheduler::EventHandle event_handle_;


      // --------------------------------------------------------------------
      unsigned int nodoSeqNum_;

      AodvUgdDefaultConfig configuracion;
      AodvUgdRoutingTable* unaTablaRuteo;
      AodvUgdRouteDiscovery* procesoDiscovrey; //subProceso
      AodvUgdProcessorHello *pAodvUgdProcessorHello;


      //log
      void logCsvRREQ( const AodvUgdRREQ& rreq, std::string detalle ) throw(); 
      void logCsvRREQ_columnas( ) throw() ;
      //testpto
      std::string testDestino;
      std::string tagContadorRenviosRREQ=tagContadorRenviosRREQ;
      AodvUgdRREQ * pTestRREQ;
  	  //maps
      std::map< par_Id_y_Direccion , shawn::EventScheduler::EventHandle> bufferRREQ;
      typedef std::list<shawn::ConstMessageHandle> MessageHistoryList;
                        //destino           //mensaje

      //std::list < std::pair<std::string, std::string> > _container;
      typedef std::pair<std::string, AodvUgdIpHeader& > ParSetMensajesPendientes;
      typedef std::list< ParSetMensajesPendientes > TipoSetMensajesPendientes; 
      TipoSetMensajesPendientes bufferMensajesPendientes;

      // --------------------------------------------------------------------
      
   };
   // --------------------------------------------------------------------
   // --------------------------------------------------------------------
   // class BufferRREQ_EventTag--------------------------------------------------------------------
	class BufferRREQ_EventTag : public shawn::EventScheduler::EventTag
	{
	public:
		//constructor
		BufferRREQ_EventTag( std::string origino, unsigned int RREQ_ID );
		//----------------------------------------------------------------------
		//destructor
		virtual ~BufferRREQ_EventTag();
		// ----------------------------------------------------------------------
		inline const std::string getOrigino () const throw();
		inline const unsigned int getRREQ_ID () const	throw();
		// ----------------------------------------------------------------------

	private:
		const std::string origino_;
		const unsigned int RREQ_ID_;
	};

}

#endif
#endif






