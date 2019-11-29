#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD

#include <iostream>
#include "legacyapps/aodvugd/aodvugd_init.h"
#include "legacyapps/aodvugd/aodvugd_processor_factory.h"
#include "sys/simulation/simulation_controller.h"
#include "sys/simulation/simulation_task_keeper.h"
#include "sys/transm_models/transmission_model_keeper.h"
#include "apps/reading/readings/reading_keeper.h"

#include "legacyapps/aodvugd/aodvugd_nodes_pings_task.h"

extern "C" void init_aodvugd( shawn::SimulationController& sc )
{
   USER( "Initialising AODV Universidad Gaston Dachary module (AODVUGD)" );
   aodvugd::AodvUgdProcessorFactory::register_factory( sc );
   aodvugd::AodvUgdProcessorFactoryHello::register_factory( sc );
   aodvugd::AodvUgdProcessorFactoryPingApp::register_factory( sc );

   sc.simulation_task_keeper_w().add( new aodvugd::NodesPingsTask );
   sc.simulation_task_keeper_w().add( new aodvugd::NodesChangeNameTask );
   //ws08::Ws08ProcessorFactory::register_factory( sc );

}

#endif
