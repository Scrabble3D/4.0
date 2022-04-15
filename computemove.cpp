#include "computemove.h"
#include "gameplay.h"

computemove::computemove(QObject* parent, board* aBoard, rackmodel* aRack, dicFile* aDictionary)
    : m_pParent(parent),
      m_pBoard(aBoard),
      m_pRack(aRack),
      m_pDictionary(aDictionary)
{
}

//sort possible moves by their value
bool sortByValue(move* aMove, move* bMove) {
    return aMove->Value() > bMove->Value();
}

//TODO: compute: no letter found -> exchange
void computemove::run(const bool isFirstMove)
{
    //clear previously calculated moves and disable spin edit in UI via emit=0
    clear();
    //reset progressbar
    m_pParent->setProperty("placedValue", 0);

    //rack + (x*y) board = letters to combine for words
    QString sRack;
    for (int i=0; i<m_pRack->rackSize(); i++)
        sRack += m_pRack->getLetter(i).What;

    QVector<Letter> aRackLetter;
    board* aBoard = new board();
    bool canPlace;

    Letter aLetter;
    QString sBoard;
    double nTotalProgress = 2 * m_pBoard->getBoardSize();
    if (m_pBoard->is3D())
        nTotalProgress *= 3 * m_pBoard->getBoardSize();
    double nCurrentProgress = 0;

    for (int nDimension = 0; nDimension < 3; nDimension++) { //dx, dy, dz
        for (int nPlane = 0; nPlane < m_pBoard->getBoardSize(); nPlane++) { //plane per dimension
            //dont try if just 2D
            if (!m_pBoard->is3D() && ((nDimension > 0) || (nPlane > 0)))
                continue;
            //set plane at 3d board

            m_pBoard->setActiveDimension(static_cast<Dimension>(nDimension));
            m_pBoard->setActivePosition(nPlane);
            //iterate at the active plane
            for (int nOrientation = 0; nOrientation <= 1; nOrientation++) { //horizontal/vertical
                for (int nColRow = 0; nColRow<m_pBoard->getBoardSize(); nColRow++) { //column/row
                    nCurrentProgress++;
                    m_pParent->setProperty("computeProgress",round((nCurrentProgress/nTotalProgress)*100));
                    sBoard = "";

                    if (isFirstMove && (!isAtStart(nColRow, nOrientation)))
                        continue;

                    for (int i=0; i<m_pBoard->getBoardSize(); i++) {
                        if ( nOrientation == 0 )
                            aLetter = m_pBoard->getLetter(i, nColRow);
                        else
                            aLetter = m_pBoard->getLetter(nColRow, i);
                        if (!aLetter.IsEmpty()) sBoard += aLetter.What;
                    }
                    //calculate possible words
                    QStringList sWords = m_pDictionary->variation(sRack+sBoard).split(",");
                    //try to place words
                    //TODO: computemove: run calculation in threads
                    for (int nWordPos = 0; nWordPos<sWords.count(); nWordPos++) {
                        //don't try to place words that cannot be placed
                        if (sWords[nWordPos].isEmpty() || (sWords[nWordPos] == sBoard))
                            continue;
                        else
                            //iterate over the board
                            for (int nBoardPos=0; nBoardPos<=m_pBoard->getBoardSize()-sWords[nWordPos].length(); nBoardPos++)
                            {
                                //do not try if there is nothing placed next
                                if (!isFirstMove && !hasNeighbors(sWords[nWordPos], nBoardPos, nColRow, nOrientation) )
                                    continue;
                                //reset temporary board
                                aBoard->initialize(m_pBoard);
                                //create new move with reference to the temporary board
                                move* aMove = new move(isFirstMove, aBoard);
                                //reset rack temporary letters
                                for (int nRackPos=0; nRackPos<m_pRack->rackSize(); nRackPos++)
                                    aRackLetter.append(m_pRack->getLetter(nRackPos));
                                //check whether placing the word is possible
                                canPlace = canPlaceWord(sWords[nWordPos], nBoardPos, nColRow, nOrientation, aMove, aBoard, aRackLetter);
                                //no letters placed meaning word consists only of previously placed letters
                                canPlace = canPlace ? (aMove->letterCount() > 0) : false;
                                //check whether placed letters comply with all rules
                                canPlace = canPlace ? aMove->checkMove() : false;
                                //check whether placed and connected words are known
                                canPlace = canPlace ? (m_pDictionary->checkWords(
                                                           aMove->PlacedWord,
                                                           aMove->ConnectedWords).isEmpty()) : false;
                                //store the move or drop it
                                if (canPlace)
                                    m_pMoves.append(aMove);
                                else
                                    delete aMove;

                                aRackLetter.clear();
                            } //nBoardPos
                    } //sWord.count()
                    sWords.clear();
                }
            }
        }
    }
    std::sort(m_pMoves.begin(),m_pMoves.end(), sortByValue);
    delete aBoard;
}

void computemove::markLettersForExchange()
{
    int nExchange = 0;
    // mark multiple letters for exchange
    for (int i=0; i<m_pRack->rackSize()-1; i++) {
        Letter aLetter = m_pRack->getLetter(i);
        if (!aLetter.IsEmpty())
            for (int j=i+1; j<m_pRack->rackSize(); j++) {
                Letter bLetter = m_pRack->getLetter(j);
                if ((aLetter.What == bLetter.What) && (!bLetter.IsExchange)) {
                    m_pRack->toggleExchangeFlag(j);
                    nExchange++;
                }
            }
    }
    // otherwise exchange all
    if (nExchange == 0) {
        for (int i=0; i<m_pRack->rackSize(); i++)
            m_pRack->toggleExchangeFlag(i);
    }
}

int computemove::getRackLetter(QVector<Letter> aRackLetter, const QString aChar)
{
    for (int i=0; i<aRackLetter.count(); i++)
        if (aRackLetter[i].What == aChar)
            return i;
    return -1;
}

bool computemove::isAtStart(const uint pos, const uint colrow)
{
    int nIndex;
    const int aBoardSize(m_pBoard->getBoardSize());
    for (int i = 0; i < aBoardSize; i++) {
        switch (colrow) {
        case 0: nIndex = m_pBoard->pointToWhere(m_pBoard->pos3D( i*aBoardSize + pos )); break;
        case 1: nIndex = m_pBoard->pointToWhere(m_pBoard->pos3D( pos*aBoardSize + i )); break;
        }
        if (m_pBoard->getFieldtype(nIndex) == ftStart)
           return true;
    }
    return false;
}

bool computemove::hasNeighbors(const QString sWord, const uint nBoardPos, const uint nColRow, const uint dim)
{
    Point3D aPoint;
    const int aBoardSize(m_pBoard->getBoardSize());
    switch (dim) {
    case 0: if (nBoardPos > 0) {
                aPoint = m_pBoard->pos3D((nBoardPos - 1) * aBoardSize + nColRow); //left of word
                if (m_pBoard->getLetter(aPoint).State != LetterState::lsEmpty)
                    return true;
            }
            if (nBoardPos + sWord.length() < m_pBoard->getBoardSize() - 1) {
                aPoint = m_pBoard->pos3D((nBoardPos + sWord.length() + 1) * aBoardSize + nColRow); //right of the word
                if (m_pBoard->getLetter(aPoint).State != LetterState::lsEmpty)
                    return true;
            }
            break;
    case 1: if (nBoardPos > 0) {
                aPoint = m_pBoard->pos3D(nColRow * aBoardSize + (nBoardPos - 1)); //above of word
                if (m_pBoard->getLetter(aPoint).State != LetterState::lsEmpty)
                return true;
            }
            if (nBoardPos + sWord.length() < m_pBoard->getBoardSize() - 1) {
                aPoint = m_pBoard->pos3D(nColRow * aBoardSize + (nBoardPos + sWord.length() + 1)); //below of the word
                if (m_pBoard->getLetter(aPoint).State != LetterState::lsEmpty)
                    return true;
            }
            break;
    } //switch
    for (uint nWordPos=0; nWordPos<sWord.length(); nWordPos++)
    {
        switch (dim) {
        case 0: if (nColRow > 0) {
                    aPoint = m_pBoard->pos3D( (nBoardPos + nWordPos) * aBoardSize + (nColRow - 1) ); //above
                    if (m_pBoard->getLetter(aPoint).State != LetterState::lsEmpty)
                      return true;
                }
                if (int(nColRow) < m_pBoard->getBoardSize() - 1) {
                    aPoint = m_pBoard->pos3D( (nBoardPos + nWordPos) * aBoardSize + (nColRow + 1) ); //below
                    if (m_pBoard->getLetter(aPoint).State != LetterState::lsEmpty)
                        return true;
                }
                break;
        case 1: if  (nColRow > 0) {
                    aPoint = m_pBoard->pos3D( (nColRow - 1) * aBoardSize + (nBoardPos + nWordPos) ); //left
                    if (m_pBoard->getLetter(aPoint).State != LetterState::lsEmpty)
                        return true;
                }
                if (int(nColRow) < m_pBoard->getBoardSize() - 1) {
                    aPoint = m_pBoard->pos3D( (nColRow + 1) * aBoardSize + (nBoardPos + nWordPos) ); //right
                    if (m_pBoard->getLetter(aPoint).State != LetterState::lsEmpty)
                        return true;
                }
                break;
        } //switch
    } // for to
    return false;
}

bool computemove::canPlaceWord(const QString sWord, const uint nBoardPos,
                               const uint nColRow, const uint dim,
                               move* aMove, board* aBoard, QVector<Letter> aRackLetter)
{

    Letter aLetter;
    for (uint nWordPos = 0; nWordPos < sWord.length(); nWordPos++) {
        if (dim==0)
            aLetter = aBoard->getLetter(nBoardPos+nWordPos, nColRow);
        else
            aLetter = aBoard->getLetter(nColRow, nBoardPos+nWordPos);
        if ( aLetter.IsEmpty() ) {
            int nRack = getRackLetter(aRackLetter, sWord[nWordPos]);
            if (nRack == -1) nRack = getRackLetter(aRackLetter, JokerChar); //try joker
            if (nRack >= 0) {
                aLetter = aRackLetter[nRack];
                if (aLetter.IsJoker) aLetter.What = sWord[nWordPos];
                aLetter.State = LetterState::lsBoard;
                aLetter.When = 1;
                aLetter.Who = 1;
                if (dim==0)
                    aLetter.Point = aBoard->pos3D( (nBoardPos + nWordPos) * aBoard->getBoardSize() + nColRow );
                else
                    aLetter.Point = aBoard->pos3D( nColRow * aBoard->getBoardSize() + (nBoardPos + nWordPos) );
                aLetter.Where = aBoard->pointToWhere(aLetter.Point);
                aLetter.RackPos = nRack;
                aBoard->setLetter(aLetter);
                aRackLetter[nRack] = EmptyLetter;
                aMove->addLetter(aLetter, false); //false = don't check the move now
                if (aMove->letterCount() == aRackLetter.count())
                    aMove->setBonus( 50, true ); //add some bonus for sorting; the real value is done in gameplay::dropletter
            }
            else
                return false;
        } else
        if (aLetter.What != sWord[nWordPos])
            return false;
    }
    return true;
}
