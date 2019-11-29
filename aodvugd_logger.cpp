#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD


#include "legacyapps/aodvugd/aodvugd_logger.h"
//#include "legacyapps/aodvugd/aodvugd_message.h"
//#include "sys/node.h"  //si o si tiene que estar
//#include "sys/world.h"
#include <iostream>
//#include "legacyapps/aodvugd/aodvugd_processor.h"


#include <sstream> //tostring
#include <stddef.h>  // defines NULL


namespace aodvugd
{
// Global static pointer used to ensure a single instance of the class.
LoggerAodv* LoggerAodv::m_pInstance = NULL;  

//clase LoggerAodv----------------------------------------------------------------------
	LoggerAodv::	//constructor vacio
	LoggerAodv( )
	{
  logRREQ_Movimientos = log4cxx::Logger::getLogger("rreq_movimientos");  
  logRREQ_Mensajes    = log4cxx::Logger::getLogger("rreq_mensajes");
  logRREQ_Buffer      = log4cxx::Logger::getLogger("rreq_buffer");
  logRREP_Movimientos = log4cxx::Logger::getLogger("rrep_movimientos");
  logDiscovery        = log4cxx::Logger::getLogger("discovery");
  logPing             = log4cxx::Logger::getLogger("ping");


  logTabla            = log4cxx::Logger::getLogger("routing_table");
  
  //RREQ
  logCsvRREQ_columnas( );
  logCsvRREQ_Mensaje_columnas();
  logCsvRREQ_Buffer_columnas();
  
  //RREP
  logCsvRREP_Movimientos_columnas();
  
  //Discovery
  logCsvPing_columnas();
  }

// ----------------------------------------------------------------------
	// LoggerAodv::	//destructor
	// ~LoggerAodv()
	// {}

/** This function is called to create an instance of the class. 
    Calling the constructor publicly is not allowed. The constructor 
    is private and is only called by this Instance function.
*/  
LoggerAodv* LoggerAodv::Instance()
{
   if (!m_pInstance)   // Only allow one instance of class to be generated.
      m_pInstance = new LoggerAodv;
   return m_pInstance;
} 
// ----------------------------------------------------------------------
 void LoggerAodv::setWorld ( const shawn::World& world) {
  world_= &world;
}
// metodos----------------------------------------------------------------------
//----------------------------------------------------------------------
   void
   LoggerAodv::
   logCsvRREQ( const AodvUgdRREQ& rreq,const shawn::Node& unNodo ,std::string detalle ) throw()  {
           std::ostringstream mensaje;
           std::string source=" ---- ";
           if (rreq.has_source() )
           {
             source=rreq.source().label();
           }
           mensaje << unNodo.current_time()  <<","<< //tiempo
                      unNodo.label()         <<","<< //nodo
                      source                 <<","<< //source
                      rreq.getOrigen()       <<","<< //origen
                      " -->> "               <<","<< //flecha
                      rreq.getDestino()         <<","<< //dest
                      detalle                <<","<<
                      rreq.getTtl()          <<","<<
                      &rreq                   ;

           LOG4CXX_DEBUG ( logRREQ_Movimientos , mensaje.str() );
   }
//----------------------------------------------------------------------
      void
   LoggerAodv:: 
   logCsvRREQ( const AodvUgdRREQ& rreq,std::string nodo ,std::string detalle ) throw()  {
           std::ostringstream mensaje;
          std::string source=" ---- ";
           if (rreq.has_source() )
           {
             source=rreq.source().label();
           }
            mensaje << world_->current_time() <<","<< //tiempo
                      nodo                   <<","<< //nodo
                      source                 <<","<< //source
                      rreq.getOrigen()       <<","<< //origen
                      " -->> "               <<","<< //flecha
                      rreq.getDestino()         <<","<< //dest
                      detalle                <<","<<
                      rreq.getTtl()          <<","<<
                      &rreq                   ;

           LOG4CXX_DEBUG ( logRREQ_Movimientos , mensaje.str() );
   }
//----------------------------------------------------------------------
   void
   LoggerAodv::
   logCsvRREQ_columnas( ) throw()  {
           std::ostringstream mensaje;

           mensaje << "Tiempo"  <<","<< //tiempo
                      "Nodo"    <<","<< //nodo
                      "Source"  <<","<< //source
                      "Origen"  <<","<< //origen
                      "-->>"    <<","<< //origen
                      "Destino" <<","<< //dest
                      "Detalle" <<","<<
                      "TTL"     <<","<<
                      "Memoria"    ;
           LOG4CXX_DEBUG ( logRREQ_Movimientos , mensaje.str() );
   }
//----------------------------------------------------------------------
//----------------------------------------------------------------------
   void
   LoggerAodv::
   logCsvRREQ_Mensaje_columnas( ) throw()  {
           std::ostringstream mensaje;
        mensaje <<"t" << "," << "Detalle" << "," <<  "dest"  <<  "," <<  "origen"  
                <<"," <<  "destSeqNum"  <<","
                <<  "origenSeqNum"  <<  "," <<  "ttl" <<  "," 
                <<  "hops"  <<  "," <<  "rreqId"  <<  "," 
                <<  "joinF"  <<  "," <<  "repairF"  <<  "," 
                <<  "rrep_gF"  <<  "," <<  "destOnlyF"  
                <<  "," <<  "UnknownSequNum"  <<  "," <<  "Memoria";
           LOG4CXX_DEBUG ( logRREQ_Mensajes, mensaje.str() );
   }
   //----------------------------------------------------------------------
   void
   LoggerAodv::
   logCsvRREQ_Buffer_columnas ( ) throw()  {
           std::ostringstream mensaje;
        mensaje << "nodo" << "," <<  "origen"  <<  "," <<  "RREQ_ID" <<  "," <<  "Programado(t)" ;
           LOG4CXX_DEBUG ( logRREQ_Buffer, mensaje.str() );
   }
   //----------------------------------------------------------------------
   void
   LoggerAodv::
   logCsvRREQ_Buffer_Renglon (std::string nodo, std::string origen , unsigned int RREQ_ID, 
                               double tiempProgramado) throw()  {
           std::ostringstream mensaje;
        mensaje  << nodo << "," <<  origen  <<  "," << RREQ_ID <<  "," << tiempProgramado;
           LOG4CXX_DEBUG ( logRREQ_Buffer, mensaje.str() );
   }
   //----------------------------------------------------------------------
   void
   LoggerAodv::
   logCsvRREQ_Buffer_RenglonSeparador ( ) throw()  {
           std::ostringstream mensaje;
           std::string separador = "----------------";
        mensaje << separador <<","<< separador <<","<< separador<<"," << separador;
           LOG4CXX_DEBUG ( logRREQ_Buffer, mensaje.str() );
   }
   //----------------------------------------------------------------------
   void
   LoggerAodv::
   logCsvRREQ_Buffer_comoTabla (std::string nodo,
   const std::map< par_Id_y_Direccion , shawn::EventScheduler::EventHandle>  
                                        bufferRREQ) throw() 
  {
    //logCsvRREQ_Buffer_RenglonSeparador ();

    //const_iterator para un map constante!!!
    std::map < par_Id_y_Direccion,shawn::EventScheduler::EventHandle >::const_iterator it;
    for ( it=bufferRREQ.begin() ; it!=bufferRREQ.end(); ++it)
    {
           logCsvRREQ_Buffer_Renglon (nodo ,
                                 it->first.second  /*origen*/,
                                 it->first.first   /*RREQ_ID*/,
                                 it->second->time() /*tiempProgramado*/) ;
    }    
    logCsvRREQ_Buffer_RenglonSeparador ();
  }
//----------------------------------------------------------------------
//Tabla de ruteo----------------------------------------------------------------------
   void
   LoggerAodv::
   logCsvRoutingTable_columnas ( std::string duenoTabla ) throw()  {
           std::ostringstream mensaje;
           mensaje << duenoTabla 
                   <<",dest,nextHop,hops,destSeqNum,validDestSeqNum,state,lifeTime";
           LOG4CXX_DEBUG ( logTabla, mensaje.str() );
   }
   //----------------------------------------------------------------------
   void
   LoggerAodv::
   logCsvRoutingTable_Renglon (AodvUgdTableEntery& unaEntrada) throw()  {
        std::ostringstream mensaje;

  mensaje << world_->current_time()            <<","<<
             unaEntrada.getDestino()              <<","<<
             unaEntrada.getNextHop()           <<","<<
             unaEntrada.getHops()              <<","<<
             unaEntrada.getDestSequenceNumber()<<","<<
             unaEntrada.getValidDestSeqNum()   <<","<<
             unaEntrada.getState()             <<","<<
             unaEntrada.getLifeTime()             ;

        LOG4CXX_DEBUG ( logTabla, mensaje.str() );
   }
   //----------------------------------------------------------------------
   void
   LoggerAodv::
   logCsvRoutingTable_RenglonSeparador ( ) throw()  {
           std::ostringstream mensaje;
           std::string separador = "----------------";
        mensaje << separador <<","<< separador <<","<< separador<<"," << separador
            <<","<< separador<<","<< separador <<","<< separador<<"," << separador;
           LOG4CXX_DEBUG ( logTabla, mensaje.str() );
   }
   //----------------------------------------------------------------------
   void
   LoggerAodv::
   logCsvRoutingTable_RenglonEspacioBlanco ( ) throw()  {
           std::ostringstream mensaje;
           std::string separador = "                ";
        mensaje << separador <<","<< separador <<","<< separador<<"," << separador
            <<","<< separador<<","<< separador <<","<< separador<<"," << separador;
           LOG4CXX_DEBUG ( logTabla, mensaje.str() );
   }
   //----------------------------------------------------------------------
   void
   LoggerAodv::
   logCsvRoutingTable_comoTabla (std::string duenoTabla, const std::map<const std::string,AodvUgdTableEntery*>  
                                  routingTableMap ) throw() 
  {
    logCsvRoutingTable_RenglonSeparador();
    logCsvRoutingTable_columnas(duenoTabla);
    logCsvRoutingTable_RenglonSeparador();
    //const_iterator para un map constante!!!
    std::map<const std::string,AodvUgdTableEntery*>::const_iterator it;

    for ( it=routingTableMap.begin() ; it!=routingTableMap.end(); ++it)
    {
        logCsvRoutingTable_Renglon( *( it->second ) /*unaEntrada*/) ; 
    }   
    logCsvRoutingTable_RenglonEspacioBlanco();
    logCsvRoutingTable_RenglonEspacioBlanco();
   
  }
//----------------------------------------------------------------------
//----------------------------------------------------------------------
      void
   LoggerAodv::
   logCsvRREQ_Mensajes( const AodvUgdRREQ& rreq , std::string detalle ) throw()  {
           std::ostringstream mensaje;
      mensaje
         <<   world_->current_time()       <<","<<
              detalle                      <<","<<
              rreq.getDestino()               <<","<<
              rreq.getOrigen()             <<","<<
              rreq.getDestSequNumb()       <<","<<
              rreq.getOrigenSequNumb()     <<","<<
              rreq.getTtl()                <<","<<
              rreq.getHops()               <<","<<
              rreq.getRREQ_ID()            <<","<<
              rreq.getJoinFlag()           <<","<<
              rreq.getRepairFlag()         <<","<<
              rreq.getRREP_gFlag()         <<","<<
              rreq.getDestOnlyFlag()       <<","<<
              rreq.getUnknownSequNum()     <<","<<
              &rreq    /*memoria*/             ;
           LOG4CXX_DEBUG ( logRREQ_Mensajes, mensaje.str() );
   }
// ----------------------------------------------------------------------
   //----------------------------------------------------------------------


   void
   LoggerAodv:: 
   logCsvRREP_Movimientos( const AodvUgdRREP& rrep,std::string nodo ,std::string detalle ) throw()  {
           std::ostringstream mensaje;
          std::string source=" ---- ";
           if (rrep.has_source() )
           {
             source=rrep.source().label();
           }
            mensaje << world_->current_time() <<","<< //tiempo
                      nodo                   <<","<< //nodo
                      source                 <<","<< //source
                      rrep.getOrigen()       <<","<< //origen
                      " -->> "               <<","<< //flecha
                      rrep.getDestino()      <<","<< //dest
                      detalle                <<","<<
                      rrep.getTtl()          <<","<<
                      &rrep                   ;

           LOG4CXX_DEBUG ( logRREP_Movimientos , mensaje.str() );
   }
// ----------------------------------------------------------------------
//----------------------------------------------------------------------
   void
   LoggerAodv::
   logCsvRREP_Movimientos_columnas( ) throw()  {
           std::ostringstream mensaje;

           mensaje << "Tiempo"  <<","<< //tiempo
                      "Nodo"    <<","<< //nodo
                      "Source"  <<","<< //source
                      "Origen"  <<","<< //origen
                      "-->>"    <<","<< //origen
                      "Destino" <<","<< //dest
                      "Detalle" <<","<<
                      "TTL"     <<","<<
                      "Memoria"    ;
           LOG4CXX_DEBUG ( logRREP_Movimientos , mensaje.str() );
   }

  //Ping----------------------------------------------------------------------
   void
   LoggerAodv::
   logCsvPing_columnas (  ) throw()  
   {
           std::ostringstream mensaje;
           mensaje <<"Tiempo,Nodo,Origen,Destino,Modulo,Detalle";
           LOG4CXX_DEBUG ( logPing , mensaje.str() );
   } 
  //----------------------------------------------------------------------
   void
   LoggerAodv::
   logCsvPingDetalle ( std::string nodo    , std::string origen  ,
                             std::string destino , std::string modulo ,
                             std::string detalle ) throw()  
   {
           std::ostringstream mensaje;

           mensaje << world_->current_time() <<","<< //tiempo
                      nodo                   <<","<< 
                      origen                 <<","<< 
                      destino                <<","<< 
                      modulo                 <<","<< 
                      detalle                <<"," 
                                                ;

           LOG4CXX_DEBUG ( logPing , mensaje.str() );
   } 
  //----------------------------------------------------------------------
  //Discovery----------------------------------------------------------------------
   void
   LoggerAodv::
   logCsvDiscovery_columnas (  ) throw()  
   {
           std::ostringstream mensaje;
           mensaje <<"Tiempo,Nodo,Origen,Destino,Detalle,Modulo";
           LOG4CXX_DEBUG ( logDiscovery , mensaje.str() );
   } 
  //----------------------------------------------------------------------
   void
   LoggerAodv::
   logCsvDiscoveryDetalle ( std::string nodo    , std::string origen  ,
                             std::string destino , std::string modulo ,
                             std::string detalle ) throw()  
   {
           std::ostringstream mensaje;

           mensaje << world_->current_time() <<","<< //tiempo
                      nodo                   <<","<< 
                      origen                 <<","<< 
                      destino                <<","<< 
                      detalle                <<","<< 
                      modulo                 <<"," 
                                                ;

           LOG4CXX_DEBUG ( logDiscovery , mensaje.str() );
   } 
  //----------------------------------------------------------------------


}
#endif
