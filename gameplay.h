#pragma once

#include <QVariant>
#include <QQmlListProperty>
#include <QTranslator>

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
#include "version.h"
#include "network.h"
#include "playersmodel.h"
#include "remotegamesmodel.h"
#include "configpath.h"
#include "locmodel.h"
#include "dictionarymodel.h"
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

    Q_PROPERTY(boardmodel* boardModel READ boardModel CONSTANT)
    Q_PROPERTY(cubemodel* cubeModel READ cubeModel CONSTANT)
    Q_PROPERTY(rackmodel* rackModel READ rackModel CONSTANT)
    Q_PROPERTY(msgmodel* msgModel READ msgModel CONSTANT)
    Q_PROPERTY(gamecoursemodel* gamecourseModel READ gamecourseModel CONSTANT)
    Q_PROPERTY(dictionarymodel* dicListModel READ dicListModel CONSTANT)
    Q_PROPERTY(locList* locListModel READ locListModel CONSTANT)
    Q_PROPERTY(playersTree* playersTreeModel READ playersTreeModel CONSTANT)
    Q_PROPERTY(remoteGamesModel* remoteGames READ remoteGames CONSTANT)
    //    Q_PROPERTY(remoteGamesProxy* remoteGames READ remoteGames CONSTANT)

    Q_PROPERTY(bool isChallenge READ getIsChallenge NOTIFY isChallengeChanged)
    Q_PROPERTY(bool isRunning READ getIsRunning NOTIFY isRunningChanged)
    Q_PROPERTY(bool isComputing READ getIsComputing NOTIFY isComputingChanged)
    Q_PROPERTY(bool isHistory READ getIsHistory NOTIFY isHistoryChanged)
    Q_PROPERTY(bool isInitialized READ getIsInitialized NOTIFY isInitializedChanged)
    Q_PROPERTY(bool is3D READ getIs3D NOTIFY is3DChanged)
    Q_PROPERTY(bool cambioSecco READ getCambioSecco NOTIFY cambioSeccoChanged)
    Q_PROPERTY(int boardSize READ getBoardSize NOTIFY boardSizeChanged)
    Q_PROPERTY(bool isAccepted READ getIsAccepted NOTIFY isAcceptedChanged)
    Q_PROPERTY(int currentMove READ getCurrentMove NOTIFY currentMoveChanged)
    Q_PROPERTY(int currentPlayer READ getCurrentPlayer NOTIFY currentPlayerChanged)
    Q_PROPERTY(int numberOfPlayers READ getNumberOfPlayers NOTIFY numberOfPlayersChanged)
    Q_PROPERTY(int activeDimension READ getActiveDimension WRITE setActiveDimension NOTIFY activeDimensionChanged)
    Q_PROPERTY(int activePosition READ getActivePosition WRITE setActivePosition NOTIFY activePositionChanged)
    Q_PROPERTY(QString lastError READ getLastError NOTIFY lastErrorChanged)
    Q_PROPERTY(QString statInfo READ getStatInfo NOTIFY statInfoChanged)
    Q_PROPERTY(QVariantList statTotal READ getStatTotal NOTIFY statTotalChanged)
    //compute move
    Q_PROPERTY(int computeProgress READ getComputeProgress WRITE setComputeProgress NOTIFY computeProgressChanged)
    Q_PROPERTY(int computeResults READ getcomputeResults WRITE setComputeResults NOTIFY computeResultsChanged)
    //dictionary
    Q_PROPERTY(int wordCount READ getWordCount NOTIFY wordCountChanged)

    //network
    Q_PROPERTY(bool isConnected READ getIsConnected NOTIFY connectedChanged)
    Q_PROPERTY(bool isLocalPlayer READ getIsLocalPlayer NOTIFY isLocalPlayerChanged)

signals:
    void boardSizeChanged();
    void isRunningChanged();
    void isComputingChanged();
    void isHistoryChanged();
    void isChallengeChanged();
    void isInitializedChanged();
    void is3DChanged();
    void cambioSeccoChanged();
    void isAcceptedChanged();
    void activeDimensionChanged();
    void activePositionChanged();
    void lastErrorChanged();
    void currentMoveChanged();
    void currentPlayerChanged();
    void numberOfPlayersChanged();
    void computeResultsChanged();
    void computeProgressChanged();
    void placedValueChanged(uint);
    void wordCountChanged();
    void statInfoChanged();
    void statTotalChanged();
    void connectedChanged();

    void showPoll(bool); //true = poll is running, false = no poll/hide the pieseries
    void isLocalPlayerChanged();
    void answerChanged(QVariantList); // 0 = no answer, 1 = yes/green, -1 = no/red
    void newGame(bool); // triggers acNewGame in mainwindow
    void applyConfig(QVariantMap); // triggers scrconfig::applyconfig()
    void showRemoteGames(); // shows ScrRemoteGames()
    void loadingFinished(int);

public:
    explicit GamePlay(QQmlEngine *engine);

    Q_INVOKABLE void checkForUpdates();

    Q_INVOKABLE void localize(const QString fileName);
    Q_INVOKABLE bool deleteLocFile(const QString fileName) { return m_pLocListModel->deleteLocFile(fileName); };
    Q_INVOKABLE void connect(QString name, QString password, QString email, QString country, QString city);
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void chat(QString aMessage);

    Q_INVOKABLE bool canDrop(const unsigned int boardIndex); // check if field is empty
    Q_INVOKABLE bool canDrag(); // block dragging if letters have been marked to exchange
    Q_INVOKABLE void exchangeLetter(const unsigned int rackIndex); // check whether letters have been placed and mark for exchange otherwise
    Q_INVOKABLE QString getLastError() { if (m_pMoves.count()>0) return m_pMoves.last()->LastError(); else return ""; }
    Q_INVOKABLE QString getPlayerName(const int index) { if (index<m_lPlayerNames.count()) return m_lPlayerNames[index]; else return ""; }
    Q_INVOKABLE void addMessage(QString aWhat, QString aWho = "") { m_pMsgModel->addMessage(aWhat, aWho); }

    Q_INVOKABLE void saveConfig(QString fileName, QVariantMap configData);
    Q_INVOKABLE QVariantMap loadConfig(QString fileName);
    Q_INVOKABLE void saveGame(QString fileName, const bool bSilent = false);
    Q_INVOKABLE void loadGame(QString fileName, const bool bSilent = false);
    Q_INVOKABLE void getRemoteGames() {
        addMessage( tr("Requesting saved games from server...") );
        emit onSend(network::nwRemoteGames, m_pNetwork->localPlayerName(), "");
    }
    Q_INVOKABLE void loadRemoteGame(const int selectedRow) {
        const QModelIndex idx = m_RemoteGamesModel->index(selectedRow,0);
        const QString fileName = m_RemoteGamesModel->data(idx, m_RemoteGamesModel->NameRole).toString();
        emit onSend(network::nwLoadGame, m_pNetwork->localPlayerName(), "Name="+fileName);
    }
    Q_INVOKABLE void loadDictionary(const QString fileName, const QString categories) { m_pDictionary->loadDictionary(fileName, categories); }
    Q_INVOKABLE void deleteDictionary(const QString fileName) { m_pDicListModel->deleteDic(fileName); }
    Q_INVOKABLE void setCategoryChecked(const QString catName, const bool isChecked, const bool doReset = false)
        { m_pDictionary->setCategoryChecked(catName,isChecked);
          if (doReset) m_pDicListModel->doReset(); }
    Q_INVOKABLE bool getCategoryChecked(const QString catName) const { return m_pDictionary->getCategoryChecked(catName); }

    Q_INVOKABLE void computeMove();
    Q_INVOKABLE void placeBestMove(const int index);

    Q_INVOKABLE QString getVariation(const QString sPattern) { return m_pDictionary->variation(sPattern.toUpper()); };
    Q_INVOKABLE QVariantMap wordByIndex(const int nIndex) { return  m_pDictionary->wordByIndex(nIndex); };
    Q_INVOKABLE int indexByWord(const QString sWord) { return m_pDictionary->indexByWord(sWord); };
    Q_INVOKABLE QString getMeaningAt(const int index);

    Q_INVOKABLE void statInfoType(const int aType) { m_nStatInfoType = aType; emit statInfoChanged(); }
    Q_INVOKABLE QString allStat() { return getAllStat(); }
    Q_INVOKABLE QString version() { return version::current() + " / " + QStringLiteral(__DATE__); }
    Q_INVOKABLE QString config() { return config::ini(); }
    Q_INVOKABLE QString replaceDigraph(const QString aLetter) { return replaceLetter.value(aLetter, aLetter); }

    Q_INVOKABLE QString networkName() { return m_pNetwork->localPlayerName(); }
    Q_INVOKABLE void doInvite(QString sName);
    Q_INVOKABLE void doLeave();
    Q_INVOKABLE void syncNewGame(QVariantMap aConfig);
    Q_INVOKABLE void rejectNewGame() { if (m_bIsConnected) emit onSend(network::nwAnswer, "group", "Answer=false"); }
    Q_INVOKABLE void syncNextPlayer();
    Q_INVOKABLE void syncChallengeWord();
    Q_INVOKABLE void syncCambioSecco();

    rackmodel *rackModel() { return m_pRackModel; }
    boardmodel *boardModel() { return m_pBoardModel; }
    cubemodel *cubeModel() { return m_pCubeModel; }
    msgmodel *msgModel() { return m_pMsgModel; }
    gamecoursemodel *gamecourseModel() { return m_pGameCourseModel; }
    dictionarymodel *dicListModel() { return m_pDicListModel; }
    locList *locListModel() { return m_pLocListModel; }
    playersTree *playersTreeModel() { return m_PlayersTreeModel; }
    remoteGamesModel *remoteGames() { return m_RemoteGamesModel; }
    //NOTE: gameplay: remoteGamesProxy <-> remoteGames();

public slots:
    void startNewGame(QVariantMap gameConfig);
    void startNewGameLater(QVariantMap gameConfig) {
        m_lGameConfig = gameConfig;
        emit onSend(network::nwAnswer, "group", "Answer=true");
    }
    void nextPlayer(const bool bIsLoading = false);

    void dropLetter(const uint rackIndex, const uint boardIndex);
    void dropLetterRack(const uint fromIndex, const uint toIndex);
    int removeLetter(const uint boardIndex);
    int removeLetter(Letter aLetter);
    void jokerLetter(const uint boardIndex, const QString aWhat);
    void doCambioSecco();
    void doDownloadFinished(DlType fileType, QString fileName);

private:
    enum PollType {ptNone, ptNewGame, ptNextPlayer, ptChallenge};

private slots:
    void startPoll(GamePlay::PollType pollType);
    void endPoll() { if (m_ePoll == ptNone) emit showPoll(false); }
    void endChallenge() { m_bIsChallenge = false; emit isChallengeChanged(); }

public slots:
    void doNetworkError(QAbstractSocket::SocketError socketError);
    void doNetworkInfo(QVariantMap aMsg);
    void doNetworkLogin(QVariantMap aMsg);
    void doDisconnect(); // signal from socket
    void doNetworkDisconnect(QString aMsg); //signal from server
    void doNetworkInvite(QVariantMap aMsg);
    void doNetworkJoin(QVariantMap aMsg);
    void doNetworkAnswer(QVariantMap aMsg);
    void doNetworkSyncNewGame(QVariantMap aMsg);
    void doNetworkNextPlayer(QVariantMap aMsg);
    void doNetworkRemoteGames(QVariantMap aMsg);
    void doNetworkGameResult(int oldRating, int newRating);
    void doNetworkChallengeMove(QString aSender);
    void doNetworkChallengeResult(QVariantMap aMsg);

    void doDictionaryLoadingFinished(QString aFileName);

    void doSelectedMoveChanged(int move);
    void setComputeProgress(const int aProgress);
    void setComputeResults(const int nResults) { m_nComputeResults = nResults; emit computeResultsChanged(); }
    void download(QString fileName) { m_pDownloadManager->download(fileName); }

signals:
    void onSend(const network::MessageType msgType, const QString msgReceiver, QString msgText);

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    bool getIsRunning() { return m_bIsRunning; };
    bool getIsComputing() { return m_bIsComputing; };
    bool getIsHistory() { return m_bIsHistory; };
    bool getIsChallenge() { return m_bIsChallenge; };
    bool getIsInitialized() { return m_bIsInitialized; };
    bool getIsAccepted() { return m_bIsAccepted; };
    unsigned int getBoardSize() {return m_pBoard->getBoardSize(); }
    bool getIs3D() { return m_pBoard->is3D(); }
    bool getCambioSecco() { if (m_nCurrentPlayer < m_bCanCambioSecco.count()) return m_bCanCambioSecco[m_nCurrentPlayer]; else return false; }
    unsigned int getCurrentMove() { if (m_bIsHistory) return m_nMoveHistory; else return m_nCurrentMove; }
    unsigned int getCurrentPlayer() { return m_nCurrentPlayer; }
    unsigned int getNumberOfPlayers() { return m_lPlayerNames.count(); }
    void setActiveDimension(const unsigned int aDimension);
    unsigned int getActiveDimension();
    void setActivePosition(const int aPosition, const bool bSilent=false);
    int getActivePosition();
    int getComputeProgress() { return m_nProgress; }
    void doComputeMove();
    void resetPieces(); //removeLetter(*)
    void rollbackLastMove();
    void doGameEnd();
    void doUpdateRack();
    bool doCheckWords(const bool bPrevious = false); //returns true if all words are found in dictionary; use nMove to check the previous
    void doStartTimer(const bool doRun = true);
    int getcomputeResults() {return m_nComputeResults; }

    int getWordCount() { return m_pDictionary->count(); }

    bool getIsConnected() { return m_bIsConnected; }
    bool getIsLocalPlayer() { return (!m_bIsConnected) || (m_pNetwork->localPlayerName() == m_lPlayerNames[m_nCurrentPlayer]); }

    int m_nStatInfoType = 0;
    QString getStatInfo() { return getStatInfoType(m_nStatInfoType); }
    QVariantList getStatTotal() {
        QVariantList aResult;
        for (uint i = 0; i < m_nPlayerCount; i++)
            aResult.append(m_pGameCourseModel->getScore(i));
        return aResult;
    }
    QString getStatInfoType(const int aType);
    QString getAllStat();

    QStringList m_lPlayerNames;               // player names; also count
    QList<Letter> m_lBag;                     // letters in bag
    bool m_bCanJokerExchange;                 // whether joker can be exchanged
    uint m_nGameEndBonus;                     // bonus for ending the game
    uint m_nNumberOfPasses;                   // number of zero moves until game ends
    bool m_bChangeIsPass;                     // whether changing letters is treated as pass and counted for game end
    TimeControlType m_eTimeControlType;       // type of time control (no, per game, per moves)
    uint m_nTimeControlValue;                 // actual time limit
    uint m_nLimitedExchange;                  // FEATURE: gameplay: limited exchange to n letters, eg. Italian rules
    bool m_bWhatif;                           // FEATURE: gameplay: whether whatif is allowed
    bool m_bAdd;                              // whether to add values of remaining tiles from other players to the winner's result
    bool m_bSubstract;                        // whether to deduct the remaining pieces' values from the individual result
    uint m_nJokerPenalty;                     // penalty for joker left on game end
    uint m_nTimePenaltyValue;                 // penalty to continue game after time out
    QList<uint> m_nTimePenaltyCount;          // number of accepted time outs at game end
    bool m_bTimeGameLost;                     // whether game is lost on game end (despite received points)
    WordCheckMode m_eWordCheckMode;           // type of word checking (asked when placed, automatic poll, player objection)
    uint m_nWordCheckPeriod;                  // time when objection is allowed
    uint m_nWordCheckPenalty;                 // penalty when challenge was wrong
    int m_nWordCheckBonus;                    // bonus for correct objection
    int m_nScrabbleBonus;                     // bonus when all pieces are placed, usually 50
    bool m_bIsCLABBERS;                       // FEATURE: gameplay: whether CLABBER variant is allowed
    uint m_nPerformance;                      // absolute value: 0 = always the best word; >0 = randomly picked => doComputeMove()
    QList<sharedMove> m_pMoves;
    int m_nCurrentMove;
    int m_nMoveHistory;
    int m_nCurrentPlayer = 0;
    uint m_nPasses;
    uint m_nPlayerCount = 1; // movecount % playercount=0 crashes

    bool m_bIsRunning = false;
    bool m_bIsComputing = false;
    bool m_bIsChallenge = false;              // set to true in nextplayer() when in network mode with m_eWordCheckMode = wcChallenge for m_nWordCheckPeriod seconds
    bool m_bIsHistory = false;
    QList<bool> m_bCanCambioSecco;

    PollType m_ePoll = ptNone;
    QString m_sChallengePlayer;               // stores the name of the player who send a challenge to the group
    bool m_bIsInitialized = false;            // false until the first game has been started; used to block save
    bool m_bIsAccepted;                       // feedback for the UI to disable nextplayer
    bool m_bIsFirstMove;                      // needed for checkmove()
    int m_nProgress;

    int m_nTimerID;
    int m_nMoveTime;
    int m_nTimeLeft;

    bool m_bIsConnected; //whether in network mode or not
    QVariantMap m_lGameConfig; //stores configuration for startNewGame()
    QVariantMap m_lAnswer; //true or false, or not yet answered, ie. < count()
    board *m_pBoard, *m_pTempBoard = nullptr; //store the board while browsing the history
    QList<Letter> m_pTempLetters;
    rackmodel *m_pRackModel;
    boardmodel *m_pBoardModel;
    cubemodel *m_pCubeModel;
    msgmodel *m_pMsgModel;
    gamecoursemodel *m_pGameCourseModel;
    dictionarymodel *m_pDicListModel;
    dictionary *m_pDictionary;
    locList *m_pLocListModel;
    playersTree *m_PlayersTreeModel;
    remoteGamesModel *m_RemoteGamesModel;
//    remoteGamesProxy *m_RemoteGamesProxy;

    computemove *m_pComputeMove;
    int m_nComputeResults = 0;
    DownloadManager *m_pDownloadManager;

    QTranslator *m_pTranslator;
    QQmlEngine *m_pEngine;
    network *m_pNetwork;
};
