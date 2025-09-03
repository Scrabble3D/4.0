#include "locmodel.h"
#include "configpath.h"
#include "version.h"

#include <QXmlStreamReader>
#include <QSettings>
#include <QFile>
#include <QTranslator>

locList::locList(QObject *parent)
{
    m_pTranslator = new QTranslator(parent);
}

QVariant locList::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    locListData aData = m_LocFiles[index.row()];
    switch (role) {
        case NativeRole: return aData.NativeName; break;
        case EnglishRole: return aData.EnglishName; break;
        case AvailableVersionRole: return aData.AvailableVersion; break;
        case FileNameRole: return aData.FileName; break;
        case InstalledVersionRole: return aData.InstalledVersion; break;
        case AuthorRole: return aData.Author; break;
        case IsLoadedRole: return (index.row() == m_nLoadedLocFile); break;
    } //switch

    return QVariant();
}

int locList::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_LocFiles.count();
}

QVariantMap locList::get(int index) const
{
    QVariantMap data;

    if ((index > -1) && (index < m_LocFiles.count())) {
        locListData aEntry = m_LocFiles[index];
        data["native"] = aEntry.NativeName;
        data["english"] = aEntry.EnglishName;
        data["filename"] = aEntry.FileName;
        data["availableversion"] = aEntry.AvailableVersion;
        data["installedversion"] = aEntry.InstalledVersion;
        data["author"] = aEntry.Author;
        data["isLoaded"] = (index == m_nLoadedLocFile);
    }
    return data;
}

int locList::indexOf(const QString fileName)
{
    int aIndex = -1;
    for (int i=0; i<m_LocFiles.count(); i++)
        if (m_LocFiles[i].FileName == fileName)  {
            aIndex = i;
            break;
        }
    return aIndex;
}

void locList::loadingFinished(const QString fileName)
{
    const int nIndex = indexOf(fileName);
    if ( nIndex > -1 )
    {
        beginResetModel();
        m_sCurrentLang = m_LocFiles[nIndex].NativeName + " (" + m_LocFiles[nIndex].EnglishName +")";
        emit onMessage( tr("Localization %1 successfully loaded.")
                           .arg( m_sCurrentLang ) );

        m_nLoadedLocFile = nIndex;
        endResetModel();
    }
}

bool locList::deleteLocFile(const QString fileName)
{
    if ( QFile::remove(config::file(fileName)) )
    {
        emit onMessage( tr("Localization %1 successfully deleted.").arg(fileName));

        updateList();
        m_nLoadedLocFile = -1;
        return true;
    };

    return false;
}

QHash<int, QByteArray> locList::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NativeRole] = "native";
    roles[EnglishRole] = "english";
    roles[FileNameRole] = "filename";
    roles[AvailableVersionRole] = "availableversion";
    roles[InstalledVersionRole] = "installedversion";
    roles[AuthorRole] = "author";
    roles[IsLoadedRole] = "isLoaded";
    return roles;
}

void locList::updateList()
{
    QFile file(config::conf());

    m_LocFiles.clear();
    m_CanUpdate.clear();

    beginResetModel();
    {
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
                    aData.NativeName = xmlReader.attributes().value("Native").toString();
                    aData.EnglishName = xmlReader.attributes().value("English").toString();
                    aData.AvailableVersion = xmlReader.attributes().value("RemoteVersion").toString();
                    aData.AvailableVersionNumber = version::fromString( aData.AvailableVersion );
                    aData.FileName=xmlReader.attributes().value("FileName").toString();

                    //content in *.qm file
                    if ( m_pTranslator->load(aData.FileName, config::file("")) ) {
                        qApp->installTranslator(m_pTranslator);
                        QString l10n_Version;
                        //: replace by the current version of this localization
                        l10n_Version = tr("0.0.0");
                        aData.InstalledVersion = qApp->translate("", l10n_Version.toLocal8Bit().data() );
                        aData.InstalledVersionNumber = version::fromString(aData.InstalledVersion);
                        QString l10n_Author;
                        //: replaced by the name of the translation author
                        l10n_Author = tr("unknown");
                        aData.Author = qApp->translate("", l10n_Author.toLocal8Bit().data() );
                        qApp->removeTranslator(m_pTranslator);
                        if (aData.InstalledVersionNumber < aData.AvailableVersionNumber) {
                            emit onMessage( tr("Localization %1: %2 < %3").arg(
                                               aData.NativeName,
                                               aData.InstalledVersion,
                                               aData.AvailableVersion) );
                            m_CanUpdate.append("Localizations/raw/main/" + aData.FileName);
                        }
                    }

                    m_LocFiles.append(aData);
                }
            }
            file.close();
        }

    }
    endResetModel();
    //no need to go through local files not stored on github
}
