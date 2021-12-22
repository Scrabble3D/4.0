#include "boardmodel.h"

boardmodel::boardmodel(QObject *parent, board* aBoard)
    : QAbstractListModel(parent),
      m_pBoard(aBoard)
{
    Q_UNUSED(parent);
}

int boardmodel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_pBoard->getBoardsize() * m_pBoard->getBoardsize();
}

void boardmodel::reset()
{
    beginResetModel();
    endResetModel();
}

void boardmodel::updateSquare(Point3D aPos)
{
    QModelIndex aIndex = this->index( m_pBoard->Index2D(aPos) );
    emit dataChanged(aIndex, aIndex, { WhatRole, ValueRole, WhoRole, WhenRole, IsPlacedRole } );
}

void boardmodel::updateAllSquares()
{
    QModelIndex aIndex;
    int z = m_pBoard->getBoardsize();
    for (int i=0; i<z; i++)
     for (int j=0; j<z; j++)
     {
         aIndex = this->index( i * m_pBoard->getBoardsize() + j );
         emit dataChanged(aIndex, aIndex, { WhatRole, ValueRole, WhoRole, WhenRole, IsPlacedRole } );
     }
}

void boardmodel::updateAllFields()
{
    QModelIndex aIndex;
    for (int i=0; i<m_pBoard->getBoardsize(); i++)
     for (int j=0; j<m_pBoard->getBoardsize(); j++)
     {
         aIndex = this->index( i * m_pBoard->getBoardsize() + j );
         emit dataChanged(aIndex, aIndex, { WhatRole, ValueRole, WhoRole, WhenRole, IsPlacedRole, FieldTypeRole, BonusTopRole, BonusBottomRole, BonusLeftRole, BonusRightRole } );
     }
}

QHash<int, QByteArray> boardmodel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[WhatRole] = "what";
    roles[ValueRole] = "value";
    roles[WhoRole] = "who";
    roles[WhenRole] = "when";
    roles[IsPlacedRole] = "isplaced";
    roles[FieldTypeRole] = "fieldtype";
    roles[BonusTopRole] = "bonustop";
    roles[BonusBottomRole] = "bonusbottom";
    roles[BonusLeftRole] = "bonusleft";
    roles[BonusRightRole] = "bonusright";
    return roles;
}

QVariant boardmodel::data(const QModelIndex &index, int role) const
{
    Letter aLetter = EmptyLetter;
    int z = m_pBoard->PointToWhere( m_pBoard->Pos3D(index.row()) );

    if (((role == WhatRole) || (role == ValueRole) || (role == WhoRole) ||
         (role == WhenRole) ||(role == IsPlacedRole)
        ) && (z < m_pBoard->getFieldsize()) //if board is not yet initialized
       ) aLetter = m_pBoard->getLetter(z);

    switch (role) {
      case WhatRole:
        return aLetter.What;
        break;
      case ValueRole:
        return aLetter.Value;
        break;
      case WhoRole:
        return aLetter.Who;
        break;
      case WhenRole:
        return aLetter.When;
        break;
      case IsPlacedRole:
        return (aLetter.State == LetterState::lsPlaced);
        break;
      case FieldTypeRole:
        return (m_pBoard->getFieldtype(z));
        break;
      case BonusTopRole ... BonusRightRole:
      {
        int x = -1;
        int y = m_pBoard->Index2D(m_pBoard->Pos3D(index.row()));
        FieldType aBonusField = ftStart; //ftStart "bonus" will be transparent

        if ((role == BonusTopRole) && (y - m_pBoard->getBoardsize() > 0))
           x = m_pBoard->PointToWhere( m_pBoard->Pos3D(y - m_pBoard->getBoardsize()) );
        else if ((role == BonusBottomRole) && (y + m_pBoard->getBoardsize() < m_pBoard->getBoardsize() * m_pBoard->getBoardsize()))
           x = m_pBoard->PointToWhere( m_pBoard->Pos3D(y + m_pBoard->getBoardsize()) );
        else if ((role == BonusLeftRole) && (y % m_pBoard->getBoardsize() > 0))
           x = m_pBoard->PointToWhere( m_pBoard->Pos3D( y - 1) );
        else if ((role == BonusRightRole) && ((y+1) % m_pBoard->getBoardsize() > 0))
           x = m_pBoard->PointToWhere( m_pBoard->Pos3D( y + 1) );

        if ((x >= 0) && (x < m_pBoard->getFieldsize()))
            aBonusField = m_pBoard->getFieldtype(x);
        if (aBonusField == ftDefault)
            aBonusField = ftStart;
        return aBonusField;
        break;
      }
    }

    return QVariant();
}
