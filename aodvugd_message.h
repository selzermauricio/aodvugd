#ifndef __SHAWN_LEGACYAPPS_AODVUGD_MESSAGE_H
#define __SHAWN_LEGACYAPPS_AODVUGD_MESSAGE_H
#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD

#include "sys/message.h"
#include <set>
#include <map>
#include "legacyapps/aodvugd/aodvugd_types.h" //tipos definidos

namespace aodvugd
{
//using mapTypeDestinosInvalidos; //= std::map <const std::string , const unsigned int> ;
//using parDestinosInvalidos; //     = std::pair<const std::string , const unsigned int> ; 
inline const extern std::string BROADCAST_ADDRESS;

   class AodvUgdFloodingMessage
       : public shawn::Message
   {
   public:
      AodvUgdFloodingMessage( int hops );
      virtual ~AodvUgdFloodingMessage();
         
      inline int hops( void ) const throw()
      { return hops_; };
         
   private:
      int hops_;
   };
   // ----------------------------------------------------------------------
   // ----------------------------------------------------------------------
   // ----------------------------------------------------------------------
   class AodvUgdStateMessage
      : public shawn::Message
   {
   public:
      AodvUgdStateMessage( const shawn::Node& initiator, 
                        const shawn::Node& dest, 
                        int initiator_hops );
      virtual ~AodvUgdStateMessage();

      inline const shawn::Node& initiator( void ) const throw()
      {  return initiator_; };
      
      inline const shawn::Node& destination( void ) const throw()
      {  return destination_; };
      
      inline int initiator_hops( void ) const throw()
      {  return initiator_hops_; };
      
   private:
      const shawn::Node& initiator_;
      const shawn::Node& destination_;
      int initiator_hops_;
   };
   
   // class AodvUgdIpHeader ----------------------------------------------------------------------
   // ----------------------------------------------------------------------
   class AodvUgdIpHeader
      : public shawn::Message
    {
   public:
      AodvUgdIpHeader( const unsigned int ipTtl,
                       const std::string  ipOrigen, 
                       const std::string  ipDestino );
      virtual ~AodvUgdIpHeader();


      const unsigned int   getTtl        (void) const  throw();
      const std::string    getIpDestino  (void) const  throw();
      const std::string    getIpOrigen   (void) const  throw();
      std::string    getMacDestino       (void) const  throw();
      
      void  setMacDestino   (std::string macDestino)   throw();
      void  disminuirTTL    ( )                        throw();

      virtual AodvUgdIpHeader*   clonarComoPaqueteIP() throw() = 0;
      //virtual AodvUgdIpHeader        *Clone()         throw() = 0 ;

      // Pure virtual functions:
      virtual std::string  toString      (void) const  throw() = 0;

   protected:
     unsigned int ipTtl_;
     std::string  ipDestino_;
     std::string  ipOrigen_;
     std::string  macDestino_; //abstracion del destino de la capa MAC, siguiente salto MAC

   };
   // ----------------------------------------------------------------------
   // ----------------------------------------------------------------------
   // class AodvUgdRREQ ----------------------------------------------------------------------
   class AodvUgdRREQ
       : public AodvUgdIpHeader
   {
   public:
	   AodvUgdRREQ();
	   AodvUgdRREQ( std::string  dest         , std::string  origen ,
                  unsigned int destSequNumb , unsigned int  origenSequNumb ,
                  bool joinFlag             , bool repairFlag ,
                  bool rrep_gFlag           , bool destOnlyFlag ,
                  bool UnknownSequNum       , unsigned int hops ,
                  unsigned int rreqId       , std::string  ipOrigen ,
                  //Ip
                  unsigned int ttl           );
      virtual ~AodvUgdRREQ();

    // get set----------------------------------------------------------------------

      const unsigned int getHops (void) const throw();
      const unsigned int getRREQ_ID ( void ) const throw();
      const std::string  getOrigen( void )const /*throw()*/;
      const std::string  getDestino( void )const throw();
      const unsigned int getOrigenSequNumb ( void ) const throw();
      const unsigned int getDestSequNumb ( void ) const throw();
      const bool		     getUnknownSequNum ( void ) const throw();
      const bool		     getDestOnlyFlag ( void ) const throw();
      const bool         getJoinFlag ( void ) const throw();
      const bool         getRepairFlag ( void ) const throw();
      const bool         getRREP_gFlag ( void ) const throw();
      
      //void	setDestSequNumb(  unsigned int newDestSeqNum )  throw();

      // metodos----------------------------------------------------------------------
      AodvUgdIpHeader*  clonarComoPaqueteIP() throw();
       std::string  toString		(void) 	const	throw();
       void   	   	show			  (void)  const	throw();


      /*      inline int getTtl( void ) const throw()
      { return ttl; };*/

   private:
      const	std::string 	dest_;			     //The IP address of the destination
      const	std::string 	origen_;		     //The IP address of the Originator
      const unsigned int	destSequNumb_;   //Destination Sequence Number
	    const unsigned int	origenSequNumb_; //Originator Sequence Number
	    const bool 			    joinFlag_;       //Join flag; reserved for multicast.
      const bool 			    repairFlag_;     //Repair flag; reserved for multicast.
      const bool 			    rrep_gFlag_;     //Gratuitous RREP flag
      const bool 			    destOnlyFlag_;   //Destination only flag;
      const bool 			    UnknownSequNum_; //Unknown sequence number; del destino
      //Reserved       Sent as 0; ignored on reception.
	    const unsigned int	hops_;      		 //The number of hops from the Originator IP Address
	    const unsigned int	rreqId_;			   //A sequence number uniquely identifying
   };
   // ----------------------------------------------------------------------
   // ----------------------------------------------------------------------
   // ----------------------------------------------------------------------
   // class AodvUgdRREP ----------------------------------------------------------------------
   class AodvUgdRREP
       : public AodvUgdIpHeader
   {
   public:
	  // AodvUgdRREP();
	   AodvUgdRREP(  std::string  dest          , std::string  origen  ,
                   unsigned int destSequNumb  , bool         ackFlag ,
                   bool         repairFlag    , unsigned int hops    , 
                   double       lifeTime      , 
                   //ip
                   unsigned int ttl           ,
                   std::string  ipDestino     , std::string  ipOrigen ); 

      virtual ~AodvUgdRREP();

    // get set----------------------------------------------------------------------

      const unsigned int getHops             ( void ) const throw();
      const std::string  getOrigen           ( void ) const throw();
      const std::string  getDestino             ( void ) const throw();
      const unsigned int getDestSequNumb     ( void ) const throw();
      const bool         getAckFlag          ( void ) const throw();
      const bool         getRepairFlag       ( void ) const throw();
      const double       getLifeTime         ( void ) const throw();
      //overload-- no se utilizo el overload :(
      // bool              is_unicast          ( void ) const throw();
      // int               unicast_target      ( void ) const throw();

/*      inline int getTtl( void ) const throw()
      { return ttl; };*/

      // metodos----------------------------------------------------------------------
       std::string  toString    (void)  const throw();
       AodvUgdIpHeader*  clonarComoPaqueteIP() throw();

   private:
      const std::string	   dest_;			    //The IP address of the destination
      const std::string	   origen_;		    //The IP address of the Originator
      const unsigned int	 destSequNumb_;	//Destination Sequence Number
	    const bool 			     ackFlag_;      //Acknowledgment required; see sections 5.4 and 6.7.
      const bool 			     repairFlag_;   //Repair flag; reserved for multicast.
      //Reserved       Sent as 0; ignored on reception.
	    const unsigned int	 hops_;      		//The number of hops from the Originator IP Address
	    const double 		     lifeTime_;			//miliseconds which nodes receiving the RREP consider the route to be valid.
	    const int 			     prefixSize_=0;	//TODO

      //overload no se usa
      // const bool           is_unicast_ =  false;
      // const int            unicast_target_;

      

   };
   // ----------------------------------------------------------------------
   // ----------------------------------------------------------------------
   // class AodvUgdRERR ----------------------------------------------------------------------
   class AodvUgdRERR
       : public AodvUgdIpHeader
   {
   public:
     // AodvUgdRERR();
      AodvUgdRERR(    bool noDeleteFlag                     , int destCount ,  
                      mapTypeDestinosInvalidos *mapDestinos ,
                      //Ip
                      unsigned int ttl                      , 
                      std::string ipDestino                 , std::string ipOrigen); 

      virtual ~AodvUgdRERR();

    // get set----------------------------------------------------------------------
      const bool getNoDeleteFlag                      ( void ) const throw();
      const int  getDestCount                         ( void ) const throw();
      const mapTypeDestinosInvalidos* getMapDestinos  ( void ) const throw();
    // metodos----------------------------------------------------------------------
       //metodo obligatorio por la clase abstracta
       std::string  toString    (void)  const throw();
       AodvUgdIpHeader*  clonarComoPaqueteIP() throw();

   private:
      

/*     No delete flag; set when a node has performed a local
      repair of a link, and upstream nodes should not delete
      the route.*/
      const bool noDeleteFlag_; 
      
/*    The number of unreachable destinations included in the
      message; MUST be at least 1.*/
      const int destCount_;

/*    The IP address of the destination that has become
      unreachable due to a link break.*/
      aodvugd::mapTypeDestinosInvalidos  *mapDestinos_;
      
   };
   // ----------------------------------------------------------------------


enum class TipoICMP
    { EchoReply=0, EchoRequest=8 };

    // class PingMessage ----------------------------------------------------------------------
   class PingMessage
       : public AodvUgdIpHeader
   {
   public:
     // PingMessage();
      PingMessage(    int size                  , TipoICMP unTipoICMP , 
                      unsigned int pingID       , unsigned int seqNum , 
                      unsigned int ttl          ,
                      std::string ipDestino     , std::string ipOrigen ); 

      virtual ~PingMessage();

    // get set----------------------------------------------------------------------
      const TipoICMP     getUnTipoICMP                    ( void ) const throw();
      const unsigned int getPingID                        ( void ) const throw();
      const unsigned int getSeqNum                        ( void ) const throw();
    // metodos----------------------------------------------------------------------
       AodvUgdIpHeader*  clonarComoPaqueteIP()   throw();


       //metodo obligatorio por la clase abstracta
       std::string  toString    (void)  const throw();

   private:
      const TipoICMP unTipoICMP_;
      const unsigned int pingID_;
      const unsigned int seqNum_;
      
   };
}
#endif
#endif
