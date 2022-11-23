#include "locmodel.h"
#include "configpath.h"

#include <QXmlStreamReader>
#include <QSettings>
#include <QFile>
#include <QTranslator>

#ifdef QT_DEBUG
#include <QDebug>
#endif

locList::locList(QObject *parent)
    : QAbstractTableModel(parent),
    m_pParent(parent),
    m_nCurrentLocFile(-1),
    m_LocFiles(0)
{
    m_pTranslator = new QTranslator(parent);
    updateList();
}

QVariant locList::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    locListData aData = m_LocFiles[index.row()];
    switch (role)
    {
    case Qt::DisplayRole: {
        if (index.column() == 0)
            return aData.NativeName;
        else if (index.column() == 1)
            return aData.EnglishName;
        else if (index.column() == 2)
            return aData.AvailableVersion;
        else if (index.column() == 3)
            return aData.InstalledVersion;
        else if (index.column() == 4)
            return aData.FileName;
        break;
    }
    case FileNameRole: {
        qDebug() << aData.FileName;
        return aData.FileName;
        break;
    }
    case IsLoadedRole: {
        return (index.row() == m_nCurrentLocFile);
        break;
    }
    //TODO: localization: author
/*
    case AuthorRole: if (index.row() == m_nCurrentDictionary) {
        return "Hello World";
        break;
    }
*/
    } //switch
    return QVariant();
}

QVariant locList::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
    {
        if (section == 0)
            return tr("Native");
        else if (section == 1)
            return tr("English");
        else if (section == 2)
            return tr("Available");
        else if (section == 3)
            return tr("Local");
        else if (section == 4)
            return tr("Filename");
    }
    return QVariant();
}

int locList::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_LocFiles.count();
}

int locList::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

void locList::setIsLoaded(const QString fileName)
{
    beginResetModel();
    for (int i=0; i<m_LocFiles.count(); i++)
        if (m_LocFiles[i].FileName == fileName)  {
            m_nCurrentLocFile = i;
            break;
        }
    endResetModel();
}

bool locList::removeLocFile(const QString fileName)
{
    QFile file( config::file(fileName) );
    if (file.exists()) {
        if ( file.remove() ) {
            beginResetModel();
            updateList();
            endResetModel();
            return true;
        };
        return false;
    };
    return false;
}

QHash<int, QByteArray> locList::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[IsLoadedRole] = "isLoaded";
    roles[AuthorRole] = "author";
    roles[LicenseRole] = "license";
    roles[FileNameRole] = "filename";
    return roles;
}

void locList::updateList()
{
    QFile file(config::conf());

    m_LocFiles.clear();

    //content in .conf file retrieved from github via downloadmanager at program start
    beginResetModel();
    if ( file.open(QIODevice::ReadOnly) )
    {
        QXmlStreamReader xmlReader;
        xmlReader.setDevice(&file);
        while (!xmlReader.atEnd())
        {
            xmlReader.readNext();

            if (xmlReader.isStartElement() &&
                (xmlReader.name().toString() == "localization"))
            {
                locListData aData;
                aData.NativeName=xmlReader.attributes().value("Native").toString();
                aData.EnglishName=xmlReader.attributes().value("English").toString();
                aData.AvailableVersion=xmlReader.attributes().value("RemoteVersion").toString();
                aData.FileName=xmlReader.attributes().value("FileName").toString();
                //content in *.qm file
                if ( m_pTranslator->load(aData.FileName, config::file("")) ) {
                    qApp->installTranslator(m_pTranslator);
                    QString l10n_Version;
                    //: replace by the current version of this localization
                    l10n_Version = tr("0.0.0");
                    aData.InstalledVersion = qApp->translate("", l10n_Version.toLocal8Bit().data() );
                    QString l10n_Author;
                    //: replaced by the name of the translation author
                    l10n_Author = tr("unknown");
                    aData.Author = qApp->translate("", l10n_Author.toLocal8Bit().data() );
                    qApp->removeTranslator(m_pTranslator);
                }
                m_LocFiles.append(aData);
            }
        }
        file.close();
    }
    endResetModel();
    //no need to go through local files not stored on github
}
