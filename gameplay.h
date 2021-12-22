#pragma once

#include <QVariant>
#include <QQmlListProperty>
#include <QtQml>

#include "board.h"
#include "boardmodel.h"
#include "cubemodel.h"
#include "dictionary.h"
#include "letter.h"
#include "move.h"
#include "msgmodel.h"
#include "rackmodel.h"
#include "statmodel.h"

enum WordCheckMode {wcAsk=0,
                    wcPoll=1,
                    wcChallenge=2};

enum TimeControlType {tcNoLimit=0,
                      tcPerMove=1,
                      tcPerGame=2};

class GamePlay : public QObject
{
    Q_OBJECT
    Q_PROPERTY(boardmodel* boardModel READ boardModel CONSTANT)
    Q_PROPERTY(cubemodel* cubeModel READ cubeModel CONSTANT)
    Q_PROPERTY(rackmodel* rackModel READ rackModel CONSTANT)
    Q_PROPERTY(msgmodel* msgModel READ msgModel CONSTANT)
    Q_PROPERTY(statmodel* statModel READ statModel CONSTANT)
    Q_PROPERTY(dictionary* dictModel READ dictModel CONSTANT)

    Q_PROPERTY(bool isRunning READ getIsRunning NOTIFY isRunningChanged)
    Q_PROPERTY(bool is3D READ getIs3D NOTIFY is3DChanged)
    Q_PROPERTY(int boardSize READ getBoardSize NOTIFY boardSizeChanged)
    Q_PROPERTY(bool isAccepted READ getIsAccepted NOTIFY isAcceptedChanged)
    Q_PROPERTY(int currentMove READ getCurrentMove CONSTANT)
    Q_PROPERTY(int currentPlayer READ getCurrentPlayer CONSTANT NOTIFY currentPlayerChanged)
    Q_PROPERTY(int numberOfPlayers READ getNumberOfPlayers CONSTANT NOTIFY numberOfPlayersChanged)
    Q_PROPERTY(int activeDimension READ getActiveDimension WRITE setActiveDimension NOTIFY activeDimensionChanged)
    Q_PROPERTY(int activePosition READ getActivePosition WRITE setActivePosition NOTIFY activePositionChanged)
    Q_PROPERTY(QString lastError READ getLastError NOTIFY lastErrorChanged)


signals:
     void boardSizeChanged();
     void isRunningChanged();
     void is3DChanged();
     void isAcceptedChanged();
     void activeDimensionChanged();
     void activePositionChanged();
     void lastErrorChanged();
     void currentPlayerChanged();
     void numberOfPlayersChanged();

public:
    explicit GamePlay(QObject *parent = nullptr);

    Q_INVOKABLE bool canDrop(const unsigned int boardIndex);
    Q_INVOKABLE QString getLastError() { if (m_lMoves.count()>0) return m_lMoves.last()->LastError(); else return "";};
    Q_INVOKABLE QString getPlayerName(const int index) { if (index<m_lPlayerNames.count()) return m_lPlayerNames[index]; else return "";};

    Q_INVOKABLE void addMessage(QString aWhat, int aWho = -1) { m_pMsgModel->addmessage(aWhat,aWho); };
    Q_INVOKABLE void saveGame(const QUrl &fileName);
    Q_INVOKABLE void loadGame(const QUrl &fileName);
    Q_INVOKABLE bool loadDictionary(const QString fileName) { return m_pDictModel->loadFrom(fileName); };
    Q_INVOKABLE void computeMove();

    rackmodel* rackModel();
    boardmodel* boardModel();
    cubemodel* cubeModel();
    msgmodel* msgModel();
    statmodel* statModel();
    dictionary* dictModel();

public slots: // slots are public methods available in QML
     void startNewGame(QStringList PlayerNames,
                       int RackSize,
                       bool is3D,
                       QVariantList FieldTypeArray,
                       QVariantList LetterList,
                       bool CanJokerExchange,
                       int GameEndBonus,
                       int NumberOfPasses,
                       int JokerPenalty,
                       bool ChangeIsPass,
                       int TimeControl,
                       int TimeControlValue,
                       int LimitedExchange,
                       bool CambioSecco,
                       bool Whatif,
                       bool Add,
                       bool Substract,
                       int TimePenaltyValue,
                       int TimePenaltyCount,
                       bool TimeGameLost,
                       int WordCheck,
                       int WordCheckPeriod,
                       int WordCheckPenalty,
                       int WordCheckBonus,
                       int ScrabbleBonus,
                       bool isCLABBERS,
                       bool RandomSequence);
     void execNextPlayer();

     void dropLetter(const unsigned int rackIndex, const unsigned int boardIndex);
     bool removeLetter(const unsigned int boardIndex, const bool startDrag);

private:
    bool getIsRunning() { return m_bIsRunning; };
    bool getIsAccepted() { return m_bIsAccepted; };
    unsigned int getBoardSize() {return m_pBoard->getBoardsize(); }
    bool getIs3D() {return m_bIs3D; }
    unsigned int getCurrentMove() { return m_nCurrentMove; }
    unsigned int getCurrentPlayer() { return m_nCurrentPlayer; }
    unsigned int getNumberOfPlayers() { return m_lPlayerNames.count(); }
    void setActiveDimension(const unsigned int aDimension);
    unsigned int getActiveDimension();
    void setActivePosition(const int aPosition);
    int getActivePosition();

    QStringList m_lPlayerNames;               // player names; also count
    uint m_nRackSize;                         // numbers of letters on rack, usually 7
    bool m_bIs3D;                             // whether game is in 3D
    QList<Letter> m_lBag;                     // letters in bag
    bool m_bCanJokerExchange;                 // whether joker can be exchanged
    uint m_nGameEndBonus;                     // bonus for ending the game
    uint m_nNumberOfPasses;                   // number of zero moves until game ends
    uint m_nJokerPenalty;                     // penalty for joker ... TODO
    bool m_bChangeIsPass;                     // whether changing letters is treated as pass and counted for game end
    TimeControlType m_eTimeControlType;       // type of time control (no, per game, per moves)
    uint m_nTimeControlValue;                 // actual time limit TODO
    uint m_nLimitedExchange;                  // ... TODO
    bool m_bCambioSecco;                      // whether cmabio secco is allowed
    bool m_bWhatif;                           // whther whatif is allowed TODO
    bool m_bAdd;                              // bonus to add if move is objected correctly
    bool m_bSubstract;                        // malus to deduct if objection was wrong
    uint m_nTimePenaltyValue;                 // malus to continue game after time out
    uint m_nTimePenaltyCount;                 // number of accepted time outs at game end
    bool m_bTimeGameLost;                     // whether game is lost on game end (despite received points)
    WordCheckMode m_eWordCheckMode;           // type of word checking (asked when placed, automatic poll, player objection)
    uint m_nWordCheckPeriod;                  // time when objection is allowed
    uint m_nWordCheckPenalty;                 // penalty for ... TODO
    int m_nWordCheckBonus;                    // bonus for correct objection TODO
    int m_nScrabbleBonus;                     // bonus when all pieces are placed, usually 50
    bool m_bIsCLABBERS;                       // whether CLABBER variant is allowed
    bool m_bRandomSequence;                   // TODO
    QList<move*> m_lMoves;

    unsigned int m_nCurrentMove;
    unsigned int m_nCurrentPlayer;
    bool m_bIsRunning;
    bool m_bIsAccepted;                       // feedback for the UI to disable nextplayer
    bool m_bIsFirstMove;                      // needed for checkmove()
    bool m_bIsLoading;                        // don't save debug.ssg when loading TODO: remove

    board *m_pBoard;
    rackmodel *m_pRackModel;
    boardmodel *m_pBoardModel;
    cubemodel *m_pCubeModel;
    msgmodel *m_pMsgModel;
    statmodel *m_pStatModel;
    dictionary *m_pDictModel;
};
