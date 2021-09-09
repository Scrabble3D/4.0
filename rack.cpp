#include "rack.h"
#include <QQmlListProperty>

rack::rack(QObject *parent) : QObject(parent) {
}
/*
QQmlListProperty<Letter> rack::therack() {
    return {this,this,
            &rackLetter,
            &addLetter,
            &removeLetter,
            &clearRack};
}
*/
Letter *rack::rackLetter(int index) const {
    return m_lRack.at(index);
}

void rack::addLetter(Letter* l) {
    m_lRack.append(l);
//    emit rackChanged();
}

void rack::removeLetter(int index) {
    m_lRack.remove(index);
//    emit rackChanged();
}

void rack::clearRack() {
    m_lRack.clear();
//    emit rackChanged();
}

int rack::countLetters() {
    return m_lRack.count();
}
Letter* rack::rackLetter(QQmlListProperty<Letter> *list, int index)
{
    return reinterpret_cast< rack* >(list->data)->rackLetter(index);
}

void rack::addLetter(QQmlListProperty<Letter>* list, Letter* l) {
    reinterpret_cast< rack* >(list->data)->addLetter(l);
}

void rack::removeLetter(QQmlListProperty<Letter>* list, int index) {
    reinterpret_cast< rack* >(list->data)->removeLetter(index);
}


int rack::countLetters(QQmlListProperty<Letter>* list) {
    return reinterpret_cast< rack* >(list->data)->countLetters();
}
void rack::clearRack(QQmlListProperty<Letter>* list) {
    reinterpret_cast< rack* >(list->data)->clearRack();
}
