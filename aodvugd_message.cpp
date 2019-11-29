#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD
//#undef NDEBUG //para que funcione el assert, no cambiar de lugar porque no anda

#include "legacyapps/aodvugd/aodvugd_message.h"
#include <assert.h>     /* assert */
#include <sstream>   /* toString */

#include "legacyapps/aodvugd/aodvugd_logger.h" //AodvUgdUtill
#include "legacyapps/aodvugd/aodvugd_types.h" //AodvUgdUtill
//borrar
//#include "legacyapps/aodvugd/aodvugd_processor.h"

namespace aodvugd
{
//using mapTypeDestinosInvalidos = std::map <const std::string , const unsigned int> ;
//using parDestinosInvalidos     = std::pair<const std::string , const unsigned int> ; 

	// ----------------------------------------------------------------------
   AodvUgdFloodingMessage::
   AodvUgdFloodingMessage( int hops )
      : hops_ ( hops )
	{}
	// ----------------------------------------------------------------------
   AodvUgdFloodingMessage::
	~AodvUgdFloodingMessage()
	{}
   // ----------------------------------------------------------------------
   // ----------------------------------------------------------------------
   // ----------------------------------------------------------------------
   AodvUgdStateMessage::
   AodvUgdStateMessage( const shawn::Node& initiator, 
                     	const shawn::Node& dest, 
                     	int initiator_hops )
      : initiator_       ( initiator ),
        destination_     ( dest ),
        initiator_hops_  ( initiator_hops )
   {}
   // ----------------------------------------------------------------------
   AodvUgdStateMessage::
   ~AodvUgdStateMessage()
   {}

   // ----------------------------------------------------------------------
   // ----------------------------------------------------------------------
   // ----------------------------------------------------------------------
   // clase AodvUgdIpHeader----------------------------------------------------------------------
   AodvUgdIpHeader::
   AodvUgdIpHeader(const unsigned int ipTtl,
   				   const std::string  ipOrigen ,
   				   const std::string  ipDestino )
     :
   //inicializacion
     ipDestino_		{ ipDestino },
     ipOrigen_		{ ipOrigen },
     ipTtl_			{ ipTtl}
   {}

   // ----------------------------------------------------------------------
   AodvUgdIpHeader::
   ~AodvUgdIpHeader()
   {}
   // ----------------------------------------------------------------------
   // get ----------------------------------------------------------------------
	const unsigned int AodvUgdIpHeader:: getTtl 	 ( void ) const throw()
	{	return ipTtl_;}
	// ----------------------------------------------------------------------
	const std::string AodvUgdIpHeader:: getIpDestino ( void ) const throw()
	{	return ipDestino_;}
	// ----------------------------------------------------------------------
	const std::string AodvUgdIpHeader:: getIpOrigen ( void ) const throw()
	{	return ipOrigen_;}
	// ----------------------------------------------------------------------
	std::string AodvUgdIpHeader::getMacDestino( void ) const throw()
	{	return macDestino_;}
	// set ----------------------------------------------------------------------
	void AodvUgdIpHeader:: setMacDestino ( std::string macDestino)   throw()
	{macDestino_ = macDestino;}
	// metodos----------------------------------------------------------------------
	void AodvUgdIpHeader:: disminuirTTL ()   throw()
	{
		ipTtl_-- ;
		assert (ipTtl_>=0);
	}
   // clase AodvUgdRREQ----------------------------------------------------------------------
   // AodvUgdRREQ::
   // AodvUgdRREQ()
   // {}

   AodvUgdRREQ::
   AodvUgdRREQ( std::string  dest 		  ,  std::string  origen ,
		   	    unsigned int destSequNumb , unsigned int	origenSequNumb ,
			    bool joinFlag 			  , bool repairFlag ,
			    bool rrep_gFlag 		  , bool destOnlyFlag ,
			    bool UnknownSequNum 	  , unsigned int hops ,
			    unsigned int rreqId 	  , std::string  ipOrigen,
			    //Ip
			    unsigned int ttl 		   )

   :
   //inicializacion
   //ip
   AodvUgdIpHeader		{ttl , ipOrigen, BROADCAST_ADDRESS },

   //RREQ
   dest_				{dest},
   origen_		 		{origen},
   destSequNumb_		{destSequNumb},
   origenSequNumb_		{origenSequNumb},
   joinFlag_           	{joinFlag},
   repairFlag_         	{repairFlag},
   rrep_gFlag_         	{rrep_gFlag},
   destOnlyFlag_       	{destOnlyFlag},
   UnknownSequNum_     	{UnknownSequNum},
   hops_     			{hops},
   rreqId_			    {rreqId}


   {   }

	// ----------------------------------------------------------------------
	AodvUgdRREQ::
	~AodvUgdRREQ(){
			   std::ostringstream mensaje;			
    		   mensaje <<"-----****destroy RREQ Memoria: " << this;

    			const AodvUgdRREQ &rreq=*(const_cast<const AodvUgdRREQ*>(this));
    		   	LoggerAodv::Instance()->logCsvRREQ( rreq , dest_ , mensaje.str() );

    		   std::cout<< "--------------------------UnProcesoDiscovrey: adress " 
    		   << this<< std::endl;

	}

	// get set----------------------------------------------------------------------
	const unsigned int AodvUgdRREQ:: getHops ( void ) const throw()
	{	return hops_;}
	// ----------------------------------------------------------------------
	const unsigned int AodvUgdRREQ:: getRREQ_ID ( void ) const throw()
	{	return rreqId_;}
	// ----------------------------------------------------------------------
	const unsigned int AodvUgdRREQ:: getOrigenSequNumb ( void ) const throw()
	{	return origenSequNumb_;}
	// ----------------------------------------------------------------------
	const unsigned int AodvUgdRREQ:: getDestSequNumb ( void ) const throw()
	{	return destSequNumb_;}
	// ----------------------------------------------------------------------
	const bool AodvUgdRREQ:: getUnknownSequNum ( void ) const throw()
	{	return UnknownSequNum_;}
	// ----------------------------------------------------------------------
	const bool AodvUgdRREQ:: getDestOnlyFlag ( void ) const throw()
	{	return destOnlyFlag_;}
	// ----------------------------------------------------------------------
	const bool AodvUgdRREQ:: getJoinFlag ( void ) const throw()
	{	return joinFlag_;}
	// ----------------------------------------------------------------------
	const bool AodvUgdRREQ:: getRepairFlag ( void ) const throw()
	{	return repairFlag_;}
	// ----------------------------------------------------------------------
	const bool AodvUgdRREQ:: getRREP_gFlag ( void ) const throw()
	{	return rrep_gFlag_;}
	// ----------------------------------------------------------------------
	const std::string AodvUgdRREQ:: getOrigen( void ) const /*throw()*/
	{
		assert(!origen_.empty());
		return origen_;
	}
	// ----------------------------------------------------------------------
	const std::string AodvUgdRREQ:: getDestino( void ) const throw()
	{
		assert( !dest_.empty() );
		return dest_;
	}
	// ----------------------------------------------------------------------
	// void AodvUgdRREQ:: setDestSequNumb( unsigned int newDestSeqNum )  throw()
	// {
	// 	destSequNumb_	=	newDestSeqNum ;
	// }

	// metodos----------------------------------------------------------------------
	// ----------------------------------------------------------------------
	AodvUgdIpHeader*	AodvUgdRREQ:: 	clonarComoPaqueteIP()	 throw()
	{
		AodvUgdRREQ * unAodvUgdRREQ = new AodvUgdRREQ( 
				dest_ 		    ,  origen_ 		  ,
		   	    destSequNumb_   , origenSequNumb_ ,
			    joinFlag_	    , repairFlag_ 	  ,
			    rrep_gFlag_     , destOnlyFlag_   ,
			    UnknownSequNum_ , hops_ 		  ,
			    rreqId_ 	    , ipOrigen_ 	  ,
			    //Ip
			     /*Ip*/getTtl()		    );

		return dynamic_cast < AodvUgdIpHeader*> (unAodvUgdRREQ );	
	}
	// ----------------------------------------------------------------------
	std::string	AodvUgdRREQ:: 	toString(void)	const throw()

			{
		std::stringstream ss;
			   ss <<  "	dest	|	origen_		| destSequNumb | origenSequNumb | ttl | hops | rreqId | joinFlag | repairFlag | rrep_gFlag | destOnlyFlag | UnknownSequNum |"	<< '\n' ;

				   ss <<	dest_					<<"		"<<
						   	origen_ 				<<"	   "<<
							destSequNumb_ 			<<"		 "<<
							origenSequNumb_ 		<<"		"<<
							ipTtl_ 					<<"	"<<
							hops_ 					<<"	"<<
							rreqId_ 				<<"	"<<
							joinFlag_ 				<<"		"<<
							repairFlag_ 			<<"		"<<
							rrep_gFlag_ 			<<"		"<<
							destOnlyFlag_ 			<<"		"<<
							UnknownSequNum_ 		<<'\n';

			   return (ss.str());
			}

	// ----------------------------------------------------------------------

	void	AodvUgdRREQ::show(void) const	throw()
	{

			std::cout << AodvUgdRREQ::toString()<< '\n';

	}
   // ----------------------------------------------------------------------
   // ----------------------------------------------------------------------
   // ----------------------------------------------------------------------

   // clase AodvUgdRREP----------------------------------------------------------------------
/*   AodvUgdRREP::
   AodvUgdRREP()
   {}*/

   AodvUgdRREP::
   AodvUgdRREP(	   std::string dest 		 , std::string origen ,
			   	   unsigned int destSequNumb , bool ackFlag 	  ,
			   	   bool repairFlag 			 , unsigned int hops  , 
			   	   double lifeTime 			 , 
				   //ip
                   unsigned int ttl           ,
                   std::string  ipDestino     , std::string  ipOrigen   )

   :
   //inicializacion
   //Ip
   AodvUgdIpHeader		{ttl , ipOrigen , ipDestino},

   //RREP
   dest_				{dest},
   origen_		 		{origen},
   destSequNumb_		{destSequNumb},
   ackFlag_           	{ackFlag},
   repairFlag_         	{repairFlag},
   hops_      			{hops},
   lifeTime_			{lifeTime}
   //prefixSize_			{prefixSize}, TODO
   //Ip parameter
   //is_unicast_			{true},

   {   }

	// ----------------------------------------------------------------------
	AodvUgdRREP::
	~AodvUgdRREP(){}

	// get set----------------------------------------------------------------------
	const unsigned int AodvUgdRREP:: getHops ( void ) const throw()
	{	return hops_;}
	// ----------------------------------------------------------------------
	const unsigned int AodvUgdRREP:: getDestSequNumb ( void ) const throw()
	{	return destSequNumb_;}
	// ----------------------------------------------------------------------
	const std::string AodvUgdRREP:: getOrigen( void ) const throw()
	{
		assert( !origen_.empty() );
		return origen_;
	}
	// ----------------------------------------------------------------------
	const std::string AodvUgdRREP:: getDestino( void ) const throw()
	{
		assert( !dest_.empty() );
		return dest_;
	}
    // ----------------------------------------------------------------------
	const bool AodvUgdRREP:: getAckFlag( void ) const throw()
	{
		assert( !dest_.empty() );
		return ackFlag_;
	}
    // ----------------------------------------------------------------------
	const bool AodvUgdRREP:: getRepairFlag( void ) const throw()
	{
		assert( !dest_.empty() );
		return repairFlag_;
	}
	// ----------------------------------------------------------------------
	const double AodvUgdRREP:: getLifeTime( void ) const throw()
	{
		assert( !dest_.empty() );
		return lifeTime_;
	}
	// ----------------------------------------------------------------------
	std::string	AodvUgdRREP:: 	toString(void)	const throw()
	{
		
	}
	// ----------------------------------------------------------------------
	AodvUgdIpHeader*	AodvUgdRREP:: 	clonarComoPaqueteIP()	 throw()
	{
		AodvUgdRREP * unAodvUgdRREP = new AodvUgdRREP(
			       dest_ 		 ,  origen_   ,
			   	   destSequNumb_ ,  ackFlag_  ,
			   	   repairFlag_ 	 ,  hops_     , 
			   	   lifeTime_	 , 				   
                  /*Ip*/getTtl() , getIpDestino() , getIpOrigen() );

		return dynamic_cast < AodvUgdIpHeader*> (unAodvUgdRREP );	
	}

	// ----------------------------------------------------------------------
	//overload
	// // ----------------------------------------------------------------------
 // 	bool AodvUgdRREP::is_unicast( void ) const throw()
 // 	{
 // 		return is_unicast_;
 // 	}
	// // ----------------------------------------------------------------------
 //    int  AodvUgdRREP::unicast_target( void ) const throw()
 //    {
 //    	return unicast_target_;
 //    }
// // ----------------------------------------------------------------------

// clase AodvUgdRERR----------------------------------------------------------------------
/* AodvUgdRERR::
   AodvUgdRERR()
   {}*/

   AodvUgdRERR::
   AodvUgdRERR(	bool noDeleteFlag 					  , int destCount ,  
   				mapTypeDestinosInvalidos *mapDestinos ,
   				//Ip
   				unsigned int ttl    		 		  ,
   				std::string ipDestino                 , std::string ipOrigen )

   :
   //inicializacion
   //ip
   AodvUgdIpHeader			{ttl , ipOrigen , ipDestino},

   //RERR
   noDeleteFlag_			{noDeleteFlag},
   destCount_		 		{destCount},
   mapDestinos_		    	{mapDestinos}

   {   }

	// ----------------------------------------------------------------------
	AodvUgdRERR::
	~AodvUgdRERR(){}

	// get set----------------------------------------------------------------------
	const bool AodvUgdRERR::getNoDeleteFlag ( void ) const throw()
	{	return noDeleteFlag_;}
	// ----------------------------------------------------------------------
	const  int AodvUgdRERR::getDestCount ( void ) const throw()
	{	return destCount_;}
	// ----------------------------------------------------------------------
	const mapTypeDestinosInvalidos* AodvUgdRERR:: getMapDestinos ( void ) const throw()
	{	return mapDestinos_;}
	// ----------------------------------------------------------------------
	std::string	AodvUgdRERR:: 	toString(void)	const throw()
	{
		
	}
	// ----------------------------------------------------------------------
	AodvUgdIpHeader*	AodvUgdRERR:: 	clonarComoPaqueteIP()	 throw()
	{
		AodvUgdRERR * unAodvUgdRERR = new AodvUgdRERR( noDeleteFlag_  , destCount_ ,  
   													   mapDestinos_,		
   												/*Ip*/ getTtl() , getIpDestino() , getIpOrigen() );
		return dynamic_cast < AodvUgdIpHeader*> (unAodvUgdRERR );
		
	}
	// ----------------------------------------------------------------------


// clase PingMessage----------------------------------------------------------------------
/* PingMessage::
   PingMessage()
   {}*/
//abstracion de ICMP
   PingMessage::
   PingMessage(   int size                  , TipoICMP unTipoICMP , 
                  unsigned int pingID       , unsigned int seqNum , 
                  unsigned int ttl          ,
                  std::string ipDestino     , std::string ipOrigen )
   :
   //inicializacion
   //ip
   AodvUgdIpHeader			{ttl , ipOrigen , ipDestino},
   //ICMP
   unTipoICMP_				{unTipoICMP},
   pingID_ 					{pingID},
   seqNum_ 					{seqNum}

   { 
   		setSize(size);
   }
	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------
	PingMessage::
	~PingMessage(){}

	// get set----------------------------------------------------------------------
	const TipoICMP PingMessage::getUnTipoICMP ( void ) const throw()
	{	return unTipoICMP_;}
	// ----------------------------------------------------------------------
	const unsigned int PingMessage::getPingID ( void ) const throw()
	{	return pingID_;}
	// ----------------------------------------------------------------------
	const unsigned int PingMessage::getSeqNum ( void ) const throw()
	{	return seqNum_;}
	// ----------------------------------------------------------------------
	std::string	PingMessage:: 	toString(void)	const throw()
	{
		
	}
	// ----------------------------------------------------------------------
	AodvUgdIpHeader*	PingMessage:: 	clonarComoPaqueteIP()	 throw()
	{
		PingMessage * unPingMessage =new PingMessage( size() , getUnTipoICMP() ,pingID_, seqNum_,
										 getTtl() ,	 getIpDestino() , getIpOrigen() );
		return dynamic_cast < AodvUgdIpHeader*> (unPingMessage );
		
	}
	// ----------------------------------------------------------------------


}
#endif
