/**
     Copyright (C) 2009-2013 IntRoLab
     http://introlab.3it.usherbrooke.ca
     Dominic Letourneau, ing. M.Sc.A.
     Dominic.Letourneau@USherbrooke.ca

     This file is part of OpenECoSys/NetworkViewer.
     OpenECoSys/NetworkViewer is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by the Free Software
     Foundation, either version 3 of the License, or (at your option) any later version.
     OpenECoSys/NetworkViewer is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
     or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
     You should have received a copy of the GNU General Public License along with
     OpenECoSys/NetworkViewer. If not, see http://www.gnu.org/licenses/.
 */

#include "Loopback.h"
#include "CoreDriverFactory.h"

namespace netcore
{


    class LoopbackDriverRegistry
    {
    public:
        //Factory should register itself when the library is loaded
        // CoreDriverInfo(QString name, quint32 major, quint32 minor, quint32 micro, QString description, QStringList defaultArgs)
        static bool loaded;
    };

    bool LoopbackDriverRegistry::loaded = CoreDriverFactoryBase::registerDriverFactory(
                Loopback::internalInfo(),new CoreDriverFactory<Loopback>());

    Loopback::Loopback(QObject *parent)
        : CoreDriver(parent)
    {

    }

    CoreDriverInfo Loopback::internalInfo()
    {
        static CoreDriverInfo info("Loopback",1,0,0,"Description",QStringList());
        return info;
    }

    CoreDriverInfo Loopback::info()
    {
        return Loopback::internalInfo();
    }

    CoreDriver::CoreDriverState Loopback::initialize(QStringList args)
    {
        return CoreDriver::DRIVER_OK;
    }

    void Loopback::terminate()
    {

    }

    CoreDriver::CoreDriverState Loopback::state()
    {
        return CoreDriver::DRIVER_OK;
    }

    bool Loopback::internalThreadRecvFunction()
    {
        return true;
    }

    bool Loopback::internalThreadSendFunction()
    {
        return true;
    }

}
