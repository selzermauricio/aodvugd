#ifndef __SHAWN_LEGACYAPPS_AODVUGD_OBSERVER_ESTADO_RUTA_H
#define __SHAWN_LEGACYAPPS_AODVUGD_OBSERVER_ESTADO_RUTA_H


namespace aodvugd
{
    /**
      *
      */
    //template<typename T>
    class ObserverEstadoRuta
    {
    protected:
        //con un metodo puro alcanza para que sea abstracta y no se pueda instanciar
        virtual void handleAvisoRutaActiva (std::string destino)     throw()=0;
        //como el observador se va a regitrar en la tabla
        virtual void observarUnaRuta(  std::string destino, std::string estado )
                                                                     throw()=0;
        virtual void cancelarObservarUnaRuta(  std::string destino, std::string estado )
                                                                     throw()=0;   

		//virtual void observers_added(T& obs) throw() = 0;

    public:
    //----------------------------------------------------------------------
		virtual ~ObserverEstadoRuta()
		{}
    //----------------------------------------------------------------------
        //maneja los avisos sobre los cambio de los estados de las rutas de su interes
        virtual void onEstadoRutaChange(std::string destino , std::string estado) throw()
        {
            if(estado == "active")
                handleAvisoRutaActiva(destino);
        }
    //----------------------------------------------------------------------

    };
    
}
    
#endif
    
/*-----------------------------------------------------------------------
* Source  $Source: /cvs/shawn/shawn/sys/misc/observable.h,v $
* Version $Revision: 38 $
* Date    $Date: 2007-06-08 14:30:12 +0200 (Fri, 08 Jun 2007) $
*-----------------------------------------------------------------------
* $Log: observable.h,v $
 *-----------------------------------------------------------------------*/

    
