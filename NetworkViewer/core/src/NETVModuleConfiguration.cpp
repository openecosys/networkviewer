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

#include "NETVModuleConfiguration.h"
#include <QFile>
#include <QCoreApplication>
#include <QMimeData>
#include <QIcon>

namespace netcore
{

    NETVModuleConfiguration::NETVModuleConfiguration(QObject *parent)
        : QAbstractItemModel(parent), m_projectID(-1), m_codeVersion(0), m_processorID(0), m_moduleState(0), m_tableVersion(0), m_deviceID(0), m_name("Untitled")
    {

    }

    NETVModuleConfiguration::NETVModuleConfiguration(int projectID, int codeVersion, int processorID, int moduleState, int tableVersion, int deviceID, QObject *parent)
        : QAbstractItemModel(parent),
        m_projectID(projectID),
        m_codeVersion(codeVersion),
        m_processorID(processorID),
        m_moduleState(moduleState),
        m_tableVersion(tableVersion),
        m_deviceID(deviceID),
        m_name("Untitled2")
    {
        //Starting by searching project file
        QStringList configList = scanConfigurations(QCoreApplication::applicationDirPath() + "/../config",projectID,-1);

        qDebug("Found %i matching configuration (s)",configList.size());

        if (configList.size() == 1)
        {
            //Loading configuration, variables only
            loadConfiguration(configList[0], true);
        }
        else if (configList.size() > 1)
        {
            qDebug("More than one configuration found. Load it manually.");
        }

    }

    NETVModuleConfiguration::NETVModuleConfiguration(const NETVModuleConfiguration &cpy)
        : QAbstractItemModel(NULL)
    {
        m_projectID = cpy.m_projectID;
        m_codeVersion = cpy.m_codeVersion;
        m_processorID = cpy.m_processorID;
        m_moduleState = cpy.m_moduleState;
        m_tableVersion = cpy.m_tableVersion;
        m_deviceID = cpy.m_deviceID;
        m_filename = cpy.m_filename;
        m_name = cpy.m_name;

        //Deep Copy variables
        for (int i = 0; i  < cpy.m_variables.size(); i++)
        {
            addVariable(new NETVVariable(*(cpy.m_variables[i])));
        }
    }

    NETVModuleConfiguration::~NETVModuleConfiguration()
    {
        qDebug("NETVModuleConfiguration::~NETVModuleConfiguration()");

        //Destroy variables
        while (m_variables.size() > 0)
        {
            removeVariable(m_variables.front());
        }


        qDebug("NETVModuleConfiguration::~NETVModuleConfiguration() (done)");
    }


    QModelIndex NETVModuleConfiguration::index(int row, int column, const QModelIndex &parent) const
    {
        if (row >= rowCount(parent) || column >= columnCount(parent))
        {
            return QModelIndex();
        }
        else
        {
            return createIndex(row,column,m_variables[row]);
        }
    }


    QModelIndex NETVModuleConfiguration::parent(const QModelIndex &child) const
    {
        return QModelIndex();
    }


    int NETVModuleConfiguration::rowCount(const QModelIndex &parent) const
    {
        return m_variables.size();
    }

    int NETVModuleConfiguration::columnCount(const QModelIndex &parent) const
    {
        return VARIABLE_ENUM_SIZE;
    }

    QVariant NETVModuleConfiguration::data(const QModelIndex &index, int role) const
    {

        /*
    The general purpose roles (and the associated types) are:

        Qt::DisplayRole             0	The key data to be rendered in the form of text. (QString)
        Qt::DecorationRole          1	The data to be rendered as a decoration in the form of an icon. (QColor, QIcon or QPixmap)
        Qt::EditRole                2	The data in a form suitable for editing in an editor. (QString)
        Qt::ToolTipRole             3	The data displayed in the item's tooltip. (QString)
        Qt::StatusTipRole           4	The data displayed in the status bar. (QString)
        Qt::WhatsThisRole           5	The data displayed for the item in "What's This?" mode. (QString)
        Qt::SizeHintRole            13	The size hint for the item that will be supplied to views. (QSize)

    Roles describing appearance and meta data (with associated types):

        Qt::FontRole                6	The font used for items rendered with the default delegate. (QFont)
        Qt::TextAlignmentRole	7	The alignment of the text for items rendered with the default delegate. (Qt::AlignmentFlag)
        Qt::BackgroundRole          8	The background brush used for items rendered with the default delegate. (QBrush)
        Qt::BackgroundColorRole	8	This role is obsolete. Use BackgroundRole instead.
        Qt::ForegroundRole          9	The foreground brush (text color, typically) used for items rendered with the default delegate. (QBrush)
        Qt::TextColorRole           9	This role is obsolete. Use ForegroundRole instead.
        Qt::CheckStateRole          10	This role is used to obtain the checked state of an item. (Qt::CheckState)

    Accessibility roles (with associated types):

        Qt::AccessibleTextRole              11	The text to be used by accessibility extensions and plugins, such as screen readers. (QString)
        Qt::AccessibleDescriptionRole	12	A description of the item for accessibility purposes. (QString)

    User roles:

        Qt::UserRole	32	The first role that can be used for application-specific purposes.

    */

        NETVVariable *var = reinterpret_cast<NETVVariable*> (index.internalPointer());

        if (!var)
        {
            qWarning("QVariant NETVModuleConfiguration::data(const QModelIndex &index, int role) : Invalid internal pointer");
            return QVariant::Invalid;
        }

        if (role == Qt::DisplayRole)
        {
            switch(index.column())
            {
            case VARIABLE_ACTIVATED:
                if (var->getActivated())
                {
                    return "On";
                }
                else
                {
                    return "Off";
                }

                break;

            case VARIABLE_NAME:
                return QVariant(var->getName());
                break;

            case VARIABLE_VALUE_TYPE:
                return QVariant(NETVVariable::typeToString(var->getType()));
                break;

            case VARIABLE_MEMORY_TYPE:

                switch (var->getMemType())
                {
                case NETVVariable::RAM_VARIABLE:
                    return QVariant("RAM");
                    break;

                case NETVVariable::EEPROM_VARIABLE:
                    return QVariant("EEPROM");
                    break;

                case NETVVariable::SCRIPT_VARIABLE:
                    return QVariant("SCRIPT");
                    break;
                }



                break;

            case VARIABLE_MEMORY_OFFSET:
                return QVariant(var->getOffset());
                break;

            case VARIABLE_VALUE:
                return var->getValue();
                break;

            case VARIABLE_ELAPSED:
                return QString::number(var->getElapsedTime());
                break;

            case VARIABLE_DESCRIPTION:
                return QVariant(var->getDescription());
                break;

            default:
                return QVariant::Invalid;
                break;
            }
        }
        else if (role == Qt::DecorationRole)
        {
            if (index.column() == VARIABLE_NAME)
            {
                return QVariant(QIcon(":/images/add.png"));
            }
        }
        else if (role == Qt::CheckStateRole)
        {
            if (index.column() == VARIABLE_ACTIVATED)
            {
                if (var->getActivated())
                {
                    return Qt::Checked;
                }
                else
                {
                    return Qt::Unchecked;
                }

            }
        }

        return QVariant::Invalid;
    }

    QMap<int, QVariant> NETVModuleConfiguration::itemData ( const QModelIndex & index ) const
    {
        //qDebug() << "QMap<int, QVariant> NETVModuleConfiguration::itemData ( const QModelIndex & index )" << index;
        QMap<int,QVariant> allData = QAbstractItemModel::itemData(index);

        //Add user role
        allData[Qt::UserRole] = QVariant((unsigned long long) m_variables[index.row()]);

        return allData;
    }

    QVariant NETVModuleConfiguration::headerData ( int section, Qt::Orientation orientation, int role) const
    {
        if (role == Qt::DisplayRole)
        {

            if (orientation == Qt::Horizontal)
            {
                switch(section)
                {
                case VARIABLE_ACTIVATED:
                    return "Activated";
                    break;

                case VARIABLE_NAME:
                    return "Name";
                    break;

                case VARIABLE_VALUE_TYPE:
                    return "Value Type";
                    break;

                case VARIABLE_MEMORY_TYPE:
                    return "Mem Type";
                    break;

                case VARIABLE_MEMORY_OFFSET:
                    return "Mem Offset";
                    break;

                case VARIABLE_VALUE:
                    return "Value";
                    break;

                case VARIABLE_ELAPSED:
                    return "Elapsed";
                    break;

                case VARIABLE_DESCRIPTION:
                    return "Description";
                    break;

                default:
                    return QVariant::Invalid;
                    break;
                }
            }
            else if (orientation == Qt::Vertical)
            {
                return QString::number(section);
            }
        }


        return QAbstractItemModel::headerData(section,orientation,role);
    }

    Qt::ItemFlags NETVModuleConfiguration::flags (const QModelIndex & index ) const
    {
        /*
        Qt::NoItemFlags	0	It does not have any properties set.
        Qt::ItemIsSelectable	1	It can be selected.
        Qt::ItemIsEditable	2	It can be edited.
        Qt::ItemIsDragEnabled	4	It can be dragged.
        Qt::ItemIsDropEnabled	8	It can be used as a drop target.
        Qt::ItemIsUserCheckable	16	It can be checked or unchecked by the user.
        Qt::ItemIsEnabled	32	The user can interact with the item.
        Qt::ItemIsTristate	64	The item is checkable with three separate states.
        */

        switch (index.column())
        {
        case VARIABLE_ACTIVATED:
            return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
            break;

        case VARIABLE_NAME:
            return Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable;
            break;

        case VARIABLE_VALUE_TYPE:
            return Qt::ItemIsEnabled;
            break;

        case VARIABLE_MEMORY_TYPE:
            return Qt::ItemIsEnabled;
            break;

        case VARIABLE_MEMORY_OFFSET:
            return Qt::ItemIsEnabled;
            break;

        case VARIABLE_VALUE:
            return Qt::ItemIsEditable | Qt::ItemIsEnabled;
            break;

        case VARIABLE_ELAPSED:
            return Qt::ItemIsEnabled;
            break;

        case VARIABLE_DESCRIPTION:
            return Qt::ItemIsEnabled;
            break;

        default:
            return Qt::ItemIsEnabled;
            break;
        }

        //This should not happen
        return Qt::NoItemFlags;
    }

    bool NETVModuleConfiguration::setData ( const QModelIndex & index, const QVariant & value, int role)
    {
        //qDebug("NETVModuleConfiguration::setData ( const QModelIndex & index, const QVariant & value, int role = %i)",role);

        NETVVariable *var = reinterpret_cast<NETVVariable*> (index.internalPointer());

        if (index.column() == VARIABLE_ACTIVATED && index.row() < m_variables.size())
        {
            if (var)
            {
                //qDebug() << "Activated value : " << value;
                var->setActivated(!var->getActivated());
                return true;
            }
        }
        if (index.column() == VARIABLE_VALUE && index.row() < m_variables.size())
        {
            if(var)
            {
                var->setUserValue(value);
                return true;
            }
        }

        return false;
    }

    NETVModuleConfiguration& NETVModuleConfiguration::operator= (const NETVModuleConfiguration& cpy)
                                                        {
        m_projectID = cpy.m_projectID;
        m_codeVersion = cpy.m_codeVersion;
        m_processorID = cpy.m_processorID;
        m_moduleState = cpy.m_moduleState;
        m_tableVersion = cpy.m_tableVersion;
        m_deviceID = cpy.m_deviceID;
        m_filename = cpy.m_filename;
        m_name = cpy.m_name;

        //Deep Copy variables
        for (int i = 0; i  < cpy.m_variables.size(); i++)
        {
            addVariable(new NETVVariable(*(cpy.m_variables[i])));
        }

        return *this;
    }

    void NETVModuleConfiguration::prettyPrint(QIODevice &device)
    {

        QTextStream stream(&device);

        stream << "<b>projectID    : </b>" << m_projectID << "\r\n";
        stream << "<b>codeVersion  : </b>" << m_codeVersion << "\r\n";
        stream << "<b>processorID  : </b>" << m_processorID << "\r\n";
        stream << "<b>moduleState  : </b>" << m_moduleState << "\r\n";
        stream << "<b>tableVersion : </b>" << m_tableVersion << "\r\n";
        stream << "<b>deviceID     : </b>" << m_deviceID << "\r\n";
    }


    bool NETVModuleConfiguration::saveConfiguration(const QString &filename)
    {
        QDomDocument document("INTROLAB-NETWORKVIEWER");

        QDomElement element = document.createElement("NETVModuleConfiguration");

        element.setAttribute("projectID",QString::number(m_projectID));
        element.setAttribute("codeVersion",QString::number(m_codeVersion));
        element.setAttribute("processorID",QString::number(m_processorID));
        element.setAttribute("moduleState",QString::number(m_moduleState));
        element.setAttribute("tableVersion",QString::number(m_tableVersion));
        element.setAttribute("deviceID",QString::number(m_deviceID));
        element.setAttribute("configName",m_name);

        for (int i = 0; i<m_variables.size(); i++)
        {
            m_variables[i]->saveXML(document,element);
        }

        document.appendChild(element);

        QFile file(filename);
        if (file.open(QIODevice::WriteOnly))
        {

            m_filename = filename;
            QTextStream stream(&file);
            document.save(stream, 5);
            return true;
        }


        return false;
    }

    int NETVModuleConfiguration::size() const
    {
        return m_variables.size();
    }

    NETVVariable* NETVModuleConfiguration::operator[] (int index) const
    {
        Q_ASSERT(index < m_variables.size() && index >= 0);
        return m_variables[index];
    }

    void NETVModuleConfiguration::addVariable(NETVVariable *variable)
    {
        m_variables.push_back(variable);

        //Make sure the variable knows to which device ID it belongs
        variable->setDeviceID(m_deviceID);

        //TODO make sure the variable know on wich interface it is mapped

        //Connect signals for user modification
        connect(variable, SIGNAL(userChanged(NETVVariable*)), this, SLOT(variableUpdated(NETVVariable*)));


        //Abstract model notification
        connect(variable,SIGNAL(valueChanged(NETVVariable*)),this,SLOT(variableInternalUpdate(NETVVariable*)));
        connect(variable,SIGNAL(variableActivated(bool,NETVVariable*)),this,SLOT(variableInternalActivated(bool,NETVVariable*)));
        connect(variable,SIGNAL(updateTimeChanged(NETVVariable*)),this,SLOT(variableInternalTimeUpdated(NETVVariable*)));

        //Emit variable added
        emit variableAdded(variable);


        //QAbstractItemModel signal
        emit layoutChanged();

    }


    int NETVModuleConfiguration::getProjectID()
    {
        return m_projectID;
    }

    int NETVModuleConfiguration::getCodeVersion()
    {
        return m_codeVersion;
    }

    int NETVModuleConfiguration::getProcessorID()
    {
        return m_processorID;
    }

    int NETVModuleConfiguration::getModuleState()
    {
        return m_moduleState;
    }

    int NETVModuleConfiguration::getTableVersion()
    {
        return m_tableVersion;
    }

    int NETVModuleConfiguration::getDeviceID()
    {
        return m_deviceID;
    }

    void NETVModuleConfiguration::setDeviceID(int id)
    {
        m_deviceID = id;
    }

    void NETVModuleConfiguration::setModuleState(int state)
    {
        m_moduleState = state;
        emit moduleStateChanged();
    }


    bool NETVModuleConfiguration::loadConfiguration(const QString &filename, bool variablesOnly)
    {
        QDomDocument doc("INTROLAB-NETWORKVIEWER");
        QFile file(filename);

        //Open the file
        if(!file.open(QIODevice::ReadOnly))
        {
            //QMessageBox::warning(0, "Warning", QString("Unable to open file : ") + filename, QMessageBox::Ok);
            qWarning() << "Unable to open file : " << filename;
            return false;
        }

        m_filename = filename;

        //Proceed to load only if file is ok.
        emit configurationAboutToLoad();

        //Clean up memory
        while(m_variables.size() > 0)
        {
            removeVariable(m_variables.front());
        }

        qDebug() << "Loading : "<<filename;

        //Set XML content
        if (!doc.setContent(&file)) {
            file.close();
            //QMessageBox::warning(0, "Warning", QString("Unable to read XML content from file : ") + filename, QMessageBox::Ok);
            qWarning() << "Unable to read XML content from file : " << filename;
            return false;
        }

        //Verify if we have a configuration
        //Get the root element
        QDomElement e = doc.documentElement(); // <ModuleConfiguration> or <NETVModuleConfiguration> element
        if(e.tagName() != "ModuleConfiguration" && e.tagName() != "NETVModuleConfiguration")
        {
            qDebug() <<"No ModuleConfiguration found. Element is "<< e.tagName();
            return false;
        }

        if (!variablesOnly)
        {
            //Get parameters
            m_projectID = e.attribute("projectID").toInt();
            m_codeVersion = e.attribute("codeVersion").toInt();
            m_processorID = e.attribute("processorID").toInt();
            m_moduleState = e.attribute("moduleState").toInt();
            m_tableVersion = e.attribute("tableVersion").toInt();
            m_deviceID = e.attribute("deviceID").toInt();
        }

        //Newly added name
        if (e.hasAttribute("configName"))
        {
            m_name = e.attribute("configName");
            qDebug() << "Config Name Found : " << m_name;
        }

        //qDebug() << "Found element : "<<e.tagName();

        //Iterate through all child (this would be variables)
        QDomNode n = e.firstChild();

        while(!n.isNull())
        {
            e = n.toElement(); // try to convert the node to an element.

            if(!e.isNull())
            {
                //Allow "ModuleVariable or NETVVariable" for compatibility with older versions
                if(e.tagName() == "ModuleVariable" || (e.tagName() == "NETVVariable"))
                {
                    //Creating variable from XML
                    NETVVariable *var = new NETVVariable(e);

                    //Adding the variable
                    addVariable(var);

                }
            }//if e.isNull
            else
            {
                qDebug("NULL element");
            }

            //Next item
            n = n.nextSibling();
        }//while n.isNull

        return true;
    }


    void NETVModuleConfiguration::variableUpdated(NETVVariable *var)
    {
        //qDebug("NETVModuleConfiguration::variableUpdated(const NETVVariable &var)");
        emit variableWrite(var);
    }

    void NETVModuleConfiguration::variableInternalUpdate(NETVVariable *var)
    {
        //AbstractItemModel must know when something changed.
        //TODO : Can this be made more efficient
        QModelIndex myIndex(index(indexOf(var),VARIABLE_VALUE));
        emit dataChanged (myIndex,myIndex) ;
    }

    void NETVModuleConfiguration::variableInternalActivated(bool state, NETVVariable *var)
    {
        //QAbstractItemModel signal
        QModelIndex myIndex(index(indexOf(var),VARIABLE_ACTIVATED));
        emit dataChanged (myIndex,myIndex) ;
    }


    QStringList NETVModuleConfiguration::scanConfigurations(const QString &basePath, int project_id, int device_id)
    {
        qDebug("NETVModuleConfiguration::scanConfigurations");

        QStringList scanList;

        //Plugins directory
        QDir dir(basePath);

        if (!dir.exists())
        {
            qWarning("NETVModuleConfiguration::scanConfigurations : plugins directory not found");
            qWarning() << "NETVModuleConfiguration::scanConfigurations : Current path : " << dir.absolutePath();
        }
        else
        {
            qDebug() << "NETVModuleConfiguration::scanConfigurations : Scanning : " << dir.absolutePath();
            recursiveScan(scanList, project_id, device_id, dir);
        }

        return scanList;
    }

    void NETVModuleConfiguration::recursiveScan(QStringList &configList, int project_id, int device_id, QDir directory, int level)
    {
        qDebug() << "NETVModuleConfiguration::recursiveScan :  Path : " << directory.path() << " project_id : "<<project_id<<" device_id : "<<device_id;

        if (level < 10 && directory.exists())
        {

            QFileInfoList myInfoList = directory.entryInfoList();

            for (int i = 0; i < myInfoList.size(); i++)
            {
                if (!myInfoList[i].fileName().startsWith("."))
                {
                    if (myInfoList[i].isDir())
                    {
                        recursiveScan(configList, project_id, device_id, QDir(directory.path() + QDir::separator() + myInfoList[i].fileName()), level + 1);
                    }
                    else
                    {
                        //Look for standard configuration file xml type
                        if (myInfoList[i].fileName().contains(".xml"))
                        {
                            NETVModuleConfiguration tempConfig;
                            tempConfig.loadConfiguration(directory.path() + QDir::separator() + myInfoList[i].fileName());

                            //If device_id does not matter (-1) put to actual device id
                            int my_device_id = device_id;
                            if (device_id == -1)
                            {
                                my_device_id = tempConfig.getDeviceID();
                            }

                            //Insert configuration if it fits the project_id and device_id
                            if (tempConfig.getProjectID() == project_id && tempConfig.getDeviceID() == my_device_id)
                            {
                                qDebug() << "Found a valid configuration, inserting : " << directory.path() + QDir::separator() + myInfoList[i].fileName();
                                configList.push_back(directory.path() + QDir::separator() + myInfoList[i].fileName());
                            }
                        }
                    }
                }//does not start with "."
            }//for infoList
        }
        else
        {
            qWarning("NETVModuleConfiguration::recursiveScan : error level : %i",level);
        }

    }

    NETVVariable* NETVModuleConfiguration::getVariableNamed(QString name)
    {
        NETVVariable *variable = NULL;

        for (int i = 0; i < m_variables.size(); i++)
        {
            if (m_variables[i]->getName() == name)
            {
                return m_variables[i];
            }
        }

        return variable;
    }

    int NETVModuleConfiguration::indexOf(NETVVariable *var)
    {
        return m_variables.lastIndexOf(var);
    }

    void NETVModuleConfiguration::removeVariable(NETVVariable *variable)
    {
        if (variable && m_variables.contains(variable))
        {
            //This must occur in this order...
            m_variables.removeAll(variable);
            emit variableRemoved(variable);

            //All signals will be disconnected
            delete variable;

            //QAbstractItemModel signal
            emit layoutChanged();
        }
    }

    QString NETVModuleConfiguration::getFilename() const
    {
        return m_filename;
    }

    QString NETVModuleConfiguration::getConfigName() const
    {
        return m_name;
    }

    void NETVModuleConfiguration::setConfigName(const QString &name)
    {
        m_name = name;
    }

    void NETVModuleConfiguration::variableInternalTimeUpdated(NETVVariable *var)
    {
        if (var)
        {
            //Update time
            QModelIndex myIndex(index(indexOf(var),VARIABLE_ELAPSED));
            emit dataChanged (myIndex,myIndex) ;
        }
    }

} //namespace netcore
