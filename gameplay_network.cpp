#include "gameplay.h"

#include <QMessageBox>
#include <QCheckBox>

void GamePlay::connect(QString name, QString password, QString email, QString country, QString city)
{
    addMessage(tr("Connecting to game server..."));

    m_pNetwork = new network(this, name, password, email, country, city, m_pLocListModel->currentLang());

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
    QObject::connect(m_pNetwork, SIGNAL(onAnswer(QVariantMap)),
                     this, SLOT(doNetworkAnswer(QVariantMap)));
    // 1. mainwindow::acNewGame -> GamePlay.syncNewGame(ScrConfig.getConfigData(false))
    // 2. GamePlay.syncNewGame -> network send/receive nwSyncNewGame
    // 3. GamePlay.doNetworkSyncNewGame -> ScrConfig applyConfig & ScrNewGame
    QObject::connect(m_pNetwork, SIGNAL(onSyncNewGame(QVariantMap)),
                     this, SLOT(doNetworkSyncNewGame(QVariantMap)));
    // 1. mainwindow::acNextPlayer -> GamePlay.syncNextPlayer()
    //    * checkword if wcAsk, message nwNextPlayer to group with move info
    // 2. onNextPlayer => GamePlay.doNetworkNextPlayer()
    //    * starts poll(), places the move, and check words if wcPoll
    //    * if wcAsk do nextPlayer()
    // 3. if wcPoll nwAnswer() to group
    // 4. GamePlay::doNetworkAnswer answer = false resetpieces
    //    + nextplayer() here
    // 5. if wcPoll checkMove() and challengeWord() are sent later
    QObject::connect(m_pNetwork, SIGNAL(onNextPlayer(QVariantMap)),
                     this, SLOT(doNetworkNextPlayer(QVariantMap)));
    QObject::connect(m_pNetwork, SIGNAL(onCambioSecco()),
                     this, SLOT(doCambioSecco()));
    QObject::connect(m_pNetwork, SIGNAL(onChallengeMove(QString)),
                     this, SLOT(doNetworkChallengeMove(QString)));
    QObject::connect(m_pNetwork, SIGNAL(onChallengeResult(QVariantMap)),
                     this, SLOT(doNetworkChallengeResult(QVariantMap)));

    QObject::connect(m_pNetwork, SIGNAL(onRemoteGames(QVariantMap)),
                     this, SLOT(doNetworkRemoteGames(QVariantMap)));
    QObject::connect(m_pNetwork, SIGNAL(onGameResult(int,int)),
                     this, SLOT(doNetworkGameResult(int,int)));

    // adds msgtype and sender and sends the msg to the server -> network::doSend
    QObject::connect(this, SIGNAL(onSend(network::MessageType,QString,QString)),
                     m_pNetwork, SLOT(doSend(network::MessageType,QString,QString)));

    //TODO: network: make server variable
    m_pNetwork->connect("scrabble3d.ignorelist.com",5001);

    m_bIsRunning = false; // cancel running game
    emit isRunningChanged();
    m_bIsConnected = true;
    emit connectedChanged();
}

void GamePlay::disconnect()
{
    if (m_pNetwork->isConnected()) //do not ask if connection has failed
    {
        QSettings settings(config::ini(), QSettings::IniFormat);

        if ( settings.value("program/confirmlogout","true").toBool() )
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Scrabble3D");
            msgBox.setText( QObject::tr("Do you really want to close the network?") );
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            QScopedPointer<QCheckBox> checkBox(new QCheckBox("Confirm before closing the connection"));
            checkBox->setCheckState(Qt::Checked);
            msgBox.setCheckBox(checkBox.get());
            int result = msgBox.exec();
            settings.setValue("program/confirmlogout", checkBox->checkState() == Qt::Checked);
            if (result == QMessageBox::No)
                return;
        }
    }
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
    // auto reject invitations from old versions
    if (m_PlayersTreeModel->playerData(aMsg["Sender"].toString()).version < 4) {
        emit onSend(network::nwInformation,aMsg["Sender"].toString(),"Info=InviteDeclined");
        return;
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("Scrabble3D");
    switch ( aMsg["JoinMode"].toInt() ) {
    case 0: {
        msgBox.setText( tr("%1 invites you to a new game.").arg(aMsg["Sender"].toString()) );
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
    case 2: //TODO: gameplay network invitation ?
        msgBox.setText( tr("%1 invites you to a new game.").arg(aMsg["Sender"].toString()) );
        break;
    }
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    if (msgBox.exec() == QMessageBox::Yes) {
        //leave current group, if exist
        emit onSend(network::nwLeave,"group","");
        //compare releases
        QString sRelease = m_PlayersTreeModel->playerData(aMsg["Sender"].toString()).release;
        int nIndex = sRelease.indexOf("_");
        if (nIndex > 0)
            sRelease = sRelease.first(nIndex);
        if (sRelease != version::current()) {
            msgBox.setText(tr("You are using different releases: %1<>%2").arg(
                sRelease, version::current()));
            msgBox.setInformativeText("");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
        }
        //join the group
        emit onSend(network::nwJoin,m_pNetwork->localPlayerName(),"Target="+aMsg["Sender"].toString());
    }
    else
        emit onSend(network::nwInformation,aMsg["Sender"].toString(),"Info=InviteDeclined");
}

void GamePlay::doNetworkJoin(QVariantMap aMsg)
{
    Q_UNUSED(aMsg);
    /*    if (aMsg["Sender"].toString() == m_pNetwork->localPlayerName()) {
        QString runningGame = aMsg["Game"].toString();
        if (!runningGame.isEmpty()) {
            emit onSend(network::nwLoadGame, m_pNetwork->localPlayerName(), "Name="+runningGame+".ssv");
        }
    }
*/    emit onSend(network::nwRefresh, "all", "");
}

void GamePlay::syncNewGame(QVariantMap aConfig)
{
    QStringList configData;

    //convert QVariantMap to QStringList
    const QStringList confKeys = aConfig.keys();
    for (int i = 0; i < confKeys.count(); i++)
        configData.append(confKeys[i] + "=" + aConfig[confKeys[i]].toString());

    //add player names
    const QStringList groupInfo = m_PlayersTreeModel->groupData(m_pNetwork->localPlayerName()).split(",");
    configData.append("playerCount="+QString::number(groupInfo.count()));
    for (int i = 0; i < groupInfo.count(); i++)
        configData.append("player"+QString::number(i+1) + "=" + groupInfo[i]);

    //seed for equal randomization
    configData.append("seed="+QString::number(time(0)));
    //share with group
    emit onSend(network::nwSyncNewGame, "group", configData.join("\a"));
}

void GamePlay::doNetworkSyncNewGame(QVariantMap aMsg)
{
    const bool bIsLoading = aMsg["IsLoading"].toBool();
    //TODO: gameplay: check doNetworkSyncNewGame
    if (!bIsLoading) startPoll(ptNewGame);
    m_lGameConfig.clear();
    emit applyConfig(aMsg);
    emit newGame(bIsLoading); //received at mainform.qml, executing newgame(), followed by startNewGameLater() -> answer -> startNewGame()
    if (bIsLoading) startNewGame(m_lGameConfig);
}

void GamePlay::syncNextPlayer()
{
    //return in wcAsk mode if word is not found and user doesn't insist to place
    if (m_eWordCheckMode == wcAsk && !doCheckWords())
        return;
    if (m_bIsHistory)
        doSelectedMoveChanged(-1);

    QStringList moveData;
    QVariant varLetter;
    const int nLetters = m_pMoves.last()->letterCount();
    if (nLetters > 0) {
        moveData.append("lettercount="+QString::number(nLetters));
        for (int i = 0; i < nLetters; i++) {
            varLetter = m_pMoves.last()->getLetter(i).toVariant();
            moveData.append("letter" + QString::number(i) + "=" + varLetter.toStringList().join(","));
        }
    } else //nLetters == 0
    {
        for (int i = 0; i < m_pRackModel->rackSize(); i++)
            if (m_pRackModel->getLetter(i).IsExchange)
                moveData.append("letter" + QString::number(moveData.count()) +
                                "=" + QString::number(m_pRackModel->getLetter(i).RackPos));
        if (moveData.count() > 0)
            moveData.append("exchangecount="+QString::number(moveData.count()));
    }
    m_pNetwork->doSend(network::nwNextPlayer, "group", moveData.join("\a"));
}

void GamePlay::syncChallengeWord()
{
    m_pNetwork->doSend(network::nwCheckWord, "group", "");
}

void GamePlay::doNetworkChallengeMove(QString aSender)
{
    startPoll(ptChallenge);
    m_sChallengePlayer = aSender;
    bool validMove = doCheckWords(true); //true = check previous move
    emit onSend(network::nwAnswer, "group", validMove ? "Answer=true" : "Answer=false");
}

void GamePlay::doNetworkChallengeResult(QVariantMap aMsg)
{
    // block second challenge
    endChallenge();

    QString sChallengePlayer = aMsg["ChallengePlayer"].toString(); //when loading a game the m_sChallengePlayer variable is not filled

    // checkword = true => challenge failed
    if (aMsg["Result"].toBool()) {
        int i = m_lPlayerNames.indexOf(sChallengePlayer);
        if (i > -1) {
            if (m_nWordCheckPenalty > 0) {
                m_pGameCourseModel->addBonus(i, -m_nWordCheckPenalty);
                m_pMsgModel->addMessage(tr("%1 illegitimately challenged the last move and receives a penalty of %2 points.")
                                            .arg(sChallengePlayer)
                                            .arg(m_nWordCheckPenalty));
            } else
                m_pMsgModel->addMessage(tr("%1 illegitimately challenged the last move.")
                                            .arg(sChallengePlayer));
        }
    } else
    // checkword = false => incorrect words, legitimately challenged
    {
        rollbackLastMove();

        int i = m_lPlayerNames.indexOf(sChallengePlayer);
        if (i > -1) {
            if (m_nWordCheckBonus > 0) {
                m_pGameCourseModel->addBonus(i, m_nWordCheckBonus);
                m_pMsgModel->addMessage(tr("%1 legitimately challenged the last move and receives a bonus of %2 points.")
                                            .arg(sChallengePlayer)
                                            .arg(m_nWordCheckBonus));
            } else
                m_pMsgModel->addMessage(tr("%1 legitimately challenged the last move.")
                                            .arg(sChallengePlayer));
        }
    }
    emit statInfoChanged();
}

void GamePlay::doNetworkNextPlayer(QVariantMap aMsg)
{
    // end challenge if the next move is faster than the objection period
    // and start a new poll if !isLoading and !wcChallenge
    endChallenge();
    if ((m_eWordCheckMode != wcChallenge) && (!aMsg["IsLoading"].toBool()))
        startPoll(ptNextPlayer);

    // place letters if isLoading or not local player
    if ((aMsg["Sender"] != m_pNetwork->localPlayerName()) ||
        aMsg["IsLoading"].toBool()) {
        QStringList sLetter;
        Letter aLetter;
        const int nLetters = aMsg["lettercount"].toInt();
        if (nLetters > 0) {
            for (int i = 0; i < nLetters; i++) {
                sLetter = aMsg["letter"+QString::number(i)].toString().split(",");
                if (sLetter.count() == 12) { //Letter::length()
                    aLetter.fromString(sLetter);
                    // dropLetter()
                    m_pBoard->setLetter(aLetter);
                    m_pBoardModel->updateSquare(aLetter.Point);
                    if (m_pBoard->is3D()) cubeModel()->updateSquare(aLetter.Point);
                    m_pRackModel->placeLetter(aLetter.RackPos);
                    m_pMoves.last()->addLetter(aLetter);
                }
#if defined(Q_OS_LINUX) && defined(QT_DEBUG)
                else
                    qWarning() << "Incorrent number of data points " << sLetter;
#endif
            }
        } else //nLetters == 0
            for (int i = 0; i < aMsg["exchangecount"].toInt(); i++)
                exchangeLetter(aMsg["letter"+QString::number(i)].toInt());

        if (m_pRackModel->rackSize() == m_pMoves.last()->letterCount())
            m_pMoves.last()->setBonus( m_nScrabbleBonus, true );
    }

    // check move if !isLoading and !wcChallenge; nextPlayer is done in doNetworkAnswer()
    if ((m_eWordCheckMode != wcChallenge) && (!aMsg["IsLoading"].toBool())) {
        bool validMove = true; //local player has confirmed the move
        if (aMsg["Sender"] != m_pNetwork->localPlayerName())
            validMove = doCheckWords();
        emit onSend(network::nwAnswer, "group", validMove ? "Answer=true" : "Answer=false");
    } else {
        nextPlayer(aMsg["IsLoading"].toBool());
        emit onSend(network::nwRefresh, "", ""); //update move number in players tree
    }
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

void GamePlay::startPoll(GamePlay::PollType pollType)
{
    //TODO: gameplay/network: suspend game, stop clock, exclude kibitzes
    //init
    m_ePoll = pollType;
    m_lAnswer.clear();
    emit showPoll(true);
    QVariantList answers;
    for (int i=0; i<m_PlayersTreeModel->groupData(m_pNetwork->localPlayerName()).count(); i++)
        answers.append(0);
    emit answerChanged(answers);
}

void GamePlay::doNetworkAnswer(QVariantMap aMsg)
{
    // append answer
    m_lAnswer[aMsg["Sender"].toString()] = aMsg["Answer"].toString();

    const QStringList players = m_PlayersTreeModel->groupData(m_pNetwork->localPlayerName()).split(",");

    // feedback
    QVariantList lAnswers;
    int nAnswers = 0;
    for (int i = 0; i < players.count(); i++)
        if (m_lAnswer[players[i]].toString() == "true") {
            lAnswers.append(1);
            nAnswers++;
        }
        else if (m_lAnswer[players[i]].toString() == "false") {
            lAnswers.append(-1);
            nAnswers++;
        }
        else
            lAnswers.append(0);
    emit answerChanged(lAnswers);

    // evaluate answers
    if (nAnswers == players.count()) {
        bool result = true;
        const QStringList keys = m_lAnswer.keys();
        for (int i = 0; i < keys.count(); i++) {
            if (m_lAnswer.value(keys[i]) != "true")
                result = false;
        }
        if (result)
            switch(m_ePoll) {
            case ptNone: break;
            case ptNewGame:
                startNewGame(m_lGameConfig);
                emit onSend(network::nwRefresh, "", "");
                break;
            case ptNextPlayer:
                nextPlayer();
                emit onSend(network::nwRefresh, "", "");
                break;
            case ptChallenge: //checkword = true => challenge faile; stores the challenge result on the game server
                if (m_sChallengePlayer == m_pNetwork->localPlayerName()) {
                    QStringList msg;
                    msg.append("MoveNumber=" + QString::number(m_nCurrentMove));
                    msg.append("Result=true");
                    msg.append("ChallengePlayer=" + m_sChallengePlayer);
                    emit onSend(network::nwChallenge, "group", msg.join("\a"));
                }
                break;
            }
        else
            switch(m_ePoll) {
            case ptNone: break;
            case ptNextPlayer:
                resetPieces();
                nextPlayer();
                emit onSend(network::nwRefresh, "", "");
                break;
            case ptNewGame: break;
            case ptChallenge: //false = incorrect words, legitimately challenged; store on gameserver to have a consistent result on load
                if (m_sChallengePlayer == m_pNetwork->localPlayerName()) {
                    QStringList msg;
                    msg.append("MoveNumber=" + QString::number(m_nCurrentMove));
                    msg.append("Result=false");
                    msg.append("ChallengePlayer=" + m_sChallengePlayer);
                    emit onSend(network::nwChallenge, "group", msg.join("\a")); //stores the challenge result on the game server
                }
                break;
            }

        // end poll and close feedback in 5s
        m_ePoll = ptNone; // do not emit endPoll if m_bIsPoll is true, ie. started again
        QTimer::singleShot(5000, this, SLOT(endPoll())); // clear feedback after 5s
    }
}

void GamePlay::doInvite(QString sName)
{
    QStringList aData;
    aData.append("GameID=" + QString::number(m_PlayersTreeModel->playerData(m_pNetwork->localPlayerName()).gameID));
    aData.append("Language=" + m_pLocListModel->currentLang() );
    aData.append("Country=" + m_PlayersTreeModel->playerData(m_pNetwork->localPlayerName()).country);
    aData.append("Dictionary=" + m_pDictionary->fileName());
    emit onSend(network::nwInvite, sName, aData.join("\a"));
}

void GamePlay::doLeave()
{
    emit onSend(network::nwLeave,"group","");
}

void GamePlay::chat(QString aMessage)
{
    if (aMessage.startsWith("/shout ")) {
        aMessage.remove(0, 7); //QString::length("/shout ")
        emit onSend(network::nwChat,"all", "Chat="+aMessage);
    }
    else if (aMessage.startsWith("/say ")) {
        aMessage.remove(0, 5); //QString::length("/say ")
        emit onSend(network::nwChat,"group", "Chat="+aMessage);
    }
    else if (aMessage.startsWith("/whisper ")) {
        aMessage.remove(0, 9); //QString::length("/whisper ")
        QString aReceiver = aMessage.first(aMessage.indexOf(" "));
        aMessage.remove(0, aReceiver.length()+1);
        emit onSend(network::nwChat, aReceiver, "Chat="+aMessage);
    }
    else if (aMessage.startsWith("/kibitzes ")) {
        aMessage.remove(0, 10); //QString::length("/kibitzes ")
        emit onSend(network::nwChat, "kibitzes", "Chat="+aMessage);
    }
    else if (aMessage.startsWith("/refresh")) {
        emit onSend(network::nwRefresh,"","");
    }
    else if (aMessage.startsWith("/best")) {
        emit onSend(network::nwInformation, "", "Info=best");
    }
    else if (aMessage.startsWith("/finger")) {
        aMessage.remove(0, 8);
        aMessage.insert(0, "Info=finger\aName=");
        emit onSend(network::nwInformation, "", aMessage);
    }
    else if (aMessage.startsWith("/")) {
#if defined(Q_OS_LINUX) && defined(QT_DEBUG)
        qWarning() << "Unknown message type at " << aMessage;
#endif
    }
    else
        emit onSend(network::nwChat,"group", "Chat="+aMessage);
}

void GamePlay::doNetworkError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        addMessage(tr("The server has closed the connection.")); break;
    case QAbstractSocket::HostNotFoundError:
        addMessage(tr("The host was not found.")); break;
    case QAbstractSocket::ConnectionRefusedError:
        addMessage(tr("The connection was refused by the sever.")); break;
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
    else if (sInfo == "LoadingFinished") {
        addMessage(tr("Game has been loaded successfully."));
        //block next player action
        m_bIsRunning = !aMsg["IsGameEnd"].toBool();
        emit isRunningChanged();
        //start timer
        if (m_bIsRunning)
            doStartTimer();
        //update statusbar
        emit statInfoChanged();

        emit onSend(network::nwRefresh, "", "");
    }
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
        if (aPlayer.release.startsWith("3"))
            aPlayer.version = 3;
        else if (aPlayer.release.startsWith(""))
            aPlayer.version = 4;
        else
            aPlayer.version = 0;

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
        emit onSend(network::nwRefresh, "all", "");
    }
}

void GamePlay::syncCambioSecco()
{
    emit onSend(network::nwCambioSecco, "group", "");
}
