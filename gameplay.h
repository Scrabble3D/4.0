#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <QPoint>
#include <QList>

#include <QObject>
#include <QtQml/qqml.h>

struct Letter
{
    char What; //character
    int Value; //value
    int Who = -1; //player
    int When = -1; //move
    QPoint Where; //position
};

class Game : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<Letter> Pieces READ getPieces)
    QML_ELEMENT

public:
    Game();
    ~Game();
    QList<Letter> getPieces();

private:
    QList<Letter> m_Pieces;
};

#endif // GAMEPLAY_H
