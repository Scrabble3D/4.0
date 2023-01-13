#pragma once

#include <QThreadPool>
#include <QMutex>

#include "board.h"
#include "rackmodel.h"
#include "dictionary.h"
#include "move.h"

class computemove : public QObject
{
    Q_OBJECT
public:
    computemove(QObject* parent);
    void run(const bool isFirstMove,
             board* aBoard,
             rackmodel* aRack,
             dicFile* aDictionary);
    void markLettersForExchange(rackmodel* aRack);
    int numberOfResults() { return m_pMoves.count(); }
    sharedMove result(int index) { return m_pMoves[index]; }
    void clear();

private slots:
    void doAddWord(sharedMove aMove);
    void threadFinished();

private:
    QObject* m_pParent;
    int m_nTotal;    // total number of operations / threads
    int m_nDone;     // number of threads done
    int m_nProgress; // percentage done (done/total)

    QList<sharedMove> m_pMoves;
    QMutex m_pMutex;
};

class computeWord : public QObject, public QRunnable
{
    Q_OBJECT
public:
    computeWord(const bool isFirstMove,
                const int nColRow,
                const int nOrientation,
                board* aBoard,
                rackmodel* aRack,
                dicFile* aDictionary);
    ~computeWord();
    void run() override;

signals:
    void onValidWord(sharedMove aMove);
    void threadFinished();

private:
    board *m_pBoard;
    const board *currentBoard;
    rackmodel* m_pRack;
    dicFile* m_pDictionary;
    bool m_bIsFirstMove;
    int m_nColRow;
    int m_nOrientation;

    bool canPlaceWord(const QString sWord, const uint nBoardPos,
                      const uint nColRow, const uint dim,
                      QSharedPointer<move> aMove, board* aBoard,
                      QVector<Letter> aRackLetter);
    bool hasNeighbors(const QString sWord, const uint nBoardPos,
                      const uint nColRow, const uint dim);
    bool isAtStart(const uint pos, const uint colrow);
    int getRackLetter(QVector<Letter> aRackLetter, const QString aChar);
};
