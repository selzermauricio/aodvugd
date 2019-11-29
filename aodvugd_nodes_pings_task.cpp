/************************************************************************
 ** This file is part of the network simulator Shawn.                  **
 ** Copyright (C) 2004-2007 by the SwarmNet (www.swarmnet.de) project  **
 ** Shawn is free software; you can redistribute it and/or modify it   **
 ** under the terms of the BSD License. Refer to the shawn-licence.txt **
 ** file in the root of the Shawn source tree for further details.     **
 ************************************************************************/
#include "_legacyapps_enable_cmake.h"
#include "legacyapps/aodvugd/aodvugd_nodes_pings_task.h"
#ifdef ENABLE_AODVUGD

#include "sys/world.h"

#include "legacyapps/aodvugd/aodvugd_ping_app.h"

namespace aodvugd
{


   NodesPingsTask::
   NodesPingsTask()
   {}
   // ----------------------------------------------------------------------
   NodesPingsTask::
   ~NodesPingsTask()
   {}
   // ----------------------------------------------------------------------
   std::string
   NodesPingsTask::
   name( void )
      const throw()
   {
      return "nodes_pings";
   }
   // ----------------------------------------------------------------------
   std::string
   NodesPingsTask::
   description( void )
      const throw()
   {
      return "Shawn AODV task: los nodos realizan pings entre ellos";
   }
   // ----------------------------------------------------------------------
   void
   NodesPingsTask::
   run( shawn::SimulationController& sc )
      throw( std::runtime_error )
   {
      require_world( sc );

      std::cout<< "--------------------------Tiempo:" <<
      sc.world().simulation_round()
      <<std::endl;

      shawn::Node* nodoSender;
      const shawn::Node* nodoDestino;


      nodoSender        =  sc.world_w().find_node_by_id_w ( 0 );
      nodoDestino       =  sc.world().find_node_by_id ( 316 ); //8
      std::string direccionDestino  = nodoDestino->label();

      //si o si hay que inicializar ahora los nodos.. asi mueve de la lista
      //new_processors a processors
      //nodoSender->init();
      PingApp *pPingApp = nodoSender->get_processor_of_type_w <PingApp> ();
      assert ( pPingApp != NULL );

      double tiempoInicioPing = 6.00;
      pPingApp->    programarPrimerPing (   direccionDestino           ,  1    /*maxPings*/    , 
                                            true  /*pingInfinito*/    ,  50 /*timeoutPing*/ ,
                                            64     /*ttl*/             ,  116        ,
                                            tiempoInicioPing   ) ;

    
/*      for( shawn::World::const_node_iterator
               it = sc.world().begin_nodes();
               it != sc.world().end_nodes();
               ++it )
      {
         count++;
      }
      
      INFO( logger(), "visited " << count << " nodes" );*/
   }

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
   NodesChangeNameTask::
   NodesChangeNameTask()
   {}
   // ----------------------------------------------------------------------
   NodesChangeNameTask::
   ~NodesChangeNameTask()
   {}
   // ----------------------------------------------------------------------
   std::string
   NodesChangeNameTask::
   name( void )
      const throw()
   {
      return "nodes_change_name";
   }
   // ----------------------------------------------------------------------
   std::string
   NodesChangeNameTask::
   description( void )
      const throw()
   {
      return "Shawn AODV task: se cambian los nombre de los nodos";
   }
   // ----------------------------------------------------------------------
   void
   NodesChangeNameTask::
   run( shawn::SimulationController& sc )
      throw( std::runtime_error )
   {
      require_world( sc );

       for(shawn::World::node_iterator  it  = sc.world_w().begin_nodes_w(), 
                                      endit = sc.world_w().end_nodes_w(); 
           it != endit  ;  ++it )
       {
         //cambiamos el label para que sean solo numeros
         std::ostringstream nodoId;
         nodoId<<"Nodo-"<< it->id();
         it->set_label ( nodoId.str() );
       }
   }



}
#endif

