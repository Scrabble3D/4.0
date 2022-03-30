#include "dictionary.h"
#include "letter.h" //jokerchar
#include "defines.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QSettings>
#include <QCoreApplication>
#include <QMessageBox>

dicFile::dicFile(QObject* parent)
    : m_Words(0),
      m_Meanings(0),
      m_Categories(0),
      m_pParent(parent)
{
}

QString decrypt(QString aLine, QString aKey)
{
    if (aKey == "")
        return aLine;

    QByteArray aByteData = aLine.toUtf8();
    aByteData = QByteArray::fromBase64(aByteData, QByteArray::Base64Encoding);

    int keyLen = aKey.length();
    for (int i=0; i<aByteData.size(); i++)
        aByteData[i] = aByteData[i] xor aKey[(i % keyLen)].toLatin1();

    return QString::fromStdString(aByteData.toStdString());
}

bool dicFile::loadDictionary(const QString fileName)
{
    QDir aPath = dictionaryPath();
    QFile inputFile(aPath.path() + "/" + fileName);

    bool bHasStandardCategory = false;
    double readSize = 0;
    double totSize = inputFile.size();
    if (inputFile.open(QIODevice::ReadOnly))
    {
        m_sFileName = fileName;
        //clear previous data
        m_CategoryNames.clear();
        m_Words.clear();
        m_Meanings.clear();
        m_Categories.clear();
        m_sChars.clear();
        QTextStream in(&inputFile);
        QString sKey="";
        QString sLine; //<word>=<meaning>;<category>
        int nEqual; //pos of =
        int nSemicolon; //pos of ;
        enum sectionType {scHeader, scCategory, scReplace, scWords};
        sectionType aSection = scHeader;

        while (!in.atEnd())
        {
            sLine = in.readLine();
            readSize += sLine.length();
            m_pParent->setProperty("computeProgress",round( (readSize/totSize)*100) );

            if (sLine == "[Header]")
                aSection=scHeader;
            //TODO: dictionaries: read chars
            else if (sLine == "[Categories]")
                aSection=scCategory;
            else if (sLine == "[Replace]")
                aSection=scReplace;
            else if (sLine == "[Words]")
                aSection=scWords;
            else
                switch (aSection) {
                case scHeader:
                    //name of the (pseudo) standard category;
                    // will be disabled and always enabled
                    if (sLine.startsWith("StandardCategory")) {
                        CatInfo aCatInfo;
                        nEqual = sLine.indexOf("=");
                        aCatInfo.value = -1;
                        aCatInfo.name = sLine.last(sLine.length()-nEqual-1);
                        aCatInfo.enabled = true;
                        m_CategoryNames.insert(0, aCatInfo);
                        bHasStandardCategory = true;
                    }
                    if (sLine.startsWith("Key")) {
                        nEqual = sLine.indexOf("=");
                        sKey = sLine.last(sLine.length()-nEqual-1);
                    }
                    break;
                case scCategory: {
                    CatInfo aCatInfo;
                    nEqual = sLine.indexOf("=");
                    aCatInfo.value = sLine.first(nEqual).toInt();
                    aCatInfo.name = sLine.last(sLine.length()-nEqual-1);
                    aCatInfo.enabled = false;
                    m_CategoryNames.append(aCatInfo);
                    break;
                }
                case scReplace: break;
                case scWords: {
                    sLine = decrypt(sLine, sKey);
                    nEqual = sLine.indexOf("=");
                    nSemicolon=sLine.indexOf(";");
                    m_Words.append(sLine.first(nEqual>-1 ? nEqual : sLine.length()));
                    m_Meanings.append(sLine.mid(nEqual+1,nSemicolon-nEqual-1));
                    m_Categories.append(nSemicolon>-1 ? sLine.last(sLine.length()-nSemicolon-1).toInt() : 0);
                    break;
                }; //scWords
                }; //switch
        };//while infile
        inputFile.close();
        m_pParent->setProperty("computeProgress",0);

        if (!bHasStandardCategory) {
            CatInfo aCatInfo;
            aCatInfo.value = -1;
            aCatInfo.name = "Standard"; //TODO: dictionary: translate
            aCatInfo.enabled = true;
            m_CategoryNames.insert(0, aCatInfo);
        }
    }  else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Scrabble3D");
        msgBox.setText( QString("Dictionary %1 is not yet available locally.").arg(fileName) );
        msgBox.setInformativeText( QString("Do you want to download now?") );
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        if (msgBox.exec() == QMessageBox::Yes)
            m_pParent->setProperty("downloadFile", "Dictionaries/raw/main/" + fileName + ".zip");
        return false;
    }

    qDebug() << "Successfully read" << m_Words.count() << "words from" << fileName;
    //get all characters
    //TODO: dic resp. gameplay: store chars with savegame
    charsFromWords();
    //update the word search spin box maximum
    return true;
}

bool dicFile::isWordInDictionary(QString word, int *index)
{
    int i, z;
    int lo = 0;
    int hi = m_Words.count()-1;
    while (lo<=hi)
    {
        z = (lo+hi) >> 1; //shr
        i = QString::compare(m_Words[z],word);
        if (i < 0)
            lo = z+1;
        else if (i == 0)
        {
            *index = z;
            return m_CategoryNames[m_Categories.at(*index)].enabled;
        }
        else if (i > 0)
            hi = z-1;
    }
    *index = lo;
    return false;
}

void dicFile::charsFromWords()
{
    if (m_sChars.isEmpty())
    {
        QStringList chars;
        for (uint i=0; i<m_Words.count(); i++)
            for (uint j=0; j<m_Words[i].length(); j++)
                if (chars.indexOf(m_Words[i][j])==-1)
                    chars += m_Words[i][j];
        chars.sort();
        m_sChars = chars.join("");
    }
    qDebug() << "contains" << m_sChars;
}

QString dicFile::variation(const QString aChars)
/* Scrabble-like variation of letters
   ABC:
   A    B    C
   AB   BA   CA
   AC   BC   CB
   ABC  BAC  CAB
   ACB  BCA  CBA
*/
{
    int i = 0; //i=index of letter
    int j = 0; //some flag
    int z;     //index of available letters
    int index; //#word in dictionary
    QStringList sResults;
    QString s = "";
    QString us;
    QString chars = aChars;
    const QString letterSet = m_sChars;

    if (letterSet.isEmpty())
        qCritical("Fatal error: no letters to vary");

    while ( true ) //((i > letterSet.length()) && (s == ""))
    {
        while ( true )
        {
            z = chars.indexOf(letterSet[i]) + 1;
            if (z == 0) z = chars.indexOf(JokerChar) + 1; // try to insert joker
            if (z > 0)
            {
                s = s + letterSet[i];
                us = s;
                //               qDebug() << "i=" << i << "j" << j << "z=" << z << us << ":";
                if ( isWordInDictionary(us, &index) )
                    sResults.append(us);
                //TODO: dictionary: can index be larger than count?
                if ( (index < m_Words.count()) &&
                     m_Words[index].startsWith(us) )          //incomplete or complete word
                {
                    chars.remove(z-1,1);
                    i = 0;
                    j = 0;
                } else
                {
                    s.chop(1);                                //clear the last letter in temp string
                    j = -1;
                }
            } else
                j = -1;

            if (j == -1)
                break;
        } // while (j > -1)

        if ((i == letterSet.length()-1) && (s.length()>0))    //if last letter in the set and something was found
        {
            while ( true )
            {
                i = letterSet.indexOf(s.last(1))+1;           //start next search with then successor in available
                s.chop(1);                                    //clear the last letter in temp string
                if (i<letterSet.length() || (s==""))
                    break;
            }

            chars = aChars;                                   //reset the available letters and delete found chars
            for (j=0; j<s.length(); j++)
            {
                z = chars.indexOf(s[j]);
                if (z == -1) z = chars.indexOf(JokerChar);           //if not in set it is a jokers
                chars.remove(z,1);
            }
        } // if ((i == letterSet.length()) && s.length()>0)
        else
            i++;                                            //next char in set

        if ( (i >= letterSet.length()) && s.isEmpty() )
            break;

    } // while ( (i > letterSet.length()) && (s == "") )
    return sResults.join(",");
}

QString dicFile::getWord(const uint index)
{
    if (index < m_Words.count())
        return m_Words[index];
    else
    {
        qWarning() << "Word index" << index << "out of bounds" << "(" << m_Words.count() << ")";
        return "";
    }
}

bool dicFile::containsWord(QString word)
{
    int index = m_Words.indexOf(word);
    if ( (index > -1) && ( !m_CategoryNames[m_Categories.at(index)].enabled) )
        index = -1;
    return index > -1;
}

QStringList dicFile::checkWords(const QString sPlacedWord, QString sConnectedWords)
{
    QStringList unknownWords;
    unknownWords.append(sPlacedWord);
    if (!sConnectedWords.isEmpty())
        unknownWords.append(sConnectedWords.split(","));

    int i = unknownWords.count()-1;
    int tmp;
    while (i >= 0)
    {
        if (unknownWords[i].isEmpty() || isWordInDictionary(unknownWords[i], &tmp))
            unknownWords.remove(i);
        i--;
    }
    return unknownWords;
}

QVariantMap dicFile::wordByIndex(const uint index)
{
    QVariantMap dicEntry;
    if (index < m_Words.count()) {
        dicEntry["word"] = m_Words[index];
        dicEntry["category"] = m_CategoryNames[m_Categories.at(index)].name;
        dicEntry["meaning"] = m_Meanings[index];
        dicEntry["included"] = m_CategoryNames[m_Categories.at(index)].enabled;
    }
    return dicEntry;
}

int dicFile::indexByWord(const QString word)
{
    return m_Words.indexOf(word.toUpper());
}

QString dicFile::categoryNames()
{
    QString sResult;
    if (m_CategoryNames.count() > 0)
        sResult = m_CategoryNames[0].name;
    for (int i=1; i<m_CategoryNames.count(); i++)
        sResult += ","+m_CategoryNames[i].name;
    return sResult;
}

void dicFile::setCategoryChecked(const QString catName, const bool isChecked)
{
    for (int i=0; i<m_CategoryNames.count(); i++)
        if (m_CategoryNames[i].name == catName) {
            m_CategoryNames[i].enabled = isChecked;
            break;
        }
}

bool dicFile::getCategoryChecked(const QString catName) const
{
    for (int i=0; i<m_CategoryNames.count(); i++)
        if (m_CategoryNames[i].name == catName) {
            return m_CategoryNames[i].enabled;
        }

    return true;
}

//*******************************************************//


dicList::dicList(QObject *parent)
    : QAbstractTableModel(parent),
      m_pParent(parent),
      m_nCurrentDictionary(-1),
      m_Dictionaries(0)
{
    dictionary = new dicFile(m_pParent);
    updateList();
}

QHash<int, QByteArray> dicList::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[IsLoadedRole] = "isLoaded";
    roles[AuthorRole] = "author";
    roles[LicenseRole] = "license";
    return roles;
}

int dicList::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_Dictionaries.count();
}

int dicList::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QVariant dicList::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
    {
        if (section == 0)
            return tr("Native");
        else if (section == 1)
            return tr("English");
        else if (section == 2)
            return tr("Filename");
        else if (section == 3)
            return tr("Available");
        else if (section == 4)
            return tr("Local");
    }
    return QVariant();
}

QVariant dicList::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    dicListData aData = m_Dictionaries[index.row()];

    switch (role)
    {
    case Qt::DisplayRole: {
        if (index.column() == 0)
            return aData.NativeName;
        else if (index.column() == 1)
            return aData.EnglishName;
        else if (index.column() == 2)
            return aData.FileName;
        else if (index.column() == 3)
            return aData.AvailableVersion;
        else if (index.column() == 4) //best value
            return aData.InstalledVersion;
        break;
    }
    case IsLoadedRole: {
        return (index.row() == m_nCurrentDictionary);
        break;
    }
    case AuthorRole: if (index.row() == m_nCurrentDictionary) {
        qDebug() << index.row() << ":"  << index.column();
        return "Hello World";
        break;
    }
    case LicenseRole: if (index.row() == m_nCurrentDictionary) {
        qDebug() << index.row() << "="  << index.column();
        return "GNU Public License v3";
        break;
    }
    }
    return QVariant();
}

void dicList::updateList()
{
    QDir aPath = dictionaryPath();
    QFile file(aPath.path() + "/Scrabble3D.conf");

    m_Dictionaries.clear();

    if ( file.open(QIODevice::ReadOnly) )
    {
        QXmlStreamReader reader;
        reader.setDevice(&file);

        while (reader.readNextStartElement())
        {
            if (reader.name().toString() == "Dictionaries")
            {
                while(reader.readNextStartElement())
                {
                    if (reader.name().toString() == "Dictionary")
                    {
                        dicListData aData;
                        aData.NativeName=reader.attributes().value("Native").toString();
                        aData.EnglishName=reader.attributes().value("English").toString();
                        aData.AvailableVersion=reader.attributes().value("RemoteVersion").toString();
                        aData.InstalledVersion=reader.attributes().value("LocalVersion").toString();
                        aData.FileName=reader.attributes().value("FileName").toString();
                        getInfo(&aData);
                        m_Dictionaries.append(aData);
                    } else
                        reader.skipCurrentElement();
                }
                break;
            } //Dictionaries
        } //start
    }

    //local files
    QStringList localDics = aPath.entryList(QStringList() << "*.dic", QDir::Files);
    bool bPublicDic;
    foreach(QString fileName, localDics)
    {
        bPublicDic = false;
        for (int i=0; i<m_Dictionaries.count(); i++)
            if (m_Dictionaries[i].FileName == fileName)
                bPublicDic = true;
        if (!bPublicDic) {
            dicListData aData;
            aData.NativeName = "";
            aData.EnglishName = "";
            aData.AvailableVersion = "";
            aData.InstalledVersion = "";
            aData.FileName = fileName;
            getInfo(&aData);
            m_Dictionaries.append(aData);
        }
    }

    emit dataChanged(this->index( 0, 0 ),
                     this->index( m_Dictionaries.count()-1, 4),
                     { Qt::DisplayRole } );
}

void dicList::getInfo(dicListData *aData)
{
    QDir aPath = dictionaryPath();
    QFileInfo fileInfo(aPath.path() + "/" + aData->FileName);
    if (fileInfo.exists())
    {
        QSettings settings(aPath.path() + "/" + aData->FileName, QSettings::IniFormat);
        QString aVersion = settings.value("Header/Version").toString();
        if (aVersion.isEmpty())
            aData->InstalledVersionNumber = -1;
        else
            aData->InstalledVersionNumber = aVersion.toInt();
        aData->InstalledVersion = versionToString(aData->InstalledVersionNumber);
        aData->Author = settings.value("Header/Author").toString();
        aData->License = settings.value("Header/Licence").toString();
        aData->Release = settings.value("Header/Release").toString();
        aData->Comment = settings.value("Header/Comment").toString();
    }
    else
        aData->InstalledVersion = "";
}

bool dicList::loadFrom(QString fileName)
{
    int aIndex = -1;
    for (int i=0; i<dicList::m_Dictionaries.count(); i++)
        if (m_Dictionaries[i].FileName == fileName) {
            aIndex = i;
            break;
        }
    if ((aIndex > -1) && (m_nCurrentDictionary != aIndex))
    {
        //update model and clear
        QModelIndex indexTL = this->index( m_nCurrentDictionary, 0 );
        QModelIndex indexBR = this->index( m_nCurrentDictionary, 4 );
//TODO: dictionary: check why m_nCurrentDictionary is set before loading succeeded
//        emit dataChanged(indexTL, indexBR, { IsLoadedRole } );

        if (!dictionary->loadDictionary(fileName))
            return false;

        m_nCurrentDictionary = aIndex;
        indexTL = this->index( m_nCurrentDictionary, 0 );
        indexBR = this->index( m_nCurrentDictionary, 4 );
        emit dataChanged(indexTL, indexBR, { IsLoadedRole } ); //set
        return true;
    } else
        return false;
}

QString dicList::currentDicName() const
{
    if ((m_nCurrentDictionary>-1) &&
        (m_nCurrentDictionary<m_Dictionaries.count()))
        return m_Dictionaries[m_nCurrentDictionary].FileName;
    else
        return "";
}

QVariantMap dicList::selectedDicInfo(const int index) const
{
    QVariantMap dicInfo;
    if ((index>-1) && (index<m_Dictionaries.count())) {
        dicListData aData = m_Dictionaries[index];
        dicInfo["author"] = aData.Author;
        dicInfo["license"] = aData.License;
        dicInfo["release"] = aData.Release;
        dicInfo["comment"] = aData.Comment;
        if (index == m_nCurrentDictionary)
            dicInfo["categories"] = dictionary->categoryNames();
        else
            dicInfo["categories"] = "";
    }
    return dicInfo;
}

QString dicList::versionToString(const int aValue)
{
    int aMajor, aMinor, aBuild;
    int temp;

    if (aValue == -1)
        return "";

    temp = aValue;
    aMajor = temp / 100000;
    temp = temp-aMajor*100000;
    aMinor = temp / 1000;
    temp = temp-aMinor*1000;
    aBuild = temp;
    return QString::number(aMajor)+'.'+QString::number(aMinor)+'.'+QString::number(aBuild);
}
