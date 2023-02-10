#pragma once

#include <QAbstractListModel>
#include "board.h"

class boardmodel : public QAbstractListModel
{
public:
    enum ModelRoles {
        FieldTypeRole = Qt::UserRole + 1,
        WhatRole,
        ValueRole,
        WhoRole,
        WhenRole,
        IsPlacedRole,
        IsJokerRole,
        IsRandomRole,
        BonusTopRole,
        BonusBottomRole,
        BonusLeftRole,
        BonusRightRole
    };

    explicit boardmodel(QObject *parent = nullptr, board* aBoard = nullptr);

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;

    void reset(); //resetmodel() -> after initialization of board the boardmodel needs to be updated
    void updateSquare(Point3D aPos); // after dropletter -> WhatRole ... IsPlacedRole
    void updateAllSquares(); //after placeletters() -> WhatRole ... IsPlacedRole
    void updateAllFields(); //after setdimension() -> WhatRole ... IsPlacedRole, FieldTypeRole ... BonusRole

protected:
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:
    board *m_pBoard;
};
