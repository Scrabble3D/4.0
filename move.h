#pragma once

#include "letter.h"
#include "board.h"

class move : public QObject
{
    Q_OBJECT

public:
    enum BonusType {
        btNone = 0x0,
        btScrabble = 0x1,
    };
    Q_DECLARE_FLAGS(Bonuses, BonusType)

    move(const bool IsFirstMove, board* aBoard);

    QString PlacedWord;
    QString ConnectedWords;

    QString LastError() { return m_LastError; }
    unsigned int Value() {return m_Value + m_nBonusValue; }
    void setBonus(unsigned int nBonus, BonusType eBonusType) {
        m_nBonusValue += nBonus;
        m_eBonusType |= eBonusType; }
    bool addLetter(Letter aLetter, const bool doCheck = true);
    Letter getLetter(unsigned int index) const;
    bool deleteLetter(const unsigned int x);
    int letterCount() { return m_PlacedLetters.count(); }
    bool checkMove();
    bool isFirstMove() { return m_IsFirstMove; }
    bool isScrabble() { return m_eBonusType.testFlag(BonusType::btScrabble); }
    int activePosition() { return getPosition(); }
    Dimension activeDimension() { return m_Dimension; }
    void setJokerLetter(const QString aWhat);//set Letter.What for Letter.IsJoker of m_PlacedLetters.last()

private:
    bool checkFirstMove();
    bool checkOneRowColumn();
    bool checkConsecutive();
    bool checkConnection();
    Letter leftOf(Point3D aPoint);
    Letter rightOf(Point3D aPoint);
    Letter aboveOf(Point3D aPoint);
    Letter belowOf(Point3D aPoint);
    Letter infrontOf(Point3D aPoint);
    Letter behindOf(Point3D aPoint);
    QString getResults(const Dimension eDimension, const Point3D nStart);
    int getPosition();
    Dimension m_Dimension;
    QList<Letter> m_PlacedLetters;
    unsigned int m_Value;
    unsigned int m_nBonusValue;
    Bonuses m_eBonusType;
    bool m_IsFirstMove;

    board* m_pBoard;
    QString m_LastError;
};
