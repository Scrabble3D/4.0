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

void rackmodel::removeLetter(const int index)
{
    if ( (index>=0) && (index<m_lPieces.count()) )
    {
        beginRemoveRows(QModelIndex(), index, index);
        m_lPieces.remove(index);
        endRemoveRows();
    }
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
    roles[IsVisibleRole] = "isVisible";
    return roles;
}

QVariant rackmodel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const Letter aLetter = m_lPieces[index.row()];
    switch (role) {
      case WhatRole:
        return aLetter.What;
        break;
      case ValueRole:
        return aLetter.Value;
        break;
      case IsVisibleRole: //TODO: pbly never read
        return (aLetter.State == LetterState::lsRack);
        break;
    }

    return QVariant();
}
