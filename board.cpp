#include "board.h"

board::board()
{
    //initialize with 2D Classic distribution
    const int ft[255] =
    {6,1,1,2,1,1,1,6,1,1,1,2,1,1,6,
     1,5,1,1,1,3,1,1,1,3,1,1,1,5,1,
     1,1,5,1,1,1,2,1,2,1,1,1,5,1,1,
     2,1,1,5,1,1,1,2,1,1,1,5,1,1,2,
     1,1,1,1,5,1,1,1,1,1,5,1,1,1,1,
     1,3,1,1,1,3,1,1,1,3,1,1,1,3,1,
     1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,
     6,1,1,2,1,1,1,0,1,1,1,2,1,1,6,
     1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,
     1,3,1,1,1,3,1,1,1,3,1,1,1,3,1,
     1,1,1,1,5,1,1,1,1,1,5,1,1,1,1,
     2,1,1,5,1,1,1,2,1,1,1,5,1,1,2,
     1,1,5,1,1,1,2,1,2,1,1,1,5,1,1,
     1,5,1,1,1,3,1,1,1,3,1,1,1,5,1,
     6,1,1,2,1,1,1,6,1,1,1,2,1,1,6};
    for (unsigned int i=0; i<sizeof(ft)/sizeof(int); i++)
    {
        m_Fieldtypes.append( static_cast<FieldType>(ft[i]) );
        m_Letters.append( EmptyLetter );
    }
    m_bIs3D = false;
    m_nBoardSize = qSqrt(sizeof(ft)/sizeof(int));
    m_nFieldSize = m_Fieldtypes.count();
    m_nActivePosition = 0;
    m_eActiveDimension = dmAbscissa;
}

void board::initialize(const QVariantList fieldTypeArray, const bool Is3D)
{
    m_eActiveDimension = dmAbscissa;
    m_nActivePosition = 0;

    m_nFieldSize = fieldTypeArray.count();
    m_bIs3D = Is3D;
    if (m_bIs3D)
        m_nBoardSize = round(cbrt(m_nFieldSize));
    else
        m_nBoardSize = round(sqrt(m_nFieldSize));
    //m_ActivePosition = round(m_BoardSize / 2);
    //clear
    m_Fieldtypes.clear();
    m_Letters.clear();
    for (int i=0; i<m_nFieldSize; i++) {
       m_Fieldtypes.append( static_cast<FieldType>(fieldTypeArray[i].toInt()) );
       m_Letters.append(EmptyLetter);
    }
}

void board::initialize(const board *aParentBoard, const int nMove)
{
    m_nBoardSize = aParentBoard->m_nBoardSize;
    m_nFieldSize = aParentBoard->m_nFieldSize;
    m_Letters.clear();
    for (int i = 0; i < aParentBoard->m_Letters.count(); i++)
       if ((nMove < 0) || (aParentBoard->m_Letters[i].When < static_cast<uint>(nMove)))
            m_Letters.append(aParentBoard->m_Letters[i]);
        else
            m_Letters.append(EmptyLetter);
    m_Fieldtypes.clear();
    m_Fieldtypes.append(aParentBoard->m_Fieldtypes);
    m_eActiveDimension = aParentBoard->m_eActiveDimension;
    m_nActivePosition = aParentBoard->m_nActivePosition;
    m_bIs3D = aParentBoard->m_bIs3D;
}

FieldType board::getFieldtype(const int index)
{
    if (index < m_Fieldtypes.count())
        return m_Fieldtypes[index];
    else {
#ifdef QT_DEBUG
        qWarning() << "Field index out of bounds in getFieldtype()" << index << "/" << m_Fieldtypes.count();
#endif
        return ftDefault;
    }
}

void board::setLetter(Letter &aLetter)
{
    aLetter.Where = pointToWhere(aLetter.Point);
    m_Letters[aLetter.Where] = aLetter;
}

Letter board::getLetter(const int index)
{
    if (index < m_Letters.count())
        return m_Letters[index];
    else {
#ifdef QT_DEBUG
        qWarning() << "Letter index out of bounds in getLetter()" << index << "/" << m_Letters.count();
#endif
        return EmptyLetter;
    }
}

Letter board::getLetter(const Point3D aWhere)
{
    return getLetter(pointToWhere(aWhere));
}

Letter board::getLetter(const int col, const int row)
{
    int aIndex = col * m_nBoardSize + row;
    return getLetter(pos3D(aIndex));
}

void board::placeLetters()
{
    for (int i=0; i < m_nFieldSize; i++)
      if (m_Letters[i].State == LetterState::lsBoard)
          m_Letters[i].State = LetterState::lsPlaced;
}

void board::removeLetter(const int aLetterIndex)
{
    if ( aLetterIndex < m_Letters.count() )
        m_Letters[aLetterIndex] = EmptyLetter;
#ifdef QT_DEBUG
    else
      qWarning() << "Letter index out of bounds in removeLetter()" << aLetterIndex << "/" << m_Letters.count();
#endif
}

int board::pointToWhere(const Point3D nPoint)
{
    return nPoint.x() +
           nPoint.y() * (m_nBoardSize) +
           nPoint.z() * (m_nBoardSize * m_nBoardSize) ;
}

int board::pointToPlane(const Point3D nPoint)
{
    switch (m_eActiveDimension) {
      case dmAbscissa: //always true for 2D
        return nPoint.x() + nPoint.y() * m_nBoardSize;
      case dmOrdinate:
        return nPoint.y() + nPoint.z() * m_nBoardSize;
      case dmApplicate:
        return nPoint.x() + nPoint.z() * m_nBoardSize;
    }
    return 0;
}

Point3D board::whereToPoint(const int aWhere)
{
    return Point3D( aWhere % m_nBoardSize,
                   (aWhere / m_nBoardSize) % m_nBoardSize,
                    aWhere / (m_nBoardSize * m_nBoardSize));
}

Point3D board::pos3D(int nIndex2D)
{
    switch (m_eActiveDimension) {
      case dmAbscissa: //always true for 2D
        return Point3D(nIndex2D % m_nBoardSize, nIndex2D / m_nBoardSize, m_nActivePosition);
      case dmOrdinate:
        return Point3D(m_nActivePosition, nIndex2D % m_nBoardSize, nIndex2D / m_nBoardSize);
      case dmApplicate:
        return Point3D(nIndex2D % m_nBoardSize, m_nActivePosition, nIndex2D / m_nBoardSize);
    }
    return Point3D(0,0,0);
}

int board::index2D(Point3D aPoint3D)
{
    int aIndex = 0;
    switch (m_eActiveDimension) {
      case dmAbscissa:
        aIndex = aPoint3D.y()*m_nBoardSize + aPoint3D.x();
        break;
      case dmOrdinate:
        aIndex = aPoint3D.z()*m_nBoardSize + aPoint3D.y();
        break;
      case dmApplicate:
        aIndex = aPoint3D.z()*m_nBoardSize + aPoint3D.x();
        break;
    }
    return aIndex;
}

QString board::getWordsAt(const int index)
{
    Point3D aPoint;
    Letter aLetter;
    QString sWordAbscissa;
    QString sWordOrdinate;
    QString sWordApplicate;

    // abscissa
    aPoint = pos3D(index);
    aLetter = getLetter(aPoint);
    if (aLetter.IsEmpty()) //no letter at empty field
        return "";

    while (!aLetter.IsEmpty()) {
        sWordAbscissa = aLetter.What + sWordAbscissa;
        if (aPoint.x() == 0) break;
        aPoint.setX( aPoint.x() - 1 );
        aLetter = getLetter(aPoint);
    }
    aPoint = pos3D(index);
    aPoint.setX( aPoint.x() + 1);
    if (aPoint.x() < m_nBoardSize)
        aLetter = getLetter(aPoint);
    else
        aLetter = EmptyLetter;
    while (!aLetter.IsEmpty()) {
        sWordAbscissa = sWordAbscissa + aLetter.What;
        if (aPoint.x() == m_nBoardSize - 1) break;
        aPoint.setX( aPoint.x() + 1 );
        aLetter = getLetter(aPoint);
    }
    if (sWordAbscissa.length() == 1) //nothing connected
        sWordAbscissa.clear();

    // ordinate
    aPoint = pos3D(index);
    aLetter = getLetter(aPoint);
    while (!aLetter.IsEmpty()) {
        sWordOrdinate = aLetter.What + sWordOrdinate;
        if (aPoint.y() == 0) break;
        aPoint.setY( aPoint.y() - 1 );
        aLetter = getLetter(aPoint);
    }
    aPoint = pos3D(index);
    aPoint.setY( aPoint.y() + 1);
    if (aPoint.y() < m_nBoardSize)
        aLetter = getLetter(aPoint);
    else
        aLetter = EmptyLetter;
    while (!aLetter.IsEmpty()) {
        sWordOrdinate = sWordOrdinate + aLetter.What;
        if (aPoint.y() == m_nBoardSize - 1) break;
        aPoint.setY( aPoint.y() + 1 );
        aLetter = getLetter(aPoint);
    }
    if (sWordOrdinate.length() == 1) //nothing connected
        sWordOrdinate.clear();

    //applicate
    if (m_bIs3D) {
        aPoint = pos3D(index);
        aLetter = getLetter(aPoint);
        while (!aLetter.IsEmpty()) {
            sWordApplicate = aLetter.What + sWordApplicate;
            if (aPoint.z() == 0) break;
            aPoint.setZ( aPoint.z() - 1 );
            aLetter = getLetter(aPoint);
        }
        aPoint = pos3D(index);
        aPoint.setZ( aPoint.z() + 1);
        if (aPoint.z() < m_nBoardSize)
            aLetter = getLetter(aPoint);
        else
            aLetter = EmptyLetter;
        while (!aLetter.IsEmpty()) {
            sWordApplicate = sWordApplicate + aLetter.What;
            if (aPoint.z() == m_nBoardSize - 1) break;
            aPoint.setZ( aPoint.z() + 1 );
            aLetter = getLetter(aPoint);
        }
        if (sWordApplicate.length() == 1) //nothing connected
            sWordApplicate.clear();
    }

    QString sWord;
    if (!sWordAbscissa.isEmpty())
        sWord = sWordAbscissa;
    if (!sWordOrdinate.isEmpty()) {
        if (!sWord.isEmpty())
            sWord += ",";
        sWord += sWordOrdinate;
    }
    if (!sWordApplicate.isEmpty()) {
        if (!sWord.isEmpty())
            sWord += ",";
        sWord += sWordApplicate;
    }

    return sWord;
}

void board::setJokerLetter(const int aLetterIndex, const QString aWhat)
{
    if ( aLetterIndex < m_Letters.count() )
        m_Letters[aLetterIndex].What = aWhat;
#ifdef QT_DEBUG
    else
        qWarning() << "Letter index out of bounds in setJokerLetter()" << aLetterIndex << "/" << m_Letters.count();
#endif
}
