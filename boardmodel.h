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
        BonusTopRole,
        BonusBottomRole,
        BonusLeftRole,
        BonusRightRole
    };

    explicit boardmodel(QObject *parent = nullptr, board* aBoard = nullptr);

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    void updateSquare(Point3D aPos);
    void updateAllSquares();
    void updateAllFields();
    void reset();

protected:
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:
    board *m_pBoard;
};
