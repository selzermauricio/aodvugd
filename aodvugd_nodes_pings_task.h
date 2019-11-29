/************************************************************************
 ** This file is part of the network simulator Shawn.                  **
 ** Copyright (C) 2004-2007 by the SwarmNet (www.swarmnet.de) project  **
 ** Shawn is free software; you can redistribute it and/or modify it   **
 ** under the terms of the BSD License. Refer to the shawn-licence.txt **
 ** file in the root of the Shawn source tree for further details.     **
 ************************************************************************/
#ifndef __SHAWN_LEGACYAPPS_AODVUGD_NODES_PINGS_TASK_H
#define __SHAWN_LEGACYAPPS_AODVUGD_NODES_PINGS_TASK_H
#include "_apps_enable_cmake.h"
#ifdef ENABLE_AODVUGD

#include "sys/simulation/simulation_controller.h"
#include "sys/simulation/simulation_task.h"

namespace aodvugd
{
   class NodesPingsTask
      : public shawn::SimulationTask
   {
   public:
      NodesPingsTask();
      virtual ~NodesPingsTask();

      virtual std::string name( void ) const throw();
      virtual std::string description( void ) const throw();

      virtual void run( shawn::SimulationController& sc )
         throw( std::runtime_error );

   };
   
// ----------------------------------------------------------------------
      class NodesChangeNameTask
      : public shawn::SimulationTask
   {
   public:
      NodesChangeNameTask();
      virtual ~NodesChangeNameTask();

      virtual std::string name( void ) const throw();
      virtual std::string description( void ) const throw();

      virtual void run( shawn::SimulationController& sc )
         throw( std::runtime_error );

   };

}

#endif
#endif
