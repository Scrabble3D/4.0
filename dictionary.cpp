#include "dictionary.h"
#include "letter.h" //jokerchar
#include "version.h" //version to string converter
#include "configpath.h"

#include <QXmlStreamReader>
#include <QSettings>
#include <QCoreApplication>
#include <QMessageBox>

#ifdef QT_DEBUG
#include <QDebug>
#endif

dicFile::dicFile(QObject* parent)
    : m_pParent(parent)
{
//    QObject::connect(this, SIGNAL(onDownload(QString)),
//                     parent, SLOT(doDownloadDictionary(QString)));
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

void dicFile::clear()
{
    m_CategoryNames.clear();
    m_Words.clear();
    m_LetterDistribution.clear();
    m_sChars.clear();
    replaceLetter.clear(); //letter.h
    m_sFileName.clear();
}

bool sortByWords(dicEntry aItem, dicEntry bItem) {
    return aItem.word < bItem.word;
}

bool dicFile::loadDictionary(const QString fileName)
{
    QFile inputFile(config::file(fileName));

    bool bHasStandardCategory = false;
    double readSize = 0;
    double totSize = inputFile.size();
    if (inputFile.open(QIODevice::ReadOnly))
    {
        //clear previous data
        clear();
        m_sFileName = fileName;
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
                        catInfo aCatInfo;
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
                    if (sLine.startsWith("Letters")) {
                        nEqual = sLine.indexOf("=");
                        QString sValue = sLine.last(sLine.length()-nEqual-1);
                        if (!sValue.isEmpty())
                            m_LetterDistribution.insert("letters", sValue);
                    }
                    if (sLine.startsWith("Values")) {
                        nEqual = sLine.indexOf("=");
                        QString sValue = sLine.last(sLine.length()-nEqual-1);
                        if (!sValue.isEmpty())
                            m_LetterDistribution.insert( "values", sValue);
                    }
                    if (sLine.startsWith("Count")) {
                        nEqual = sLine.indexOf("=");
                        QString sValue = sLine.last(sLine.length()-nEqual-1);
                        if (!sValue.isEmpty())
                            m_LetterDistribution.insert( "count", sValue);
                    }
                    break;
                case scCategory: {
                    catInfo aCatInfo;
                    nEqual = sLine.indexOf("=");
                    aCatInfo.value = sLine.first(nEqual).toInt();
                    aCatInfo.name = sLine.last(sLine.length()-nEqual-1);
                    aCatInfo.enabled = false;
                    m_CategoryNames.append(aCatInfo);
                }
                break;
                case scReplace: {
                    nEqual = sLine.indexOf("=");
                    QString sKey = sLine.left(nEqual>-1 ? nEqual : sLine.length());
                    QString sValue = sLine.right(nEqual>-1 ? sLine.length() - nEqual - 1 : sLine.length());
                    replaceLetter[sKey] = sValue;
                }
                break;
                case scWords: {
                    sLine = decrypt(sLine, sKey);
                    nEqual = sLine.indexOf("=");
                    nSemicolon=sLine.indexOf(";");
                    dicEntry aItem;
                    aItem.word = sLine.first(nEqual>-1 ? nEqual : sLine.length());
                    aItem.meaning = sLine.mid(nEqual+1,nSemicolon-nEqual-1);
                    aItem.category = nSemicolon>-1 ? sLine.last(sLine.length()-nSemicolon-1).toInt() : 0;
                    m_Words.append(aItem);
                }; //scWords
                break;
                }; //aSection
        };//while infile
        inputFile.close();
        // the dictionary should be properly sorted in first place; takes a couple of seconds here
/*      std::sort(m_Words.begin(), m_Words.end(), sortByWords);
        QFile aFile("test.txt");
        if (aFile.open(QIODevice::WriteOnly)) {
            QTextStream aOut(&aFile);
            for (int i=0; i<m_Words.count(); i++) {
                dicEntry aEntry = m_Words[i];
                aOut << aEntry.word;
                if (aEntry.word != aEntry.meaning)
                    aOut << "=" << aEntry.meaning;
                else
                    if (aEntry.category > 0)
                    aOut << "=";
                if (aEntry.category > 0)
                    aOut << ";" << m_Words[i].category;
                aOut << "\n";
            }
        }
        aFile.close(); */

        m_pParent->setProperty("computeProgress", 0);

        if (!bHasStandardCategory) {
            catInfo aCatInfo;
            aCatInfo.value = -1;
            //: default category name if not defined in the dictionary
            aCatInfo.name = QObject::tr("Standard");
            aCatInfo.enabled = true;
            m_CategoryNames.insert(0, aCatInfo);
        }
    }  else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Scrabble3D");
        msgBox.setText( QObject::tr("Dictionary %1 is not locally available.").arg(fileName) );
        msgBox.setInformativeText( QObject::tr("Do you want to download it now?") );
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        if (msgBox.exec() == QMessageBox::Yes)
//            emit onDownload(fileName + ".zip");
            m_pParent->setProperty("downloadFile", "Dictionaries/raw/main/" + fileName + ".zip");
        return false;
    }

#ifdef QT_DEBUG
    qDebug() << "Successfully read" << m_Words.count() << "words from" << fileName;
#endif

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
        i = QString::compare(m_Words[z].word, word);
        if (i < 0)
            lo = z+1;
        else if (i == 0)
        {
            *index = z;
            return m_CategoryNames[m_Words[z].category].enabled;
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
            for (uint j=0; j<m_Words[i].word.length(); j++)
                if (chars.indexOf(m_Words[i].word[j]) == -1)
                    chars += m_Words[i].word[j];
        chars.sort();
        m_sChars = chars.join("");
    }

#ifdef QT_DEBUG
    qDebug() << "contains" << m_sChars;
#endif
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

    if (m_sChars.isEmpty())
        charsFromWords();

    const QString letterSet = m_sChars;

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
                if ( isWordInDictionary(us, &index) )
                    sResults.append(us);
                //TODO: dictionary: can index be larger than count?
                if ( (index < m_Words.count()) &&
                     m_Words[index].word.startsWith(us) )          //incomplete or complete word
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
    if (qsizetype(index) < m_Words.count())
        return m_Words[index].word;
    else
    {
        qWarning() << "Word index" << index << "out of bounds" << "(" << m_Words.count() << ")";
        return "";
    }
}

QString dicFile::getMeanings(const QString aWords)
{
    QStringList sWords = aWords.split(",");
    QString sResult = "<html>\n";
    int aIndex;

    for (int i = 0; i<sWords.count(); i++) {
        aIndex = indexByWord(sWords[i]);
        if (i > 0) sResult += "</p>";
        sResult += sWords[i];
        if ((aIndex > -1) && !m_Words[aIndex].meaning.isEmpty())
            sResult += ": <i>" + m_Words[aIndex].meaning + "</i>";
    }
    sResult += "</html>";
    return sResult;
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
    if (qsizetype(index) < m_Words.count()) {
        dicEntry["word"] = m_Words[index].word;
        dicEntry["category"] = m_CategoryNames[m_Words[index].category].name;
        dicEntry["meaning"] = m_Words[index].meaning;
        dicEntry["included"] = m_CategoryNames[m_Words[index].category].enabled;
    }
    return dicEntry;
}

int dicFile::indexByWord(const QString word)
{
    int index;
    if (!isWordInDictionary(word, &index))
        index = -1; //function returns >-1 for variation()

    return index;
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

QVariantList dicFile::getLetterDistribution(QVariantList currentDistribution)
{
    QVariantList aResult;
    if (m_LetterDistribution.isEmpty())
        return aResult;

    QStringList aLetters = m_LetterDistribution["letters"].toString().split(",");
    QStringList aValues = m_LetterDistribution["values"].toString().split(",");
    QStringList aCount = m_LetterDistribution["count"].toString().split(",");

    if ((aLetters.count() == aValues.count()) &&
        (aLetters.count() == aCount.count())) {
        for (int i=0; i < aLetters.count(); i++) {
            aResult.append(aLetters[i]);
            aResult.append(aValues[i].toInt());
            aResult.append(aCount[i].toInt());
        }
        if (aResult != currentDistribution) {
            QMessageBox msgBox;
            //: dialog title
            msgBox.setWindowTitle(QObject::tr("Letter Distribution") );
            msgBox.setText( QObject::tr("Letter distribution in dictionary does not match the current configuration.") );
            msgBox.setInformativeText( QObject::tr("Do you want to update the letter set?") );
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            if (msgBox.exec() == QMessageBox::No)
                aResult.clear();
        } else
            aResult.clear();
    }

    return aResult;
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
    //TODO: dictionary dummy for author and license; currently done per selectedDicInfo()
/*
    case AuthorRole: if (index.row() == m_nCurrentDictionary) {
        return "Hello World";
        break;
    }
    case LicenseRole: if (index.row() == m_nCurrentDictionary) {
        return "GNU Public License v3";
        break;
    }
*/
    }
    return QVariant();
}

void dicList::updateList()
{
    QFile file(config::conf());

    m_Dictionaries.clear();
    m_CanUpdate.clear();

    beginResetModel();
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

    //local files
    QStringList localDics = config::path().entryList(QStringList() << "*.dic", QDir::Files);
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
    endResetModel();
}

void dicList::getInfo(dicListData *aData)
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
        aData->Author = settings.value("Header/Author").toString();
        aData->License = settings.value("Header/Licence").toString();
        aData->Release = settings.value("Header/Release").toString();
        aData->Comment = settings.value("Header/Comment").toString();
        // update?
        if ((aData->InstalledVersionNumber > -1) &&
            (aData->InstalledVersionNumber < aData->AvailableVersionNumber))
        {
            m_pParent->setProperty("addMsg", tr("Dictionary %1: %2 < %3").arg(
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

int dicList::indexOf(QString fileName)
{
    int aIndex = -1;
    for (int i=0; i<dicList::m_Dictionaries.count(); i++)
        if (m_Dictionaries[i].FileName == fileName) {
            aIndex = i;
            break;
        }
    return aIndex;
}


bool dicList::loadFrom(QString fileName)
{
    const int aIndex = indexOf(fileName);
    if ((aIndex > -1) && (m_nCurrentDictionary != aIndex))
    {
        //update model and clear
        QModelIndex indexTL = this->index( m_nCurrentDictionary, 0 );
        QModelIndex indexBR = this->index( m_nCurrentDictionary, 4 );
        m_nCurrentDictionary = -1;
        emit dataChanged(indexTL, indexBR, { IsLoadedRole } );

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

bool dicList::deleteDic(QString fileName)
{
    const bool bResult = QFile::remove(config::file(fileName));
    if (bResult) {
        const int nIndex = indexOf(fileName);

        //clear info
        dicListData *aData = &m_Dictionaries[nIndex];
        aData->InstalledVersionNumber = -1;
        aData->InstalledVersion = "";
        aData->Author = "";
        aData->License = "";
        aData->Release = "";
        aData->Comment = "";

        //currently loaded
        if (m_nCurrentDictionary == nIndex) {
            dictionary->clear();
            m_nCurrentDictionary = -1;
        }

        //update qml table
        QModelIndex indexTL = this->index( nIndex, 0 );
        QModelIndex indexBR = this->index( nIndex, 4 );
        emit dataChanged(indexTL, indexBR, { Qt::DisplayRole, IsLoadedRole } );
    }
    return bResult;
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

