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
    uint version; //3.x = 3, 4.x = 4
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
        version = Qt::UserRole + 7,
        mates = Qt::UserRole + 8,
        registered = Qt::UserRole + 9,
        rating = Qt::UserRole + 10,
        gamesPlayed = Qt::UserRole + 11,
        menuLang = Qt::UserRole + 12,
        allowKibitz = Qt::UserRole + 13,
        isKibitz = Qt::UserRole + 14,
        groupID = Qt::UserRole + 15,
        gameID = Qt::UserRole + 16,
        clientState = Qt::UserRole + 17,
        flag = Qt::UserRole + 18,
    };
    void playerTree(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    void addPlayer(clientInfo aPlayer);
    QDateTime fromTDateTime(const double tDateTime);
    QString fromTDateTimeF(const double tDateTime);
    void clearAll();
    const clientInfo playerData(QString name);
    const QString groupData(QString name);

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

