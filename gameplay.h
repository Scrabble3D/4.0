#pragma once

#include <letter.h>
#include <rack.h>
#include <QVariant>
#include <QQmlListProperty>
#include <QtQml>

enum Dimension {dmAbscissa=0,   //x-axis
                dmOrdinate=1,   //y-axis
                dmApplicate=2}; //z-axis

enum WordCheckMode {wcAsk=0,
                    wcPoll=1,
                    wcChallenge=2};

enum TimeControlType {tcNoLimit=0,
                      tcPerMove=1,
                      tcPerGame=2};

enum FieldType {ftStart=0,
                ftDefault=1,
                ftDoubleLetter=2,
                ftTrippleLetter=3,
                ftQuadLetter=4,
                ftDoubleWord=5,
                ftTrippleWord=6,
                ftQuadWord=7,
                ftPiece=8};

class QMove
{
  QList<Letter> PlacedLetters;
  Dimension PlacedAt;
  QString PlacedWord;
  QString ConnectedWords;
  QString UnknownWords;
  unsigned int Value;
  bool isScrabble;
  bool isChecked;
};

struct QGame
{
    QStringList lPlayerNames;               //names of players; also count
    int nRackSize;                          //numbers of letters on rack
    bool is3D;                              //whether game is in 3D
    QList<FieldType> lFieldTypeArray;       //configuration of bonus fields
    QList<Letter> lBag;                     //letters in bag
    bool bCanJokerExchange;
    int nGameEndBonus;
    int nNumberOfPasses;
    int nJokerPenalty;
    bool bChangeIsPass;
    TimeControlType eTimeControlType;
    int nTimeControlValue;
    int nLimitedExchange;
    bool bCambioSecco;
    bool bWhatif;
    bool bAdd;
    bool bSubstract;
    int nTimePenaltyValue;
    int nTimePenaltyCount;
    bool bTimeGameLost;
    WordCheckMode eWordCheckMode;
    int nWordCheckPeriod;
    int nWordCheckPenalty;
    int nWordCheckBonus;
    int nScrabbleBonus;
    bool isCLABBERS;
    bool bRandomSequence;
};

class QGamePlay : public QObject
{
    Q_OBJECT
    Q_ENUM(Dimension)
    Q_PROPERTY(int boardSize READ getBoardSize NOTIFY boardSizeChanged)
    Q_PROPERTY(int rackSize READ getRackSize NOTIFY rackSizeChanged)

//    Q_PROPERTY(Letter rackLetter READ getRackLetter NOTIFY rackChanged)
//    Q_PROPERTY(QList<Letter> rack READ getRack NOTIFY rackChanged)
    Q_PROPERTY(bool isRunning READ getIsRunning NOTIFY isRunningChanged)

signals:
     void rackSizeChanged();
     void boardSizeChanged();
     void isRunningChanged();
     void boardChanged();

public:
    QGamePlay();
    Q_INVOKABLE bool canDrop(const unsigned int index);
    Q_INVOKABLE int fieldType(const unsigned int index);
//    Q_INVOKABLE bool rackIndex(const unsigned int index);
    Q_INVOKABLE Letter boardLetter(const unsigned int boardIndex);
//    Q_INVOKABLE Letter rackLetter(const unsigned int rackIndex);

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

     void placeLetter(const unsigned int rackIndex, const unsigned int boardIndex);
     void removeLetter(const unsigned int boardIndex);
//     Letter getRackLetter(const unsigned int rackIndex);
//     QList<Letter> getRack() { return(m_lRack); };

private:
     int getRackSize();
     int getBoardSize();
     bool getIsRunning();
     Letter getRackLetter();

    QGame m_aGame;
//    QQmlListProperty<Letter> m_lRack;
    rack m_lRack;
    unsigned int m_nRackIndex;
    QList<Letter> m_lPlaced;
    QList<QMove> m_aGameCourse;
    int m_nCurrentMove;
    int m_nCurrentPlayer;
    bool m_IsRunning;
    Letter EmptyLetter;
};
