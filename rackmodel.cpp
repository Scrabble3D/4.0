#include "rackmodel.h"

rackmodel::rackmodel(QObject *parent)
    : QAbstractListModel(parent),
      m_lPieces(0)
{
    m_nRackSize = 0;
    m_nPlayerCount = 0;
}

void rackmodel::initialize(const int nPlayerCount, const int nRackSize)
{
    m_nPlayerCount = nPlayerCount;
    m_nActivePlayer = 0;
    m_nLocalPlayer = 0;
    m_nRackSize = nRackSize;

    beginResetModel();

    m_lPieces.clear();
    for (int i=0; i < m_nPlayerCount; i++) {
        QVector<Letter> aLetters;
        for (int j=0; j < m_nRackSize; j++)
            aLetters.append(EmptyLetter);
        m_lPieces.append(aLetters);
    }

    endResetModel();
}

void rackmodel::setLetter(const Letter aLetter, const bool bSilent, int nPlayer)
{
    const int nIndex = aLetter.RackPos;

    if (nPlayer == -1)
        nPlayer = m_nActivePlayer;
    if (nPlayer > m_nPlayerCount-1) {
        qFatal("Setting letter failed"); //WARNING: rackmodel: make fatal just a warning
        return;
    }
    if ( (nIndex >= 0) && (nIndex < m_nRackSize) &&
         (m_lPieces[nPlayer][nIndex].IsEmpty()) )
    {
        if (!bSilent) beginResetModel();
        m_lPieces[nPlayer][nIndex] = aLetter;
        if (!bSilent) endResetModel();
    }
}

void rackmodel::placeLetter(const int index, const bool bVisible)
{
    if ( (index >= 0) && (index < m_lPieces[m_nActivePlayer].count()) ) {
        if (bVisible) beginResetModel();
        m_lPieces[m_nActivePlayer][index] = EmptyLetter;
        if (bVisible) endResetModel();
    } else
        qFatal("Cannot place letter");
}

void rackmodel::toggleExchangeFlag(const int index)
{
    m_lPieces[m_nActivePlayer][index].IsExchange = !m_lPieces[m_nActivePlayer][index].IsExchange;
    QModelIndex aIndex = this->index( index );
    emit dataChanged(aIndex, aIndex, { IsExchangeRole } );
}

void rackmodel::move(const uint fromIndex, const uint toIndex)
{
    beginResetModel();
    m_lPieces[m_nActivePlayer][fromIndex].RackPos = toIndex;
    m_lPieces[m_nActivePlayer][toIndex].RackPos = fromIndex;
    m_lPieces[m_nActivePlayer].swapItemsAt(fromIndex, toIndex);
    endResetModel();
}

Letter rackmodel::getLetter(const int nRackIndex, int nPlayer) const
{
    if (nPlayer == -1)
        nPlayer = m_nActivePlayer;

    if ((nPlayer < m_nPlayerCount) && (nRackIndex < m_nRackSize))
        return m_lPieces[nPlayer][nRackIndex];
    else {
        qFatal("Retrieving rack letter failed");
        return EmptyLetter;
    }
}

void rackmodel::setActivePlayer(const int nPlayer)
{
    if (nPlayer<m_nPlayerCount) {
        beginResetModel();
        m_nActivePlayer = nPlayer;
        endResetModel();
    }
    else
        qFatal("Active player cannot be set");
}

void rackmodel::setLocalPlayer(const int nPlayer)
{
    if (nPlayer<m_nPlayerCount) {
        if (nPlayer != m_nLocalPlayer) {
            beginResetModel();
            m_nLocalPlayer = nPlayer;
            endResetModel();
        }
    }
    else
        qFatal("Local player cannot be set");
}

bool rackmodel::isRackEmpty()
{
    Letter aLetter;
    for (int i = 0; i < m_nRackSize; i++) {
        aLetter = m_lPieces[m_nActivePlayer][i];
        if (!aLetter.IsEmpty())
            return false;
    }
    return true;
}

int rackmodel::exchangeNumber()
{
    int result = 0;
    Letter aLetter;
    for (int i = 0; i < m_nRackSize; i++) {
        aLetter = m_lPieces[m_nActivePlayer][i];
        if (aLetter.IsExchange)
            result++;
    }
    return result;
}

QHash<int, QByteArray> rackmodel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[WhatRole] = "what";
    roles[ValueRole] = "value";
    roles[IsJokerRole] = "isJoker";
    roles[IsExchangeRole] = "isExchange";
    roles[IsVisibleRole] = "isVisible";
    return roles;
}

QVariant rackmodel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || (m_nLocalPlayer>m_nPlayerCount)) {
        qFatal("Invalid rackmodel request");
        return QVariant();
    }

    const Letter aLetter = m_lPieces[m_nLocalPlayer][index.row()];
    switch (role) {
      case WhatRole: return replaceLetter.value(aLetter.What, aLetter.What); break;
      case ValueRole: return aLetter.Value; break;
      case IsJokerRole: return aLetter.IsJoker; break;
      case IsExchangeRole: return aLetter.IsExchange; break;
      //NOTE: rackmodel: invisible role probably never read
      case IsVisibleRole: return (aLetter.State == LetterState::lsRack); break;
    }

    return QVariant();
}

int rackmodel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_nRackSize;
}
