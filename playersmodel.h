#pragma once

#include <QTime>
#include <QMetaType>
#include <QStandardItemModel>

enum EclientState { csOpenForGames,csNotOpen,csAfk,csBusy };

struct clientInfo {
    QString playerName;
    QString country;
    QString city;
    QDateTime gameStarted;
    uint moveNumber;
    QString release;
    QStringList mates;
    QDateTime registered;
    uint rating;
    uint gamesPlayed;
    QString menuLang;
    bool allowKibitz;
    bool isKibitz;
    uint groupID;
    uint gameID;
    EclientState clientState;
    bool isExpanded;
};
Q_DECLARE_METATYPE(clientInfo);

class playersTree : public QStandardItemModel
{
    Q_OBJECT

public:
    enum ModelRoles {
        playerName = Qt::UserRole + 1,
        icon = Qt::UserRole + 17,
        country = Qt::UserRole + 2,
        city = Qt::UserRole + 3,
        gameStarted = Qt::UserRole + 4,
        moveNumber = Qt::UserRole + 5,
        release = Qt::UserRole + 6,
        mates = Qt::UserRole + 7,
        registered = Qt::UserRole + 8,
        rating = Qt::UserRole + 9,
        gamesPlayed = Qt::UserRole + 10,
        menuLang = Qt::UserRole + 11,
        allowKibitz = Qt::UserRole + 12,
        isKibitz = Qt::UserRole + 13,
        groupID = Qt::UserRole + 14,
        gameID = Qt::UserRole + 15,
        clientState = Qt::UserRole + 16,
        flag = Qt::UserRole + 17
    };
    void playerTree(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    void addPlayer(clientInfo aPlayer);
    QDateTime fromTDateTime(const double tDateTime);
    QString fromTDateTimeF(const double tDateTime);
    void clearAll();
    const clientInfo playerData(QString name);
    const QStringList groupData(QString name);

public slots:
    void doLogout(QString playerName);
    void doRefresh(QVariantMap msg);

protected:
    QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:
    int playerIndex(QString sPlayerName);
    QList<clientInfo> m_lPlayers;

    void updateTree(); //create root nodes for games and makes resp. players children of it
};

