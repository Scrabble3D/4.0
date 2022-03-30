#pragma once

#include <QAbstractListModel>
#include "letter.h"

class rackmodel : public QAbstractListModel
{

public:
    enum ModelRoles {
        WhatRole = Qt::UserRole + 1,
        ValueRole,
        IsJokerRole,
        IsVisibleRole
    };
//TODO: rack: per player
    explicit rackmodel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;

    void addLetter(const Letter aLetter);
    void setLetter(const Letter aLetter, const bool bSilent = false); //make rack item a Letter
    void placeLetter(const int index, const bool bVisible = true); //make rack item an EmptyLetter; visible to show in UI
    void move(const uint fromIndex, const uint toIndex);
    Letter getLetter(const int index) const {return m_lPieces[index]; }
    // used to clear the rack on new game
    void clearRack();
    int rackSize() {return m_lPieces.count(); }

protected:
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    // returns the letter from rackindex for placeLetter
    QVector<Letter> m_lPieces;

};
