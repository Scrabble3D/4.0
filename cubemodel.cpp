#include "cubemodel.h"
#include "board.h"

cubemodel::cubemodel(QObject *parent, board* aBoard)
    : QAbstractListModel(parent),
      m_pBoard(aBoard)
{
}

int cubemodel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_pBoard->getBoardSize() * m_pBoard->getBoardSize() * m_pBoard->getBoardSize();
}

void cubemodel::updateSquare(Point3D aPos)
{
    QModelIndex aIndex = this->index( m_pBoard->pointToWhere(aPos) );
    emit dataChanged(aIndex, aIndex, { WhatRole, ValueRole, WhoRole, WhenRole, IsPlacedRole } );
}

void cubemodel::reset()
{
    beginResetModel();
    endResetModel();
}

QHash<int, QByteArray> cubemodel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[FieldTypeRole] = "fieldtype";
    roles[WhatRole] = "what";
    roles[WhoRole] = "who";
    roles[WhenRole] = "when";
    roles[ValueRole] = "value";
    roles[IsPlacedRole] = "isPlaced";
    return roles;
}

QVariant cubemodel::data(const QModelIndex &index, int role) const
{
    Letter aLetter = EmptyLetter;
    const int z = index.row();

    if ((role == WhatRole) ||
        (role == WhoRole) ||
        (role == WhenRole) ||
        (role == ValueRole) ||
        (role == IsPlacedRole)) {
        if (z < m_pBoard->getFieldSize()) //if board is not yet initialized
            aLetter = m_pBoard->getLetter(z);
    }
    else if (z > m_pBoard->getFieldSize())
        return(1); //ftDefault

    switch (role) {
      case WhatRole:
        return aLetter.What;
        break;
      case WhoRole:
        return aLetter.Who;
        break;
      case WhenRole:
        return aLetter.When;
        break;
      case ValueRole:
        return aLetter.Value;
        break;
      case IsPlacedRole:
        return (aLetter.State == LetterState::lsPlaced);
        break;
      case FieldTypeRole:
        return (m_pBoard->getFieldtype(z));
        break;
    }

    return QVariant();
}
