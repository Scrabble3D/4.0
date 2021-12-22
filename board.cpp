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
    for (unsigned int i=0; i<256; i++)
    {
        m_Fieldtypes.append( static_cast<FieldType>(ft[i]) );
        m_Letters.append( EmptyLetter );
    }
    m_nBoardsize = 15;
    m_nActivePosition = 0;
    m_eActiveDimension = dmAbscissa;
}

void board::initialize(const QVariantList fieldTypeArray, const bool Is3D)
{
    m_eActiveDimension = dmAbscissa;
    m_nActivePosition = 0;

    m_nFieldsize = fieldTypeArray.count();
    if (Is3D)
        m_nBoardsize = round(cbrt(m_nFieldsize));
    else
        m_nBoardsize = round(sqrt(m_nFieldsize));
    //m_ActivePosition = round(m_BoardSize / 2);
    //clear
    m_Fieldtypes.clear();
    m_Letters.clear();
    for (int i=0; i<m_nFieldsize; i++) {
       m_Fieldtypes.append( static_cast<FieldType>(fieldTypeArray[i].toInt()) );
       m_Letters.append(EmptyLetter);
    }
}

FieldType board::getFieldtype(const uint index)
{
    if (index<m_Fieldtypes.count())
        return m_Fieldtypes[index];
    else
    {
        qWarning() << "Field index" << index << "out of bounds" << "(" << m_Fieldtypes.count() << ")";
        return ftDefault;
    }
}

void board::setLetter(Letter &aLetter)
{
    aLetter.Where = PointToWhere(aLetter.Point);
    m_Letters[aLetter.Where] = aLetter;
}

Letter board::getLetter(const uint index)
{
    if (index<m_Letters.count())
        return m_Letters[index];
    else
    {
        qWarning() << "Letter index" << index << "out of bounds" << "(" << m_Letters.count() << ")";
        return EmptyLetter;
    }
}

void board::placeLetters()
{
    for (int i=0; i < m_nFieldsize; i++)
      if (m_Letters[i].State == LetterState::lsBoard)
          m_Letters[i].State = LetterState::lsPlaced;
}

void board::removeLetter(const unsigned int aLetterIndex)
{
    if ( aLetterIndex < m_Letters.count() )
        m_Letters[aLetterIndex] = EmptyLetter;
}

unsigned int board::PointToWhere(const Point3D nPoint)
{
    return nPoint.x() +
           nPoint.y() * (m_nBoardsize) +
           nPoint.z() * (m_nBoardsize * m_nBoardsize) ;
}

Point3D board::WhereToPoint(const unsigned int aWhere)
{
    return Point3D( aWhere % m_nBoardsize,
                   (aWhere / m_nBoardsize) % m_nBoardsize,
                    aWhere / (m_nBoardsize * m_nBoardsize));
}

Point3D board::Pos3D(int nIndex2D)
{
    Point3D aPoint;
    switch (m_eActiveDimension) {
      case dmAbscissa: //always true for 2D
        aPoint = Point3D(nIndex2D % m_nBoardsize, nIndex2D / m_nBoardsize, m_nActivePosition);
        break;
      case dmOrdinate:
        aPoint = Point3D(m_nActivePosition, nIndex2D % m_nBoardsize, nIndex2D / m_nBoardsize);
        break;
      case dmApplicate:
        aPoint = Point3D(nIndex2D % m_nBoardsize, m_nActivePosition, nIndex2D / m_nBoardsize);
        break;
    }
    return aPoint;
}

int board::Index2D(Point3D aPoint3D)
{
    int aIndex;
    switch (m_eActiveDimension) {
      case dmAbscissa:
        aIndex = aPoint3D.y()*m_nBoardsize + aPoint3D.x();
        break;
      case dmOrdinate:
        aIndex = aPoint3D.z()*m_nBoardsize + aPoint3D.y();
        break;
      case dmApplicate:
        aIndex = aPoint3D.z()*m_nBoardsize + aPoint3D.x();
        break;
    }
    return aIndex;
}
