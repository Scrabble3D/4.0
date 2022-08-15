#include "gameplay.h"

#include <random>
#include <QtMath>
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QQmlEngine> //translation
#include <QQmlFile>

#ifdef QT_DEBUG
#include <QDebug>
#endif

GamePlay::GamePlay(QQmlEngine *engine)
{
    m_pTranslator = new QTranslator(this);
    m_pEngine = engine;

    m_bIsRunning = false;

    //TODO: download: check all files for updates
    m_pDownloadManager = new DownloadManager(this);
    if (m_pDownloadManager->lastChecked() > 7)
        download("Binaries/raw/main/Scrabble3D.conf");

    m_pBoard = new board();
    m_pRackModel = new rackmodel(this);
    m_pBoardModel = new boardmodel(this, m_pBoard);
    m_pCubeModel = new cubemodel(this, m_pBoard);
    m_pMsgModel = new msgmodel(this);
    m_pGameCourseModel = new gamecoursemodel(this);
    m_pDicListModel = new dicList(this);
    m_pComputeMove = new computemove(this, m_pBoard, m_pRackModel, m_pDicListModel->dictionary);
    m_bIsConnected = false;
    m_PlayersTreeModel = new playersTree();
    m_RemoteGamesModel = new remoteGamesModel();

#ifdef Q_OS_ANDROID
    const QDir aPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString fileName = aPath.path() +"/debug.ssg";
    loadGame(QUrl::fromLocalFile(fileName));
#endif

}

void GamePlay::connect(QString name, QString password, QString email, QString country, QString city)
{
    addMessage(tr("Connecting to game server..."));

    m_pNetwork = new network(this, name, password, email, country, city);

    // connect -> network::doConnected
    // readyread -> network::doReadyRead
    QObject::connect(m_pNetwork, SIGNAL(disconnected()),
                     this, SLOT(doDisconnect()));
    QObject::connect(m_pNetwork, SIGNAL(onDisconnect(QString)),
                     this, SLOT(doNetworkDisconnect(QString)));
    QObject::connect(m_pNetwork, SIGNAL(errorOccurred(QAbstractSocket::SocketError)),
                     this, SLOT(doNetworkError(QAbstractSocket::SocketError)));

    QObject::connect(m_pNetwork, SIGNAL(onLogin(QVariantMap)),
                     this, SLOT(doNetworkLogin(QVariantMap)));
    QObject::connect(m_pNetwork, SIGNAL(onLogout(QString)),
                     m_PlayersTreeModel, SLOT(doLogout(QString)));
    QObject::connect(m_pNetwork, SIGNAL(onRefresh(QVariantMap)),
                     m_PlayersTreeModel, SLOT(doRefresh(QVariantMap)));

    QObject::connect(m_pNetwork, SIGNAL(onInvite(QVariantMap)),
                     this, SLOT(doNetworkInvite(QVariantMap)));
    QObject::connect(m_pNetwork, SIGNAL(onJoin(QVariantMap)),
                     this, SLOT(doNetworkJoin(QVariantMap)));

    QObject::connect(m_pNetwork, SIGNAL(onInfo(QVariantMap)),
                     this, SLOT(doNetworkInfo(QVariantMap)));
    QObject::connect(m_pNetwork, SIGNAL(onChat(QString,QString)),
                     m_pMsgModel, SLOT(addMessage(QString,QString)));
    QObject::connect(m_pNetwork, SIGNAL(onPoll(QVariantMap)),
                     this, SLOT(doNetworkPoll(QVariantMap)));
    QObject::connect(m_pNetwork, SIGNAL(onAnswer(QVariantMap)),
                     this, SLOT(doNetworkAnswer(QVariantMap)));
    // 1. mainwindow::acNewGame -> GamePlay.syncNewGame(ScrConfig.getConfigData(false))
    // 2. GamePlay.syncNewGame -> network send/receive nwSyncNewGame
    // 3. GamePlay.doNetworkSyncNewGame -> ScrConfig applyConfig & ScrNewGame
    QObject::connect(m_pNetwork, SIGNAL(onSyncNewGame(QVariantMap)),
                     this, SLOT(doNetworkSyncNewGame(QVariantMap)));
    QObject::connect(m_pNetwork, SIGNAL(onNextPlayer(QVariantMap)),
                     this, SLOT(doNetworkNextPlayer(QVariantMap)));

    QObject::connect(m_pNetwork, SIGNAL(onRemoteGames(QVariantMap)),
                     this, SLOT(doNetworkRemoteGames(QVariantMap)));
    QObject::connect(m_pNetwork, SIGNAL(onGameResult(int, int)),
                     this, SLOT(doNetworkGameResult(int, int)));

    // adds msgtype and sender and sends the msg to the server -> network::doSend
    QObject::connect(this, SIGNAL(onSend(network::MessageType,QString,QString)),
                     m_pNetwork, SLOT(doSend(network::MessageType,QString,QString)));

    //TODO: network: make server variable
    m_pNetwork->connect("scrabble3d.ignorelist.com",5001);

    m_bIsConnected = true;
    emit connectedChanged();
}


void GamePlay::disconnect()
{
    m_bIsConnected = false;
    emit connectedChanged();

    m_PlayersTreeModel->clearAll();
    m_pNetwork->deleteLater();
}

void GamePlay::doDisconnect()
{
    addMessage(tr("You were disconnected"));
    if (m_bIsConnected) //signal is sent on manual disconnect but also by the server
        disconnect();
}

void GamePlay::doNetworkDisconnect(QString aMsg)
{
    if (aMsg == "Error_Connections")
        addMessage(tr("Too many connections"));
    else if (aMsg == "Error_Release")
        addMessage("Please update your application");
    else if (aMsg == "Error_Name")
        addMessage("Please choose another name");
    else if (aMsg == "Error_Banned")
        addMessage("You are banned on this server");
    else if (aMsg == "Error_Password")
        addMessage("Wrong username or password");
    else if (aMsg == "Error_Accounts")
        addMessage("Too many accounts");
}

void GamePlay::doNetworkInvite(QVariantMap aMsg)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Scrabble3D");
    switch ( aMsg["JoinMode"].toInt() ) {
    case 0: {
        msgBox.setText( tr("%1 invites you to a new game").arg(aMsg["Sender"].toString()) );
        msgBox.setInformativeText(
            tr("Language: %1").arg(aMsg["Language"].toString()) + "\n" +
            tr("Country: %1").arg(aMsg["Country"].toString()) + "\n" +
            tr("Dictionary: %1").arg(aMsg["Dictionary"].toString())
            );
        break;
    }
    case 1:
        msgBox.setText( tr("%1 invites you to kibitz.").arg(aMsg["Sender"].toString()) );
        break;
    case 2:
        msgBox.setText( tr("%1 invites you to a new game.").arg(aMsg["Sender"].toString()) );
        break;
    }
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    if (msgBox.exec() == QMessageBox::Yes) {
        //leave current group, if exist
        m_pNetwork->doSend(network::nwLeave,"group","");
        //compare releases
        QString sRelease = m_PlayersTreeModel->playerData(aMsg["Sender"].toString()).release;
        int nIndex = sRelease.indexOf("_");
        if (nIndex > 0)
            sRelease = sRelease.first(nIndex);
        if (sRelease != versionString()) {
            msgBox.setText(tr("You are using different releases: %1<>%2").arg(
                sRelease,versionString()));
            msgBox.setInformativeText("");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
        }
        //join the group
        m_pNetwork->doSend(network::nwJoin,m_pNetwork->localPlayerName(),"Target="+aMsg["Sender"].toString());
    }
    else
        m_pNetwork->doSend(network::nwInformation,aMsg["Sender"].toString(),"Info=InviteDeclined");
}

void GamePlay::doNetworkJoin(QVariantMap aMsg)
{
    if (aMsg["Sender"].toString() == m_pNetwork->localPlayerName()) {
        QString runningGame = aMsg["Game"].toString();
        if (!runningGame.isEmpty()) {
            m_pNetwork->doSend(network::nwLoadGame, m_pNetwork->localPlayerName(), "Name="+runningGame+".ssv");
        }
    }
    m_pNetwork->doSend(network::nwRefresh, "all", "");
}

void GamePlay::syncNewGame(QVariantMap aConfig)
{
    //init poll
    m_pNetwork->doSend(network::nwPoll, "group", "");
    QStringList configData;

    //convert QVariantMap to QStringList
    const QStringList confKeys = aConfig.keys();
    for (int i = 0; i < confKeys.count(); i++)
        configData.append(confKeys[i] + "=" + aConfig[confKeys[i]].toString());

    //add player names
    const QStringList groupInfo = m_PlayersTreeModel->groupData(m_pNetwork->localPlayerName());
    configData.append("playerCount="+QString::number(groupInfo.count()));
    for (int i=0; i<groupInfo.count(); i++)
        configData.append("player"+QString::number(i+1) + "=" + groupInfo[i]);

    //seed for equal randomization
    configData.append("seed="+QString::number(time(0)));
    //share with group
    m_pNetwork->doSend(network::nwSyncNewGame, "group", configData.join("\a"));
}

void GamePlay::doNetworkSyncNewGame(QVariantMap aMsg)
{
    emit applyConfig(aMsg);
    emit newGame(aMsg["IsLoading"].toBool());
}

void GamePlay::syncNextPlayer()
{
    QStringList moveData;
    QVariant varLetter;
    const int nLetters = m_lMoves.last()->letterCount();
    moveData.append("lettercount="+QString::number(nLetters));
    for (int i = 0; i < nLetters; i++) {
        varLetter = m_lMoves.last()->getLetter(i).toVariant();
        moveData.append("letter" + QString::number(i) + "=" + varLetter.toStringList().join(","));
    }
    m_pNetwork->doSend(network::nwNextPlayer, "group", moveData.join("\a"));
}

void GamePlay::doNetworkNextPlayer(QVariantMap aMsg)
{
    QCoreApplication::processEvents();
    if (aMsg["IsLoading"].toBool() || (aMsg["Sender"] != m_pNetwork->localPlayerName()))
    {
        QStringList sLetter;
        Letter aLetter;
        const int nLetters = aMsg["lettercount"].toInt();

        for (int i = 0; i < nLetters; i++) {
            sLetter = aMsg["letter"+QString::number(i)].toString().split(",");
            if (sLetter.count() == 12) { //Letter::length()
                aLetter.fromString(sLetter);
                // dropLetter()
                m_pBoard->setLetter(aLetter);
                m_pBoardModel->updateSquare(aLetter.Point);
                if (m_pBoard->is3D()) cubeModel()->updateSquare(aLetter.Point);
                m_pRackModel->placeLetter(aLetter.RackPos);
                m_lMoves.last()->addLetter(aLetter);
            } else
                qWarning() << sLetter;
        }
        if (m_pRackModel->rackSize() == m_lMoves.last()->letterCount())
            m_lMoves.last()->setBonus( m_nScrabbleBonus, move::btScrabble );
    }
    nextPlayer();//aMsg["IsLoading"].toBool());
}

void GamePlay::doNetworkRemoteGames(QVariantMap aMsg)
{
    m_RemoteGamesModel->setRemoteGames(aMsg, m_pNetwork->localPlayerName());
    emit showRemoteGames();
}

void GamePlay::doNetworkGameResult(int oldRating, int newRating)
{
    m_pMsgModel->addMessage(tr("Your rating has changed from %1 to %2")
                              .arg(oldRating)
                              .arg(newRating));
}

void GamePlay::doNetworkPoll(QVariantMap aMsg)
{
    //TODO: gameplay/network: suspend game, stop clock, exclude kibitzes
    //init
    m_lAnswer.clear();
    m_bIsPoll = true;
    emit isPollChanged();
    emit answerChanged();
}

void GamePlay::doNetworkAnswer(QVariantMap aMsg)
{
qDebug() << aMsg;
    m_lAnswer[aMsg["Sender"].toString()] = aMsg["Answer"].toString();
    if (m_lAnswer.count() == m_PlayersTreeModel->groupData(m_pNetwork->localPlayerName()).count())
        m_bIsPoll = false;
}

bool GamePlay::waitForPoll()
{
    //wait; receive answer via nwAnswer
    while (m_bIsPoll)
        QCoreApplication::processEvents(QEventLoop::AllEvents);

    //evaluate
qDebug() << m_lAnswer.values();
    bool result = true;
    const QStringList keys = m_lAnswer.keys();
    for (int i = 0; i < keys.count(); i++) {
        if (m_lAnswer.value(keys[i]) != "true")
            result = false;
    }

    emit isPollChanged();
    return result;
}

void GamePlay::doInvite(QString sName)
{
    QStringList aData;
    aData.append("GameID=" + QString::number(m_PlayersTreeModel->playerData(m_pNetwork->localPlayerName()).gameID));
    aData.append("Language=" + QString("en")); //TODO: gameplay: menu language
    aData.append("Country=" + m_PlayersTreeModel->playerData(m_pNetwork->localPlayerName()).country);
    aData.append("Dictionary=" + m_pDicListModel->dictionary->fileName());
    m_pNetwork->doSend(network::nwInvite, sName, aData.join("\a"));
}

void GamePlay::doLeave()
{
    m_pNetwork->doSend(network::nwLeave,"group","");
}

void GamePlay::chat(QString aMessage)
{
    if (aMessage.startsWith("/shout ")) {
        aMessage.remove(0, 7); //QString::length("/shout ")
        m_pNetwork->doSend(network::nwChat,"all", "Chat="+aMessage);
    }
    else if (aMessage.startsWith("/say ")) {
        aMessage.remove(0, 5); //QString::length("/say ")
        m_pNetwork->doSend(network::nwChat,"group", "Chat="+aMessage);
    }
    else if (aMessage.startsWith("/whisper ")) {
        aMessage.remove(0, 9); //QString::length("/whisper ")
        QString aReceiver = aMessage.first(aMessage.indexOf(" "));
        aMessage.remove(0, aReceiver.length()+1);
        m_pNetwork->doSend(network::nwChat, aReceiver, "Chat="+aMessage);
    }
    else if (aMessage.startsWith("/kibitzes ")) {
        aMessage.remove(0, 10); //QString::length("/kibitzes ")
        m_pNetwork->doSend(network::nwChat, "kibitzes", "Chat="+aMessage);
    }
    else if (aMessage.startsWith("/refresh")) {
        m_pNetwork->doSend(network::nwRefresh,"","");
    }
    else if (aMessage.startsWith("/best")) {
        m_pNetwork->doSend(network::nwInformation, "", "Info=best");
    }
    else if (aMessage.startsWith("/finger")) {
        aMessage.remove(0, 8);
        aMessage.insert(0, "Info=finger\aName=");
        m_pNetwork->doSend(network::nwInformation, "", aMessage);
    }
    else if (aMessage.startsWith("/")) {
        qWarning() << "Unknown message type at " << aMessage;
    }
    else
        m_pNetwork->doSend(network::nwChat,"group", "Chat="+aMessage);
}

void GamePlay::doNetworkError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        addMessage(tr("The server has closed the connection."));
    case QAbstractSocket::HostNotFoundError:
        addMessage(tr("The host was not found."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        addMessage(tr("The connection was refused by the sever."));
        break;
    default:
        addMessage(tr("The following error occurred: %1.").arg(m_pNetwork->errorString()));
    }

    if (m_bIsConnected) //signal is sent on manual disconnect but also by the server
        disconnect();
}

void GamePlay::doNetworkInfo(QVariantMap aMsg)
{
    const QString sInfo = aMsg["Info"].toString();
    const QString sData = aMsg["Result"].toString().split("\r").join("\n");

    if (sInfo == "ChatStored")
        addMessage(tr("Recipient is not online. Your message is stored and will be transmitted on next connection."));
    else if (sInfo == "ChatUnknownReceiver")
        addMessage(tr("Unknown receiver"));
    else if (sInfo == "InviteDeclined")
        addMessage(tr("%1 declined").arg(aMsg["Sender"].toString()));
    else if (sInfo == "GroupPrivate")
        addMessage(tr("Kibitzed game has been set to private."));
    else if (sInfo == "KickedByAnother")
        addMessage(tr("You have been kicked because another player logged in with your name."));
    else if (sInfo == "LoadingFinished")
        //TODO: network: game end?
        m_pNetwork->doSend(network::nwRefresh, "", "");
    else // if ((sInfo == "finger") || (sInfo == "best"))
        addMessage(sData);
/*
     'LoadingFinished':
       begin
         Scrabble.GameState:=Scrabble.GameState-[gsLoading];
         OnProgress(self,101);
         OnMessage(smInformation,rMain_LoadingFinished);
         //during gsLoading repaint is adjourned
         if not (gsKibitz in Scrabble.GameState) then
          for i:=0 to Scrabble.NumberOfPlayers-1 do
           if Scrabble.Player[i].Name=TCPClient.PlayerData.PlayerName then
            Scrabble.LocalPlayer:=i;

         DoRepaint(seUnknown);
         if (gsGameEnd in Scrabble.GameState) and
            not aMsg.Value('IsGameEnd',false) then
              DoGameEnd(self);
         TCPClient.OnSend('nwRefresh','all','');
       end;
     else
       OnMessage(smError,aMsg.Value('Info',''));
    end;
*/
}

void GamePlay::doNetworkLogin(QVariantMap aMsg)
{
    clientInfo aPlayer;
    int nPlayer = aMsg["Count"].toInt(0);
    for (int i = 0; i < nPlayer; i++) {
        aPlayer.playerName = aMsg[QString::number(i) + "_Name"].toString();
        aPlayer.city = aMsg[QString::number(i) + "_City"].toString();
        aPlayer.country = aMsg[QString::number(i) + "_Country"].toString();
        aPlayer.menuLang = aMsg[QString::number(i) + "_MenuLang"].toString();
        aPlayer.registered = m_PlayersTreeModel->fromTDateTime(aMsg[QString::number(i) + "_Registered"].toDouble());
        aPlayer.release = aMsg[QString::number(i) + "_Release"].toString();
        m_PlayersTreeModel->addPlayer(aPlayer);
    }

    if (aMsg["Sender"] == m_pNetwork->localPlayerName()) {
        addMessage(aMsg["News"].toString());
        int i = 0;
        QString aChat;
        do {
            aChat = aMsg["Chat"+QString::number(i)].toString();
            if (!aChat.isEmpty()) addMessage(aChat);
            i++;
        }
        while (!aChat.isEmpty());

        //player who loged in sends a refresh message to all clients
        //which updates the variable attributes like grouping, current move...
        m_pNetwork->doSend(network::nwRefresh,"all","");
    }
}

//TODO: main: finish l10n
void GamePlay::localize(QString fileName)
{
    const QDir aPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

    if ( m_pTranslator->load(fileName,aPath.path()) ) {
        qApp->installTranslator(m_pTranslator);
        m_pEngine->retranslate();

    } else
        qWarning() << "Failed to load translation file";
}

//TODO: main: download signal-slot
void GamePlay::confDownloadFinished(const QString aFileName)
{
    Q_UNUSED(aFileName);
    m_pDownloadManager->checkUpdates();
    m_pDicListModel->updateList();
}


rackmodel *GamePlay::rackModel()
{
    return m_pRackModel;
}

boardmodel *GamePlay::boardModel()
{
    return m_pBoardModel;
}

cubemodel *GamePlay::cubeModel()
{
    return m_pCubeModel;
}

msgmodel *GamePlay::msgModel()
{
    return m_pMsgModel;
}

gamecoursemodel *GamePlay::gamecourseModel()
{
    return m_pGameCourseModel;
}

dicList *GamePlay::dicListModel()
{
    return m_pDicListModel;
}

playersTree *GamePlay::playersTreeModel()
{
//    if (m_pNetwork)
        return m_PlayersTreeModel;

    //TODO: gameplay: return null if no network
}

remoteGamesModel *GamePlay::remoteGames()
{
    return m_RemoteGamesModel;
}

void GamePlay::startNewGame(QStringList PlayerNames,
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
                             bool RandomSequence,
                             int seed)
{
    if (m_bIsConnected) {
        //newgame confirmed, sending answer to group
        m_pNetwork->doSend(network::nwAnswer, "group", "Answer=true");
        //wait for mates and cancel if one rejected
        if (!waitForPoll())
            return;
    }

    m_lPlayerNames.clear();
    emit numberOfPlayersChanged(); //refreh names in case the number is equal
    m_lPlayerNames = PlayerNames;

    if (RandomSequence) {
        if (seed == -1) seed = time(0); //TODO: gameplay: seed is int (and must be due to qml) but time() returns long; maybe use QRandomGenerator
        std::mt19937 rng(seed);
        std::shuffle(m_lPlayerNames.begin(), m_lPlayerNames.end(), rng);
    }
    m_pMsgModel->playerNames(m_lPlayerNames); //to highlight player messages
    emit numberOfPlayersChanged();

    m_pBoard->initialize(FieldTypeArray, is3D);
    m_pBoardModel->update();
    if (is3D) m_pCubeModel->reset();
    emit boardSizeChanged();
    emit is3DChanged();
    m_nPlayerCount = m_lPlayerNames.count();
    m_nCurrentPlayer = 0;
    m_pRackModel->initialize(m_nPlayerCount, RackSize);
    m_lBag.clear();
    QString sChars;
    for (int i=0; i<LetterList.count(); i+=3) {
        Letter aLetter;
        aLetter.What = LetterList[i].toString();
        sChars += aLetter.What;
        aLetter.Value = LetterList[i+1].toInt();
        for (int j=0; j<LetterList[i+2].toInt(); j++)
            m_lBag.append(aLetter);
    }
    m_pDicListModel->dictionary->setLetterSet(sChars);

    for (int i=0; i<NumberOfJokers; i++) {
        Letter aLetter;
        aLetter.IsJoker = true;
        aLetter.What = JokerChar;
        aLetter.Value = 0;
        m_lBag.append(aLetter);
    }
//    std::random_device rd;
    if (seed == -1) seed = time(0);
    std::mt19937 rng(seed);
    std::shuffle(m_lBag.begin(), m_lBag.end(), rng);

    for (uint nPlayerIndex = 0; nPlayerIndex < m_nPlayerCount; nPlayerIndex++)
     for (int nRackIndex = 0; nRackIndex < m_pRackModel->rackSize(); nRackIndex++) {
            Letter aLetter = m_lBag[0];
            aLetter.State = LetterState::lsRack;
            aLetter.RackPos = nRackIndex;
            m_pRackModel->setLetter(aLetter, true, nPlayerIndex);
            m_lBag.removeAt(0);
        }

    m_bCanJokerExchange = CanJokerExchange;
    m_nGameEndBonus = GameEndBonus;
    m_nNumberOfPasses = NumberOfPasses * m_nPlayerCount;
    m_nJokerPenalty = JokerPenalty;
    m_bChangeIsPass = ChangeIsPass;
    m_eTimeControlType = static_cast<TimeControlType>(TimeControl);
    m_nTimeControlValue = TimeControlValue;
    m_nLimitedExchange = LimitedExchange;
    m_bCambioSecco = CambioSecco;
    m_bWhatif = Whatif;
    m_bAdd = Add;
    m_bSubstract = Substract;
    m_nTimePenaltyValue = TimePenaltyValue;
    m_nTimePenaltyCount = TimePenaltyCount;
    m_bTimeGameLost = TimeGameLost;
    m_eWordCheckMode = static_cast<WordCheckMode>(WordCheck);
    m_nWordCheckPeriod = WordCheckPeriod;
    m_nWordCheckPenalty = WordCheckPenalty;
    m_nWordCheckBonus = WordCheckBonus;
    m_nScrabbleBonus = ScrabbleBonus;
    m_bIsCLABBERS = isCLABBERS;

    m_bIsRunning = true;
    m_bIsInitialized = true;
    m_bIsFirstMove = true;
    m_nPasses = 0;

    m_lMoves.clear();
    m_lMoves.append(new move(m_bIsFirstMove, m_pBoard));
    m_nCurrentMove = 0;
    m_nCurrentPlayer = 0;
    doUpdateRack(); // fill with letters
    if (m_bIsConnected)
        m_pRackModel->setLocalPlayer(m_lPlayerNames.indexOf(m_pNetwork->localPlayerName()));
    else
        m_pRackModel->setLocalPlayer(m_nCurrentPlayer);
    m_pRackModel->setActivePlayer(m_nCurrentPlayer);

    m_pGameCourseModel->clear();
    doStartTimer();
    emit currentPlayerChanged();
    emit isLocalPlayerChanged();
    emit isRunningChanged();
    emit isInitializedChanged();
    emit statInfoChanged();

    m_pMsgModel->addMessage( tr("New game has been started successfully :-).") );
}

void GamePlay::nextPlayer(const bool bIsLoading)
{
    // stop the timer
    if (!bIsLoading && (m_nTimerID > -1)) {
        killTimer(m_nTimerID);
        m_nTimerID = -1;
    }

    //check word(s)
    if ((!bIsLoading) && //no check necessary when called from loadGame()
        (!m_lMoves.last()->PlacedWord.isEmpty()))
    {
        if (!doCheckWords())
            return;
    }

    //result message
    if ((!bIsLoading) &&
        (!m_lMoves.last()->PlacedWord.isEmpty()))
        m_pMsgModel->addMessage(tr("%1 places %2 %3 and receives %4 points.")
                                    .arg(m_lPlayerNames[m_nCurrentPlayer])
                                    .arg(m_lMoves.last()->PlacedWord)
                                    .arg(!m_lMoves.last()->ConnectedWords.isEmpty() ? tr("and") + " " + m_lMoves.last()->ConnectedWords : "")
                                    .arg(QString::number(m_lMoves.last()->Value())),
                                m_lPlayerNames.at(m_nCurrentPlayer));

    //increase number of passes
    const int nExchange = m_pRackModel->exchangeNumber();
    bool isPass = m_lMoves.last()->PlacedWord.isEmpty();
    if ((nExchange > 0) && !m_bChangeIsPass)
        isPass = false;
    if (isPass) {
        m_nPasses++;
        if (!bIsLoading && (nExchange == 0))
            m_pMsgModel->addMessage(tr("%1 passes the move.").arg(
                                        m_lPlayerNames[m_nCurrentPlayer]),
                                    m_lPlayerNames[m_nCurrentPlayer]);
    }
    else
        m_nPasses = 0;

    //TODO: gameplay: nice literal number and plural (%2 piece/s)
    if (nExchange > 0)
        m_pMsgModel->addMessage(tr("%1 exchanges %2 pieces.")
                                    .arg(m_lPlayerNames[m_nCurrentPlayer])
                                    .arg(nExchange),
                                m_lPlayerNames[m_nCurrentPlayer]);

    //lMoves is zero before first move
    if (m_lMoves.last()->Value() > 0)
        m_bIsFirstMove = false;

    //FIXME: addMove() done after gameend when loading game that has ended
    //update game course
    m_pGameCourseModel->addMove(replaceAllLetters(m_lMoves.last()->PlacedWord),
                                replaceAllLetters(m_lMoves.last()->ConnectedWords),
                                m_nCurrentPlayer,
                                m_lMoves.last()->Value(),
                                m_pComputeMove->numberOfResults() > 0 ? m_pComputeMove->result(0)->Value() : 0,
                                m_lMoves.last()->isScrabble(),
                                m_nMoveTime);
    emit statInfoChanged();

    //check game end
    if (!bIsLoading && //when loading bIsRunning is saved and restored
        ((m_pRackModel->isRackEmpty() && //all letters placed and...
          (m_lBag.count()==0)) || //... no letter left in bag or... TODO: gameplay: obsolete check?
         (m_nPasses == m_nNumberOfPasses) )) //max numbers of passes reached
    {
        doGameEnd();
        emit statInfoChanged(); //update statusbar for bonus calculation
    }

    //updates the UI
    m_pBoard->placeLetters();
    m_pBoardModel->updateAllSquares();

    if (!m_bIsRunning) //game has ended
        return;

    //new pieces for the placed or marked to exchange
    if (!bIsLoading)
        doUpdateRack();

    //new empty move
    m_lMoves.append(new move(m_bIsFirstMove, m_pBoard));

    //increase currentmove and currentplayer
    m_nCurrentMove++;
    m_nCurrentPlayer = m_nCurrentMove % m_nPlayerCount;
    if (m_bIsConnected)
        m_pRackModel->setLocalPlayer(m_lPlayerNames.indexOf(m_pNetwork->localPlayerName()));
    else
        m_pRackModel->setLocalPlayer(m_nCurrentPlayer);
    m_pRackModel->setActivePlayer(m_nCurrentPlayer);
    emit currentPlayerChanged();
    emit isLocalPlayerChanged();

    //start the timer
    doStartTimer(!bIsLoading);

    if (bIsLoading)
        return; //do not update the board when loading a game

    //allow next player action
    m_bIsAccepted = true;
    m_pMsgModel->addMessage(tr("It's %1's turn.").arg(
                                m_lPlayerNames[m_nCurrentPlayer]),
                            m_lPlayerNames[m_nCurrentPlayer]);
    emit isAcceptedChanged();
    emit lastErrorChanged();

    //TODO: nextplayer: remove debug.ssg on every move
    if (!m_bIsConnected) {
        const QDir aPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        QString fileName = aPath.path() +"/debug.ssg";
        saveGame(QUrl::fromLocalFile(fileName));
    }

    //clear computemove
    m_pComputeMove->clear();
    emit bestMoveCountChanged();

    //compute and place if player is "Computer"
    if (m_lPlayerNames[m_nCurrentPlayer] == "Computer") {
        computeMove();
        if ((m_pComputeMove->numberOfResults() == 0) &&
            (m_lBag.count() >= m_pRackModel->rackSize()))
            m_pComputeMove->markLettersForExchange();
        else
            placeBestMove(1);
        nextPlayer();
    }
}

void GamePlay::doGameEnd()
{
    // if all pieces are placed, ie. not ending by passes, the end game bonus is
    // given to the player who ended the game; which is last()-1 here
    // first call of setGameEndBonus() will also find the winner, after adding the game end bonus
    if ( (m_nPasses < m_nNumberOfPasses) && (m_nGameEndBonus > 0) ) {
        m_pGameCourseModel->addBonus(m_nCurrentPlayer, m_nGameEndBonus);
        m_pMsgModel->addMessage(tr("%1 has ended the game and receives a bonus of %2 points")
                                    .arg(m_lPlayerNames[m_nCurrentPlayer])
                                    .arg(m_nGameEndBonus)
                                );
    } else
        m_pMsgModel->addMessage(tr("%1 has ended the game.")
                                    .arg(m_lPlayerNames[m_nCurrentPlayer]));

    int nAdd = 0;
    int nSubstract;
    Letter aLetter;
    for (uint nPlayer = 0; nPlayer < m_nPlayerCount; nPlayer++) {
        nSubstract = 0;
        for (int nRackPos = 0; nRackPos < m_pRackModel->rackSize(); nRackPos++) {
            aLetter = m_pRackModel->getLetter(nRackPos, nPlayer);
            nSubstract += aLetter.Value;
            nAdd += aLetter.Value;
            if (aLetter.IsJoker)
                nSubstract += m_nJokerPenalty;
        }
        if (m_bSubstract && (nSubstract > 0)) {
                m_pGameCourseModel->addBonus(nPlayer, -nSubstract);
                m_pMsgModel->addMessage(tr("%1: %2 minus %3 points")
                                            .arg(m_lPlayerNames[nPlayer])
                                            .arg(m_pGameCourseModel->getScore(nPlayer) + nSubstract)
                                            .arg(nSubstract)
                                        );
        }
    }

    if (m_bAdd) {
        m_pGameCourseModel->addBonus(m_nCurrentPlayer, nAdd);
        m_pMsgModel->addMessage(tr("%1: %2 plus %3 points")
                                    .arg(m_lPlayerNames[m_nCurrentPlayer])
                                    .arg(m_pGameCourseModel->getScore(m_nCurrentPlayer) - nAdd)
                                    .arg(nAdd)
                                );
    }
    QList<int> nWinner;
    m_pGameCourseModel->getWinner(nWinner);
    QString sWinner;
    if (nWinner.count() > 1) {
        sWinner = tr("Draw between ");
        for (int i = 0; i < nWinner.count(); i++) {
            sWinner += m_lPlayerNames[nWinner[i]];
            if (i < nWinner.count()-1)
                sWinner += " " + tr("and") + " ";
        }
    }
    else if (nWinner.count() > 0) //all player score 0 or below
        sWinner = tr("%1 wins the game.").arg(m_lPlayerNames[nWinner[0]]);

    if (!sWinner.isEmpty())
        m_pMsgModel->addMessage(sWinner);

    //network
    if (m_bIsConnected) {
        QStringList aData;
        int nScore;
        for (uint i=0; i<m_nPlayerCount; i++) {
            aData.append("Player"+QString::number(i)+"Name="+m_lPlayerNames[i]);
            nScore = m_pGameCourseModel->getScore(i);
            aData.append("Player"+QString::number(i)+"Value="+QString::number(nScore));
            if (m_lPlayerNames[i] == m_pNetwork->localPlayerName())
                aData.append("MyResult="+QString::number(nScore));
        }
        m_pNetwork->doSend(network::nwGameResult, "group", aData.join("\a"));
    }

    m_bIsRunning = false;
    emit isRunningChanged();
}

void GamePlay::doUpdateRack()
{
    for (int i=0; i<m_pRackModel->rackSize(); i++) {
        Letter rackLetter = m_pRackModel->getLetter(i);
        if (rackLetter.IsEmpty()) {
            if (m_lBag.count()>0) {
                Letter aLetter = m_lBag[0];
                aLetter.State = LetterState::lsRack;
                aLetter.RackPos = i;
                m_pRackModel->setLetter(aLetter);
                m_lBag.removeAt(0);
            }
        }
        else if (rackLetter.IsExchange) {
            //TODO: gameplay: treat exchange optionally as pass
            rackLetter.State = LetterState::lsBag;
            rackLetter.RackPos = 0;
            rackLetter.IsExchange = false;
            m_lBag.append(rackLetter); //FIXME: do not append but randomly insert exchanged letters
            m_pRackModel->placeLetter(i,false); //set empty
            Letter aLetter = m_lBag[0];
            aLetter.State = LetterState::lsRack;
            aLetter.RackPos = i;
            m_pRackModel->setLetter(aLetter);
            m_lBag.removeAt(0);
        }
    }
}

bool GamePlay::doCheckWords()
{
    QStringList unknownWords = m_pDicListModel->dictionary->checkWords(m_lMoves.last()->PlacedWord,m_lMoves.last()->ConnectedWords);
    if (!unknownWords.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Scrabble3D");
        QString msgText = tr("Unknown word(s):")+"\n%1";
        msgBox.setText(msgText.arg(replaceAllLetters(unknownWords.join(","))));
        /* msgBox.setText(tr("%n Word(s) %1 not found.","",
                                 unknownWords.count()).arg(unknownWords.join(","))); */
        msgBox.setInformativeText(tr("Do you want to accept anyway?"));
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        if (msgBox.exec() == QMessageBox::No)
            return false;
    }
    return true;
}

void GamePlay::doStartTimer(const bool doRun)
{
    m_nMoveTime = 0;
    if (m_eTimeControlType == tcNoLimit)
        m_nTimeLeft = 0;
    else if (m_eTimeControlType == tcPerMove)
        m_nTimeLeft = m_nTimeControlValue;
    else if (m_eTimeControlType == tcPerGame)
        m_nTimeLeft = m_nTimeControlValue - m_pGameCourseModel->timeTotalPerPlayer(m_nCurrentPlayer);
    if (doRun) {
        if ((m_eTimeControlType != tcNoLimit) && (m_nTimeLeft == 0))
            nextPlayer();
        else
            m_nTimerID = startTimer(1000);
    }
}

bool GamePlay::canDrop(const unsigned int boardIndex)
{
    if (!getIsLocalPlayer())
        return false;
    int z = m_pBoard->pointToWhere( m_pBoard->pos3D(boardIndex) );
    return (m_pBoard->getLetter(z).State == LetterState::lsEmpty);
}

bool GamePlay::canDrag()
{
    for (int i=0; i<m_pRackModel->rackSize(); i++)
        if (m_pRackModel->getLetter(i).IsExchange) {
            //TODO: gameplay: message to statusbar
            m_pMsgModel->addMessage(tr("Letters have been marked for exchange"));
            return false;
        }
    return true;
}

void GamePlay::exchangeLetter(const unsigned int rackIndex)
{
    //bag needs to have letters left
    if (m_lBag.count() < m_pRackModel->rackSize()) {
        m_pMsgModel->addMessage(tr("Not enough tile left to exchange letters."));
        return;
    }
    //no letter must have been placed
    const Letter aLetter = m_pRackModel->getLetter(rackIndex);
    if (!aLetter.IsExchange) {
        for (int i=0; i<m_pRackModel->rackSize(); i++)
            if (m_pRackModel->getLetter(i).IsEmpty()) {
                //TODO: gameplay: message to statusbar
                m_pMsgModel->addMessage(tr("Exchanging letters is not possible."));
                return;
            }
    }
    //do toggle
    m_pRackModel->toggleExchangeFlag(rackIndex);
}

void GamePlay::resetPieces()
{
    Letter aLetter;
    for (int i=0; i<m_pBoard->getFieldSize(); i++)
    {
        aLetter = m_pBoard->getLetter(i);
        if (aLetter.State == LetterState::lsBoard)
            removeLetter(aLetter);
    }
}

void GamePlay::timerEvent(QTimerEvent *event)
{
    if ( m_pRackModel->isLocalIsActive() &&
         (m_eTimeControlType != tcNoLimit) && (m_nMoveTime >= m_nTimeLeft))
        nextPlayer();

    m_nMoveTime++;

    if (m_nStatInfoType == 2)
        emit statInfoChanged();

    Q_UNUSED(event);
}

QString GamePlay::getStatInfoType(const int aType)
{
//    if (!m_bIsRunning)
//        return "";

    switch (aType) {
    case 0:
        return QString::number(m_lBag.count());
    case 1: {
        QString aResult;
        for (uint i = 0; i < m_nPlayerCount; i++)
            aResult += QString::number(m_pGameCourseModel->getScore(i)) + ",";
        aResult.chop(1);
        return aResult;
    }
    case 2: {
        QTime aTime(0,0,0);
        if (m_eTimeControlType == tcNoLimit)
            aTime = aTime.addSecs(m_nMoveTime);
        else
            aTime = aTime.addSecs(m_nTimeLeft - m_nMoveTime);

        if (m_eTimeControlType == tcPerGame)
            return aTime.toString();
        else
            return aTime.toString("mm:ss");
    }
    default:
        return "";
    } //switch
}

QString GamePlay::getAllStat()
{
    return tr("Letter:") + " " + getStatInfoType(0) + "\n" +
           tr("Game score:") + " " + getStatInfoType(1) + "\n" +
           tr("Time left:" ) + " " + getStatInfoType(2);
}

void GamePlay::dropLetter(const uint rackIndex, const uint boardIndex)
{
    Letter aLetter = m_pRackModel->getLetter(rackIndex);

    aLetter.State = LetterState::lsBoard;
    aLetter.When = m_nCurrentMove;
    aLetter.Who = m_nCurrentPlayer;
    aLetter.Point = m_pBoard->pos3D(boardIndex);
    aLetter.Where = m_pBoard->pointToWhere(aLetter.Point);
    m_pBoard->setLetter(aLetter);
    m_pBoardModel->updateSquare(aLetter.Point);
    if (m_pBoard->is3D()) cubeModel()->updateSquare(aLetter.Point);//aIndex);
    m_pRackModel->placeLetter(rackIndex);

    m_bIsAccepted = m_lMoves.last()->addLetter(aLetter);
    if (m_bIsAccepted)
    {
        if (m_pRackModel->rackSize() == m_lMoves.last()->letterCount())
            m_lMoves.last()->setBonus( m_nScrabbleBonus, move::btScrabble );
    }
    emit placedValueChanged();
    emit isAcceptedChanged();
    emit lastErrorChanged();
}

void GamePlay::dropLetterRack(const uint fromIndex, const uint toIndex)
{
    m_pRackModel->move(fromIndex, toIndex);
}

void GamePlay::setJokerLetter(const uint boardIndex, const QString aWhat)
{
    int z = m_pBoard->pointToWhere( m_pBoard->pos3D(boardIndex) );
    m_pBoard->setJokerLetter(z, aWhat);
    m_lMoves.last()->setJokerLetter(aWhat); //last placed letter
    m_lMoves.last()->checkMove(); //re-check to update placeword
    Letter aLetter = m_pBoard->getLetter(z);
    m_pBoardModel->updateSquare(aLetter.Point);
}

void GamePlay::removeLetter(const uint boardIndex)
{
    int z = m_pBoard->pointToWhere( m_pBoard->pos3D(boardIndex) );
    Letter aLetter = m_pBoard->getLetter(z);
    removeLetter(aLetter);
}

void GamePlay::removeLetter(Letter aLetter)
{
    Point3D aPoint = aLetter.Point;
    int z = aLetter.Where;
    if (aLetter.State == LetterState::lsBoard) {
        aLetter.State = LetterState::lsRack;
        aLetter.When = 0;
        aLetter.Who = 0;
        aLetter.Where = 0;
        if (aLetter.IsJoker) aLetter.What = JokerChar;
        aLetter.Point = Point3D(0,0,0);

        m_pBoard->removeLetter(z);
        m_pBoardModel->updateSquare(aPoint);
        if (m_pBoard->is3D()) cubeModel()->updateSquare(aPoint);//aIndex);

//        if (!startDrag)
            m_pRackModel->setLetter(aLetter);

        m_bIsAccepted = m_lMoves.last()->deleteLetter(z);

        emit placedValueChanged();
        emit isAcceptedChanged();
        emit lastErrorChanged();
    }
}

void GamePlay::setActiveDimension(const unsigned int aDimension)
{
    Dimension aDim;
    if (aDimension<3)
       aDim = static_cast<Dimension>(aDimension);
    else
       aDim = dmAbscissa;

    if (aDim != m_pBoard->getActiveDimension())
    {
        m_pBoard->setActiveDimension(aDim);
        m_pBoardModel->updateAllFields();
        emit activeDimensionChanged();
    }
}

unsigned int GamePlay::getActiveDimension()
{
    return static_cast<int>(m_pBoard->getActiveDimension());
}

void GamePlay::setActivePosition(const int aPosition, const bool bSilent)
{
    if ( m_pBoard->is3D() &&
         (aPosition < m_pBoard->getBoardSize()) && (aPosition>=0) &&
         (aPosition != getActivePosition())
       )
    {
        m_pBoard->setActivePosition(aPosition);
        if (bSilent) return;
        m_pBoardModel->updateAllFields();
        emit activePositionChanged();
    }
}

int GamePlay::getActivePosition()
{
    return m_pBoard->getActivePosition();
}

void GamePlay::setComputeProgress(const int aProgress)
{
    if (m_nProgress != aProgress) {
        m_nProgress = aProgress;
        emit computeProgressChanged();
        //refresh
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
}

void GamePlay::saveConfig(QString fileName, QVariantMap configData)
{
    const QDir aPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (fileName.isEmpty()) //new game
        fileName = aPath.path() + "/Scrabble3D.ini";
//FIXME: gameplay: Android cannot access content:// urls; native filedialog not yet implemented
#ifndef Q_OS_ANDROID
    else
        fileName = QUrl(fileName).toLocalFile();
#endif
    QSettings settings(fileName, QSettings::IniFormat);

    settings.beginGroup("config");
    for (auto i = configData.begin(); i!=configData.end(); i++)
        settings.setValue(i.key(), i.value());
    settings.endGroup();
}

QVariantMap GamePlay::loadConfig(QString fileName)
{
    const QDir aPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (fileName.isEmpty()) //new game
        fileName = aPath.path() + "/Scrabble3D.ini";
#ifndef Q_OS_ANDROID
    else
        fileName = QUrl(fileName).toLocalFile();
#endif

    QSettings settings(fileName, QSettings::IniFormat);

    QVariantMap configData;
    settings.beginGroup("config");
    QStringList keys = settings.childKeys();
    for (int i=0; i<keys.count(); i++)
        configData[keys[i]] = settings.value(keys[i]).toString();
    settings.endGroup();
    return configData;
}

void GamePlay::saveGame(const QUrl &fileName)
{
//    m_pMsgModel->addMessage(tr("Game saved as %1").arg(fileName.toLocalFile()));
    QSettings settings(fileName.toLocalFile(), QSettings::IniFormat);

    //clear previous values
    settings.clear();

    //TODO: saveGame: store randseed => maybe not neccessary since bag is randomized at beginning
    if (m_pBoard->is3D())
    {
        settings.setValue("ActiveDimension",m_pBoard->getActiveDimension());
        settings.setValue("ActivePosition",m_pBoard->getActivePosition());
    }
    settings.beginGroup("game");
    {
        settings.setValue("PlayerNames", m_lPlayerNames);
        settings.setValue("RackSize", m_pRackModel->rackSize());
        settings.setValue("3D", m_pBoard->is3D());
        settings.setValue("CanJokerExchange", m_bCanJokerExchange);
        settings.setValue("GameEndBonus", m_nGameEndBonus);
        settings.setValue("NumberOfPasses", m_nNumberOfPasses);
        settings.setValue("JokerPenalty", m_nJokerPenalty);
        settings.setValue("ChangeIsPass", m_bChangeIsPass);
        settings.setValue("TimeControlType", m_eTimeControlType);
        settings.setValue("TimeControlValue", m_nTimeControlValue);
        settings.setValue("LimitedExchange", m_nLimitedExchange);
        settings.setValue("CambioSecco", m_bCambioSecco);
        settings.setValue("Whatif", m_bWhatif);
        settings.setValue("Add", m_bAdd);
        settings.setValue("Substract", m_bSubstract);
        settings.setValue("TimePenaltyValue", m_nTimePenaltyValue);
        settings.setValue("TimePenaltyCount", m_nTimePenaltyCount);
        settings.setValue("TimeGameLost", m_bTimeGameLost);
        settings.setValue("WordCheckMode", m_eWordCheckMode);
        settings.setValue("WordCheckPeriod", m_nWordCheckPeriod);
        settings.setValue("WordCheckPenalty", m_nWordCheckPenalty);
        settings.setValue("WordCheckBonus", m_nWordCheckBonus);
        settings.setValue("ScrabbleBonus", m_nScrabbleBonus);
        settings.setValue("CLABBERS", m_bIsCLABBERS);
        settings.setValue("IsRunning", m_bIsRunning);
    }
    settings.endGroup();

    settings.beginWriteArray("bag");
    {
        for (int i = 0; i < m_lBag.count(); ++i) {
            settings.setArrayIndex(i);
            settings.setValue("letter", m_lBag[i].toVariant());
        }
    }
    settings.endArray();

    settings.beginGroup("board");
    {
        QVariantList aList;
        for (int i=0; i<m_pBoard->getFieldSize(); i++)
            aList.append( m_pBoard->getFieldtype(i) );
        settings.setValue("fieldtypes", aList);
    }
    settings.endGroup();

    settings.beginWriteArray("moves");
    {
        for (int i = 0; i < m_lMoves.count(); i++) {
            settings.setArrayIndex(i);
            settings.beginWriteArray("move");
            for (int j = 0; j < m_lMoves[i]->letterCount(); j++) {
                settings.setArrayIndex(j);
                settings.setValue("letter", m_lMoves[i]->getLetter(j).toVariant());
            }
            settings.endArray();
            settings.setValue("time", m_pGameCourseModel->timePerMove(i));
        }
    }
    settings.endArray();

    settings.beginWriteArray("player");
    {
        for (uint nPlayerIndex = 0; nPlayerIndex < m_nPlayerCount; nPlayerIndex++) {
            settings.setArrayIndex( nPlayerIndex );
            settings.beginWriteArray("rack");
            {
                for (int nRackIndex = 0; nRackIndex < m_pRackModel->rackSize(); nRackIndex++) {
                    settings.setArrayIndex( nRackIndex );
                    settings.setValue("letter",m_pRackModel->getLetter( nRackIndex, nPlayerIndex ).toVariant());
                }
            }
            settings.endArray();
        }
    }
    settings.endArray();

    settings.beginGroup("extra");
    {
        settings.setValue("letterset",m_pDicListModel->dictionary->getLetterSet());
    }
    settings.endGroup();
}

void GamePlay::loadGame(const QUrl &fileName)
{
#ifdef QT_DEBUG
    qDebug() << "Load game from " << fileName.toLocalFile();
#endif

    QFile aFile;
    if (!aFile.exists(fileName.toLocalFile()))
        return;

    if (m_bIsRunning && (m_nTimerID > -1)) {
        killTimer(m_nTimerID);
        m_nTimerID = -1;
    }

    m_pMsgModel->addMessage(tr("Loading game from %1...").arg(fileName.toLocalFile()));
    QSettings settings(fileName.toLocalFile(), QSettings::IniFormat);
    m_pGameCourseModel->clear();
    m_lPlayerNames.clear();
    bool bIs3D;
    int aRackSize;
    settings.beginGroup("game");
    {
        m_lPlayerNames = settings.value("PlayerNames").toStringList();
        m_nPlayerCount = m_lPlayerNames.count();
        aRackSize = settings.value("RackSize").toUInt();
        bIs3D = settings.value("3D").toBool();
        m_bCanJokerExchange = settings.value("CanJokerExchange").toBool();
        m_nGameEndBonus = settings.value("GameEndBonus").toUInt();
        m_nNumberOfPasses = settings.value("NumberOfPasses").toUInt();
        m_nJokerPenalty = settings.value("JokerPenalty").toUInt();
        m_bChangeIsPass = settings.value("ChangeIsPass").toBool();
        m_eTimeControlType = static_cast<TimeControlType>(settings.value("TimeControlType").toInt());
        m_nTimeControlValue = settings.value("TimeControlValue").toUInt();
        m_nLimitedExchange = settings.value("LimitedExchange").toUInt();
        m_bCambioSecco = settings.value("CambioSecco").toBool();
        m_bWhatif = settings.value("/Whatif").toBool();
        m_bAdd = settings.value("Add").toBool();
        m_bSubstract = settings.value("Substract").toBool();
        m_nTimePenaltyValue = settings.value("TimePenaltyValue").toUInt();
        m_nTimePenaltyCount = settings.value("TimePenaltyCount").toUInt();
        m_bTimeGameLost = settings.value("TimeGameLost").toBool();
        m_eWordCheckMode = static_cast<WordCheckMode>(settings.value("WordCheckMode").toInt());
        m_nWordCheckPeriod = settings.value("WordCheckPeriod").toUInt();
        m_nWordCheckPenalty = settings.value("WordCheckPenalty").toUInt();
        m_nWordCheckBonus = settings.value("WordCheckBonus").toUInt();
        m_nScrabbleBonus = settings.value("ScrabbleBonus").toUInt();
        m_bIsCLABBERS = settings.value("CLABBERS").toBool();
    }
    settings.endGroup();

    settings.beginGroup("board");
    {
        QVariantList aList;
        aList = settings.value("fieldtypes").toList();
        m_pBoard->initialize(aList, bIs3D);
        m_pBoardModel->update();
        if (bIs3D) m_pCubeModel->reset();
        emit boardSizeChanged();
        emit is3DChanged();
    }
    settings.endGroup();

    int size = settings.beginReadArray("bag");
    {
        m_lBag.clear();
        Letter aLetter;
        for (int i = 0; i < size; ++i) {
            settings.setArrayIndex(i);
            aLetter.fromString(settings.value("letter").toStringList());
            m_lBag.append(aLetter);
        }
    }
    settings.endArray();

    m_pRackModel->initialize(m_nPlayerCount, aRackSize);
    Letter aLetter;
    settings.beginReadArray("player");
    {
        for (uint nPlayerIndex = 0; nPlayerIndex < m_nPlayerCount; nPlayerIndex++) {
            settings.setArrayIndex(nPlayerIndex);

            settings.beginReadArray("rack");
            for (int nRackIndex = 0; nRackIndex < m_pRackModel->rackSize(); nRackIndex++) {
                settings.setArrayIndex(nRackIndex);
                aLetter.fromString(settings.value("letter").toStringList());
                m_pRackModel->setLetter(aLetter, true, nPlayerIndex);
            }
            settings.endArray();
        }
    }
    settings.endArray();

    m_bIsAccepted = true;
    int nMoves = settings.beginReadArray("moves");
    {
        m_nCurrentMove = 0;
        m_nCurrentPlayer = 0;
        m_bIsRunning = true;
        m_bIsFirstMove = true;
        m_lMoves.clear();
        //initialize game
        m_lMoves.append(new move(m_bIsFirstMove, m_pBoard));

        Letter aLetter;
        m_nMoveTime = 0; //zero at beginning
        for (int i = 0; i < nMoves; i++) {
            settings.setArrayIndex(i);
            m_nMoveTime = settings.value("time").toInt();
            int nLetters = settings.beginReadArray("move");
            for (int j=0; j < nLetters; j++) {
                settings.setArrayIndex(j);
                aLetter.fromString(settings.value("letter").toStringList());
                m_pBoard->setLetter(aLetter);
                m_bIsAccepted = m_lMoves.last()->addLetter(aLetter);
            }
            if (nLetters == m_pRackModel->rackSize())
                m_lMoves.last()->setBonus( m_nScrabbleBonus, move::btScrabble ); //while playing done in dropLetter()
            settings.endArray();
            if (i < nMoves-1)
                nextPlayer(true); //bIsLoading = true; last move when game has ended follows below
            else
                m_pBoardModel->updateAllSquares(); //refresh for currently placed pieces
        }
    }
    settings.endArray();
    if (settings.value("game/IsRunning").toBool() == false) {
        if (m_lMoves.last()->PlacedWord.isEmpty())
            m_nPasses++;
        doGameEnd(); //game end is not done in nextplayer when loading but m_bIsRunning needs to be true
        nextPlayer(true); //execute last move
    }
    if (m_bIsRunning)
        m_pMsgModel->addMessage(tr("It's %1's turn.").arg(
                                    m_lPlayerNames.at(m_nCurrentPlayer)),
                                m_lPlayerNames.at(m_nCurrentPlayer));

    if (m_pBoard->is3D())
    {
        cubeModel()->reset(); //dropLetter() updates single cell, reset does begin/endResetModel()
        setActiveDimension(settings.value("ActiveDimension").toInt());
        emit activeDimensionChanged();
        setActivePosition(settings.value("ActivePosition").toInt());
        emit activePositionChanged();
    }

    settings.beginGroup("extra");
    {
        m_pDicListModel->dictionary->setLetterSet( settings.value("letterset").toString() );
    }
    settings.endGroup();

    emit numberOfPlayersChanged();
    emit currentPlayerChanged();
    emit isAcceptedChanged();
    emit lastErrorChanged();
    emit isRunningChanged();
    m_bIsInitialized = true;
    emit isInitializedChanged();

    if ((m_eTimeControlType != tcNoLimit) && (m_nTimeLeft == 0))
        nextPlayer();
    else
        m_nTimerID = startTimer(1000);
}

void GamePlay::computeMove()
{
    resetPieces(); //return previously placed pieces
    m_pComputeMove->run(m_bIsFirstMove);
    setComputeProgress(0);//clear
    //feedback
    if (m_lPlayerNames[m_nCurrentPlayer] != "Computer") {
        if (m_pComputeMove->numberOfResults()==0)
            addMessage(tr("No word found"));
        else
            addMessage(tr("Out of %1 possible words found the best scores %2 points.")
                          .arg(m_pComputeMove->numberOfResults())
                          .arg(m_pComputeMove->result(0)->Value()));
        emit bestMoveCountChanged();
    }

}

void GamePlay::placeBestMove(const int index)
{
    //clear board
    resetPieces();

    if ((index<1) || (index>m_pComputeMove->numberOfResults()))
        return;

    Letter aLetter;
    move* aMove = m_pComputeMove->result(index-1);
    if (m_pBoard->is3D()) {
        setActivePosition(aMove->activePosition());
        setActiveDimension(aMove->activeDimension()); //move's dimension is also used for position at col/row
    }
    for (int j=0; j<aMove->letterCount(); j++) {
        aLetter = aMove->getLetter(j);
        // dropping letter via qml always takes the current 2d plane - and converts into 3d
        // so we have to convert the correct 3d point into the preliminary plane value
        dropLetter(aLetter.RackPos, m_pBoard->pointToPlane(aLetter.Point));
        if (aLetter.IsJoker)
            setJokerLetter(aLetter.Where, aLetter.What);
    }
}
