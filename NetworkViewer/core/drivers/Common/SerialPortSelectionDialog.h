/**
     Copyright (C) 2009-2012 IntRoLab
     http://introlab.gel.usherbrooke.ca
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

#ifndef _SERIAL_PORT_SELECTION_DIALOG_H_
#define _SERIAL_PORT_SELECTION_DIALOG_H_

#include <QDialog>
#include <QSerialPortInfo>
#include "DriversCommon.h"
#include "ui_SerialPortSelectionDialog.h"

class DRIVERS_COMMON_EXPORT SerialPortSelectionDialog : public QDialog, public Ui::SerialPortSelectionDialog
{
    Q_OBJECT

public:

    SerialPortSelectionDialog(QWidget *parent = NULL);
    QString getSerialPortString();
    QString getBaudRateString();
    bool getDebugStatus();
    int getDelay();

protected slots:

    void portIndexChanged (int index );
    void baudRateIndexChanged (int index);
    void updateInfo(const QSerialPortInfo &info);

protected:

    int m_currentPortIndex;
    int m_currentBaudRateIndex;

};


#endif
