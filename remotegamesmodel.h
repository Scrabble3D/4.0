#pragma once

#include <QObject>
#include <QAbstractTableModel>

//TODO: remotegames: sort
/*
#include <QSortFilterProxyModel>

class remoteGamesProxy: public QSortFilterProxyModel
{
    Q_OBJECT

public:
    remoteGamesProxy(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};
*/
class remoteGamesModel: public QAbstractTableModel
{
    Q_OBJECT

public:
    enum ModelRoles {
        DateRole = Qt::UserRole + 1,
        LastAccessRole = Qt::UserRole + 2,
        PlayerNameRole = Qt::UserRole + 3,
        MovesRole = Qt::UserRole + 4,
        NameRole = Qt::UserRole + 5,
        OwnTurnRole = Qt::UserRole + 6,
        HasEndedRole = Qt::UserRole + 7,
    };

    explicit remoteGamesModel(QObject *parent = nullptr);;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    void setRemoteGames(const QVariantMap games, const QString sOwnName);

protected:
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

public:
    struct gameInfo {
        QString created;
        QString lastaccess;
        QString players;
        int moves;
        QString fileName;
        bool isOwnMove;
        bool hasEnded;
    };

private:
    QList<gameInfo> m_RemoteGames;
};
