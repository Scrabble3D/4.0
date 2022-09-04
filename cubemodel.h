#pragma once

#include <QAbstractListModel>
#include "board.h"

class cubemodel : public QAbstractListModel
{
public:
    enum ModelRoles {
        FieldTypeRole = Qt::UserRole + 10,
        WhatRole,
        WhoRole,
        WhenRole,
        ValueRole,
        IsPlacedRole
    };
    explicit cubemodel(QObject *parent = nullptr, board* aBoard = nullptr);

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    void updateSquare(Point3D aPos);
    void reset();

protected:
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:
    board *m_pBoard;
};
