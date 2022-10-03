#include "network.h"
#include "version.h"

network::network(QObject* parent, const QString name, const QString password, const QString email,
                 const QString country, const QString city)
    : m_pParent(parent),
      m_sName(name),
      m_sPassword(password),
      m_sEmail(email),
      m_sCountry(country),
      m_sCity(city)
{
//    this->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    this->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
}

void network::connect(const QString sHostname, const uint iPort)
{
    QObject::connect(this, SIGNAL(connected()),
                     this, SLOT(doConnected()));
    QObject::connect(this, SIGNAL(readyRead()),
                     this, SLOT(doReadyRead()));
    //disconnect -> gameplay::doNetworkDisconnect
    //errorOccurred -> gameplay::doNetworkError
    this->connectToHost(sHostname, iPort);
    //TODO: network: feedback when fail
}

void network::doSend(const MessageType msgType, QString msgReceiver, QString msgText)
{
    if (!msgText.isEmpty()) msgText += "\a";
    msgText += "MessageType=" + QVariant::fromValue(msgType).toString() + "\a";
    msgText += "Sender=" + m_sName + "\a"; //TODO: network: variable login
    if (msgReceiver.isEmpty()) msgReceiver = m_sName;
    msgText += "Receiver=" + msgReceiver + "\a";
    msgText += "\r\n";
    this->write( msgText.toUtf8() );
//    this->waitForBytesWritten(1000);
    this->flush();
#ifdef QT_DEBUG
    qDebug() << "sent" << QVariant::fromValue(msgType).toString() << "from" << m_sName;
#endif

}

void network::doConnected()
{
    QString sOS;

#ifdef Q_OS_WINDOWS
    sOS = "Windows";
#endif

#ifdef Q_OS_LINUX
    sOS = "Linux";
#endif

#ifdef Q_OS_MACOS
    sOS = "macOS";
#endif

#ifdef Q_OS_ANDROID //needs to come after Linux
    sOS = "Android";
#endif

#ifdef Q_OS_IOS
    sOS = "iOS";
#endif

    QStringList lData;
    lData.append("Password=" + m_sPassword);
    lData.append("MenuLang=en");//TODO: network: menu lang
    lData.append("Version=5"); //server version, must not be <4
    lData.append("Country=" + m_sCountry);
    lData.append("City=" + m_sCity);
    lData.append("UID="); //TODO: network: mac address
    lData.append("Email=" + m_sEmail);
    lData.append("Release=" + version::current() + "_" + sOS); //v3.1 uses underscore to separate platform

    doSend(nwConnect, m_sName, lData.join("\a"));
}

void network::doReadyRead()
{
    if (!this->bytesAvailable())
        return;

    QVariantMap aLastMessage;
    //NOTE: network: double check https://stackoverflow.com/questions/16023533/how-to-make-sure-that-readyread-signals-from-qtcpsocket-cant-be-missed
    m_aData += this->readAll();

    if (!m_aData.endsWith("\n"))
        return;

    int z = m_aData.indexOf("\r\n");
    while (z > -1) {
        //convert to QVariantMap
        QList lData( m_aData.first(z).split(0x0007) ); // "\a"
        m_aData.remove(0, z + 2);

        QStringList aLine;
        aLastMessage.clear();
        for (int i=0; i<lData.count(); i++) {
            aLine.clear();
            aLine << static_cast<QString>(lData.at(i)).split("=");
            if (aLine.isEmpty() || aLine[0].isEmpty() || (aLine[0] == "\r\n"))
                continue;
            if (aLine.count() != 2) {
                qWarning() << "Cannot parse line" << aLine << ": " << lData.at(i) << ": " << m_aData;
                break; //FIXME: network: remove break
            }
            aLastMessage[aLine[0]] = aLine[1];
        }

        //execute action
        auto metaEnum = QMetaEnum::fromType<MessageType>();
        switch (metaEnum.keyToValue(aLastMessage["MessageType"].toString().toStdString().c_str())) {
        //a player/me has connected -> gameplay::doNetworkLogin
        case nwLogin: emit onLogin(aLastMessage); break;
        //a player/me has disconnected -> gameplay::doNetworkDisconnect
        case nwLogout: emit onLogout(aLastMessage["Sender"].toString()); break;
        //server denies connection, eg. wrong password or too many connections
        case nwDisconnect: emit onDisconnect(aLastMessage["Message"].toString()); break;
        //updated on player data, eg. new group, move -> playerstree::doRefresh
        case nwRefresh: emit onRefresh(aLastMessage); break;
        //static information received, eg. feedback or finger/best -> gameplay::doNetworkInfo
        case nwInformation: emit onInfo(aLastMessage); break;
        //chat message received, emits onChat(), passed via gameplay to -> msgmodel::addMessage
        case nwChat: {
            QString sChatString;
            if (aLastMessage["Receiver"] == "all")
                sChatString = tr("%1 shouts: %2");
            else if (aLastMessage["Receiver"] == "group")
                sChatString = tr("%1 says: %2");
            else if (aLastMessage["Receiver"] == "kibitzes")
                sChatString = tr("%1 tattles: %2");
            else
                sChatString = tr("%1 whispers: %2");

            emit onChat(sChatString.arg(
                            aLastMessage["Sender"].toString(),
                            aLastMessage["Chat"].toString()),
                        aLastMessage["Sender"].toString());
        } break;
        //insert answer into poll -> gameplay::doNetworkAnswer
        case nwAnswer: emit onAnswer(aLastMessage); break;
        //gameplay::doInvite() invitation received to join a group, gameplay::doNetworkInvite
        case nwInvite: emit onInvite(aLastMessage); break;
        //send = about leaving a group, receive = abort running game in TODO: network nwLeave
        case nwLeave: {
            //TODO: network: doSend(nwLoadGame,"","");
            //reset game
            //TODO: network: kibitz must not be the last player in a group!
            doSend(nwRefresh, "all", "");
        } break;
        //send = invitation accepted, receive = loadgame in TODO: network nwJoin
        case nwJoin: emit onJoin(aLastMessage); break;
        case nwSyncNewGame: emit onSyncNewGame(aLastMessage); break;
        case nwNextPlayer: emit onNextPlayer(aLastMessage); break;
        case nwCheckWord: emit onChallengeMove(aLastMessage["Sender"].toString()); break;
        case nwChallenge: emit onChallengeResult(aLastMessage); break;
        case nwRemoteGames: emit onRemoteGames(aLastMessage); break;
        case nwBestValues: break; //TODO: network: bestvalues into gamrcourse
        case nwGameResult: {
            if (aLastMessage["Sender"] == m_sName)
                emit onGameResult(aLastMessage["OldRating"].toInt(), aLastMessage["NewRating"].toInt());
        } break;
            //TODO: network: unknown message should be readable, eg. in case of /tell Foo (instead of whisper)
        default: qWarning() << "Unknown message: " << aLastMessage["MessageType"].toString();
        }
        z = m_aData.indexOf("\r\n"); //in case of more than one message
    }
    m_aData.clear();
#ifdef QT_DEBUG
    qDebug() << "received" << aLastMessage["MessageType"].toString() << "from" << aLastMessage["Sender"].toString();
#endif
}

