#include "gameplay.h"
#include "configpath.h"

#include <QMessageBox>
#include <QSettings>
#include <QQmlEngine> // localize()

GamePlay::GamePlay(QQmlEngine *engine)
{

    m_pTranslator = new QTranslator(this);
    m_pEngine = engine;

    m_bIsRunning = false;

    m_pBoard = new board();
    m_pRackModel = new rackmodel(this);
    m_pBoardModel = new boardmodel(this, m_pBoard);
    m_pCubeModel = new cubemodel(this, m_pBoard);
    m_pMsgModel = new msgmodel(this);
    m_pGameCourseModel = new gamecoursemodel(this);
    QObject::connect(m_pGameCourseModel, SIGNAL(onSelectedMoveChanged(int)),
                     this, SLOT(doSelectedMoveChanged(int)));

    // download manager
    m_pDownloadManager = new DownloadManager(this);
    QObject::connect(m_pDownloadManager, SIGNAL(onFinished(DlType,QString)),
                     this, SLOT(doDownloadFinished(DlType,QString)));
    QObject::connect(m_pDownloadManager, SIGNAL(onMessage(QString,QString)),
                     m_pMsgModel, SLOT(addMessage(QString,QString)));
    QObject::connect(m_pDownloadManager, SIGNAL(onProgress(int)),
                     this, SLOT(setComputeProgress(int)));

    // dictionary
    m_pDictionary = new dictionary(this);
    m_pDicListModel = new dictionarymodel(this);
    QObject::connect(m_pDictionary, SIGNAL(onLoadingFinished(QString)),
                     this, SLOT(doDictionaryLoadingFinished(QString)));
    QObject::connect(m_pDicListModel, SIGNAL(onClearDictionary()),
                     m_pDictionary, SLOT(doClear()));
    QObject::connect(m_pDicListModel, SIGNAL(onMessage(QString,QString)),
                     m_pMsgModel, SLOT(addMessage(QString,QString)));
    QObject::connect(m_pDictionary, SIGNAL(onProgress(int)),
                     this, SLOT(setComputeProgress(int)));
    QObject::connect(m_pDictionary, SIGNAL(onDownload(QString)),
                     this, SLOT(download(QString)));
    m_pDicListModel->updateList(); //update after message is connected

    //localization
    m_pLocListModel = new locList(this);
    QObject::connect(m_pLocListModel, SIGNAL(onMessage(QString,QString)),
                     m_pMsgModel, SLOT(addMessage(QString,QString)));
    m_pLocListModel->updateList();

    //compute
    m_pComputeMove = new computemove(this);
    QObject::connect(m_pComputeMove, SIGNAL(onComputeProgress(int)),
                     this, SLOT(setComputeProgress(int)));
    QObject::connect(m_pComputeMove, SIGNAL(onComputeResults(int)),
                     this, SLOT(setComputeResults(int)));

    m_bIsConnected = false;
    m_PlayersTreeModel = new playersTree();
    m_RemoteGamesModel = new remoteGamesModel(this);

    if (m_pDownloadManager->lastChecked() > 7)
        checkForUpdates();

    QSettings settings(config::ini(), QSettings::IniFormat);
    QString locFile = settings.value("General/110n", "").toString();
    if (!locFile.isEmpty()) {
        QFileInfo finfo(config::file(locFile));
        if (finfo.exists()) //loading a l10n-file that has been deleted triggers a download confirmation, but shouldn't on startup
            localize(locFile);
    }
/*    m_RemoteGamesProxy = new remoteGamesProxy(this);
    m_RemoteGamesProxy->setSourceModel(m_RemoteGamesModel);
    m_RemoteGamesProxy->setDynamicSortFilter(true);
//    m_RemoteGamesProxy->setSortRole(remoteGamesModel::LastAccessRole);
*/

#ifdef Q_OS_ANDROID
    loadGame(config::file("debug.ssg"), true); //silent
#endif
}

void GamePlay::checkForUpdates()
{
    download("Binaries/raw/main/Scrabble3D.conf");
}

void GamePlay::localize(const QString fileName)
{
    QFileInfo finfo(config::file(fileName));
    if (finfo.exists()) {
        qApp->removeTranslator(m_pTranslator);
        if ( m_pTranslator->load(fileName, config::file("")) ) {
            qApp->installTranslator(m_pTranslator);
            m_pEngine->retranslate();
            m_pLocListModel->loadingFinished(fileName);

            QSettings settings(config::ini(), QSettings::IniFormat);
            settings.setValue("General/110n", fileName);
        } else
            addMessage(tr("Failed to load localization %1.").arg(fileName));
    } else {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Scrabble3D");
        msgBox.setText( QObject::tr("Localization %1 is not locally available.").arg(fileName) );
        msgBox.setInformativeText( QObject::tr("Do you want to download it now?") );
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        if (msgBox.exec() == QMessageBox::Yes)
            download("Localizations/raw/main/" + fileName);
    };
}

//NOTE: gameplay: better signal/slot when dic has been loaded
void GamePlay::doDictionaryLoadingFinished(QString aFileName)
{
    m_pDicListModel->doLoadingFinished(aFileName);
    emit loadingFinished(m_pDicListModel->indexOf(aFileName));
    emit wordCountChanged(); //update word search spinbox max
}

void GamePlay::doDownloadFinished(DlType fileType, QString fileName)
{
    switch (fileType) {
    case DlType::dmConfig: {
        m_pDicListModel->updateList();
        m_pLocListModel->updateList();

        QStringList canUpdate;

        QFile rFile(config::conf());
        if ( rFile.open(QIODevice::ReadOnly) ) {
            QXmlStreamReader xmlReader;
            xmlReader.setDevice(&rFile);
            while (!xmlReader.atEnd()) {
                xmlReader.readNext();
                if (xmlReader.isStartElement() &&
                    (xmlReader.name().toString() == "version"))
                {
                    QString sRemoteVersion = xmlReader.readElementText();
                    if ( version::fromString( version::current() ) <
                        version::fromString( sRemoteVersion ) ) {
                        m_pMsgModel->addMessage( tr("Application: %1 < %2").arg(
                            version::current(), sRemoteVersion));
                        if (!InstFileName.isEmpty()) {
                            const QString sLink = QString("<a href=\"%1\">%1</a>").arg(server + "Binaries/raw/main/" + InstFileName);
                            m_pMsgModel->addMessage( tr("Please update from %1").arg(sLink) );
                        }
                    }
                }
            } //start
            rFile.close();
        }
        canUpdate << m_pLocListModel->canUpdate()
                  << m_pDicListModel->canUpdate();
        canUpdate.removeAll({}); //clear empty entries
        m_pDownloadManager->checkUpdates(canUpdate);
        break;
    }
    case DlType::dmDictionary: {
        m_pDicListModel->updateList();
        if (!fileName.isEmpty())
            loadDictionary(fileName, "");
        break;
    }
    case DlType::dmLocalization: {
        m_pLocListModel->updateList();
        if (!fileName.isEmpty())
            localize(fileName);
        break;
    }
    } //switch
}

void GamePlay::startNewGame(QVariantMap gameConfig)
{
    doSelectedMoveChanged(-1);
    m_lPlayerNames.clear();
    emit numberOfPlayersChanged(); //refreh names in case the number is equal
    m_lPlayerNames = gameConfig["PlayerNames"].toStringList();

    long nSeed = gameConfig["Seed"].toInt();
    if (gameConfig["RandomSequence"].toBool()) {
        if (nSeed == -1) nSeed = static_cast<int>( time(0) );
        std::mt19937 rng(nSeed);
        std::shuffle(m_lPlayerNames.begin(), m_lPlayerNames.end(), rng);
    }
    m_pMsgModel->playerNames(m_lPlayerNames); //to highlight player messages
    emit numberOfPlayersChanged();

    m_pBoard->initialize(gameConfig["FieldTypeArray"].toList(), gameConfig["Is3D"].toBool());
    m_pBoardModel->reset();
    if ( gameConfig["Is3D"].toBool() ) m_pCubeModel->updateSquare();
    emit boardSizeChanged();
    emit is3DChanged();
    m_nPlayerCount = m_lPlayerNames.count();
    m_nCurrentPlayer = 0;
    m_pRackModel->initialize( m_nPlayerCount, gameConfig["RackSize"].toInt() );

    m_lBag.clear();
    QString sChars;
    QVariantList aLetterList = gameConfig["LetterList"].toList();
    for (int i=0; i<aLetterList.count(); i+=3) {
        Letter aLetter;
        aLetter.What = aLetterList[i].toString();
        sChars += aLetter.What;
        aLetter.Value = aLetterList[i+1].toInt();
        for (int j=0; j<aLetterList[i+2].toInt(); j++)
            m_lBag.append(aLetter);
    }
    m_pDictionary->setLetterSet(sChars);

    const int nRandom = gameConfig["NumberOfRandoms"].toInt() | 0;
    if (nRandom > 0) {
        if (nSeed == -1) nSeed = time(0);
        QRandomGenerator rand(nSeed);
        const int nBagCount = m_lBag.count();
        for (int i = 0; i < nRandom; i++) {
            Letter aLetter = m_lBag[ rand.bounded(0, nBagCount) ];
            aLetter.IsRandom = true;
            m_lBag.append(aLetter);
        }
    }

    const int nJoker = gameConfig["NumberOfJokers"].toInt() | 0;
    for (int i = 0; i < nJoker; i++) {
        Letter aLetter;
        aLetter.IsJoker = true;
        aLetter.What = JokerChar;
        aLetter.Value = 0;
        m_lBag.append(aLetter);
    }
//    std::random_device rd;
    if (nSeed == -1) nSeed = time(0);
    std::mt19937 rng(nSeed);
    std::shuffle(m_lBag.begin(), m_lBag.end(), rng);

    for (uint nPlayerIndex = 0; nPlayerIndex < m_nPlayerCount; nPlayerIndex++) {

        for (int nRackIndex = 0; nRackIndex < m_pRackModel->rackSize(); nRackIndex++) {
            Letter aLetter = m_lBag[0];
            aLetter.State = LetterState::lsRack;
            aLetter.RackPos = nRackIndex;
            m_pRackModel->setLetter(aLetter, true, nPlayerIndex);
            m_lBag.removeAt(0);
        }
    }

    m_bCanJokerExchange = gameConfig["CanJokerExchange"].toBool();
    m_nGameEndBonus = gameConfig["GameEndBonus"].toInt();
    m_nNumberOfPasses = gameConfig["NumberOfPasses"].toInt() * m_nPlayerCount;
    m_nJokerPenalty = gameConfig["JokerPenalty"].toInt();
    m_bChangeIsPass = gameConfig["ChangeIsPass"].toBool();
    m_eTimeControlType = static_cast<TimeControlType>(gameConfig["TimeControlType"].toInt());
    m_nTimeControlValue = gameConfig["TimeControlValue"].toInt();
    m_nLimitedExchange = gameConfig["LimitedExchange"].toInt();
    const bool bCambioSecco = gameConfig["CambioSecco"].toBool();
    m_bCanCambioSecco.clear();
    for (uint nPlayerIndex = 0; nPlayerIndex < m_nPlayerCount; nPlayerIndex++)
        m_bCanCambioSecco.append( bCambioSecco );
    emit cambioSeccoChanged();
    m_bWhatif = gameConfig["Whatif"].toBool();
    m_bAdd = gameConfig["Add"].toBool();
    m_bSubstract = gameConfig["Substract"].toBool();
    m_bTimeGameLost = gameConfig["TimeGameLost"].toBool();
    m_nTimePenaltyValue = gameConfig["TimePenaltyValue"].toInt();
    m_nTimePenaltyCount.clear();
    for (uint i = 0; i < m_nPlayerCount; i++)
        m_nTimePenaltyCount.append(!m_bTimeGameLost ? gameConfig["TimePenaltyCount"].toInt() : 0);
    m_nScrabbleBonus = gameConfig["ScrabbleBonus"].toInt();
    m_eWordCheckMode = static_cast<WordCheckMode>(gameConfig["WordCheckType"].toInt());
    m_nWordCheckPeriod = gameConfig["WordCheckPeriod"].toInt();
    m_nWordCheckPenalty = gameConfig["WordCheckPenalty"].toInt();
    m_nWordCheckBonus = gameConfig["WordCheckBonus"].toInt();
    m_bIsCLABBERS = gameConfig["IsCLABBERS"].toBool();
    m_bIsRunning = true;
    m_bIsAccepted = true;
    m_bIsInitialized = true;
    m_bIsFirstMove = true;
    m_nPasses = 0;

    m_pMoves.clear();
    m_pMoves.append(sharedMove(new move(m_bIsFirstMove, m_pBoard)));
    m_nCurrentMove = 0;
    m_nCurrentPlayer = 0;

    if (m_bIsConnected)
        m_pRackModel->setLocalPlayer(m_lPlayerNames.indexOf(m_pNetwork->localPlayerName()));
    else
        m_pRackModel->setLocalPlayer(m_nCurrentPlayer);
    m_pRackModel->setActivePlayer(m_nCurrentPlayer);

    if (m_pBoard->is3D()) {
        setActiveDimension(dmAbscissa);
        setActivePosition(m_pBoard->getBoardSize() / 2);
    }

    m_pGameCourseModel->clear();
    doStartTimer();
    emit currentMoveChanged();
    emit currentPlayerChanged();
    emit isLocalPlayerChanged();
    emit isRunningChanged();
    emit isInitializedChanged();
    emit statInfoChanged();
    emit statTotalChanged();
    m_bIsComputing = false;
    emit isComputingChanged();

    if ( gameConfig["IsLoading"].toBool() )
        m_pMsgModel->addMessage( tr("New game has been started successfully :-).") );

    //compute and place if player is "Computer"
    if (m_lPlayerNames[m_nCurrentPlayer] == "Computer")
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        doComputeMove();
    }
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
        (!m_bIsConnected) && //word check has been done before in network mode
        (!m_pMoves.last()->PlacedWord.isEmpty()))
    {
        if ( !doCheckWords() )
            return;
    }

    //result message
    if ((!bIsLoading) &&
        (!m_pMoves.last()->PlacedWord.isEmpty()))
        //: <Player 1> places <Foo> <<and>> <Far, Faz> and receives 42 points. ("and" is only added if the connected words = %2 are not empty)
        m_pMsgModel->addMessage(tr("%1 places %2 %3 and receives %4 points.")
                                    .arg( m_lPlayerNames[m_nCurrentPlayer] )
                                    .arg( m_pMoves.last()->PlacedWord )
                                    .arg( !m_pMoves.last()->ConnectedWords.isEmpty() ? tr("and") + " " + m_pMoves.last()->ConnectedWords : "" )
                                    .arg( QString::number(m_pMoves.last()->Value()) ),
                                m_lPlayerNames.at(m_nCurrentPlayer));

    // increase number of passes
    const int nExchange = m_pRackModel->exchangeNumber();
    bool isPass = m_pMoves.last()->PlacedWord.isEmpty();
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

    // exchange message
    const QString numerals[] = { tr("one"), tr("two"), tr("three"), tr("four"),
                                 tr("five"), tr("six"), tr("seven")};
    if (nExchange > 0)
        m_pMsgModel->addMessage(tr("%1 exchanges %2 piece(s).", "%1 = player name, %2 = one, two, three..., 8, 9...", nExchange)
                                    .arg( m_lPlayerNames[m_nCurrentPlayer] )
                                    .arg( nExchange < 8 ? numerals[nExchange-1] : QString::number(nExchange) ),
                                m_lPlayerNames[m_nCurrentPlayer]);

    //lMoves is zero before first move
    if (m_pMoves.last()->Value() > 0)
        m_bIsFirstMove = false;

    //update game course
    m_pGameCourseModel->addMove(replaceAllLetters(m_pMoves.last()->PlacedWord),
                                replaceAllLetters(m_pMoves.last()->ConnectedWords),
                                m_nCurrentPlayer,
                                m_pMoves.last()->Value(),
                                m_nComputeResults > 0 ? m_pComputeMove->result(0)->Value() : 0,
                                m_pMoves.last()->isScrabble(),
                                m_nMoveTime,
                                m_pRackModel->getInitialRack());

    //check game end
    if (!bIsLoading && //when loading bIsRunning is saved and restored
        ((m_pRackModel->isRackEmpty() && //all letters placed and...
          (m_lBag.count()==0)) || //... no letter left in bag or...
         (m_nPasses == m_nNumberOfPasses) )) //max numbers of passes reached
    {
        doGameEnd();
    }

    //updates the UI
    m_pBoard->placeLetters();
    m_pBoardModel->updateAllSquares();

    if (!m_bIsRunning) //game has ended
        return;

    //new pieces for the placed or marked to exchange
    doUpdateRack();

    //new empty move
    m_pMoves.append( sharedMove(new move(m_bIsFirstMove, m_pBoard)) );

    //increase currentmove and currentplayer
    m_nCurrentMove++;
    m_nCurrentPlayer = m_nCurrentMove % m_nPlayerCount;
    if (!bIsLoading) {
        if (m_bIsConnected)
            m_pRackModel->setLocalPlayer(m_lPlayerNames.indexOf(m_pNetwork->localPlayerName()));
        else
            m_pRackModel->setLocalPlayer(m_nCurrentPlayer);
        m_pRackModel->setActivePlayer(m_nCurrentPlayer);
        emit currentPlayerChanged();
        emit isLocalPlayerChanged();
    }
    emit currentMoveChanged();

    if (m_lBag.count() < m_pRackModel->rackSize())
        for (int i : m_bCanCambioSecco) m_bCanCambioSecco[i] = false;
    emit cambioSeccoChanged();

    //start the timer
    doStartTimer(!bIsLoading);
    emit statInfoChanged();
    emit statTotalChanged();

    if ( m_bIsConnected && // only in network mode
        (m_eWordCheckMode == wcChallenge) && // if wordcheck mode is active
        !m_bIsFirstMove && // not in case of the first move
        !m_pMoves[m_pMoves.count()-2]->PlacedWord.isEmpty() ) // if the last move was not a pass
    {
        m_bIsChallenge = true;
        emit isChallengeChanged();
        QTimer::singleShot(m_nWordCheckPeriod * 1000, this, SLOT(endChallenge()));
    }

    if (bIsLoading)
        return; //do not update the board when loading a game

    //allow next player action
    m_bIsAccepted = true;
    m_pMsgModel->addMessage(tr("It's %1's turn.").arg(
                                m_lPlayerNames[m_nCurrentPlayer]),
                            m_lPlayerNames[m_nCurrentPlayer]);
    emit isAcceptedChanged();
    emit lastErrorChanged();

#ifdef Q_OS_ANDROID //store moves on Android to load when coming back from sleep
    if (!m_bIsConnected)
        saveGame(config::file("debug.ssg"), true); //silent
#endif

    //clear computemove
    m_pComputeMove->clear();

    //compute and place if player is "Computer"
    if (m_lPlayerNames[m_nCurrentPlayer] == "Computer")
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        doComputeMove();
    }
}

void GamePlay::doGameEnd()
{
    // if all pieces are placed, ie. not ending by passes, the end game bonus is
    // given to the player who ended the game; which is last()-1 here
    // first call of setGameEndBonus() will also find the winner, after adding the game end bonus
    if ( (m_nPasses < m_nNumberOfPasses) && (m_nGameEndBonus > 0) ) {
        m_pGameCourseModel->addBonus(m_nCurrentPlayer, m_nGameEndBonus);
        m_pMsgModel->addMessage(tr("%1 has ended the game and receives a bonus of %2 points.")
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

    for (uint nPlayer = 0; nPlayer < m_nPlayerCount; nPlayer++) {
        if ((m_eTimeControlType == tcPerGame) && //time control per game
            (m_pGameCourseModel->timeTotalPerPlayer(nPlayer) >= int(m_nTimeControlValue)) &&
            (m_bTimeGameLost))
            m_pGameCourseModel->zerototal(nPlayer);
    }

    QList<int> nWinner;
    m_pGameCourseModel->getWinner(nWinner);
    QString sWinner;
    if (nWinner.count() > 1) {
        sWinner = tr("Draw between")+" ";
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
        aData.append("NumberOfPlayers=" + QString::number(m_nPlayerCount));
        for (uint i=0; i<m_nPlayerCount; i++) {
            aData.append("Player"+QString::number(i)+"Name="+m_lPlayerNames[i]);
            nScore = m_pGameCourseModel->getScore(i);
            aData.append("Player"+QString::number(i)+"Value="+QString::number(nScore));
            if (m_lPlayerNames[i] == m_pNetwork->localPlayerName())
                aData.append("MyResult="+QString::number(nScore));
        }
        emit onSend(network::nwGameResult, "group", aData.join("\a"));
    }

    m_bIsRunning = false;
    emit isRunningChanged();
}

// called from nextPlayer() and cambioSecco()
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
            rackLetter.State = LetterState::lsBag;
            rackLetter.RackPos = 0;
            rackLetter.IsExchange = false;
            //FIXME: gameplay: do not append exchanged letters but randomly insert
            m_lBag.append(rackLetter);
            m_pRackModel->placeLetter(i, false); //set empty
            Letter aLetter = m_lBag[0];
            aLetter.State = LetterState::lsRack;
            aLetter.RackPos = i;
            m_pRackModel->setLetter(aLetter);
            m_lBag.removeAt(0);
        }
    }
}

bool GamePlay::doCheckWords(const bool bPrevious)
{
    const QString placed = bPrevious ? m_pMoves[m_pMoves.count()-2]->PlacedWord
                                     : m_pMoves.last()->PlacedWord;
    const QString connected = bPrevious ? m_pMoves[m_pMoves.count()-2]->ConnectedWords
                                        : m_pMoves.last()->ConnectedWords;

    QStringList unknownWords = m_pDictionary->checkWords(placed,connected);
    if (!unknownWords.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Scrabble3D");
        QString msgText = tr("Unknown word(s):")+"\n%1";
        msgBox.setText(msgText.arg(replaceAllLetters(unknownWords.join(","))));
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

    if (doRun) { //do not run when loading
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
            m_pMoves.last()->setLastError(tr("No placing possible after tiles have been marked for exchange."));
            emit lastErrorChanged();
            return false;
        }

    return true;
}

void GamePlay::exchangeLetter(const unsigned int rackIndex)
{
    //bag needs to have letters left
    if (m_lBag.count() < m_pRackModel->rackSize()) {
        m_pMoves.last()->setLastError(tr("Not enough tiles left."));
        emit lastErrorChanged();
        return;
    }
    //no letter must have been placed
    const Letter aLetter = m_pRackModel->getLetter(rackIndex);
    if (!aLetter.IsExchange) {
        for (int i=0; i<m_pRackModel->rackSize(); i++)
            if (m_pRackModel->getLetter(i).IsEmpty()) {
                m_pMoves.last()->setLastError(tr("No tile exchange possible after placing."));
                emit lastErrorChanged();
                return;
            }
    }
    //do toggle
    m_pRackModel->toggleExchangeFlag(rackIndex);
}

void GamePlay::resetPieces()
{
    m_pMoves.last()->clear();
    Letter aLetter;
    for (int i=0; i<m_pBoard->getFieldSize(); i++)
    {
        aLetter = m_pBoard->getLetter(i);
        if (aLetter.State == LetterState::lsBoard)
            removeLetter(aLetter);
    }
}

void GamePlay::rollbackLastMove()
{
    const int nLastPlayer = (m_nCurrentMove-1) % m_nPlayerCount;
    m_pMoves[m_pMoves.count()-2]->clear();
    m_pGameCourseModel->clearLastMove();

    Letter boardLetter;
    Letter rackLetter;
    for (int i=0; i<m_pBoard->getFieldSize(); i++) {
        boardLetter = m_pBoard->getLetter(i);
        if ((boardLetter.State == LetterState::lsPlaced) &&
            (static_cast<int>(boardLetter.When) == m_nCurrentMove-1)) {
            int aWhere = boardLetter.Where;
            Point3D aPoint = boardLetter.Point;

            boardLetter.State = LetterState::lsRack;
            boardLetter.When = 0;
            boardLetter.Who = 0;
            boardLetter.Where = 0;
            if (boardLetter.IsJoker) boardLetter.What = JokerChar;
            boardLetter.Point = Point3D(0,0,0);

            rackLetter = m_pRackModel->getLetter(boardLetter.RackPos, nLastPlayer);
            rackLetter.State = LetterState::lsBag;
            rackLetter.RackPos = 0;
            m_lBag.insert(0, rackLetter);

            m_pRackModel->setLetter(boardLetter, false, nLastPlayer);

            m_pBoard->removeLetter(aWhere);
            m_pBoardModel->updateSquare(aPoint);
            if (m_pBoard->is3D()) cubeModel()->updateSquare(aPoint);//aIndex);

            //NOTE: gameplay: rollbackLastMove() fails with multiple start fields
            if (m_pBoard->getFieldtype(i) == FieldType::ftStart) { // make current move the first
                m_pMoves.last()->setFirstMove(true);
                m_bIsFirstMove = true;
            }
        }
    }
}

void GamePlay::timerEvent(QTimerEvent *event)
{
    if ((m_eTimeControlType == tcPerGame) && //time control per game
        (m_nMoveTime >= m_nTimeLeft) &&      //no time left
        (m_nTimePenaltyCount[m_nCurrentPlayer] > 0) && //buying time is possible
        (m_pGameCourseModel->getScore(m_nCurrentPlayer) > int(m_nTimePenaltyValue))) { //player has enough points to buy time

        m_nMoveTime -= 60; //deduct 60s from current move time
        m_nTimePenaltyCount[m_nCurrentPlayer]--; //decrease number of possible additions
        m_pGameCourseModel->addBonus(m_nCurrentPlayer, -m_nTimePenaltyValue); //add penalty
        m_pMsgModel->addMessage(tr("Player %1 run out of time and one minute was added at a penalty of %2 points (%3 remaining).")
                                .arg(m_lPlayerNames.at(m_nCurrentPlayer))
                                .arg(m_nTimePenaltyValue)
                                .arg(m_nTimePenaltyCount[m_nCurrentPlayer]));
        if (m_nStatInfoType == 1)
            emit statInfoChanged();
    }

    if ( m_pRackModel->isLocalIsActive() &&
         (m_eTimeControlType != tcNoLimit) && (m_nMoveTime >= m_nTimeLeft))
        nextPlayer();

    m_nMoveTime++;

    if (m_nStatInfoType == 2)
        emit statInfoChanged();

    Q_UNUSED(event);
}

//called directly or via syncCambioSecco() => onCambioSecco() in network mode
void GamePlay::doCambioSecco()
{
    m_pMsgModel->addMessage(tr("%1 uses the Cambio Secco to exchange all letters.").arg(
                                m_lPlayerNames[m_nCurrentPlayer]),
                            m_lPlayerNames[m_nCurrentPlayer]);

    resetPieces(); //remove if pieces are on the board
    m_pComputeMove->clear(); //reset computed moves
    for (int i = 0; i < m_pRackModel->rackSize(); i++)
        m_pRackModel->toggleExchangeFlag(i);
    doUpdateRack();
    m_pRackModel->setInitialRack(); //update rack for game history; done in first place with setActivePlayer()

    m_bCanCambioSecco[m_nCurrentPlayer] = false;
    emit cambioSeccoChanged();
}

QString GamePlay::getStatInfoType(const int aType)
{
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

        if (m_bIsRunning) {
            if (m_eTimeControlType == tcNoLimit)
                aTime = aTime.addSecs(m_nMoveTime);
            else
                aTime = aTime.addSecs(m_nTimeLeft - m_nMoveTime);
        }

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
    QString sResult = tr("Letter:") + " " + getStatInfoType(0) + "\n";
    sResult += tr("Game score:") + " " + getStatInfoType(1) + "\n";
    if (m_eTimeControlType == tcNoLimit)
        sResult += tr("Time spend:" );
    else
        sResult += tr("Time left:" );
    sResult += " " + getStatInfoType(2);

    return sResult;
}

void GamePlay::dropLetter(const uint rackIndex, const uint boardIndex)
{
    Letter aLetter = m_pRackModel->getLetter(rackIndex);

    aLetter.State = LetterState::lsBoard;
    aLetter.When = m_bIsHistory ? m_nMoveHistory : m_nCurrentMove;
    aLetter.Who = m_nCurrentPlayer;
    aLetter.Point = m_pBoard->pos3D(boardIndex);
    aLetter.Where = m_pBoard->pointToWhere(aLetter.Point);
    m_pBoard->setLetter(aLetter);
    m_pBoardModel->updateSquare(aLetter.Point);
    if (m_pBoard->is3D()) cubeModel()->updateSquare(aLetter.Point);
    m_pRackModel->placeLetter(rackIndex);

    if (!m_bIsHistory) {
        m_bIsAccepted = m_pMoves.last()->addLetter(aLetter);
        if (m_bIsAccepted) //FIXME: history bonus not done correctly due to last()
        {
            if (m_pRackModel->rackSize() == m_pMoves.last()->letterCount())
                m_pMoves.last()->setBonus( m_nScrabbleBonus, true );
        }
    }

    emit placedValueChanged();
    emit isAcceptedChanged();
    emit lastErrorChanged();
}

void GamePlay::dropLetterRack(const uint fromIndex, const uint toIndex)
{
    m_pRackModel->move(fromIndex, toIndex);
}

void GamePlay::jokerLetter(const uint boardIndex, const QString aWhat)
{
    int z = m_pBoard->pointToWhere( m_pBoard->pos3D(boardIndex) );
    m_pBoard->setJokerLetter(z, aWhat);
    if (!m_bIsHistory)
        m_pMoves.last()->setJokerLetter(z, aWhat);
    m_pMoves.last()->checkMove(); //re-check to update placeword
    Letter aLetter = m_pBoard->getLetter(z);
    m_pBoardModel->updateSquare(aLetter.Point);
}

int GamePlay::removeLetter(const uint boardIndex)
{
    int z = m_pBoard->pointToWhere( m_pBoard->pos3D(boardIndex) );
    Letter aLetter = m_pBoard->getLetter(z);
    removeLetter(aLetter);
    return aLetter.RackPos;
}

int GamePlay::removeLetter(Letter aLetter)
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

        m_pRackModel->setLetter(aLetter);

        m_bIsAccepted = m_pMoves.last()->deleteLetter(z); // deleteLetter() clears the bonus too

        emit placedValueChanged();
        emit isAcceptedChanged();
        emit lastErrorChanged();
    }
    return aLetter.RackPos;
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
        emit activePositionChanged(); //TODO: gameplay: consolidate activePosition & activeDimension
    }
}

int GamePlay::getActivePosition()
{
    return m_pBoard->getActivePosition();
}

QString GamePlay::getMeaningAt(const int index)
{
    QString sWords = m_pBoard->getWordsAt(index);

    bool bPlaced = false;
    for (int i=0; i<m_pRackModel->rackSize(); i++)
        if (m_pRackModel->getLetter(i).IsEmpty())
            bPlaced = true;

    if ( m_bIsHistory || // browsing through game course
        !m_bIsRunning || // game has ended
        !m_pRackModel->isLocalIsActive() || //local player is on move
        !bPlaced) // nothing has been placed //TODO: gameplay: getMeaningAt() fails at game end
    {
        sWords = m_pDictionary->getMeanings(sWords);
        sWords = replaceAllLetters(sWords);
    }
    return sWords;
}

void GamePlay::doSelectedMoveChanged(int move)
{
    m_nMoveHistory = move;
    QList<Letter> aLetters;

    if (m_nMoveHistory == -1) {
        m_bIsHistory = false; emit isHistoryChanged();
        // restore board
        if (m_pTempBoard != nullptr)
            m_pBoard->initialize(m_pTempBoard);
        delete m_pTempBoard;
        m_pTempBoard = nullptr;
        // restore rack
        if ( !m_pTempLetters.isEmpty() ) {
            aLetters = m_pTempLetters;
            m_pTempLetters.clear();
        }
        m_nCurrentPlayer = m_nCurrentMove % m_nPlayerCount;
        // reset move flags and 3d position
        if ( m_pMoves.count() > 0 ) {
            m_bIsFirstMove = m_pMoves.last()->isFirstMove();
            if (m_pBoard->is3D()) {
                setActivePosition(m_pMoves.last()->activePosition());
                emit activePositionChanged();
                setActiveDimension(m_pMoves.last()->activeDimension()); //move's dimension is also used for position at col/row
                emit activeDimensionChanged();
            }
        }
    } else {
        resetPieces(); //clear last() move
        m_bIsHistory = true; emit isHistoryChanged();
        // store board and rack
        if (m_pTempBoard == nullptr) {
            m_pTempBoard = new board();
            m_pTempBoard->initialize(m_pBoard);
            m_pTempLetters.clear();
            for (int i=0; i<m_pRackModel->rackSize(); i++)
                m_pTempLetters.append( m_pRackModel->getLetter(i) );
        }
        // setup board and rack resp. selected move
        m_pBoard->initialize(m_pTempBoard, m_nMoveHistory-1);
        aLetters = m_pGameCourseModel->getRack(m_nMoveHistory-1);
        m_bIsFirstMove = m_pMoves[m_nMoveHistory-1]->isFirstMove();
        m_nCurrentPlayer = (m_nMoveHistory-1) % m_nPlayerCount;
        if (m_pBoard->is3D()) {
            setActivePosition(m_pMoves[m_nMoveHistory-1]->activePosition());
            setActiveDimension(m_pMoves[m_nMoveHistory-1]->activeDimension()); //move's dimension is also used for position at col/row
        }
    }
    emit currentPlayerChanged();
    emit currentMoveChanged();

    // resets the 2D and 3D board models
    m_pBoardModel->updateAllSquares();
    if (m_pBoard->is3D())
        m_pCubeModel->updateSquare(m_pCubeModel->WhatRole);

    // reset rack
    for (int i = 0; i < aLetters.count(); i++)
        m_pRackModel->setLetter(aLetters[i]);

    // drop placed letters
    if (m_nMoveHistory > -1) {
        Letter aLetter;
        for (int i = 0; i < m_pMoves[m_nMoveHistory-1]->letterCount(); i++) {
            aLetter = m_pMoves[m_nMoveHistory-1]->getLetter(i);
            dropLetter(aLetter.RackPos, m_pBoard->pointToPlane(aLetter.Point));
            if (aLetter.IsJoker) {
                m_pBoard->setJokerLetter(aLetter.Where, aLetter.What);
                m_pBoardModel->updateSquare(aLetter.Point);
            }
        }
    }
    m_pComputeMove->clear(); // clear computed moves; allow to recalculate
}

void GamePlay::computeMove()
{
    resetPieces(); //return previously placed pieces
    m_bIsComputing = true;  emit isComputingChanged();
    m_pComputeMove->run( m_bIsFirstMove, m_pBoard, m_pRackModel, m_pDictionary );
    m_bIsComputing = false; emit isComputingChanged();
    //feedback
    if (m_bIsHistory || (m_lPlayerNames[m_nCurrentPlayer] != "Computer"))
        addMessage((m_nComputeResults == 0)
            ? tr("No word found")
            : tr("Out of %1 possible words found the best scores %2 points.")
                 .arg(m_nComputeResults)
                 .arg(m_pComputeMove->result(0)->Value()));
}

void GamePlay::doComputeMove()
{
    computeMove();
    if ((m_nComputeResults == 0) &&
        (m_lBag.count() >= m_pRackModel->rackSize()))
        m_pComputeMove->markLettersForExchange(m_pRackModel);
    else
        placeBestMove(1);
    nextPlayer();
}

void GamePlay::placeBestMove(const int index)
{
    //clear board
    resetPieces();

    if ( (index < 1) || (index > m_nComputeResults) )
        return;

    Letter aLetter;
    sharedMove aMove = m_pComputeMove->result(index-1);

    if (m_pBoard->is3D()) {
        setActivePosition(aMove->activePosition());
        setActiveDimension(aMove->activeDimension()); //move's dimension is also used for position at col/row
    }

    for (int j=0; j<aMove->letterCount(); j++) {
        aLetter = aMove->getLetter(j);
        const int boardIndex = m_pBoard->pointToPlane(aLetter.Point);
        dropLetter(aLetter.RackPos, boardIndex);
        if (aLetter.IsJoker)
            jokerLetter(boardIndex, aLetter.What);
    }
}

void GamePlay::setComputeProgress(const int aProgress)
{
    if (m_nProgress != aProgress) {
        m_nProgress = aProgress;
        emit computeProgressChanged();
    }
}

void GamePlay::saveConfig(QString fileName, QVariantMap configData)
{
    if (fileName.isEmpty()) //new game
        fileName = config::ini();
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
    if (fileName.isEmpty()) //new game
        fileName = config::ini();
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

void GamePlay::saveGame(QString fileName, const bool bSilent)
{
#ifndef Q_OS_ANDROID
    fileName = QUrl(fileName).toLocalFile(); //android reports content:// while it's file:// on desktop
#endif

    QSettings settings(fileName, QSettings::IniFormat);

    //clear previous values
    settings.clear();

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
        QStringList aList;
        for (uint i = 0; i < m_bCanCambioSecco.count(); i++)
            aList.append(QString::number(m_bCanCambioSecco[i]));
        settings.setValue("CambioSecco", aList.join(","));
        settings.setValue("Whatif", m_bWhatif);
        settings.setValue("Add", m_bAdd);
        settings.setValue("Substract", m_bSubstract);
        settings.setValue("TimePenaltyValue", m_nTimePenaltyValue);
        aList.clear();
        for (uint i = 0; i < m_nPlayerCount; i++)
            aList.append(QString::number(m_nTimePenaltyCount[i]));
        settings.setValue("TimePenaltyCount", aList.join(","));
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
        for (int i = 0; i < m_pMoves.count(); i++) {
            settings.setArrayIndex(i);
            //move
            settings.beginWriteArray("move");
            for (int j = 0; j < m_pMoves[i]->letterCount(); j++) {
                settings.setArrayIndex(j);
                settings.setValue("letter", m_pMoves[i]->getLetter(j).toVariant());
            }
            settings.endArray();
            //rack
            QList<Letter> aLetters;
            if (i < m_pMoves.count()-1) { //current move has no history yet
                aLetters = m_pGameCourseModel->getRack(i);
                settings.beginWriteArray("rack");
                for (int j = 0; j < aLetters.count(); j++) {
                    settings.setArrayIndex(j);
                    settings.setValue("letter", aLetters[j].toVariant());
                }
                settings.endArray();
            }
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
        settings.setValue("letterset",m_pDictionary->getLetterSet());
    }
    settings.endGroup();

    if (!bSilent)
        m_pMsgModel->addMessage(tr("Game saved to %1.").arg(fileName));
}

void GamePlay::loadGame(QString fileName, const bool bSilent)
{
#ifndef Q_OS_ANDROID
    fileName = QUrl(fileName).toLocalFile(); //android reports content:// while desktop uses file://
#endif

    QFile aFile;
    if (!aFile.exists(fileName))
        return;

    if (m_bIsRunning && (m_nTimerID > -1)) {
        killTimer(m_nTimerID);
        m_nTimerID = -1;
    }

    QSettings settings(fileName, QSettings::IniFormat);
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
        QStringList aList = settings.value("TimePenaltyCount").toString().split(",");
#if defined(Q_OS_LINUX) && defined(QT_DEBUG)
        if (aList.count() != m_nPlayerCount)
            qWarning() << "Cambio Secco count doesn't match player count!";
#endif
        m_bCanCambioSecco.clear();
        for (int i = 0; i < aList.count(); i++)
            m_bCanCambioSecco.append(aList[i].toInt());
        m_bWhatif = settings.value("/Whatif").toBool();
        m_bAdd = settings.value("Add").toBool();
        m_bSubstract = settings.value("Substract").toBool();
        m_nTimePenaltyValue = settings.value("TimePenaltyValue").toUInt();
        aList.clear();
        aList = settings.value("TimePenaltyCount").toString().split(",");
#if defined(Q_OS_LINUX) && defined(QT_DEBUG)
        if (aList.count() != m_nPlayerCount)
            qWarning() << "Penalty count doesn't match player count!";
#endif
        m_nTimePenaltyCount.clear();
        for (int i = 0; i < aList.count(); i++)
            m_nTimePenaltyCount.append(aList[i].toUInt());
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
        m_pBoardModel->reset();
        if (bIs3D) m_pCubeModel->updateSquare();
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

        m_pMoves.clear();
        //initialize game
        m_pMoves.append(sharedMove(new move(m_bIsFirstMove, m_pBoard)));

        Letter aLetter;
        m_nMoveTime = 0; //zero at beginning
        for (int i = 0; i < nMoves; i++) {
            settings.setArrayIndex(i);
            m_nMoveTime = settings.value("time").toInt();
            //update move
            int nLetters = settings.beginReadArray("move");
            for (int j=0; j < nLetters; j++) {
                settings.setArrayIndex(j);
                aLetter.fromString(settings.value("letter").toStringList());
                m_pBoard->setLetter(aLetter);
                m_bIsAccepted = m_pMoves.last()->addLetter(aLetter);
            }
            if (nLetters == m_pRackModel->rackSize())
                m_pMoves.last()->setBonus( m_nScrabbleBonus, true ); //while playing done in dropLetter()
            settings.endArray();
            //update rack for gamecourse browsing
            QList<Letter> aLetters;
            nLetters = settings.beginReadArray("rack");
            for (int j=0; j < nLetters; j++) {
                settings.setArrayIndex(j);
                aLetter.fromString(settings.value("letter").toStringList());
                aLetters.append(aLetter);
            }
            settings.endArray();
            if (i < nMoves-1) //initial rack of very last move is stored in setLocalPlayer()
                m_pRackModel->setInitialRack(aLetters);

            if (i < nMoves-1)
                nextPlayer(true); //bIsLoading = true; last move when game has ended follows below
            else
                m_pBoardModel->updateAllSquares(); //refresh for currently placed pieces
        }
    }
    settings.endArray();

    if (settings.value("game/IsRunning").toBool() == false) {
        if (m_pMoves.last()->PlacedWord.isEmpty())
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
        cubeModel()->updateSquare(); //dropLetter() updates single cell, reset does begin/endResetModel()
        setActiveDimension(settings.value("ActiveDimension").toInt());
        emit activeDimensionChanged();
        setActivePosition(settings.value("ActivePosition").toInt());
        emit activePositionChanged();
    }

    settings.beginGroup("extra");
    {
        m_pDictionary->setLetterSet( settings.value("letterset").toString() );
    }
    settings.endGroup();

    doSelectedMoveChanged(-1);
    if (m_bIsConnected)
        m_pRackModel->setLocalPlayer(m_lPlayerNames.indexOf(m_pNetwork->localPlayerName()));
    else
        m_pRackModel->setLocalPlayer(m_nCurrentPlayer);
    m_pRackModel->setActivePlayer(m_nCurrentPlayer);

    emit numberOfPlayersChanged();
    emit currentPlayerChanged();
    emit isLocalPlayerChanged();
    emit isAcceptedChanged();
    emit lastErrorChanged();
    emit isRunningChanged();
    m_bIsInitialized = true;
    emit isInitializedChanged();
    m_bIsComputing = false;
    emit isComputingChanged();
    emit statTotalChanged();

    if ((m_eTimeControlType != tcNoLimit) && (m_nTimeLeft == 0))
        nextPlayer();
    else
        m_nTimerID = startTimer(1000);

    if (!bSilent)
        m_pMsgModel->addMessage(tr("Successfully loaded game from %1.").arg(fileName));
}
