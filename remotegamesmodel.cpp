#include "remotegamesmodel.h"

#include <QDateTime>

remoteGamesModel::remoteGamesModel(QObject *parent)
    : QAbstractTableModel(parent),
    m_RemoteGames(0)
{
    /*
    QSortFilterProxyModel proxyModel;
    proxyModel.setSourceModel( this );
    table.setModel( &proxyModel );
*/
}

QVariant remoteGamesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    gameInfo aGame = m_RemoteGames.at(index.row());

    QVariant aReturn;
    switch (role) {
    case DateRole: aReturn = aGame.created; break;
    case LastAccessRole: aReturn = aGame.lastaccess; break;
    case PlayerNameRole: aReturn = aGame.players; break;
    case MovesRole: aReturn = aGame.moves; break;
    case NameRole: aReturn = aGame.fileName; break;
    case OwnTurnRole: aReturn = aGame.isOwnMove; break;
    case HasEndedRole: aReturn = aGame.hasEnded; break;
    } //switch
    return aReturn;
}

int remoteGamesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_RemoteGames.count();
}

int remoteGamesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QHash<int, QByteArray> remoteGamesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[DateRole] = "created";
    roles[LastAccessRole] = "lastaccess";
    roles[PlayerNameRole] = "playernames";
    roles[MovesRole] = "moves";
    roles[NameRole] = "filename";
    roles[OwnTurnRole] = "isOwnTurn";
    roles[HasEndedRole] = "hasEnded";

    return roles;
}

QString fromTDateTime(const double tDateTime)
{
    const double since01Jan1970 = 25569.16666; //TDateTime starts at December 30, 1899
    const int secPerDay = 24*60*60;
    int time_t = (int) ((tDateTime - since01Jan1970) * secPerDay);
    const QDateTime datetime = QDateTime::fromSecsSinceEpoch(time_t, Qt::LocalTime);
    QLocale loc;
    return datetime.toString(loc.dateTimeFormat(QLocale::ShortFormat));
}

bool sortByLastAccess(remoteGamesModel::gameInfo &p1, remoteGamesModel::gameInfo &p2) {
    return p1.lastaccess > p2.lastaccess;
}

void remoteGamesModel::setRemoteGames(const QVariantMap games, const QString sOwnName)
{
    gameInfo aGame;
    beginResetModel();
    m_RemoteGames.clear();
    int nRows = games["NumberOfGames"].toInt();
    for (int i = 0; i < nRows; i++) {
        QString sPrefix = "Game" + QString::number(i);
        aGame.created = fromTDateTime(games[sPrefix + ".Date"].toDouble());
        aGame.lastaccess = fromTDateTime(games[sPrefix + ".LastAccess"].toDouble());
        aGame.players = games[sPrefix + ".Player"].toString();
        aGame.moves = games[sPrefix + ".Moves"].toInt();
        aGame.fileName = games[sPrefix + ".Name"].toString();
        QStringList aSequence = aGame.players.split(",");
//FIXME: stored sequence of players not according the actual game
/*
#if defined(Q_OS_LINUX) && defined(QT_DEBUG)
        qDebug() << aSequence;
        qDebug() << aGame.moves;
        qDebug() << aSequence.count();
        qDebug() << aGame.moves % aSequence.count();
        qDebug() << aSequence[aGame.moves % aSequence.count()];
        qDebug() << sOwnName;
#endif
*/
        aGame.isOwnMove = aSequence[aGame.moves % aSequence.count()] == sOwnName;
        aGame.hasEnded = games[sPrefix + ".GameEnd"].toBool();
        /*
#if defined(Q_OS_LINUX) && defined(QT_DEBUG)
        qDebug() << games;
        qDebug() << aGame.fileName << aGame.isOwnMove << aGame.hasEnded;
#endif
*/
        m_RemoteGames.append(aGame);
    }
    std::sort(m_RemoteGames.begin(),m_RemoteGames.end(), sortByLastAccess);
    endResetModel();
}
/*
remoteGamesProxy::remoteGamesProxy(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool remoteGamesProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);
    qDebug() << leftData << rightData;
    if (leftData.userType() == QMetaType::QDateTime) {
        return leftData.toDateTime() < rightData.toDateTime();
    } else {
        static const QRegularExpression emailPattern("[\\w\\.]*@[\\w\\.]*");

        QString leftString = leftData.toString();
        if (left.column() == 1) {
            const QRegularExpressionMatch match = emailPattern.match(leftString);
            if (match.hasMatch())
                leftString = match.captured(0);
        }
        QString rightString = rightData.toString();
        if (right.column() == 1) {
            const QRegularExpressionMatch match = emailPattern.match(rightString);
            if (match.hasMatch())
                rightString = match.captured(0);
        }

        return QString::localeAwareCompare(leftString, rightString) < 0;
    }
}
*/
