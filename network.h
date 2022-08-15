#pragma once

#include <QtNetwork>

#include <QVariant>
#include <QList>
#include <QAbstractTableModel>

class network : public QTcpSocket
{
    Q_OBJECT

public:
    enum MessageType {
        nwConnect,       // a player/me has connected -> gameplay::doNetworkLogin
        nwLogin,         // a player/me has disconnected -> gameplay::doNetworkDisconnect
        nwLogout,        // server denies connection, eg. wrong password or too many connections
        nwDisconnect,
        nwRefresh,       // updated on player data, eg. new group, move -> playerstree::doRefresh
        nwInvite,        // gameplay::doInvite() invitation received to join a group, gameplay::doNetworkInvite
        nwLeave,         // send = about leaving a group, receive = abort running game in TODO: network nwLeave
        nwJoin,          // send = invitation accepted, receive = loadgame in TODO: network nwJoin
        nwInformation,   // static information received, eg. feedback or finger/best -> gameplay::doNetworkInfo
        nwChat,          // chat message received, emits onChat(), passed via gameplay to -> msgmodel::addMessage
        nwPoll,          // suspend the running game and ask all mates whether a word should be accepted or a new game started, for example -> gameplay::doNetworkPoll
        nwAnswer,        // answer to poll; QString::number(true/false)
        nwSyncNewGame,   // game configuration sent by the player who initiated the new game -> gameplay::syncNewGame
        nwNextPlayer,    // submit move within group
        nwRemoteGames,   // retrieves a list of games on server that can be loaded (and continued if not ended)
        nwLoadGame,      // loads a game stored on the server by its consecutive number
        nwGameResult,    // updates rating; also sets GameEnd flag in saved game on game server, returns message with new rating
        /*
    'nwAnswer'       : DoAnswer;
    'nwPoll'         : DoPoll;
    'nwPause'        : acPauseExecute(nil);
    'nwKibitz'       : DoKibitz;
    'nwCambioSecco'  : DoCambioSecco;
    'nwJokerize'     : DoJokerize(TLetter(Scrabble.Letters[aMsg.Value('LetterIndex',-1)]));
    'nwCheckWord'    : DoCheckWord;
    'nwHighscore'    : DoHighscore;
    'nwChallenge'    : DoChallengeCall;
    'nwEndGame'      : if (gsRunning in Scrabble.GameState) then DoGameEnd(self);
    'nwAddTime'      : DoAddTime;
    'nwBestValues'   : GameCourse.AllBestMovesValues:=aMsg.Value('BestValues','');
    'nwPing'         : Pinged:=false;
    'nwLostByTime'   : DoLostByTime;
*/

    };
    Q_ENUM(MessageType);

    network(QObject *parent = nullptr,
            const QString name = "", const QString password = "", const QString email = "",
            const QString country = "", const QString city = "");
    void connect(const QString sHostname, const uint iPort); //connect to host and send nwConnect message
    const QString localPlayerName() { return m_sName; }

public slots:
    void doSend(const network::MessageType msgType, QString msgReceiver, QString msgText);

signals:
    // void onError(int socketError, const QString &message); //gameplay
    void onLogin(QVariantMap aMsg); //gameplay
    void onLogout(QString aMsg); //playersmodel
    void onDisconnect(QString aMsg); //gameplay
    void onRefresh(QVariantMap aMsg); //playersmodel
    void onInfo(QVariantMap aMsg); //gameplay
    void onChat(const QString aWhat, const QString aWho); //gameplay
    void onInvite(QVariantMap aMsg); //gameplay
    void onJoin(QVariantMap aMsg); //gameplay
//    void onLeave(QVariantMap aMsg); //gameplay
    void onSyncNewGame(QVariantMap aMsg); //gameplay
    void onNextPlayer(QVariantMap aMsg); //gameplay
    void onRemoteGames(QVariantMap aMsg); //gameplay
    void onPoll(QVariantMap aMsg); //gameplay
    void onAnswer(QVariantMap aMsg); //gameplay
    void onGameResult(int oldRating, int newRating); //gameplay

private slots:
    void doConnected();
    void doReadyRead();

private:
    QObject *m_pParent;

    QString m_sName;
    QString m_sPassword;
    QString m_sEmail;
    QString m_sCountry;
    QString m_sCity;

    QByteArray m_aData;
};

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

private:
    struct gameInfo {
        QString created;
        QString lastaccess;
        QString players;
        int moves;
        QString fileName;
        bool isOwnMove;
        bool hasEnded;
    };
    QList<gameInfo> m_RemoteGames;
};
