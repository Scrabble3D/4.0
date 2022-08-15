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
        IsExchangeRole,
        IsVisibleRole
    };

    explicit rackmodel(QObject *parent = nullptr);
    void initialize(const int nPlayerCount, const int nRackSize);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;

    void setLetter(const Letter aLetter, const bool bSilent = false, int nPlayer = -1); //make rack item a Letter
    void placeLetter(const int index, const bool bVisible = true); //make rack item an EmptyLetter; visible to show in UI
    void toggleExchangeFlag(const int index);
    void move(const uint fromIndex, const uint toIndex);
    Letter getLetter(const int nRackIndex, int nPlayer = -1) const;
    int rackSize() {return m_nRackSize; }
    void setActivePlayer(const int nPlayer); // set via gameplay.nextPlayer()
    void setLocalPlayer(const int nPlayer);
    bool isLocalIsActive() {return m_nActivePlayer == m_nLocalPlayer; }
    bool isRackEmpty(); //returns true if all rack pieces are empty
    int exchangeNumber(); //returns true if at least one letter is marked for exchange

protected:
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    QVector<QVector<Letter>> m_lPieces; //[nPlayer][nRackPos]
    int m_nRackSize;
    int m_nPlayerCount;
    int m_nActivePlayer;
    int m_nLocalPlayer;
};
