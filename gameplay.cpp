#include "gameplay.h"

#include <algorithm>
#include <random>
#include <QtMath>
#include <QMessageBox>

#include <QDebug>

GamePlay::GamePlay(QObject *parent)
{
    Q_UNUSED(parent);

    m_bIsLoading = false;
    m_nRackSize = 0;
    m_bIs3D = false;
    m_bIsRunning = false;
    m_pBoard = new board();
    m_pRackModel = new rackmodel(this);
    m_pBoardModel = new boardmodel(this, m_pBoard);
    m_pCubeModel = new cubemodel(this, m_pBoard);
    m_pMsgModel = new msgmodel(this);
    m_pStatModel = new statmodel(this);
    m_pDictModel = new dictionary(this);
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

statmodel *GamePlay::statModel()
{
    return m_pStatModel;
}

dictionary *GamePlay::dictModel()
{
    return m_pDictModel;
}

void GamePlay::startNewGame(QStringList PlayerNames,
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
                             bool RandomSequence)
{
    m_lPlayerNames.clear();
    m_lPlayerNames = PlayerNames;
    emit numberOfPlayersChanged();
    m_bIs3D = is3D;

    m_pBoard->initialize(FieldTypeArray, m_bIs3D);
    m_pBoardModel->reset();
    if (m_bIs3D) m_pCubeModel->reset();
    emit boardSizeChanged();
    emit is3DChanged();

    m_nRackSize = RackSize;

    m_pRackModel->clearRack();
    m_lBag.clear();
    for (int i=0; i<LetterList.count(); i+=3) {
        Letter aLetter;
        aLetter.What=LetterList[i].toString();
        aLetter.Value=LetterList[i+1].toInt();
        for (int j=0; j<LetterList[i+2].toInt(); j++)
            m_lBag.append(aLetter);
    }
    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(m_lBag.begin(), m_lBag.end(), rng);

    m_bCanJokerExchange = CanJokerExchange;
    m_nGameEndBonus = GameEndBonus;
    m_nNumberOfPasses = NumberOfPasses;
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
    m_bRandomSequence = RandomSequence;

    m_nCurrentMove = -1;
    m_bIsRunning = true;
    m_bIsFirstMove = true;
    emit isRunningChanged();
    m_pMsgModel->addmessage( tr("New game has been started successfully :-).") );

    m_lMoves.clear();
    statModel()->clear();
    execNextPlayer();
}

void GamePlay::execNextPlayer()
{
    //check word(s)
    if ((m_lMoves.count()>0) &&
        !m_bIsLoading &&
        (!m_lMoves.last()->PlacedWord.isEmpty()))
    {
        QStringList unknownWords;
        unknownWords.append(m_lMoves.last()->PlacedWord);
        unknownWords.append(m_lMoves.last()->ConnectedWords.split(","));
        int i = unknownWords.count();
        while (i > 0)
        {
            if (m_pDictModel->contains(unknownWords[i-1]))
                unknownWords.remove(i-1);
            i--;
        }

        if (unknownWords.count()>0)
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Scrabble3D");
            msgBox.setText(tr("Word(s) %1 not found.","",unknownWords.count()).arg(unknownWords.join(",")));
            msgBox.setInformativeText(tr("Do you want to accept anyway?"));
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            if (msgBox.exec() == QMessageBox::No)
                return;
        }
    }
    //result message
    if ((m_lMoves.count()>0) &&
        (!m_bIsLoading) &&
        (!m_lMoves.last()->PlacedWord.isEmpty()))
        m_pMsgModel->addmessage(tr("%1 places %2 %3 and receives %4 points.")
                               .arg(m_lPlayerNames[m_nCurrentPlayer])
                               .arg(m_lMoves.last()->PlacedWord)
                               .arg(!m_lMoves.last()->ConnectedWords.isEmpty() ? tr("and") + " " + m_lMoves.last()->ConnectedWords : "")
                               .arg(QString::number(m_lMoves.last()->Value())),
                               m_nCurrentPlayer);
    //new blank move
    if (m_lMoves.count()>0)
        m_pStatModel->addMove(m_lMoves.last()->PlacedWord,
                              m_lMoves.last()->ConnectedWords,
                              m_nCurrentPlayer,
                              m_lMoves.last()->Value(),
                              0,false,0); //TODO: finish

    m_nCurrentMove++;
    m_nCurrentPlayer = m_nCurrentMove % m_lPlayerNames.count();
    emit currentPlayerChanged();

    //TODO: lMoves is zero before first move
    if ((m_lMoves.count()>0) && (m_lMoves.last()->Value()>0))
        m_bIsFirstMove = false;

    m_lMoves.append(new move(m_bIsFirstMove, m_bIs3D, m_pBoard));
    //boardModel updates the UI and CurrentMove needs to be set before
    m_pBoard->placeLetters();
    if (m_bIsLoading) return; //do not touch the rack not update the UI

    m_pBoardModel->updateAllSquares();
    for (unsigned int i = m_pRackModel->rackSize(); i < m_nRackSize; i++) {
        if (m_lBag.count()>0) {
            Letter aLetter = m_lBag[0];
            aLetter.State = LetterState::lsRack;
            aLetter.RackPos = i;
            m_pRackModel->addLetter(aLetter);
            m_lBag.removeAt(0);
        }
    }
    //allow next player action
    //TODO: if game has not ended
    m_bIsAccepted = true;
    m_pMsgModel->addmessage(tr("It's %1's turn.").arg(m_lPlayerNames[m_nCurrentPlayer]));

    emit isAcceptedChanged();
    emit lastErrorChanged();

    saveGame(QUrl::fromLocalFile(QCoreApplication::applicationDirPath()+"/debug.ssg")); //TODO: remove
}

bool GamePlay::canDrop(const unsigned int boardIndex)
{
    int z = m_pBoard->PointToWhere( m_pBoard->Pos3D(boardIndex) );
    return (m_pBoard->getLetter(z).State == LetterState::lsEmpty);
}

void GamePlay::dropLetter(const unsigned int rackIndex, const unsigned int boardIndex)
{
    Letter aLetter = m_pRackModel->getLetter(rackIndex);

    aLetter.State = LetterState::lsBoard;
    aLetter.When = m_nCurrentMove;
    aLetter.Who = m_nCurrentPlayer;
    aLetter.Point = m_pBoard->Pos3D(boardIndex);
    aLetter.Where = m_pBoard->PointToWhere(aLetter.Point);
    m_pBoard->setLetter(aLetter);
    m_pBoardModel->updateSquare(aLetter.Point);
    if (m_bIs3D) cubeModel()->updateSquare(aLetter.Point);//aIndex);
    m_pRackModel->removeLetter(rackIndex);

    m_bIsAccepted = m_lMoves.last()->addLetter(aLetter);
    if (m_bIsAccepted)
    {
        if (m_pRackModel->rackSize() == 0)
            m_lMoves.last()->setBonus( m_nScrabbleBonus );
    }
    emit isAcceptedChanged();
    emit lastErrorChanged();
}

bool GamePlay::removeLetter(const unsigned int boardIndex, const bool startDrag)
{
    int z = m_pBoard->PointToWhere( m_pBoard->Pos3D(boardIndex) );
    Letter aLetter = m_pBoard->getLetter(z);
    Point3D aPoint = aLetter.Point;

    if (aLetter.State != LetterState::lsEmpty) {
        aLetter.State = LetterState::lsRack;
        aLetter.When = 0;
        aLetter.Who = 0;
        aLetter.Where = 0;
        aLetter.Point = Point3D(0,0,0);

        m_pBoard->removeLetter(z);
        m_pBoardModel->updateSquare(aPoint);
//        QModelIndex aIndex = m_pBoard->removeLetter(z);
        if (m_bIs3D) cubeModel()->updateSquare(aPoint);//aIndex);

        if (!startDrag)
            m_pRackModel->addLetter(aLetter);
        m_bIsAccepted = m_lMoves.last()->deleteLetter(z);

        emit isAcceptedChanged();
        emit lastErrorChanged();
        return true;
    }
    return false;
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

void GamePlay::setActivePosition(const int aPosition)
{
    if ( m_bIs3D &&
         (aPosition < m_pBoard->getBoardsize()) && (aPosition>=0) &&
         (aPosition != getActivePosition())
       )
    {
        m_pBoard->setActivePosition(aPosition);
        m_pBoardModel->updateAllFields();
        emit activePositionChanged();
    }
}

int GamePlay::getActivePosition()
{
    return m_pBoard->getActivePosition();
}

void GamePlay::saveGame(const QUrl &fileName)
{
    qDebug() << "Save game to " << fileName.toLocalFile(); //TODO: debug
    m_pMsgModel->addmessage(tr("Game saved as %1").arg(fileName.toLocalFile()));
    QSettings settings(fileName.toLocalFile(), QSettings::IniFormat); //TODO: path
//                       QSettings::UserScope, "scrabble3d", "Scrabble3D");
    //clear previous values
    settings.clear();
//TODO: save randseed
    if (m_bIs3D)
    {
        settings.setValue("ActiveDimension",m_pBoard->getActiveDimension());
        settings.setValue("ActivePosition",m_pBoard->getActivePosition());
    }
    settings.beginGroup("game");
    {
        settings.setValue("PlayerNames", m_lPlayerNames);
        settings.setValue("RackSize", m_nRackSize);
        settings.setValue("3D", m_bIs3D);
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
        settings.setValue("RandomSequence", m_bRandomSequence);
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
        for (int i=0; i<m_pBoard->getFieldsize(); i++)
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
        }
    }
    settings.endArray();

    settings.beginWriteArray("rack");
    {
        for (int i=0; i<m_pRackModel->rackSize(); i++) {
            settings.setArrayIndex(i);
            settings.setValue("letter",m_pRackModel->getLetter(i).toString());
        }
    }
    settings.endArray();
}

void GamePlay::loadGame(const QUrl &fileName)
{
    qDebug() << "Load game from " << fileName.toLocalFile(); //TODO: debug
    m_pMsgModel->addmessage(tr("Loading game from %1...").arg(fileName.toLocalFile()));
    m_bIsLoading = true;
    QSettings settings(fileName.toLocalFile(), QSettings::IniFormat); //TODO: path
//                       QSettings::UserScope, "scrabble3d", "Scrabble3D");
    statModel()->clear();
    m_lPlayerNames.clear();
    settings.beginGroup("game");
    {
        m_lPlayerNames = settings.value("PlayerNames").toStringList();
        m_nRackSize = settings.value("RackSize").toUInt();
        m_bIs3D = settings.value("3D").toBool();
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
        m_bRandomSequence = settings.value("RandomSequence").toBool();
    }
    settings.endGroup();

    settings.beginGroup("board");
    {
        QVariantList aList;
        aList = settings.value("fieldtypes").toList();
        m_pBoard->initialize(aList, m_bIs3D);
        m_pBoardModel->reset();
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

    m_pRackModel->clearRack();
    size = settings.beginReadArray("rack");
    {
        Letter aLetter;
        for (int i = 0; i < size; ++i) {
            settings.setArrayIndex(i);
            aLetter.fromString(settings.value("letter").toStringList());
            m_pRackModel->addLetter(aLetter);
        }
    }
    settings.endArray();

    int nMoves = settings.beginReadArray("moves");
    {
        m_nCurrentMove = -1;
        m_bIsRunning = true;
        m_bIsFirstMove = true;
        m_lMoves.clear();
        Letter aLetter;
        for (int i=0; i < nMoves; i++) {
            execNextPlayer();
            settings.setArrayIndex(i);
            uint nLetters = settings.beginReadArray("move");
            for (uint j=0; j < nLetters; j++) {
                settings.setArrayIndex(j);
                aLetter.fromString(settings.value("letter").toStringList());
                m_pBoard->setLetter(aLetter);
                m_bIsAccepted = m_lMoves.last()->addLetter(aLetter);
            }
            if (nLetters == m_nRackSize)
                m_lMoves.last()->setBonus( m_nScrabbleBonus );

            settings.endArray();
        }
        m_pMsgModel->addmessage(QString("It's player %1's turn.").arg(QString::number(m_nCurrentPlayer+1)), m_nCurrentPlayer);
    }
    m_pBoardModel->updateAllSquares();
    if (m_bIs3D) cubeModel()->reset();
    settings.endArray();

    if (m_bIs3D)
    {
        setActiveDimension(settings.value("ActiveDimension").toInt());
        setActivePosition(settings.value("ActivePosition").toInt());
    }

    m_bIsLoading = false;

    emit numberOfPlayersChanged();
    emit currentPlayerChanged();//TODO: check
    emit isAcceptedChanged();
    emit lastErrorChanged();
    emit isRunningChanged();
}

void GamePlay::computeMove()
{
    QString s;
    for (int i=0; i<rackModel()->rackSize(); i++)
        s += rackModel()->getLetter(i).What;
    s = dictModel()->variation(s);
    addMessage(s);
}
