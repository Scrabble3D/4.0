#include "rackmodel.h"

rackmodel::rackmodel(QObject *parent)
    : QAbstractListModel(parent),
      m_lPieces(0)
{
}

int rackmodel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_lPieces.count();
}

void rackmodel::addLetter(const Letter aLetter)
{
    const int i = m_lPieces.count();
    beginInsertRows(QModelIndex(), i, i);
    m_lPieces.append(aLetter);
    endInsertRows();
}

void rackmodel::setLetter(const Letter aLetter, const bool bSilent)
{
    const int nIndex = aLetter.RackPos;

    if ( (nIndex >= 0) &&
         (nIndex < m_lPieces.count()) &&
         (m_lPieces[nIndex].IsEmpty()) ) {
        if (!bSilent) beginResetModel();
        m_lPieces[nIndex] = aLetter;
        if (!bSilent) endResetModel();
    }
}

void rackmodel::placeLetter(const int index, const bool bVisible)
{
    if ( (index >= 0) && (index < m_lPieces.count()) ) {
        if (bVisible) beginResetModel();
        m_lPieces[index] = EmptyLetter;
        if (bVisible) endResetModel();
    }
/*    {
        beginRemoveRows(QModelIndex(), index, index);
        m_lPieces.remove(index);
        endRemoveRows();
    } */
}

void rackmodel::move(const uint fromIndex, const uint toIndex)
{
//    beginMoveRows(QModelIndex(),fromIndex,fromIndex,QModelIndex(),toIndex);
    beginResetModel();
    m_lPieces[fromIndex].RackPos = toIndex;
    m_lPieces[toIndex].RackPos = fromIndex;
    //TODO: rackmodel: QML(?) move piece not reliably working
    m_lPieces.move(fromIndex, toIndex);
    endResetModel();
//    endMoveRows();
}

void rackmodel::clearRack()
{
    beginResetModel();
    m_lPieces.clear();
    endResetModel();
}

QHash<int, QByteArray> rackmodel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[WhatRole] = "what";
    roles[ValueRole] = "value";
    roles[IsJokerRole] = "isJoker";
    roles[IsVisibleRole] = "isVisible";
    return roles;
}

QVariant rackmodel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const Letter aLetter = m_lPieces[index.row()];
    switch (role) {
      case WhatRole:  return aLetter.What;  break;
      case ValueRole: return aLetter.Value; break;
      case IsJokerRole: return aLetter.IsJoker; break;
      //NOTE: rackmodel: invisible role probably never read
      case IsVisibleRole: return (aLetter.State == LetterState::lsRack); break;
    }

    return QVariant();
}
