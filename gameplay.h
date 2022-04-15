#pragma once

#include <QVariant>
#include <QQmlListProperty>
//#include <QtQml>

#include "defines.h"
#include "board.h"
#include "boardmodel.h"
#include "cubemodel.h"
#include "dictionary.h"
#include "letter.h"
#include "move.h"
#include "msgmodel.h"
#include "rackmodel.h"
#include "gamecoursemodel.h"
#include "computemove.h"
#include "downloadmanager.h"

enum WordCheckMode {wcAsk=0,
                    wcPoll=1,
                    wcChallenge=2};

enum TimeControlType {tcNoLimit=0,
                      tcPerMove=1,
                      tcPerGame=2};

class GamePlay : public QObject
{
    Q_OBJECT
    //
    Q_PROPERTY(boardmodel* boardModel READ boardModel CONSTANT)
    Q_PROPERTY(cubemodel* cubeModel READ cubeModel CONSTANT)
    Q_PROPERTY(rackmodel* rackModel READ rackModel CONSTANT)
    Q_PROPERTY(msgmodel* msgModel READ msgModel CONSTANT)
    Q_PROPERTY(gamecoursemodel* gamecourseModel READ gamecourseModel CONSTANT)
    Q_PROPERTY(dicList* dicListModel READ dicListModel CONSTANT)

    Q_PROPERTY(bool isRunning READ getIsRunning NOTIFY isRunningChanged)
    Q_PROPERTY(bool is3D READ getIs3D NOTIFY is3DChanged)
    Q_PROPERTY(int boardSize READ getBoardSize NOTIFY boardSizeChanged)
    Q_PROPERTY(bool isAccepted READ getIsAccepted NOTIFY isAcceptedChanged)
    Q_PROPERTY(int currentMove READ getCurrentMove CONSTANT)
    Q_PROPERTY(int currentPlayer READ getCurrentPlayer NOTIFY currentPlayerChanged)
    Q_PROPERTY(int numberOfPlayers READ getNumberOfPlayers NOTIFY numberOfPlayersChanged)
    Q_PROPERTY(int activeDimension READ getActiveDimension WRITE setActiveDimension NOTIFY activeDimensionChanged)
    Q_PROPERTY(int activePosition READ getActivePosition WRITE setActivePosition NOTIFY activePositionChanged)
    Q_PROPERTY(QString lastError READ getLastError NOTIFY lastErrorChanged)
    Q_PROPERTY(QString statInfo READ getStatInfo NOTIFY statInfoChanged)

    Q_PROPERTY(QString addMsg WRITE addMessage CONSTANT);
    Q_PROPERTY(QString dicDownloadFinsihed WRITE dicDownloadFinsihed CONSTANT);
    Q_PROPERTY(QString downloadFile WRITE download CONSTANT);
    //compute move
    Q_PROPERTY(int computeProgress READ getComputeProgress WRITE setComputeProgress NOTIFY computeProgressChanged)
    Q_PROPERTY(int placedValue READ getPlacedValue NOTIFY placedValueChanged)
    Q_PROPERTY(int bestMoveCount READ getBestMoveCount NOTIFY bestMoveCountChanged)
    //dictionary
    Q_PROPERTY(int dicWordCount READ getDicWordCount NOTIFY dicWordCountChanged)

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
    void bestMoveCountChanged();
    void computeProgressChanged();
    void placedValueChanged();
    void dicWordCountChanged();
    void statInfoChanged();

public:
    explicit GamePlay(QObject *parent = nullptr);

    Q_INVOKABLE bool canDrop(const unsigned int boardIndex); // check if field is empty
    Q_INVOKABLE bool canDrag(); // block dragging if letters have been marked to exchange
    Q_INVOKABLE void exchangeLetter(const unsigned int rackIndex); // check whether letters have been placed and mark for exchange otherwise
    Q_INVOKABLE QString getLastError() { if (m_lMoves.count()>0) return m_lMoves.last()->LastError(); else return ""; }
    Q_INVOKABLE QString getPlayerName(const int index) { if (index<m_lPlayerNames.count()) return m_lPlayerNames[index]; else return ""; }
    Q_INVOKABLE void addMessage(QString aWhat, int aWho = -1) { m_pMsgModel->addMessage(aWhat, aWho); }

    Q_INVOKABLE void saveConfig(QString fileName, QVariantMap configData);
    Q_INVOKABLE QVariantMap loadConfig(QString fileName);
    Q_INVOKABLE void saveGame(const QUrl &fileName);
    Q_INVOKABLE void loadGame(const QUrl &fileName);

    Q_INVOKABLE bool loadDictionary(const QString fileName) { const bool result = m_pDicListModel->loadFrom(fileName); emit dicWordCountChanged(); return result; }
    Q_INVOKABLE QString currentDicName() const { return m_pDicListModel->currentDicName(); }
    Q_INVOKABLE QVariantMap selectedDicInfo(const int index) const { return m_pDicListModel->selectedDicInfo(index); }
    Q_INVOKABLE void setCategory(const QString catName, const bool isChecked) { m_pDicListModel->setCategory(catName,isChecked); }
    Q_INVOKABLE bool getCategory(const QString catName) const { return m_pDicListModel->getCategory(catName); }

    Q_INVOKABLE void computeMove();
    Q_INVOKABLE void placeBestMove(const int index);
    Q_INVOKABLE QString getVariation(const QString sPattern) { return m_pDicListModel->dictionary->variation(sPattern.toUpper()); };
    Q_INVOKABLE QVariantMap wordByIndex(const int nIndex) { return  m_pDicListModel->dictionary->wordByIndex(nIndex); };
    Q_INVOKABLE int indexByWord(const QString sWord) { return m_pDicListModel->dictionary->indexByWord(sWord); };
    //TODO: gameplay: block meaning if currentplayer = currentmove
    Q_INVOKABLE QString meaningAt(const int index) { return replaceAllLetters(m_pDicListModel->dictionary->getMeanings( m_pBoard->getWordsAt(index)) ); }

    Q_INVOKABLE QUrl documentPath() { return QUrl::fromLocalFile(docPath().path()); }
    Q_INVOKABLE void download(QString fileName) { m_pDownloadManager->download(fileName); }
    Q_INVOKABLE void statInfoType(const int aType) { m_nStatInfoType = aType; emit statInfoChanged(); }
    Q_INVOKABLE QString allStat() { return getAllStat(); }

    rackmodel* rackModel();
    boardmodel* boardModel();
    cubemodel* cubeModel();
    msgmodel* msgModel();
    gamecoursemodel* gamecourseModel();
    dicList* dicListModel();

public slots: // slots are public methods available in QML
    void startNewGame(QStringList PlayerNames,
                      int RackSize,
                      bool is3D,
                      QVariantList FieldTypeArray,
                      QVariantList LetterList,
                      int NumberOfJokers,
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
    void execNextPlayer(const bool bIsLoading = false);

    void dropLetter(const uint rackIndex, const uint boardIndex);
    void dropLetterRack(const uint fromIndex, const uint toIndex);
    void removeLetter(const uint boardIndex);
    void removeLetter(Letter aLetter);
    void setJokerLetter(const uint boardIndex, const QString aWhat);
    void dicDownloadFinsihed(const QString aFileName) {m_pDicListModel->updateList(); if (!aFileName.isEmpty()) loadDictionary(aFileName); }

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    bool getIsRunning() { return m_bIsRunning; };
    bool getIsAccepted() { return m_bIsAccepted; };
    unsigned int getBoardSize() {return m_pBoard->getBoardSize(); }
    bool getIs3D() {return m_pBoard->is3D(); }
    unsigned int getCurrentMove() { return m_nCurrentMove; }
    unsigned int getCurrentPlayer() { return m_nCurrentPlayer; }
    unsigned int getNumberOfPlayers() { return m_lPlayerNames.count(); }
    void setActiveDimension(const unsigned int aDimension);
    unsigned int getActiveDimension();
    void setActivePosition(const int aPosition, const bool bSilent=false);
    int getActivePosition();
    int getComputeProgress() { return m_nProgress; }
    void setComputeProgress(const int aProgress);
    int getPlacedValue() { return m_lMoves.count()>0 ? m_lMoves.last()->Value() : 0; }
    void resetPieces(); //removeLetter(*)

    int getBestMoveCount() {return m_pComputeMove->numberOfResults(); }
    int getDicWordCount() { return m_pDicListModel->dictionary->count(); }

    int m_nStatInfoType = 0;
    QString getStatInfo() { return getStatInfoType(m_nStatInfoType); }
    QString getStatInfoType(const int aType);
    QString getAllStat();

    QStringList m_lPlayerNames;               // player names; also count
    QList<Letter> m_lBag;                     // letters in bag
    bool m_bCanJokerExchange;                 // whether joker can be exchanged
    uint m_nGameEndBonus;                     // //TODO: gameplay: bonus for ending the game
    uint m_nNumberOfPasses;                   // number of zero moves until game ends
    uint m_nJokerPenalty;                     // //TODO: gameplay: penalty for joker left on game end
    bool m_bChangeIsPass;                     // whether changing letters is treated as pass and counted for game end
    TimeControlType m_eTimeControlType;       // type of time control (no, per game, per moves)
    uint m_nTimeControlValue;                 // actual time limit
    uint m_nLimitedExchange;                  // //TODO: gameplay: limited exchange to n letters, eg. Italian rules
    bool m_bCambioSecco;                      // //TODO: gameplay: whether cabio secco is allowed
    bool m_bWhatif;                           // //TODO: gameplay: whether whatif is allowed
    bool m_bAdd;                              // //TODO: gameplay: whether to add values of remaining tiles from other players to the winner's result
    bool m_bSubstract;                        // //TODO: gameplay: whether to deduct the remaining pieces' values from the individual result
    uint m_nTimePenaltyValue;                 // //TODO: gameplay: malus to continue game after time out
    uint m_nTimePenaltyCount;                 // //TODO: gameplay: number of accepted time outs at game end
    bool m_bTimeGameLost;                     // //TODO: gameplay: whether game is lost on game end (despite received points)
    WordCheckMode m_eWordCheckMode;           // type of word checking (asked when placed, automatic poll, player objection)
    uint m_nWordCheckPeriod;                  // //TODO: gameplay: time when objection is allowed
    uint m_nWordCheckPenalty;                 // //TODO: gameplay: penalty when challenge was wrong
    int m_nWordCheckBonus;                    // //TODO: gameplay: bonus for correct objection
    int m_nScrabbleBonus;                     // bonus when all pieces are placed, usually 50
    bool m_bIsCLABBERS;                       // //TODO: gameplay: whether CLABBER variant is allowed
    QList<move*> m_lMoves;

    int m_nCurrentMove;
    int m_nCurrentPlayer;
    uint m_nPasses;
    uint m_nPlayerCount;

    bool m_bIsRunning;
    bool m_bIsAccepted;                       // feedback for the UI to disable nextplayer
    bool m_bIsFirstMove;                      // needed for checkmove()
    int m_nProgress;

    int m_nTimerID;
    int m_nMoveTime;
    int m_nTimeLeft;

    board *m_pBoard;
    rackmodel *m_pRackModel;
    boardmodel *m_pBoardModel;
    cubemodel *m_pCubeModel;
    msgmodel *m_pMsgModel;
    gamecoursemodel *m_pGameCourseModel;
    dicList *m_pDicListModel;
    computemove *m_pComputeMove;
    DownloadManager *m_pDownloadManager;
};
