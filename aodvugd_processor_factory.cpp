#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD

#include "legacyapps/aodvugd/aodvugd_processor_factory.h"
#include "legacyapps/aodvugd/aodvugd_processor.h"
#include "legacyapps/aodvugd/aodvugd_processorhello.h"
#include "legacyapps/aodvugd/aodvugd_ping_app.h"

#include "sys/processors/processor_keeper.h"
#include "sys/simulation/simulation_controller.h"
#include <iostream>

using namespace std;
using namespace shawn;

namespace aodvugd
{

   // ----------------------------------------------------------------------
   void
   AodvUgdProcessorFactory::
   register_factory( SimulationController& sc )
   throw()
   {
      sc.processor_keeper_w().add( new AodvUgdProcessorFactory );
   }
   // ----------------------------------------------------------------------
   AodvUgdProcessorFactory::
   AodvUgdProcessorFactory()
   {
      cout << "AodvUgdProcessorFactory constructor" << endl;
   }
   // ----------------------------------------------------------------------
   AodvUgdProcessorFactory::
   ~AodvUgdProcessorFactory()
   {
      cout << "AodvUgdProcessorFactory destructor" << endl;
   }
   // ----------------------------------------------------------------------
   std::string
   AodvUgdProcessorFactory::
   name( void )
   const throw()
   {
      return "aodvugd";
   }
   // ----------------------------------------------------------------------
   std::string
   AodvUgdProcessorFactory::
   description( void )
   const throw()
   {
      return "Shawn AODV Universidad Gaston Dachary Processor";
   }
   // ----------------------------------------------------------------------
   shawn::Processor*
   AodvUgdProcessorFactory::
   create( void )
   throw()
   {
      return new aodvugd::AodvUgdProcessor;
   }

	// ----------------------------------------------------------------------
   // ------------------------     HELLO   ----------------------------------------------
   // ----------------------------------------------------------------------
   void
   AodvUgdProcessorFactoryHello::
   register_factory( SimulationController& sc )
   throw()
   {
      sc.processor_keeper_w().add( new AodvUgdProcessorFactoryHello );
   }
   // ----------------------------------------------------------------------
   AodvUgdProcessorFactoryHello::
   AodvUgdProcessorFactoryHello()
   {
      cout << "AodvUgdProcessorFactoryHello constructor" << endl;
   }
   // ----------------------------------------------------------------------
   AodvUgdProcessorFactoryHello::
   ~AodvUgdProcessorFactoryHello()
   {
      cout << "AodvUgdProcessorFactoryHello destructor" << endl;
   }
   // ----------------------------------------------------------------------
   std::string
   AodvUgdProcessorFactoryHello::
   name( void )
   const throw()
   {
      return "aodvugdhello";
   }
   // ----------------------------------------------------------------------
   std::string
   AodvUgdProcessorFactoryHello::
   description( void )
   const throw()
   {
      return "Shawn AODV Universidad Gaston Dachary Processor";
   }
   // ----------------------------------------------------------------------
   shawn::Processor*
   AodvUgdProcessorFactoryHello::
   create( void )
   throw()
   {
      return new aodvugd::AodvUgdProcessorHello;
   }


   // ----------------------------------------------------------------------
   // ------------------------     PING   ----------------------------------------------
   // ----------------------------------------------------------------------
   void
   AodvUgdProcessorFactoryPingApp::
   register_factory( SimulationController& sc )
   throw()
   {
      sc.processor_keeper_w().add( new AodvUgdProcessorFactoryPingApp );
   }
   // ----------------------------------------------------------------------
   AodvUgdProcessorFactoryPingApp::
   AodvUgdProcessorFactoryPingApp()
   {
      cout << "AodvUgdProcessorFactoryPingApp constructor" << endl;
   }
   // ----------------------------------------------------------------------
   AodvUgdProcessorFactoryPingApp::
   ~AodvUgdProcessorFactoryPingApp()
   {
      cout << "AodvUgdProcessorFactoryPingApp destructor" << endl;
   }
   // ----------------------------------------------------------------------
   std::string
   AodvUgdProcessorFactoryPingApp::
   name( void )
   const throw()
   {
      return "aodvPingApp";
   }
   // ----------------------------------------------------------------------
   std::string
   AodvUgdProcessorFactoryPingApp::
   description( void )
   const throw()
   {
      return "Shawn AODV modulo Ping";
   }
   // ----------------------------------------------------------------------
   shawn::Processor*
   AodvUgdProcessorFactoryPingApp::
   create( void )
   throw()
   {
      return new aodvugd::PingApp;
   }
	

}

#endif
