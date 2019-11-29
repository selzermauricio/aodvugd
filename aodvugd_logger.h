#ifndef __SHAWN_LEGACYAPPS_AODVUGD_LOGGER_H
#define __SHAWN_LEGACYAPPS_AODVUGD_LOGGER_H
#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD

#include "legacyapps/aodvugd/aodvugd_processor.h"
#include "sys/world.h"

namespace aodvugd
{ 
//class AodvUgdRREQ;
//class log4cxx;
// clase LoggerAodv----------------------------------------------------------------------
class LoggerAodv{
public:
  static LoggerAodv* Instance();
   void setWorld( const shawn::World& world);
 //log RREQ
  void logCsvRREQ( const AodvUgdRREQ& rreq,const shawn::Node& unNodo ,std::string detalle ) throw(); 
  void logCsvRREQ( const AodvUgdRREQ& rreq,std::string nodo , std::string detalle ) throw(); 
  void logCsvRREQ_columnas( ) throw() ;

//logRREQ Mensajes
  void logCsvRREQ_Mensajes( const AodvUgdRREQ& rreq , std::string detalle ) throw();
  void logCsvRREQ_Mensaje_columnas( ) throw();

//bufferRREQ map
     //taypeDef
  typedef std::pair <unsigned int, std::string> par_Id_y_Direccion;
  void logCsvRREQ_Buffer_comoTabla (std::string nodo, 
  const std::map< par_Id_y_Direccion , shawn::EventScheduler::EventHandle> bufferRREQ) 
  throw();


  //logCsvRREP Movimientos
  void logCsvRREP_Movimientos( const AodvUgdRREP& rrep,std::string nodo ,std::string detalle ) 
                              throw();
  void logCsvRREP_Movimientos_columnas( ) throw() ;


  //logCsvRoutingTable
  void logCsvRoutingTable_comoTabla (std::string duenoTabla,
                   const std::map<const std::string,AodvUgdTableEntery*>  
                   routingTableMap )   throw() ;
  //logCsvPingDetalle
  void logCsvPingDetalle ( std::string nodo    , std::string origen ,
                                 std::string destino , std::string modulo,
                                 std::string detalle ) throw() ;

    //logCsvPingDetalle
  void  logCsvDiscoveryDetalle ( std::string nodo    , std::string origen  ,
                                 std::string destino , std::string modulo  ,
                                 std::string detalle ) throw()   ;

private:
   LoggerAodv();  // Private so that it can  not be called
   LoggerAodv(LoggerAodv const&){};             // copy constructor is private
   LoggerAodv& operator=(LoggerAodv const&){};  // assignment operator is private
   static LoggerAodv* m_pInstance;
   // virtual ~LoggerAodv();
   log4cxx::LoggerPtr logRREQ_Movimientos ;//= log4cxx::Logger::getLogger("test_inundacion_rreq");
   log4cxx::LoggerPtr logRREQ_Mensajes ;
   log4cxx::LoggerPtr logRREQ_Buffer ;
  
   log4cxx::LoggerPtr logRREP_Movimientos ;
   log4cxx::LoggerPtr logTabla ;
   log4cxx::LoggerPtr logDiscovery ;
   log4cxx::LoggerPtr logPing ;
  
   const shawn::World* world_;

   //bufferRREQ map
  void logCsvRREQ_Buffer_columnas ( ) throw() ;
  void logCsvRREQ_Buffer_Renglon (std::string nodo, std::string origen ,
                                   unsigned int RREQ_ID, double tiempProgramado) 
                                   throw() ;
  void logCsvRREQ_Buffer_RenglonSeparador ( ) throw() ;

   //logCsvRoutingTable
  void logCsvRoutingTable_columnas ( std::string duenoTabla ) throw();
  void logCsvRoutingTable_Renglon (AodvUgdTableEntery& unaEntrada) throw();
  void logCsvRoutingTable_RenglonSeparador ( ) throw();
  void logCsvRoutingTable_RenglonEspacioBlanco ( ) throw() ; 

   //logPing
  void logCsvPing_columnas ( ) throw(); 

  //logDiscovery
  void logCsvDiscovery_columnas ( ) throw();

};
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

}

#endif
#endif




