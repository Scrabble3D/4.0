#include "gameplay.h"
#include "defines.h"

#include <algorithm>
#include <random>
#include <QtMath>
#include <QMessageBox>
#include <QSettings>

#include <QDebug>

GamePlay::GamePlay(QObject *parent)
{
    Q_UNUSED(parent);

    m_bIsRunning = false;

    //TODO: download: check all files for updates
    m_pDownloadManager = new DownloadManager(this);
    if (!QFileInfo::exists(dictionaryPath().path() + "/Scrabble3D.conf"))
        download("Binaries/raw/main/Scrabble3D.conf.zip");

    m_pBoard = new board();
    m_pRackModel = new rackmodel(this);
    m_pBoardModel = new boardmodel(this, m_pBoard);
    m_pCubeModel = new cubemodel(this, m_pBoard);
    m_pMsgModel = new msgmodel(this);
    m_pGameCourseModel = new gamecoursemodel(this);
    m_pDicListModel = new dicList(this);
    m_pComputeMove = new computemove(this, m_pBoard, m_pRackModel, m_pDicListModel->dictionary);
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
                             bool RandomSequence)
{
    m_lPlayerNames.clear();
    emit numberOfPlayersChanged(); //refreh names in case the number is equal
    m_lPlayerNames = PlayerNames;
    if (RandomSequence) {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::shuffle(m_lPlayerNames.begin(), m_lPlayerNames.end(), rng);
    }
    emit numberOfPlayersChanged();

    m_pBoard->initialize(FieldTypeArray, is3D);
    m_pBoardModel->reset();
    if (is3D) m_pCubeModel->reset();
    emit boardSizeChanged();
    emit is3DChanged();
    m_nPlayerCount = m_lPlayerNames.count();
    m_nCurrentPlayer = 0;
    m_pRackModel->initialize(m_nPlayerCount, RackSize);
    //TODO: newgame: letters per players
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
    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(m_lBag.begin(), m_lBag.end(), rng);

    for (int nPlayerIndex = 0; nPlayerIndex < m_nPlayerCount; nPlayerIndex++)
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

    m_nCurrentMove = -1; //inc'ed in first nextplayer()
    m_bIsRunning = true;
    m_bIsFirstMove = true;
    emit isRunningChanged();
    m_pMsgModel->addMessage( tr("New game has been started successfully :-).") );

    m_lMoves.clear();
    m_pGameCourseModel->clear();
    execNextPlayer();
}

void GamePlay::execNextPlayer(const bool bIsLoading)
{
    if (!bIsLoading)
        killTimer(m_nTimerID);

    //check word(s)
    if ((m_lMoves.count()>0) && // at least one word found
        (!bIsLoading) && //function not called from loadGame()
        (!m_lMoves.last()->PlacedWord.isEmpty())) //TODO: gamplay: nonsense?
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
                return;
        }
    }

    //result message
    if ((m_lMoves.count()>0) &&
        (!bIsLoading) &&
        (!m_lMoves.last()->PlacedWord.isEmpty()))
        m_pMsgModel->addMessage(tr("%1 places %2 %3 and receives %4 points.")
                               .arg(m_lPlayerNames[m_nCurrentPlayer])
                               .arg(m_lMoves.last()->PlacedWord)
                               .arg(!m_lMoves.last()->ConnectedWords.isEmpty() ? tr("and") + " " + m_lMoves.last()->ConnectedWords : "")
                               .arg(QString::number(m_lMoves.last()->Value())),
                               m_nCurrentPlayer);
    //new blank move
    if (m_lMoves.count()>0)
        m_pGameCourseModel->addMove(replaceAllLetters(m_lMoves.last()->PlacedWord),
                              replaceAllLetters(m_lMoves.last()->ConnectedWords),
                              m_nCurrentPlayer,
                              m_lMoves.last()->Value(),
                              m_pComputeMove->numberOfResults() > 0 ? m_pComputeMove->result(0)->Value() : 0,
                              m_lMoves.last()->isScrabble(),
                              m_nMoveTime);
    //TODO: nextplayer: add bonus, show in statistics

    //new pieces for the placed or marked to exchange
    int nEmptyLetters = 0;
    for (int i=0; i<m_pRackModel->rackSize(); i++) {
        Letter rackLetter = m_pRackModel->getLetter(i);
        if (rackLetter.IsEmpty()) {
            nEmptyLetters++;
            if (m_lBag.count()>0) {
                nEmptyLetters--;
                Letter aLetter = m_lBag[0];
                aLetter.State = LetterState::lsRack;
                aLetter.RackPos = i;
                m_pRackModel->setLetter(aLetter);
                m_lBag.removeAt(0);
            }
        } else
        if (rackLetter.IsExchange) {
            //TODO: gameplay: treat exchange optionally as pass
            rackLetter.State = LetterState::lsBag;
            rackLetter.RackPos = 0;
            rackLetter.IsExchange = false;
            m_lBag.append(rackLetter);
            m_pRackModel->placeLetter(i,false); //set empty
            Letter aLetter = m_lBag[0];
            aLetter.State = LetterState::lsRack;
            aLetter.RackPos = i;
            m_pRackModel->setLetter(aLetter);
            m_lBag.removeAt(0);
        }
    }
    if ((m_lMoves.count()>0) && //no move at the beginning
        (m_lMoves.last()->PlacedWord.isEmpty())) {
        if (!bIsLoading)
            m_pMsgModel->addMessage(tr("%1 passes the move.").arg(m_lPlayerNames[m_nCurrentPlayer]));
        m_nPasses++;
    }
    else
        m_nPasses = 0;

    //lMoves is zero before first move
    if ((m_lMoves.count()>0) && (m_lMoves.last()->Value()>0))
        m_bIsFirstMove = false;

    m_lMoves.append(new move(m_bIsFirstMove, m_pBoard));

    //increase currentmove and currentplayer
    m_nCurrentMove++;
    m_nCurrentPlayer = m_nCurrentMove % m_nPlayerCount;
    m_pRackModel->activePlayer(m_nCurrentPlayer);
    emit currentPlayerChanged();
    emit statInfoChanged();

    //boardModel updates the UI; m_nCurrentMove needs to be set before
    m_pBoard->placeLetters();
    m_pBoardModel->updateAllSquares();

    //clear computemove
    m_pComputeMove->clear();
    emit bestMoveCountChanged();

    //check game end
    if (!bIsLoading && //when loading bIsRunning is saved and restored
        (((nEmptyLetters == m_pRackModel->rackSize()) && //all letters placed and...
         (m_lBag.count()==0)) || //... no letter left in bag or...
        (m_nPasses == m_nNumberOfPasses) )) { //max numbers of passes reached
        m_pMsgModel->addMessage(tr("%1 has ended the game.").arg(m_lPlayerNames[(m_nCurrentMove-1) % m_nPlayerCount]));
        m_bIsRunning = false;
        //TODO: gameplay: add/susbstract
        emit isRunningChanged();
        return;
    }

    m_nMoveTime = 0;
    if (m_eTimeControlType == tcNoLimit)
        m_nTimeLeft = 0;
    else if (m_eTimeControlType == tcPerMove)
        m_nTimeLeft = m_nTimeControlValue;
    else if (m_eTimeControlType == tcPerGame)
        m_nTimeLeft = m_nTimeControlValue - m_pGameCourseModel->timeTotalPerPlayer(m_nCurrentPlayer);
    //TODO: gameplay: add time

    if (bIsLoading) return; //do not update the board when loading a game

    //allow next player action
    m_bIsAccepted = true;
    m_pMsgModel->addMessage(tr("It's %1's turn.").arg(m_lPlayerNames[m_nCurrentPlayer]));

    emit isAcceptedChanged();
    emit lastErrorChanged();

    //TODO: nextplayer: remove debug.ssg on every move
    QString fileName = configPath().path() +"/debug.ssg";
    saveGame(QUrl::fromLocalFile(fileName));

    if ((m_eTimeControlType != tcNoLimit) && (m_nTimeLeft == 0))
        execNextPlayer();
    else
        m_nTimerID = startTimer(1000);

    //compute and place if player is "Computer"
    if (m_lPlayerNames[m_nCurrentPlayer] == "Computer") {
        computeMove();
        if ((m_pComputeMove->numberOfResults() == 0) &&
            (m_lBag.count() >= m_pRackModel->rackSize()))
            m_pComputeMove->markLettersForExchange();
        else
            placeBestMove(1);
        execNextPlayer();
    }
}

bool GamePlay::canDrop(const unsigned int boardIndex)
{
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
    if ((m_eTimeControlType != tcNoLimit) && (m_nMoveTime >= m_nTimeLeft))
        execNextPlayer();

    m_nMoveTime++;

    if (m_nStatInfoType == 2)
        emit statInfoChanged();

    Q_UNUSED(event);
}

QString GamePlay::getStatInfoType(const int aType)
{
    if (!m_bIsRunning)
        return "";

    switch (aType) {
    case 0:
        return QString::number(m_lBag.count());
    case 1: {
        QList<int> aValues;
        for (uint i=0; i<m_nPlayerCount; i++)
            aValues.append(0);
        m_pGameCourseModel->getScores(aValues);
        QString aResult;
        for (uint i=0; i<m_nPlayerCount; i++)
            aResult += QString::number(aValues[i]) + ",";
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
            m_lMoves.last()->setBonus( m_nScrabbleBonus, true );
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
    if (fileName.isEmpty()) //new game
        fileName = configPath().path() + "/Scrabble3D.ini";
    else
        fileName = QUrl(fileName).toLocalFile();
    QSettings settings(fileName, QSettings::IniFormat);

    settings.beginGroup("config");
    for (auto i = configData.begin(); i!=configData.end(); i++)
        settings.setValue(i.key(), i.value());
    settings.endGroup();
}

QVariantMap GamePlay::loadConfig(QString fileName)
{
    if (fileName.isEmpty()) //new game
        fileName = configPath().path() + "/Scrabble3D.ini";
    else
        fileName = QUrl(fileName).toLocalFile();

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
    qDebug() << "Save game to " << fileName.toLocalFile();
    m_pMsgModel->addMessage(tr("Game saved as %1").arg(fileName.toLocalFile()));
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
            settings.setValue("letter", m_lBag[i].toString());
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
        for (int i=0; i < m_lMoves.count(); i++) {
            settings.setArrayIndex(i);
            settings.beginWriteArray("move");
            for (int j=0; j < m_lMoves[i]->letterCount(); j++) {
                settings.setArrayIndex(j);
                settings.setValue("letter", m_lMoves[i]->getLetter(j).toString());
            }
            settings.endArray();
            settings.setValue("time", m_pGameCourseModel->timePerMove(i));
        }
    }
    settings.endArray();

    settings.beginWriteArray("player");
    {
        for (int nPlayerIndex = 0; nPlayerIndex < m_nPlayerCount; nPlayerIndex++) {
            settings.setArrayIndex( nPlayerIndex );
            settings.beginWriteArray("rack");
            {
                for (int nRackIndex = 0; nRackIndex < m_pRackModel->rackSize(); nRackIndex++) {
                    settings.setArrayIndex( nRackIndex );
                    settings.setValue("letter",m_pRackModel->getLetter( nRackIndex, nPlayerIndex ).toString());
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
    qDebug() << "Load game from " << fileName.toLocalFile();

    if (m_bIsRunning) killTimer(m_nTimerID);
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
        m_pBoardModel->reset();
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
        for (int nPlayerIndex = 0; nPlayerIndex < m_nPlayerCount; nPlayerIndex++) {
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
        m_nCurrentMove = -1; // inc in first nextplayer()
        m_bIsRunning = true;
        m_bIsFirstMove = true;
        m_lMoves.clear();
        Letter aLetter;
        m_nMoveTime = 0; //zero at beginning
        m_nTimerID = -1; //suppress killtimer()
        for (int i=0; i < nMoves; i++) {
            execNextPlayer(true); //bIsLoading = true
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
                m_lMoves.last()->setBonus( m_nScrabbleBonus, true ); //while playing done in dropLetter()
            settings.endArray();
        }
    }
    settings.endArray();
    if (settings.value("game/IsRunning").toBool() == false) {
        m_pMsgModel->addMessage(tr("%1 has ended the game.").arg(m_lPlayerNames[(m_nCurrentMove-1) % m_nPlayerCount]));
        m_bIsRunning = false;
        //TODO: gameplay: add/susbstract
        emit isRunningChanged();
    }
    if (m_bIsRunning)
        m_pMsgModel->addMessage(tr("It's player %1's turn.").arg(QString::number(m_nCurrentPlayer+1)), m_nCurrentPlayer);

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

    if ((m_eTimeControlType != tcNoLimit) && (m_nTimeLeft == 0))
        execNextPlayer();
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
