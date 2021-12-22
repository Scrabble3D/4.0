#pragma once

#include "letter.h"
#include "board.h"

class move : public QObject
{
public:
    move(const bool IsFirstMove, const bool Is3D, board* aBoard);

    QString PlacedWord;
    QString ConnectedWords;

    QString LastError() { return m_LastError; }
    unsigned int Value() {return m_Value + m_Bonus; }
    void setBonus(unsigned int nBonus) {m_Bonus = nBonus;}
    bool addLetter(Letter aLetter);
    Letter getLetter(unsigned int index) const;
    bool deleteLetter(const unsigned int x);
    int letterCount() { return m_PlacedLetters.count(); }
    bool checkMove();
    QVariantList exportPlacedLetters();

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
    Dimension m_Dimension;
    QList<Letter> m_PlacedLetters;
    unsigned int m_Value;
    unsigned int m_Bonus;
    bool m_IsFirstMove;
    bool m_Is3D;

    board* m_pBoard;
    bool m_IsScrabble;
    QString m_LastError;
};
