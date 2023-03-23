#pragma once

#define threaded

#ifdef threaded
#include <QThreadPool>
#include <QMutex>
#endif

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
             dictionary* aDictionary);
    void markLettersForExchange(rackmodel* aRack);
    sharedMove result(int index) { return m_pMoves[index]; }
    void clear();

signals:
    void onComputeProgress(int);
    void onComputeResults(int);

private slots:
    void doAddWord(sharedMove aMove);
    void threadFinished();

private:
    int m_nTotal;    // total number of operations / threads
    int m_nDone;     // number of threads done
    int m_nProgress; // percentage done (done/total)

    QList<sharedMove> m_pMoves;
#ifdef threaded
    QMutex m_pMutex;
#endif
};

#ifdef threaded
class computeWord : public QObject, public QRunnable
#else
class computeWord : public QObject
#endif
{
    Q_OBJECT
public:
    computeWord(const bool isFirstMove,
                const int nColRow,
                const int nOrientation,
                board* aBoard,
                rackmodel* aRack,
                dictionary* aDictionary);
    ~computeWord();
#ifdef threaded
    void run() override;
#else
    void run();
#endif
signals:
    void onValidWord(sharedMove aMove);
    void threadFinished();

private:
    board *m_pBoard;
    rackmodel* m_pRack;
    dictionary* m_pDictionary;
    bool m_bIsFirstMove;
    int m_nColRow;
    int m_nOrientation;

    bool canPlaceWord(const QString sWord, const int nBoardPos,
                      const int nColRow, const int dim,
                      QSharedPointer<move> aMove, board* aBoard,
                      QVector<Letter> aRackLetter);
    bool hasNeighbors(const QString sWord, const int nBoardPos,
                      const int nColRow, const int dim);
    bool isAtStart(const int pos, const int colrow);
    int getRackLetter(QVector<Letter> aRackLetter, const QString aChar);
};
