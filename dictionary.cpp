#include "dictionary.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QSettings>
#include <QElapsedTimer>

dictionary::dictionary(QObject *parent)
    : QAbstractTableModel(parent),
      m_nCurrentDictionary(-1),
      m_lData(0)
{
//    QStringList test;

/*
    QSettings settings("foo.dic", QSettings::IniFormat);
    settings.beginGroup("Test");
    settings.setValue("BAR","");
    settings.setValue("BÄR","");
    settings.setValue("OST","");
    settings.setValue("ÖSE","");
    settings.endGroup();
*/
/*
    QSettings settings("test.dic", QSettings::IniFormat);
    settings.beginGroup("Test");
    test = settings.allKeys();
    settings.endGroup();
*/
//    test << "BAR" << "BÄR" << "OST" << "ÖSE";
/*
    qDebug() << test.indexOf("BÄR",Qt::CaseInsensitive);
    qDebug() << test.join(",");
*/
    readModelData();
    loadFrom("english.dic"); //TODO: Debug
}

void dictionary::readModelData()
{
    QFile file("Scrabble3D.conf");

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
                        modelData aData;
                        aData.NativeName=reader.attributes().value("Native").toString();
                        aData.EnglishName=reader.attributes().value("English").toString();
                        aData.AvailableVersion=reader.attributes().value("RemoteVersion").toString();
                        aData.InstalledVersion=reader.attributes().value("LocalVersion").toString();
                        aData.FileName=reader.attributes().value("FileName").toString();
                        getInfo(&aData);
                        m_lData.append(aData);
                   } else
                        reader.skipCurrentElement();
                }
                break;
            } //Dictionaries
        } //start
    } //FILE

    //TODO: scan local files
}

QVariant dictionary::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    modelData aData = m_lData[index.row()];

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
        case IsSelectedRole: {
            return (index.row() == m_nCurrentDictionary);
            break;
        }
        case AuthorRole: {
            return aData.Author;
            break;
        }
        case LicenseRole: {
            return aData.License;
            break;
        }
        case ReleaseRole: {
            return aData.Release;
            break;
        }
        case CommentRole: {
            return aData.Comment;
            break;
        }
    }

    return QVariant();
}

QVariant dictionary::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
    {
        if (section == 0)
            return QString("Native"); //TODO localize
        else if (section == 1)
            return QString("English");
        else if (section == 2)
            return QString("Filename");
        else if (section == 3)
            return QString("Available");
        else if (section == 4)
            return QString("Local");
    }

    return QVariant();

}

int dictionary::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_lData.count();
}

int dictionary::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QHash<int, QByteArray> dictionary::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[IsSelectedRole] = "isSelected";
    roles[AuthorRole] = "author";
    roles[LicenseRole] = "license";
    roles[ReleaseRole] = "release";
    roles[CommentRole] = "comment";
    return roles;
}

bool dictionary::loadFrom(QString fileName)
{
    int aIndex = -1;
    for (int i=0; i<m_lData.count(); i++)
        if (m_lData[i].FileName == fileName) {
            aIndex = i;
            break;
        }
    if ((aIndex > -1) && (m_nCurrentDictionary != aIndex))
    {
        QModelIndex indexTL = this->index( m_nCurrentDictionary, 0 );
        QModelIndex indexBR = this->index( m_nCurrentDictionary, 4 );
        m_nCurrentDictionary = aIndex;
        emit dataChanged(indexTL, indexBR, { IsSelectedRole } ); //clear

        QSettings settings(fileName, QSettings::IniFormat); //TODO: local path
        m_sChars = settings.value("Header/Characters").toString();
        settings.beginGroup("Words");
        m_Words = settings.allKeys();
        m_Words.sort();
        settings.endGroup();

        qDebug() << m_Words.count() << "read from" << fileName;//TODO: categories / QStringView
        //        qDebug() << words.indexOf("MÜHEN"); //TODO: debug

        //get all characters
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

        indexTL = this->index( m_nCurrentDictionary, 0 );
        indexBR = this->index( m_nCurrentDictionary, 4 );
        emit dataChanged(indexTL, indexBR, { IsSelectedRole } ); //set
        return true;
    } else
        return false;
}


QString dictionary::versionToString(const int aValue)
{
  int aMajor, aMinor, aBuild;
  int temp;

  temp = aValue;
  aMajor = temp / 100000;
  temp = temp-aMajor*100000;
  aMinor = temp / 1000;
  temp = temp-aMinor*1000;
  aBuild = temp;
  return QString::number(aMajor)+'.'+QString::number(aMinor)+'.'+QString::number(aBuild);
}

void dictionary::getInfo(modelData *aData)
{
    QFileInfo fileInfo(aData->FileName); //TODO: path
    if (fileInfo.exists())
    {
        QSettings settings(aData->FileName, QSettings::IniFormat);
        aData->InstalledVersionNumber = settings.value("Header/Version").toInt();
        aData->InstalledVersion = versionToString(aData->InstalledVersionNumber);
        aData->Author = settings.value("Header/Author").toString();
        aData->StandardCategory = settings.value("Header/StandardCategory").toString();
        aData->License = settings.value("Header/Licence").toString();
        aData->Release = settings.value("Header/Release").toString();
        aData->Comment = settings.value("Header/Comment").toString();
    }
    else
        aData->InstalledVersion = "";
}

bool dictionary::isWordInDictionary(QString sWord, int *index)
{
  int i, z;
  int lo = 0;
  int hi = m_Words.count()-1;
  while (lo<=hi)
  {
      z = (lo+hi) >> 1; //shr
      i = QString::compare(m_Words[z],sWord);
      if (i < 0)
          lo = z+1;
      else if (i == 0)
      {
          *index = lo;
          return true;
      }
      else if (i > 0)
          hi = z-1;
  }
  *index = lo;
  return false;
}

/*
 Scrabble-like variation of letters
  ABC:
  A    B    C
  AB   BA   CA
  AC   BC   CB
  ABC  BAC  CAB
  ACB  BCA  CBA
*/

QString dictionary::variation(const QString aChars)
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

    while ( true ) //((i > letterSet.length()) && (s == ""))
    {
        while ( true )
        {
            z = chars.indexOf(letterSet[i]) + 1;
//            if ( z==0 )
//                z = letterSet.indexOf("?",aChars); // try to insert joker
            if (z>0)
            {
               s = s + letterSet[i];
               us = s;
//               qDebug() << "i=" << i << "j" << j << "z=" << z << us << ":";
               if ( isWordInDictionary(us, &index) )
                   sResults.append(us);

               if ( (index < m_Words.count()) &&        //TODO: can index be larger than count?
                    m_Words[index].startsWith(us) )     //incomplete or complete word
               {
                   chars.remove(z-1,1);
                   i = 0;
                   j = 0;
               } else
               {
                   s.chop(1);                                    //clear the last letter in temp string
                   j = -1;
               }
            } else
                j = -1;

            if (j == -1)
                break;
        } // while (j > -1)

        if ((i == letterSet.length()-1) && (s.length()>0))              //if last letter in the set and something was found
        {
            while ( true )
            {
                i = letterSet.indexOf(s.last(1))+1;             //start next search with then successor in available
                s.chop(1);                                    //clear the last letter in temp string
                if (i<letterSet.length() || (s==""))
                   break;
            }

            chars = aChars;                                //reset the available letters and delete found chars
            for (j=0; j<s.length(); j++)
            {
               z = chars.indexOf(s[j]);
               //if (z == 0) z = chars.indexOf("?"); //TODO: joker
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
    if (index<m_Words.count())
        return m_Words[index];
    else
    {
        qWarning() << "Word index" << index << "out of bounds" << "(" << m_Words.count() << ")";
        return "";
    }
}

