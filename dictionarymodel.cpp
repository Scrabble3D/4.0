#include "dictionarymodel.h"
#include "configpath.h"
#include "version.h" //version to string converter

#include <QXmlStreamReader>
#include <QSettings>

dictionarymodel::dictionarymodel(QObject *parent)
{
    Q_UNUSED(parent);
}

QVariant dictionarymodel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant(" ");

    dicListData aData = m_Dictionaries[index.row()];
    switch (role) {
        case NativeRole: return aData.NativeName; break;
        case EnglishRole: return aData.EnglishName; break;
        case FilenameRole: return aData.FileName; break;
        case AvailableVersionRole: return aData.AvailableVersion; break;
        case InstalledVersionRole: return aData.InstalledVersion; break;
        case AuthorRole: return aData.Author; break;
        case LicenseRole: return aData.License; break;
        case ReleaseRole: return aData.Release; break;
        case CommentRole: return aData.Comment; break;
        case CategoriesRole: return aData.Categories; break;
        case IsLoadedRole: return (index.row() == m_nLoadedDictionary); break;
    }
    return QVariant();
}

int dictionarymodel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_Dictionaries.count();
}

void dictionarymodel::doLoadingFinished(QString fileName)
{
    const int nIndex = indexOf(fileName);
    if ( nIndex > -1 )
    {
        emit onMessage( tr("Dictionary %1 successfully loaded.")
                           .arg(m_Dictionaries[nIndex].NativeName + " (" + m_Dictionaries[nIndex].EnglishName +")"));
        // QModelIndex modelindex = this->index( nOldIndex );
        // emit dataChanged(modelindex, modelindex, { IsLoadedRole } ); //clear

        // whole model needs to be reset in order to update the repeater
        // in ConfigDictionary which shows the checkboxes for categories
        beginResetModel();
        m_nLoadedDictionary = nIndex;
        endResetModel();
    }
}

void dictionarymodel::deleteDic(const QString fileName)
{
    if ( QFile::remove(config::file(fileName)) )
    {
        const int nIndex = indexOf(fileName);

        emit onMessage( tr("Dictionary %1 successfully deleted.").arg(fileName));

        //clear info
        dicListData *aData = &m_Dictionaries[nIndex];
        aData->InstalledVersionNumber = -1;
        aData->InstalledVersion = "";

        //clear if currently loaded
        if (m_nLoadedDictionary == nIndex) {
            emit onClearDictionary();
            m_nLoadedDictionary = -1;
        }

        //update qml
        QModelIndex modelindex = this->index( nIndex );
        emit dataChanged(modelindex, modelindex, { InstalledVersionRole, IsLoadedRole } );
    } else
        emit onMessage( tr("Deletion of %1 failed.").arg(fileName));
}

void dictionarymodel::updateList()
{
    QFile file(config::conf());

    m_Dictionaries.clear();
    m_CanUpdate.clear();

    beginResetModel();
    {
        // conf
        if ( file.open(QIODevice::ReadOnly) )
        {
            QXmlStreamReader xmlReader;
            xmlReader.setDevice(&file);
            while (!xmlReader.atEnd())
            {
                xmlReader.readNext();

                if (xmlReader.isStartElement() &&
                    (xmlReader.name().toString() == "dictionary"))
                {
                    dicListData aData;
                    aData.NativeName=xmlReader.attributes().value("Native").toString();
                    aData.EnglishName=xmlReader.attributes().value("English").toString();
                    aData.AvailableVersion=xmlReader.attributes().value("RemoteVersion").toString();
                    aData.AvailableVersionNumber=version::fromString(aData.AvailableVersion);
                    aData.FileName=xmlReader.attributes().value("FileName").toString();
                    getInfo(&aData);
                    m_Dictionaries.append(aData);
                }
            }
            file.close();
        }

        // local files
        QStringList localDics = config::path().entryList(QStringList() << "*.dic", QDir::Files);
        bool bPublicDic;
        foreach(QString fileName, localDics)
        {
            bPublicDic = false;
            // is local file in conf list?
            for (int i=0; i<m_Dictionaries.count(); i++)
                if (m_Dictionaries[i].FileName == fileName)
                    bPublicDic = true;
            // if not fill with info
            if (!bPublicDic) {
                dicListData aData;
                aData.NativeName = fileName; //for local file the filename is the native identifier
                aData.EnglishName = "";
                aData.AvailableVersion = "";
                aData.InstalledVersion = "";
                aData.FileName = fileName;
                getInfo(&aData);
                m_Dictionaries.append(aData);
            }
        }
    }
    endResetModel();
}

void dictionarymodel::getInfo(dicListData *aData)
{
    const QString sFileName = config::file(aData->FileName);
    QFileInfo fileInfo(sFileName);
    if (fileInfo.exists())
    {
        QSettings settings(sFileName, QSettings::IniFormat);
        QString aVersion = settings.value("Header/Version").toString();
        if (aVersion.isEmpty())
            aData->InstalledVersionNumber = -1;
        else
            aData->InstalledVersionNumber = aVersion.toInt();
        aData->InstalledVersion = version::toString(aData->InstalledVersionNumber);
        // INFO: use quotes for strings containing commas or semicolons
        aData->Author  = settings.value("Header/Author" ).toString();
        aData->License = settings.value("Header/Licence").toString();
        if (aData->License.isEmpty())
            aData->License = settings.value("Header/License").toString();
        aData->Release = settings.value("Header/Release").toString();
        if (aData->Release.isEmpty())
            aData->Release = settings.value("Header/Version").toString();
        aData->Comment = settings.value("Header/Comment").toString();
        // letter distribution
        aData->LetterDistribution.clear();
        QStringList aLetters = settings.value("Header/Letters").toStringList();
        QStringList aValues = settings.value("Header/Values").toStringList();
        QStringList aCounts = settings.value("Header/Counts").toStringList();
        aData->LetterDistribution = "";
        if ( (aLetters.count() > 0) &&
             (aLetters.count() == aValues.count()) &&
             (aLetters.count() == aCounts.count())) {
            for (int i = 0; i < aLetters.count(); i++)
                aData->LetterDistribution += aLetters[i] + "," + aValues[i] + "," + aCounts[i] + ",";
            aData->LetterDistribution.chop(1); // removes trailing comma
        }
        // categories
        aData->StandardCategory = settings.value("Header/StandardCategory").toString();
        settings.beginGroup("Categories");
            aData->Categories.clear();
            aData->Categories.append(aData->StandardCategory);
            QStringList aKeys = settings.childKeys();
            for (int i = 0; i < aKeys.count(); i++)
                aData->Categories.append( settings.value(aKeys[i]).toString() );
        settings.endGroup();
        // update
        if ((aData->InstalledVersionNumber > -1) &&
            (aData->InstalledVersionNumber < aData->AvailableVersionNumber))
        {
            emit onMessage( tr("Dictionary %1: %2 < %3").arg(
                               aData->NativeName,
                               aData->InstalledVersion,
                               aData->AvailableVersion));
            m_CanUpdate.append("Dictionaries/raw/main/" + aData->FileName + ".zip");
        }
    }
    else {
        aData->InstalledVersion = "";
        aData->InstalledVersionNumber = -1;
    }
    aData->AvailableVersionNumber = version::fromString(aData->AvailableVersion);
}

int dictionarymodel::indexOf(const QString fileName)
{
    int aIndex = -1;
    for (int i = 0; i < m_Dictionaries.count(); i++)
        if (m_Dictionaries[i].FileName == fileName) {
            aIndex = i;
            break;
        }
    return aIndex;
}

QHash<int, QByteArray> dictionarymodel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NativeRole] = "native";
    roles[EnglishRole] = "english";
    roles[FilenameRole] = "filename";
    roles[AvailableVersionRole] = "availableversion";
    roles[InstalledVersionRole] = "installedversion";
    roles[AuthorRole] = "author";
    roles[LicenseRole] = "license";
    roles[ReleaseRole] = "release";
    roles[CommentRole] = "comment";
    roles[CategoriesRole] = "categories";
    roles[IsLoadedRole] = "isLoaded";
    return roles;
}

QVariantMap dictionarymodel::get(int index) const
{
    QVariantMap data;

    if ((index > -1) && (index < m_Dictionaries.count())) {
        dicListData aEntry = m_Dictionaries[index];
        data["native"] = aEntry.NativeName;
        data["english"] = aEntry.EnglishName;
        data["filename"] = aEntry.FileName;
        data["availableversion"] = aEntry.AvailableVersion;
        data["installedversion"] = aEntry.InstalledVersion;
        data["categories"] = aEntry.Categories;
        data["distribution"] = aEntry.LetterDistribution;
        data["isLoaded"] = (index == m_nLoadedDictionary);
    }
    return data;
}

