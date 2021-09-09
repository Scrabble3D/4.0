#include "gameplay.h"

#include <algorithm>
#include <random>
#include <QtMath>
#include <QChar>

#include <QDebug>

QGamePlay::QGamePlay() {
    const int fieldsClassicScrabble[255]=
       {6,1,1,2,1,1,1,6,1,1,1,2,1,1,6,
        1,5,1,1,1,3,1,1,1,3,1,1,1,5,1,
        1,1,5,1,1,1,2,1,2,1,1,1,5,1,1,
        2,1,1,5,1,1,1,2,1,1,1,5,1,1,2,
        1,1,1,1,5,1,1,1,1,1,5,1,1,1,1,
        1,3,1,1,1,3,1,1,1,3,1,1,1,3,1,
        1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,
        6,1,1,2,1,1,1,0,1,1,1,2,1,1,6,
        1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,
        1,3,1,1,1,3,1,1,1,3,1,1,1,3,1,
        1,1,1,1,5,1,1,1,1,1,5,1,1,1,1,
        2,1,1,5,1,1,1,2,1,1,1,5,1,1,2,
        1,1,5,1,1,1,2,1,2,1,1,1,5,1,1,
        1,5,1,1,1,3,1,1,1,3,1,1,1,5,1,
        6,1,1,2,1,1,1,6,1,1,1,2,1,1,6};

    m_aGame.nRackSize = 0;
    m_aGame.is3D = false;
    m_IsRunning = false;
    for (int i=0; i<255; i++ )
       m_aGame.lFieldTypeArray.append( static_cast<FieldType>(fieldsClassicScrabble[i]) );

    EmptyLetter.m_sWhat = QChar::Null;
    EmptyLetter.m_nValue = 0;

}

void QGamePlay::startNewGame(QStringList PlayerNames,
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
    m_aGame.lPlayerNames = PlayerNames;
    m_aGame.is3D = is3D;

    m_aGame.lFieldTypeArray.clear();
    for (int i=0; i<FieldTypeArray.count(); i++)
       m_aGame.lFieldTypeArray.append( static_cast<FieldType>(FieldTypeArray[i].toInt()) );
    emit boardSizeChanged();

    m_aGame.nRackSize = RackSize;
//    emit rackSizeChanged();

    qDebug() << "New Game";
    m_lRack.clearRack();
    m_lPlaced.clear();
    m_aGame.lBag.clear();
    for (int i=0; i<LetterList.count(); i+=3) {
        Letter aLetter;
        aLetter.m_sWhat=LetterList[i].toString();
        aLetter.m_nValue=LetterList[i+1].toInt();
        for (int j=0; j<LetterList[i+2].toInt(); j++)
            m_aGame.lBag.append(aLetter);
    }
    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(m_aGame.lBag.begin(), m_aGame.lBag.end(), rng);

    m_aGame.bCanJokerExchange = CanJokerExchange;
    m_aGame.nGameEndBonus = GameEndBonus;
    m_aGame.nNumberOfPasses = NumberOfPasses;
    m_aGame.nJokerPenalty = JokerPenalty;
    m_aGame.bChangeIsPass = ChangeIsPass;
    m_aGame.eTimeControlType = static_cast<TimeControlType>(TimeControl);
    m_aGame.nTimeControlValue = TimeControlValue;
    m_aGame.nLimitedExchange = LimitedExchange;
    m_aGame.bCambioSecco = CambioSecco;
    m_aGame.bWhatif = Whatif;
    m_aGame.bAdd = Add;
    m_aGame.bSubstract = Substract;
    m_aGame.nTimePenaltyValue = TimePenaltyValue;
    m_aGame.nTimePenaltyCount = TimePenaltyCount;
    m_aGame.bTimeGameLost = TimeGameLost;
    m_aGame.eWordCheckMode = static_cast<WordCheckMode>(WordCheck);
    m_aGame.nWordCheckPeriod = WordCheckPeriod;
    m_aGame.nWordCheckPenalty = WordCheckPenalty;
    m_aGame.nWordCheckBonus = WordCheckBonus;
    m_aGame.nScrabbleBonus = ScrabbleBonus;
    m_aGame.isCLABBERS = isCLABBERS;
    m_aGame.bRandomSequence = RandomSequence;

    m_nCurrentMove = 0;
    m_IsRunning = true;
    emit isRunningChanged();
    execNextPlayer();
}

void QGamePlay::execNextPlayer() {
    qDebug() << "Next Player";
    for (int i = m_lRack.countLetters(); i < m_aGame.nRackSize; i++) {
        if (i<m_aGame.lBag.count()) {
            m_aGame.lBag[i].m_eState = LetterState::lsRack;
            m_aGame.lBag[i].m_nRackPos = i;
            m_lRack.addLetter(&m_aGame.lBag[i]);
            m_aGame.lBag.removeAt(i);
        }
    }
    emit rackSizeChanged();
    m_nCurrentMove++;
    m_nCurrentPlayer = 1; //TODO
}

int QGamePlay::getRackSize() {
    return m_aGame.nRackSize;
}

int QGamePlay::getBoardSize() {
    if (m_aGame.is3D)
       return qPow(m_aGame.lFieldTypeArray.length(),1/3.);
    else
       return qSqrt(m_aGame.lFieldTypeArray.length());
}

int QGamePlay::fieldType(const unsigned int index) {
    if (index < m_aGame.lFieldTypeArray.length())
        return m_aGame.lFieldTypeArray[index];
    else
        return FieldType::ftDefault;
}


/*
bool QGamePlay::rackIndex(const unsigned int index) {
    qDebug() << index;
    if (index<m_lRack.length()) {
        m_nRackIndex = index; //used for subsequential calls
        return (m_lRack[m_nRackIndex].m_eState == LetterState::lsRack);
    }
    return false;
}
*/
/*
Letter QGamePlay::rackLetter(const unsigned int rackIndex) {
    if (rackIndex < m_lRack.length()) {
        log(m_lRack[rackIndex].m_nRackPos);
        return(m_lRack[rackIndex]); }
    else
        return EmptyLetter;
}
*/
/*
Letter QGamePlay::getRackLetter() {
    if (m_nRackIndex < m_lRack.length()) {
        return(m_lRack[m_nRackIndex]);
    } else
        return (EmptyLetter);
}
*/

void QGamePlay::placeLetter(const unsigned int rackIndex, const unsigned int boardIndex) {
    Letter *aLetter = m_lRack.rackLetter(rackIndex);
    aLetter->m_eState = LetterState::lsBoard;
    aLetter->m_nWhen = m_nCurrentMove;
    aLetter->m_nWho = m_nCurrentPlayer;
    aLetter->m_nWhere = boardIndex;
    m_lPlaced.append(*aLetter);
    m_lRack.removeLetter(rackIndex);
    qDebug() << "letter placed";
//    emit rackChanged();
    emit boardChanged();
}

void QGamePlay::removeLetter(const unsigned int boardIndex) {
    Letter aLetter;
    for (int i=0; i<m_lPlaced.length(); i++) {
        aLetter = m_lPlaced[i];
        if (aLetter.m_nWhere == boardIndex && aLetter.m_eState == LetterState::lsBoard) {
            aLetter.m_eState = LetterState::lsRack;
            aLetter.m_nWhen = 0;
            aLetter.m_nWho = 0;
            aLetter.m_nWhere = 0;
            m_lPlaced.removeAt(i);
            return;
        }
    }
}

Letter QGamePlay::boardLetter(const unsigned int boardIndex) {
    Letter aLetter;
    for (int i=0; i<m_lPlaced.length(); i++) {
        aLetter = m_lPlaced[i];
        if (aLetter.m_nWhere == boardIndex)
            return(aLetter);
    }
    return(EmptyLetter);
}

bool QGamePlay::canDrop(const unsigned int index) {
   if (index < m_aGame.lFieldTypeArray.count())
       return ( m_aGame.lFieldTypeArray[index] == FieldType::ftDefault);
   else
       return false;
}

bool QGamePlay::getIsRunning() {
    log(m_IsRunning);
    return(m_IsRunning);
}
