#ifndef __SHAWN_LEGACYAPPS_AODVUGD_PROCESSOR_FACTORY_H
#define __SHAWN_LEGACYAPPS_AODVUGD_PROCESSOR_FACTORY_H
#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD

#include "sys/processors/processor_factory.h"

namespace shawn
{
   class SimulationController;
   class Processor;
}

namespace aodvugd
{
   // ----------------------------------------------------------------------
   class AodvUgdProcessorFactory
      : public shawn::ProcessorFactory
   {
      public:
         AodvUgdProcessorFactory();
         virtual ~AodvUgdProcessorFactory();

         virtual std::string name( void ) const throw();
         virtual std::string description( void ) const throw();
         virtual shawn::Processor* create( void ) throw();

         static void register_factory( shawn::SimulationController& ) throw();
   };

   // ----------------------------------------------------------------------
    class AodvUgdProcessorFactoryHello
      : public shawn::ProcessorFactory
   {
      public:
         AodvUgdProcessorFactoryHello();
         virtual ~AodvUgdProcessorFactoryHello();

         virtual std::string name( void ) const throw();
         virtual std::string description( void ) const throw();
         virtual shawn::Processor* create( void ) throw();

         static void register_factory( shawn::SimulationController& ) throw();
   };
   // ----------------------------------------------------------------------
       class AodvUgdProcessorFactoryPingApp
      : public shawn::ProcessorFactory
   {
      public:
         AodvUgdProcessorFactoryPingApp();
         virtual ~AodvUgdProcessorFactoryPingApp();

         virtual std::string name( void ) const throw();
         virtual std::string description( void ) const throw();
         virtual shawn::Processor* create( void ) throw();

         static void register_factory( shawn::SimulationController& ) throw();
   };
}

#endif
#endif
