#ifndef LETTER_H
#define LETTER_H

#include <QObject>

namespace LetterState {
    Q_NAMESPACE
    enum State {lsEmpty,    //no letter placed
                lsBag,      //in bag
                lsRack,     //on rack, can be changed or dragged
                lsDragging, //on rank, just dragging; need for removing a letter from board by calling in Pieces.DoUpdate
                lsChange,   //on rank, marked for change, can't be dragged
                lsBoard,    //on board, currently placed, can be dragged
                lsPlaced};  //on board placed, can't be dragged
    Q_ENUM_NS(State)
}

struct Letter
{
    Q_GADGET

    Q_PROPERTY(QString what MEMBER m_sWhat)        // actual letter
    Q_PROPERTY(int value MEMBER m_nValue)          // points received when placed
    Q_PROPERTY(bool isJoker MEMBER m_bIsJoker)     // whether a joker; needed to draw the indicator
    Q_PROPERTY(bool isRandom MEMBER m_bIsRandom)   // whether a random letter, needed to draw the indicator
    Q_PROPERTY(LetterState::State state MEMBER m_eState) // important how to handle the letter
    Q_PROPERTY(int when MEMBER m_nWhen)            // placed at move #  (0 if not placed)
    Q_PROPERTY(int who MEMBER m_nWho)              // placed by player # (0 if not placed)
    Q_PROPERTY(int where MEMBER m_nWho)            // placed by player # (0 if not placed)
    Q_PROPERTY(int rackPos MEMBER m_nRackPos)      // position, assigned by NextPlayer or ExchangeLetter

public:
    QString m_sWhat;
    unsigned int m_nValue;
    bool m_bIsJoker;
    bool m_bIsRandom;
    unsigned int m_nWhen;
    unsigned int m_nWho;
    unsigned int m_nRackPos;
    unsigned int m_nWhere;
    LetterState::State m_eState;
};

//Q_DECLARE_METATYPE(Letter);

#endif // LETTER_H
