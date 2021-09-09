#pragma once

#include <QQmlListProperty>
#include <QtQml>
#include <letter.h>

class rack : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Letter> scrRack READ rackLetter NOTIFY rackChanged)
    QML_ELEMENT

signals:
    void rackChanged(QQmlListProperty<Letter>);

public:
    explicit rack(QObject *parent = nullptr);

//    QQmlListProperty<Letter> therack();

    Letter *rackLetter(int) const;
    void addLetter(Letter*);
    void removeLetter(int);
    int countLetters();
    void clearRack();

private:
    static Letter* rackLetter(QQmlListProperty<Letter>*, int);
    static void addLetter(QQmlListProperty<Letter>*, Letter*);
    static void removeLetter(QQmlListProperty<Letter>*, int);
    static int countLetters(QQmlListProperty<Letter>*);
    static void clearRack(QQmlListProperty<Letter>*);

    QList<Letter *> m_lRack;
};
