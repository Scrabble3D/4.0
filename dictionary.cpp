#include "dictionary.h"
#include "letter.h" //jokerchar

#include "configpath.h"

#include <QMessageBox>

#include <QThread>
#include <QtConcurrent>

#if defined(Q_OS_LINUX) && defined(QT_DEBUG)
#include <QDebug>
#endif

#undef doSort

//NOTE: dicitionary: DAWG / Aho–Corasick algorithm

dictionary::dictionary(QObject* parent)
{
    Q_UNUSED(parent);
}

void dictionary::doClear()
{
    m_CategoryNames.clear();
    m_Words.clear();
    m_sChars.clear();
    replaceLetter.clear(); //letter.h
    m_sFileName.clear();
}

#ifdef doSort
bool sortByWords(dicEntry aItem, dicEntry bItem) {
    return aItem.word < bItem.word;
}
#endif

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

void dictionary::doLoad(QString fileName, QString categories)
{
    QFile inputFile(config::file(fileName));

    bool bHasStandardCategory = false;
    double readSize = 0;
    double totSize = inputFile.size();

    if (inputFile.open(QIODevice::ReadOnly))
    {
        //clear previous data
        doClear();
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
            emit onProgress( round( (readSize/totSize)*100) );

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
                        aCatInfo.value = 0;
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
        }; //while infile
        inputFile.close();

#ifdef doSort
        // the dictionary should be properly sorted in first place as it takes a couple of seconds here
        std::sort(m_Words.begin(), m_Words.end(), sortByWords);
        //save word list to replace dic content
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
        aFile.close();
#endif
        emit onProgress(0);

        if (!bHasStandardCategory) {
            catInfo aCatInfo;
            aCatInfo.value = 0;
            //: default category name if not defined in the dictionary
            aCatInfo.name = QObject::tr("Standard");
            aCatInfo.enabled = true;
            m_CategoryNames.insert(0, aCatInfo);
        }
        QStringList cat = categories.split(",");
        for (int i = 0; i < cat.count(); i++)
            setCategoryChecked(cat[i], true);

        emit onLoadingFinished(m_sFileName);
    }

#if defined(Q_OS_LINUX) && defined(QT_DEBUG)
    qDebug() << "Successfully read" << m_Words.count() << "words from" << fileName;
#endif
}

void dictionary::loadDictionary(const QString fileName, const QString categories)
{
    // config::reset() applies an empty filename
    // but configuration of dictionary should be kept
    if (fileName.isEmpty())
        return;

    doClear();

    if (!QFileInfo::exists(config::file(fileName))) {
        // message box
        QMessageBox msgBox;
        msgBox.setWindowTitle("Scrabble3D");
        msgBox.setText( QObject::tr("Dictionary %1 is not locally available.").arg(fileName) );
        msgBox.setInformativeText( QObject::tr("Do you want to download it now?") );
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        // download dictionary
        if (msgBox.exec() == QMessageBox::Yes)
            emit onDownload("Dictionaries/raw/main/" + fileName + ".zip");
    } else {
        // load in extra thread
        QFuture<void> future = QtConcurrent::run([=]() {
            doLoad(fileName, categories);
        });
    }
}

int dictionary::indexByWord(const QString sFind)
{
    int index;
    if (isWordInDictionary(sFind.toUpper(), &index) == srNotFound)
        index = -1;
    return index;
}

dictionary::searchResult dictionary::isWordInDictionary(QString sFind, int *index)
{
    int i, z;
    int lo = 0;
    int hi = m_Words.count()-1;
    while (lo<=hi)
    {
        z = (lo+hi) >> 1; //shr
        i = QString::compare(m_Words[z].word, sFind);
        if (i < 0)
            lo = z+1;
        else if (i == 0)
        {
            *index = z;
            if (m_CategoryNames[m_Words[z].category].enabled)
                return srMatch;
            else
                return srDisabled;
        }
        else if (i > 0)
            hi = z-1;
    }
    *index = lo;
    return srNotFound;
}

void dictionary::charsFromWords()
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

#if defined(Q_OS_LINUX) && defined(QT_DEBUG)
    qDebug() << "contains" << m_sChars;
#endif
}

//NOTE: dictionary: More Efficient String Construction https://doc.qt.io/qt-6/qstring.html
QString dictionary::variation(const QString aChars)
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
                if ( isWordInDictionary(us, &index) == srMatch)
                    sResults.append(us);
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

QString dictionary::getWord(const uint index)
{
    if (qsizetype(index) < m_Words.count())
        return m_Words[index].word;
    else
    {
#if defined(Q_OS_LINUX) && defined(QT_DEBUG)
        qWarning() << "Word index" << index << "out of bounds" << "(" << m_Words.count() << ")";
#endif
        return "";
    }
}

QString dictionary::getMeanings(const QString aWords)
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

QStringList dictionary::checkWords(const QString sPlacedWord, QString sConnectedWords)
{
    QStringList unknownWords;
    unknownWords.append(sPlacedWord);
    if (!sConnectedWords.isEmpty())
        unknownWords.append(sConnectedWords.split(","));

    int i = unknownWords.count()-1;
    int tmp;
    while (i >= 0)
    {
        if (unknownWords[i].isEmpty() || (isWordInDictionary(unknownWords[i], &tmp) == srMatch))
            unknownWords.remove(i);
        i--;
    }
    return unknownWords;
}

QVariantMap dictionary::wordByIndex(const uint index)
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

void dictionary::setCategoryChecked(const QString catName, const bool isChecked)
{
    for (int i=0; i<m_CategoryNames.count(); i++)
        if (m_CategoryNames[i].name == catName) {
            m_CategoryNames[i].enabled = isChecked;
            break;
        }
}

bool dictionary::getCategoryChecked(const QString catName) const
{
    for (int i=0; i<m_CategoryNames.count(); i++)
        if (m_CategoryNames[i].name == catName) {
            return m_CategoryNames[i].enabled;
        }

    return true;
}
