#ifndef __SHAWN_LEGACYAPPS_AODVUGD_AODVUGD_INIT_H__
#define __SHAWN_LEGACYAPPS_AODVUGD_AODVUGD_INIT_H__
#include "_legacyapps_enable_cmake.h"
#ifdef ENABLE_AODVUGD

namespace shawn
{ 
	class SimulationController; 
}

/** This method is invoked by Shawn to initialize the current module
  */
extern "C" void init_aodvugd( shawn::SimulationController& );


#endif
#endif
