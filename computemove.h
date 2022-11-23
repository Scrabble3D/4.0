#pragma once

#include "board.h"
#include "rackmodel.h"
#include "dictionary.h"
#include "move.h"

class computemove : public QObject
{
public:
    computemove(QObject* parent, board* aBoard, rackmodel* aRack, dicFile* aDictionary);
    void run(const bool isFirstMove);
    void markLettersForExchange();
    int numberOfResults() { return m_pMoves.count(); }
    move* result(int index) { return m_pMoves[index]; }
    void clear() { m_pMoves.clear(); m_pParent->setProperty("bestMoveCount", 0); }

private:
    QObject* m_pParent;
    board* m_pBoard;
    rackmodel* m_pRack;
    dicFile* m_pDictionary;
    QList<move*> m_pMoves;
    bool canPlaceWord(const QString sWord, const uint nBoardPos,
                      const uint nColRow, const uint dim,
                      move* aMove, board* aBoard,
                      QVector<Letter> aRackLetter);
    bool hasNeighbors(const QString sWord, const uint nBoardPos,
                      const uint nColRow, const uint dim);
    bool isAtStart(const uint pos, const uint colrow);
    int getRackLetter(QVector<Letter> aRackLetter, const QString aChar);
};
