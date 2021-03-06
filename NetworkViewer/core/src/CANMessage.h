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

#ifndef _CAN_MESSAGE_H_
#define _CAN_MESSAGE_H_

#include "Core.h"
#include "CoreMessage.h"

namespace netcore
{
    class NETVCORE_EXPORT CANMessage : public CoreMessage
    {
    public:
        static const quint32 MaxPayloadSize = 8;

        //flags must be bit maskable
        static const quint32 NoFlag       = 0x00000000;
        static const quint32 RTRFlag      = 0x00000001;
        static const quint32 ExtendedFlag = 0x00000002;
        static const quint32 ErrorFlag    = 0x00000004;
        static const quint32 InvalidFlag  = 0x00000008;

        //Constructors
        CANMessage(const CANMessage &cpy);
        CANMessage(quint32 eid, quint32 flags, const QByteArray &data = QByteArray());

        //Operators
        CANMessage& operator=(const CANMessage& cpy);

        //Visitor pattern for serialization / deserialization
        virtual bool serialize(CoreSerializer& ser, QIODevice &dev);
        virtual bool deserialize(CoreDeserializer &des, QIODevice &dev);


        ///Cloning message
        virtual CoreMessage* clone() const;

        //Setters
        void setFlags(quint32 flags);
        void clearFlags(quint32 flags);
        bool setPayload(const QByteArray &data);

        //Getters
        QByteArray payload() const;
        int payloadSize() const;
        bool isExtended() const;
        bool isError() const;
        bool isRemote() const;
        bool isValid() const;
        quint32 eid() const;
        quint32 flags() const;
        int maxPayloadSize() const;


    protected:
        quint32 m_eid;
        quint32 m_flags;
        QByteArray m_data;

    private:

        CANMessage();
        bool validityCheck();
    };

}

#endif //_CAN_MESSAGE_H_
