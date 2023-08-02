#include "move.h"

move::move(bool IsFirstMove, board* aBoard)
    : m_Dimension(dmAbscissa),
      m_PlacedLetters(0),
      m_Value(0),
      m_nBonusValue(0),
      m_IsFirstMove(IsFirstMove),
      m_IsScrabble(false),
      m_pBoard(aBoard)
{
}

bool move::addLetter(Letter aLetter, const bool doCheck)
{
    m_PlacedLetters.append(aLetter);
    if (doCheck)
        return checkMove(); //to set LastError
    else
        return true;
}

Letter move::getLetter(int index) const
{
    if (index < m_PlacedLetters.count())
        return m_PlacedLetters[index];
    else
        return EmptyLetter;
}

bool move::deleteLetter(const unsigned int x)
{
    for (int i = 0; i < m_PlacedLetters.count(); i++)
        if (x == m_PlacedLetters.at(i).Where) {
            m_PlacedLetters.remove(i);
            m_IsScrabble = false;
            m_nBonusValue = 0;
            return checkMove();
        }
    return true;
}

bool sortByPos(const Letter &v1, const Letter &v2)
{
    return v1.Where < v2.Where;
}

bool move::checkMove()
{
    m_Value = 0;
    PlacedWord = "";
    ConnectedWords = "";

    if (m_PlacedLetters.count()>0)
    {
        if (m_IsFirstMove && !checkFirstMove()) {
            m_LastError = QObject::tr("At the first move, place a piece on the start square");
            return false;
        }

        std::sort(m_PlacedLetters.begin(), m_PlacedLetters.end(), sortByPos);

        if (m_PlacedLetters.count()>1) {
            if (!checkOneRowColumn()) {
                m_LastError = QObject::tr("Place pieces in one row or column");
                return false;
            }
            if (!checkConsecutive()) {
                m_LastError = QObject::tr("Place pieces consecutively without gaps");
                return false;
            }
        }
        //dimension is a side result from checkConsecutive but only in case of >1 piece
        else if (m_PlacedLetters.count()==1)
        {
            if (m_IsFirstMove) //single piece on start square
                m_Dimension = dmAbscissa;
            else
            {
                Point3D aPoint = m_PlacedLetters[0].Point;

                if ((leftOf(aPoint).State == LetterState::lsPlaced) ||
                    (rightOf(aPoint).State == LetterState::lsPlaced))
                    m_Dimension = dmAbscissa;
                else if ((aboveOf(aPoint).State == LetterState::lsPlaced) ||
                         (belowOf(aPoint).State == LetterState::lsPlaced))
                    m_Dimension = dmOrdinate;
                else if (m_pBoard->is3D() &&
                         ((behindOf(aPoint).State == LetterState::lsPlaced) ||
                          (infrontOf(aPoint).State == LetterState::lsPlaced)))
                    m_Dimension = dmApplicate;
            }
        }

        ConnectedWords = "";
        //connected words are gathered per getResults() in checkConnection()
        if (!m_IsFirstMove && !checkConnection())
        {
            m_LastError = QObject::tr("Pieces need to be connected with previous");
            m_Value = 0;
            PlacedWord = "";
            ConnectedWords = "";
            return false;
        }
    }
    if (m_PlacedLetters.count()>0)
        PlacedWord = getResults(m_Dimension,m_PlacedLetters.first().Point);
    m_LastError = "";
    return true;
}

void move::setJokerLetter(const uint aWhere, const QString aWhat)
{
    for (int i = 0; i < m_PlacedLetters.count(); i++) {
        if ((m_PlacedLetters[i].Where == aWhere) &&
             m_PlacedLetters[i].IsJoker )
        {
            m_PlacedLetters[i].What = aWhat;
            return;
        }
    }
#if defined(Q_OS_LINUX) && defined(QT_DEBUG)
    qWarning() << "No joker in move";
#endif
}

void move::clear()
{
    PlacedWord = "";
    ConnectedWords = "";
    m_PlacedLetters.clear();
    m_Value = 0;
    m_nBonusValue = 0;
    m_IsScrabble = false;
}

bool move::checkFirstMove()
{
    for (unsigned int i=0; i<qPow(m_pBoard->getBoardSize(), m_pBoard->is3D() ? 3 : 2); i++)
        if ((m_pBoard->getFieldtype(i) == FieldType::ftStart) &&
            (m_pBoard->getLetter(i).State != LetterState::lsEmpty))
           return true;

    return false;
}

bool move::checkOneRowColumn()
{
    const int nx = m_PlacedLetters[0].Point.x();
    const int ny = m_PlacedLetters[0].Point.y();
    const int nz = m_PlacedLetters[0].Point.z();

    if ((nx == m_PlacedLetters[1].Point.x()) &&
        (ny == m_PlacedLetters[1].Point.y()))
        m_Dimension = dmApplicate;
    else
    if ((nx == m_PlacedLetters[1].Point.x()) &&
        (nz == m_PlacedLetters[1].Point.z()))
        m_Dimension = dmOrdinate;
    else
    if ((ny == m_PlacedLetters[1].Point.y()) &&
        (nz == m_PlacedLetters[1].Point.z()))
        m_Dimension = dmAbscissa;
    else
        return false;

    for (int i = 2; i < m_PlacedLetters.count(); i++) {
        if (((m_Dimension == dmAbscissa) &&
             ((ny != m_PlacedLetters[i].Point.y()) ||
              (nz != m_PlacedLetters[i].Point.z()))
             ) ||
             ((m_Dimension == dmOrdinate) &&
              ((nx != m_PlacedLetters[i].Point.x()) ||
               (nz != m_PlacedLetters[i].Point.z()))
             ) ||
             ((m_Dimension == dmApplicate) &&
              ((nx != m_PlacedLetters[i].Point.x()) ||
               (ny != m_PlacedLetters[i].Point.y()))
              )
           ) return false;
    }

    return true;
}

bool move::checkConsecutive()
{
    bool bConsecutive = true;

    const int nx = m_PlacedLetters[0].Point.x();
    const int ny = m_PlacedLetters[0].Point.y();
    const int nz = m_PlacedLetters[0].Point.z();
    int z;

    switch (m_Dimension) {
        case dmAbscissa: {
            for (int i=nx; i < m_PlacedLetters.last().Point.x(); i++) {
                z = m_pBoard->pointToWhere(Point3D(i,ny,nz));
                if (m_pBoard->getLetter(z).State == LetterState::lsEmpty) {
                    bConsecutive = false;
                    break;
                }
            }
            break;
        }
        case dmOrdinate: {
            for (int i = ny; i < m_PlacedLetters.last().Point.y(); i++) {
                z = m_pBoard->pointToWhere(Point3D(nx,i,nz));
                if (m_pBoard->getLetter(z).State == LetterState::lsEmpty) {
                    bConsecutive = false;
                    break;
                }
            }
            break;
        }
        case dmApplicate: {
            for (int i = nz; i < m_PlacedLetters.last().Point.z(); i++) {
                z = m_pBoard->pointToWhere(Point3D(nx,ny,i));
                if (m_pBoard->getLetter(z).State == LetterState::lsEmpty) {
                    bConsecutive = false;
                    break;
                }
             break;
            }
        }
    } //switch

    return bConsecutive;
}

Letter move::leftOf(Point3D aPoint)
{
    if (aPoint.x() > 0)
    {
        aPoint.setX(aPoint.x() - 1);
        return m_pBoard->getLetter(m_pBoard->pointToWhere(aPoint));
    }
    else
        return EmptyLetter;
}
Letter move::rightOf(Point3D aPoint)
{
    if (aPoint.x() < m_pBoard->getBoardSize()-1)
    {
        aPoint.setX(aPoint.x() + 1);
        return m_pBoard->getLetter(m_pBoard->pointToWhere(aPoint));
    }
    else
        return EmptyLetter;
}
Letter move::aboveOf(Point3D aPoint)
{
    if (aPoint.y() > 0)
    {
        aPoint.setY(aPoint.y() - 1);
        return m_pBoard->getLetter(m_pBoard->pointToWhere(aPoint));
    }
    else
        return EmptyLetter;
}
Letter move::belowOf(Point3D aPoint)
{
    if (aPoint.y() < m_pBoard->getBoardSize()-1)
    {
        aPoint.setY(aPoint.y() + 1);
        return m_pBoard->getLetter(m_pBoard->pointToWhere(aPoint));
    }
    else
        return EmptyLetter;
}
Letter move::infrontOf(Point3D aPoint)
{
    if (aPoint.z() > 0)
    {
        aPoint.setZ(aPoint.z() - 1);
        return m_pBoard->getLetter(m_pBoard->pointToWhere(aPoint));
    }
    else
        return EmptyLetter;
}
Letter move::behindOf(Point3D aPoint)
{
    if (aPoint.z() < m_pBoard->getBoardSize()-1)
    {
        aPoint.setZ(aPoint.z() + 1);
        return m_pBoard->getLetter(m_pBoard->pointToWhere(aPoint));
    }
    else
        return EmptyLetter;
}

bool move::checkConnection()
{
    bool bHasConnection = false;
    Point3D aPoint;

    for (int i = 0; i <= m_PlacedLetters.count()-1; i++)
    {
        aPoint = m_PlacedLetters[i].Point;
        if ((leftOf(aPoint).State == LetterState::lsPlaced) ||
            (rightOf(aPoint).State == LetterState::lsPlaced))
        {
           if (m_Dimension != dmAbscissa)
           {
               if (!ConnectedWords.isEmpty())
                   ConnectedWords += ",";
               ConnectedWords += getResults(dmAbscissa,aPoint);
           }
           bHasConnection = true;
        }
        if ((aboveOf(aPoint).State == LetterState::lsPlaced) ||
            (belowOf(aPoint).State == LetterState::lsPlaced))
        {
            if (m_Dimension != dmOrdinate)
            {
                if (!ConnectedWords.isEmpty())
                    ConnectedWords += ",";
                ConnectedWords += getResults(dmOrdinate,aPoint);
            }
            bHasConnection = true;
        }
        if (m_pBoard->is3D() &&
            ((behindOf(aPoint).State == LetterState::lsPlaced) ||
             (infrontOf(aPoint).State == LetterState::lsPlaced)))
        {
            if (m_Dimension != dmApplicate)
            {
                if (!ConnectedWords.isEmpty())
                    ConnectedWords += ",";
                ConnectedWords += getResults(dmApplicate,aPoint);
            }
            bHasConnection = true;
        }
    }
    return bHasConnection;
}

QString move::getResults(const Dimension eDimension, const Point3D nStart)
{
    //iterate the dimension until the first/last empty square
    unsigned int nfirst = 0;
    unsigned int nlast = 0;
    QString aWord;

    switch (eDimension)
    {
        case dmAbscissa:
            nfirst = nStart.x();
            for (int i=nfirst-1; i>=0; i--)
                if (m_pBoard->getLetter(m_pBoard->pointToWhere(Point3D(i,nStart.y(),nStart.z()))).State == LetterState::lsEmpty)
                    break;
                else nfirst = i;
            nlast = m_PlacedLetters.last().Point.x();
            for (int i=nlast+1; i<m_pBoard->getBoardSize(); i++)
                if (m_pBoard->getLetter(m_pBoard->pointToWhere(Point3D(i,nStart.y(),nStart.z()))).State == LetterState::lsEmpty)
                    break;
                else nlast = i;
            break;
        case dmOrdinate:
            nfirst = nStart.y();
            for (int i=nfirst-1; i>=0; i--)
                if (m_pBoard->getLetter(m_pBoard->pointToWhere(Point3D(nStart.x(),i,nStart.z()))).State == LetterState::lsEmpty)
                    break;
                else nfirst = i;
            nlast = m_PlacedLetters.last().Point.y();
            for (int i=nlast+1; i<m_pBoard->getBoardSize(); i++)
                if (m_pBoard->getLetter(m_pBoard->pointToWhere(Point3D(nStart.x(),i,nStart.z()))).State == LetterState::lsEmpty)
                    break;
                else nlast = i;
            break;
        case dmApplicate:
            nfirst = nStart.z();
            for (int i=nfirst-1; i>=0; i--)
                if (m_pBoard->getLetter(m_pBoard->pointToWhere(Point3D(nStart.x(),nStart.y(),i))).State == LetterState::lsEmpty)
                    break;
                else nfirst = i;
            nlast = m_PlacedLetters.last().Point.z();
            for (int i=nlast+1; i<m_pBoard->getBoardSize(); i++)
                if (m_pBoard->getLetter(m_pBoard->pointToWhere(Point3D(nStart.x(),nStart.y(),i))).State == LetterState::lsEmpty)
                    break;
                else nlast = i;
            break;
    }

    unsigned int z = 0;
    unsigned int aWordBonus = 1;
    unsigned int aLetterBonus;
    unsigned int aValue = 0;
    Letter aLetter;

    for (unsigned int i = nfirst; i <= nlast; i++)
    {
        switch (eDimension)
        {
          case dmAbscissa: z = m_pBoard->pointToWhere(Point3D(i,nStart.y(),nStart.z())); break;
          case dmOrdinate: z = m_pBoard->pointToWhere(Point3D(nStart.x(),i,nStart.z())); break;
          case dmApplicate: z = m_pBoard->pointToWhere(Point3D(nStart.x(),nStart.y(),i)); break;
        }
        aWord += m_pBoard->getLetter(z).What;
        if (m_pBoard->getLetter(z).State == LetterState::lsPlaced)
            aValue += m_pBoard->getLetter(z).Value;
        else if (m_pBoard->getLetter(z).State == LetterState::lsBoard)
        {
            aLetterBonus = 1;
            switch (m_pBoard->getFieldtype(z))
            {
              case ftStart: aWordBonus *= 2; break;
              case ftDefault: break;
              case ftDoubleLetter: aLetterBonus = 2; break;
              case ftTrippleLetter: aLetterBonus = 3; break;
              case ftQuadLetter: aLetterBonus = 4; break;
              case ftDoubleWord: aWordBonus *= 2; break;
              case ftTrippleWord: aWordBonus *= 3; break;
              case ftQuadWord: aWordBonus *= 4; break;
              case ftPiece: qFatal("Field is occupied"); break;
            }
            aValue += m_pBoard->getLetter(z).Value * aLetterBonus;
         }
    }
    m_Value += aValue * aWordBonus;
    return aWord;
}

int move::getPosition()
{
    int aResult;

    if (m_PlacedLetters.length() > 0) {
        switch (m_Dimension) {
        case dmAbscissa:
            aResult = m_PlacedLetters[0].Point.z();
            break;
        case dmOrdinate:
            aResult = m_PlacedLetters[0].Point.x();
            break;
        case dmApplicate:
            aResult = m_PlacedLetters[0].Point.y();
            break;
        default:
#if defined(Q_OS_LINUX) && defined(QT_DEBUG)
            qWarning() << "Unexpected dimension in getPosition()";
#endif
            break;
        }
    } else
        aResult = m_pBoard->getBoardSize() / 2; //some default for moves w/o placed letters

    return aResult;
}
