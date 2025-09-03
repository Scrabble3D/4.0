#include "playersmodel.h"

void playersTree::playerTree(QObject *parent)
{
    Q_UNUSED(parent);
}

unsigned constexpr const_hash(char const *input) {
    return *input ?
    static_cast<unsigned int>(*input) + 33 * const_hash(input + 1) :
    5381;
}

QString flagName(const QVariant aCountry) {
    switch (const_hash(aCountry.toString().toStdString().c_str())) {
    case const_hash("Afghanistan"): return "af.png";
    case const_hash("Albania"): return "al.png";
    case const_hash("Algeria"): return "dz.png";
    case const_hash("American Samoa"): return "as.png";
    case const_hash("Ancient Rome"): return "latin.png";
    case const_hash("Andorra"): return "ad.png";
    case const_hash("Angola"): return "ao.png";
    case const_hash("Anguilla"): return "ai.png";
    case const_hash("Antarctica"): return "aq.png";
    case const_hash("Antigua and Barbuda"): return "ag.png";
    case const_hash("Argentina"): return "ar.png";
    case const_hash("Armenia"): return "am.png";
    case const_hash("Aruba"): return "aw.png";
    case const_hash("Australia"): return "au.png";
    case const_hash("Austria"): return "at.png";
    case const_hash("Azerbaijan"): return "az.png";
    case const_hash("Bahrain"): return "bh.png";
    case const_hash("Bangladesh"): return "bd.png";
    case const_hash("Barbados"): return "bb.png";
    case const_hash("Belarus"): return "by.png";
    case const_hash("Belgium"): return "be.png";
    case const_hash("Belize"): return "bz.png";
    case const_hash("Benin"): return "bj.png";
    case const_hash("Bermuda"): return "bm.png";
    case const_hash("Bhutan"): return "bt.png";
    case const_hash("Bolivia"): return "bo.png";
    case const_hash("Bosnia and Herzegovina"): return "ba.png";
    case const_hash("Botswana"): return "bw.png";
    case const_hash("Bouvet Island"): return "bv.png";
    case const_hash("Brazil"): return "br.png";
    case const_hash("British Indian Ocean Territory"): return "io.png";
    case const_hash("British Virgin Islands"): return "vg.png";
    case const_hash("Brunei Darussalam"): return "bn.png";
    case const_hash("Bulgaria"): return "bg.png";
    case const_hash("Burkina Faso"): return "bf.png";
    case const_hash("Burundi"): return "bi.png";
    case const_hash("Cambodia"): return "kh.png";
    case const_hash("Cameroon"): return "cm.png";
    case const_hash("Canada"): return "ca.png";
    case const_hash("Catalonia"): return "catalonia.png";
    case const_hash("Cape Verde"): return "cv.png";
    case const_hash("Cayman Islands"): return "ky.png";
    case const_hash("Central African Republic"): return "cf.png";
    case const_hash("Chad"): return "td.png";
    case const_hash("Chile"): return "cl.png";
    case const_hash("China"): return "cn.png";
    case const_hash("Christmas Island"): return "cx.png";
    case const_hash("Cocos (Keeling) Islands"): return "cc.png";
    case const_hash("Colombia"): return "co.png";
    case const_hash("Comoros"): return "km.png";
    case const_hash("Congo"): return "cg.png";
    case const_hash("Cook Islands"): return "ck.png";
    case const_hash("Costa Rica"): return "cr.png";
    case const_hash("Cote D''Ivoire"): return "ci.png";
    case const_hash("Croatia"): return "hr.png";
    case const_hash("Cuba"): return "cu.png";
    case const_hash("Cyprus"): return "cy.png";
    case const_hash("Czech Republic"): return "cz.png";
    case const_hash("Denmark"): return "dk.png";
    case const_hash("Djibouti"): return "dj.png";
    case const_hash("Dominica"): return "dm.png";
    case const_hash("Dominican Republic"): return "do.png";
    case const_hash("East Timor"): return "tl.png";
    case const_hash("East Timor (Timor-Leste)"): return "tp.png";
    case const_hash("Ecuador"): return "ec.png";
    case const_hash("Egypt"): return "eg.png";
    case const_hash("El Salvador"): return "sv.png";
    case const_hash("England"): return "england.png";
    case const_hash("Equatorial Guinea"): return "gq.png";
    case const_hash("Eritrea"): return "er.png";
    case const_hash("Estonia"): return "ee.png";
    case const_hash("Ethiopia"): return "et.png";
    case const_hash("European Union"): return "europeanunion.png";
    case const_hash("Falkland Islands"): return "fk.png";
    case const_hash("Faroe Islands"): return "fo.png";
    case const_hash("Fiji"): return "fj.png";
    case const_hash("Finland"): return "fi.png";
    case const_hash("France"): return "fr.png";
    case const_hash("French Guiana"): return "gf.png";
    case const_hash("French Polynesia"): return "pf.png";
    case const_hash("French Southern Territories"): return "tf.png";
    case const_hash("Gabon"): return "ga.png";
    case const_hash("Georgia"): return "ge.png";
    case const_hash("Germany"): return "de.png";
    case const_hash("Ghana"): return "gh.png";
    case const_hash("Gibraltar"): return "gi.png";
    case const_hash("Greece"): return "gr.png";
    case const_hash("Greenland"): return "gl.png";
    case const_hash("Grenada"): return "gd.png";
    case const_hash("Guadeloupe"): return "gp.png";
    case const_hash("Guam"): return "gu.png";
    case const_hash("Guatemala"): return "gt.png";
    case const_hash("Guernsey and Alderney"): return "gg.png";
    case const_hash("Guinea"): return "gn.png";
    case const_hash("Guinea-Bissau"): return "gw.png";
    case const_hash("Guyana"): return "gy.png";
    case const_hash("Haiti"): return "ht.png";
    case const_hash("Heard and McDonald Island"): return "hm.png";
    case const_hash("Honduras"): return "hn.png";
    case const_hash("Hong Kong"): return "hk.png";
    case const_hash("Hungary"): return "hu.png";
    case const_hash("Iceland"): return "is.png";
    case const_hash("India"): return "in.png";
    case const_hash("Indonesia"): return "id.png";
    case const_hash("Iran"): return "ir.png";
    case const_hash("Iraq"): return "iq.png";
    case const_hash("Ireland"): return "ie.png";
    case const_hash("Isle of Man"): return "im.png";
    case const_hash("Israel"): return "il.png";
    case const_hash("Italy"): return "it.png";
    case const_hash("Jamaica"): return "jm.png";
    case const_hash("Japan"): return "jp.png";
    case const_hash("Jersey"): return "je.png";
    case const_hash("Jordan"): return "jo.png";
    case const_hash("Kazakhstan"): return "kz.png";
    case const_hash("Kenya"): return "ke.png";
    case const_hash("Kiribati"): return "ki.png";
    case const_hash("Kosovo"): return "kv.png";
    case const_hash("Kuwait"): return "kw.png";
    case const_hash("Kyrgyzstan"): return "kg.png";
    case const_hash("Laos"): return "la.png";
    case const_hash("Latvia"): return "lv.png";
    case const_hash("Lebanon"): return "lb.png";
    case const_hash("Lesotho"): return "ls.png";
    case const_hash("Liberia"): return "lr.png";
    case const_hash("Libya"): return "ly.png";
    case const_hash("Liechtenstein"): return "li.png";
    case const_hash("Lithuania"): return "lt.png";
    case const_hash("Luxembourg"): return "lu.png";
    case const_hash("Macao"): return "mo.png";
    case const_hash("Macedonia"): return "mk.png";
    case const_hash("Madagascar"): return "mg.png";
    case const_hash("Malawi"): return "mw.png";
    case const_hash("Malaysia"): return "my.png";
    case const_hash("Maldives"): return "mv.png";
    case const_hash("Mali"): return "ml.png";
    case const_hash("Malta"): return "mt.png";
    case const_hash("Marshall Islands"): return "mh.png";
    case const_hash("Martinique"): return "mq.png";
    case const_hash("Mauritania"): return "mr.png";
    case const_hash("Mauritius"): return "mu.png";
    case const_hash("Mayotte"): return "yt.png";
    case const_hash("Mexico"): return "mx.png";
    case const_hash("Micronesia"): return "fm.png";
    case const_hash("Moldova"): return "md.png";
    case const_hash("Monaco"): return "mc.png";
    case const_hash("Mongolia"): return "mn.png";
    case const_hash("Montenegro"): return "cs.png";
    case const_hash("Montserrat"): return "ms.png";
    case const_hash("Morocco"): return "ma.png";
    case const_hash("Mozambique"): return "mz.png";
    case const_hash("Myanmar"): return "mm.png";
    case const_hash("Namibia"): return "na.png";
    case const_hash("Nauru"): return "nr.png";
    case const_hash("Nepal"): return "np.png";
    case const_hash("Netherlands"): return "nl.png";
    case const_hash("Netherlands Antilles"): return "an.png";
    case const_hash("New Caledonia"): return "nc.png";
    case const_hash("New Zealand"): return "nz.png";
    case const_hash("Nicaragua"): return "ni.png";
    case const_hash("Niger"): return "ne.png";
    case const_hash("Nigeria"): return "ng.png";
    case const_hash("Niue"): return "nu.png";
    case const_hash("Norfolk Island"): return "nf.png";
    case const_hash("North Korea"): return "kp.png";
    case const_hash("Northern Mariana Islands"): return "mp.png";
    case const_hash("Norway"): return "no.png";
    case const_hash("Oman"): return "om.png";
    case const_hash("Pakistan"): return "pk.png";
    case const_hash("Palau"): return "pw.png";
    case const_hash("Palestinian Territory"): return "ps.png";
    case const_hash("Panama"): return "pa.png";
    case const_hash("Papua New Guinea"): return "pg.png";
    case const_hash("Paraguay"): return "py.png";
    case const_hash("Peru"): return "pe.png";
    case const_hash("Philippines"): return "ph.png";
    case const_hash("Pitcairn Island"): return "pn.png";
    case const_hash("Poland"): return "pl.png";
    case const_hash("Portugal"): return "pt.png";
    case const_hash("Puerto Rico"): return "pr.png";
    case const_hash("Qatar"): return "qa.png";
    case const_hash("Reunion"): return "re.png";
    case const_hash("Romania"): return "ro.png";
    case const_hash("Russia"): return "ru.png";
    case const_hash("Rwanda"): return "rw.png";
    case const_hash("Saint Barthelemy"): return "fr.png";
    case const_hash("Saint Helena"): return "sh.png";
    case const_hash("Saint Kitts and Nevis"): return "kn.png";
    case const_hash("Saint Lucia"): return "lc.png";
    case const_hash("Saint Martin"): return "fr.png";
    case const_hash("Saint Vincent and the Grenadines"): return "vc.png";
    case const_hash("Saint-Pierre und Miquelon"): return "pm.png";
    case const_hash("Samoa"): return "ws.png";
    case const_hash("San Marino"): return "sm.png";
    case const_hash("Sao Tome and Principe"): return "st.png";
    case const_hash("Saudi Arabia"): return "sa.png";
    case const_hash("Scotland"): return "scotland.png";
    case const_hash("Senegal"): return "sn.png";
    case const_hash("Serbia"): return "rs.png";
    case const_hash("Seychelles"): return "sc.png";
    case const_hash("Sierra Leone"): return "sl.png";
    case const_hash("Singapore"): return "sg.png";
    case const_hash("Slovakia"): return "sk.png";
    case const_hash("Slovenia"): return "si.png";
    case const_hash("Solomon Islands"): return "sb.png";
    case const_hash("Somalia"): return "SO.png";
    case const_hash("South Africa"): return "za.png";
    case const_hash("South Georgia and the South Sandwich Islands"): return "gs.png";
    case const_hash("South Korea"): return "kr.png";
    case const_hash("South Sudan"): return "ss.png";
    case const_hash("Spain"): return "es.png";
    case const_hash("Sri Lanka"): return "lk.png";
    case const_hash("Sudan"): return "sd.png";
    case const_hash("Suriname"): return "sr.png";
    case const_hash("Svalbard and Jan Mayen"): return "sj.png";
    case const_hash("Swaziland"): return "sz.png";
    case const_hash("Sweden"): return "se.png";
    case const_hash("Switzerland"): return "ch.png";
    case const_hash("Syrian Arab Republic"): return "sy.png";
    case const_hash("Taiwan"): return "tw.png";
    case const_hash("Tajikistan"): return "tj.png";
    case const_hash("Tanzania"): return "tz.png";
    case const_hash("Thailand"): return "th.png";
    case const_hash("The Bahamas"): return "bs.png";
    case const_hash("The Gambia"): return "gm.png";
    case const_hash("Togo"): return "tg.png";
    case const_hash("Tokelau"): return "tk.png";
    case const_hash("Tonga"): return "to.png";
    case const_hash("Trinidad and Tobago"): return "tt.png";
    case const_hash("Tunisia"): return "tn.png";
    case const_hash("Turkey"): return "tr.png";
    case const_hash("Turkmenistan"): return "tm.png";
    case const_hash("Turks and Caicos Islands"): return "tc.png";
    case const_hash("Tuvalu"): return "tv.png";
    case const_hash("Uganda"): return "ug.png";
    case const_hash("Ukraine"): return "ua.png";
    case const_hash("United Arab Emirates"): return "ae.png";
    case const_hash("United Kingdom"): return "gb.png";
    case const_hash("United States"): return "us.png";
    case const_hash("United States Minor Outlying Islands"): return "um.png";
    case const_hash("United States Virgin Islands"): return "vi.png";
    case const_hash("Uruguay"): return "uy.png";
    case const_hash("Uzbekistan"): return "uz.png";
    case const_hash("Vanuatu"): return "vu.png";
    case const_hash("Vatican"): return "va.png";
    case const_hash("Venezuela"): return "ve.png";
    case const_hash("Viet Nam"): return "vn.png";
    case const_hash("Wales"): return "wales.png";
    case const_hash("Wallis and Futuna"): return "wf.png";
    case const_hash("Western Sahara"): return "eh.png";
    case const_hash("Yemen"): return "ye.png";
    case const_hash("Zambia"): return "zm.png";
    case const_hash("Zimbabwe"): return "zw.png";
    default:
        return "unknown";
    }
}

QVariant playersTree::data(const QModelIndex &index, int role) const
{
    QVariant aValue;

    if (index.isValid())
    {
        if (role == flag)
            aValue = flagName(QStandardItemModel::data(index, country));
        else if (role == registered) {
            QLocale loc;
            QDateTime tmp = QStandardItemModel::data(index, registered).toDateTime();
            aValue = tmp.toString(loc.dateTimeFormat(QLocale::ShortFormat));
        }
        else
           aValue = QStandardItemModel::data(index, role);
    }

    return aValue;
}

QHash<int, QByteArray> playersTree::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[playerName] = "name";
    roles[country] = "country";
    roles[city] = "city";
    roles[gameStarted] = "gameStarted";
    roles[moveNumber] = "moveNumber";
    roles[release] = "release";
    roles[version] = "version";
    roles[mates] = "mates";
    roles[registered] = "registered";
    roles[rating] = "rating";
    roles[gamesPlayed] = "gamesPlayed";
    roles[menuLang] = "menuLang";
    roles[allowKibitz] = "allowKibitz";
    roles[isKibitz] = "isKibitz";
    roles[groupID] = "groupID";
    roles[gameID] = "gameID";
    roles[isOpenForGames] = "isOpenForGames";
    roles[flag] = "flag";
    return roles;
}

/*
QModelIndex playersTree::playerIndex(QString name)
{
    QModelIndex aIndex;
    for(int i=0, maxRow=rowCount(); i<maxRow;++i)
    {
        aIndex = index(i,0);
        if (data(aIndex, playerName) == name)
            return aIndex;
    }
    return index(-1,-1);
}
*/

// TDateTime: https://wiki.freepascal.org/TDateTime
QDateTime playersTree::fromTDateTime(const double tDateTime)
{
    const double since01Jan1970 = 25569.16666; //TDateTime starts at December 30, 1899
    const int secPerDay = 24 * 60 * 60;
    int time_t = (int) ((tDateTime - since01Jan1970) * secPerDay);
    return QDateTime::fromSecsSinceEpoch(time_t, Qt::LocalTime);
}

QString playersTree::fromTDateTimeF(const double tDateTime)
{
    QLocale loc;
    return fromTDateTime(tDateTime).toString(loc.dateTimeFormat(QLocale::ShortFormat));
}

void playersTree::clearAll()
{
    beginResetModel();
    clear();
    m_lPlayers.clear();
    endResetModel();
}

const clientInfo playersTree::playerData(QString name)
{
    int nIndex = playerIndex(name);
    if ( nIndex > -1 )
        return m_lPlayers.at(nIndex);
    else
        return {};
}

const QString playersTree::groupData(QString name)
{
    int nLocalPlayer = playerIndex(name);
    QStringList groupInfo;
    for (int i = 0; i < m_lPlayers.count(); i++)
        if (m_lPlayers.at(i).groupID == m_lPlayers.at(nLocalPlayer).groupID)
            groupInfo.append(m_lPlayers.at(i).playerName);
    return groupInfo.join(",");
}

void playersTree::doLogout(QString playerName)
{
    for (int i = 0; i < m_lPlayers.count(); i++)
        if (m_lPlayers[i].playerName == playerName)
        {
            m_lPlayers.removeAt(i);
            updateTree();
            return;
        }
}

void playersTree::doRefresh(QVariantMap msg)
{
    clientInfo aClient;
    int nPlayer = msg["Count"].toInt(0);
    for (int i = 0; i < nPlayer; i++) {
        aClient.playerName = msg[QString::number(i) + "_Name"].toString();
        int nPlayerIndex = playerIndex(aClient.playerName);
        if (nPlayerIndex > -1) {
            m_lPlayers[nPlayerIndex].rating = msg[QString::number(i) + "_Rating"].toUInt();
            m_lPlayers[nPlayerIndex].groupID = msg[QString::number(i) + "_Group"].toUInt();
            m_lPlayers[nPlayerIndex].gameID = msg[QString::number(i) + "_GameID"].toUInt();
            m_lPlayers[nPlayerIndex].gamesPlayed = msg[QString::number(i) + "_GamesPlayed"].toUInt();
            m_lPlayers[nPlayerIndex].isKibitz = msg[QString::number(i) + "_IsKibitz"].toBool();
            m_lPlayers[nPlayerIndex].gameStarted = fromTDateTime(msg[QString::number(i) + "_GameStarted"].toDouble());
            m_lPlayers[nPlayerIndex].moveNumber = msg[QString::number(i) + "_MoveNumber"].toInt();
            m_lPlayers[nPlayerIndex].allowKibitz = msg[QString::number(i) + "_AllowKibitz"].toBool();
            QString aState = msg[QString::number(i) + "_ClientState"].toString();
            // ignore the states csAfk and csBusy
            if (aState == "csNotOpen")
                m_lPlayers[nPlayerIndex].isOpenForGames = false;
            else if (aState == "csOpenForGames")
                m_lPlayers[nPlayerIndex].isOpenForGames = true;
        }
    }
    updateTree();
}

int playersTree::playerIndex(QString sPlayerName)
{
    for (int i=0; i<m_lPlayers.count(); i++)
        if (m_lPlayers.at(i).playerName == sPlayerName)
            return i;

    return -1;
}

void playersTree::addPlayer(clientInfo aPlayer)
{
    if (playerIndex(aPlayer.playerName) == -1)
        m_lPlayers.append(aPlayer);
}

bool sortByGroup(clientInfo aClient, clientInfo bClient) {
    return aClient.groupID < bClient.groupID;
}

void playersTree::updateTree()
{
    beginResetModel();
    clear();

    // game started info with locale setting
    QLocale loc;

    std::sort(m_lPlayers.begin(), m_lPlayers.end(), sortByGroup);

    QStandardItem *gameNode = new QStandardItem();
    gameNode->setData("", playerName);

    for (int i = 0; i < m_lPlayers.count(); i++)
    {
        if ( (i > 0) &&
             (m_lPlayers.at(i).groupID != m_lPlayers.at(i-1).groupID))
        {
            gameNode = new QStandardItem();
            gameNode->setData("", playerName);
        }
        if ((gameNode->data(playerName) == "") && //new game node
            ( (i < m_lPlayers.count()-1) && //not the last item
              ((m_lPlayers.at(i+1).groupID == m_lPlayers.at(i).groupID) || //first player of a group
               (m_lPlayers.at(i).gameStarted > QDateTime::fromSecsSinceEpoch(0)) //game has started
           )))
        {
//TODO: playersmodel: suspect test for game started
            if ( QString::number(m_lPlayers[i].moveNumber).toInt() > 0)
                gameNode->setData(tr("Game started %1, move #%2").arg(
                                      m_lPlayers[i].gameStarted.toString(loc.dateTimeFormat(QLocale::ShortFormat)),
                                      QString::number(m_lPlayers[i].moveNumber + 1)),
                                  playerName);
            else
                gameNode->setData(tr("joined"), playerName);
            invisibleRootItem()->appendRow(gameNode);
        }

        QStandardItem *aNode = new QStandardItem();
        aNode->setData(m_lPlayers[i].playerName, playerName);
        aNode->setData(m_lPlayers[i].menuLang, menuLang);
        aNode->setData(m_lPlayers[i].country, country);
        aNode->setData(m_lPlayers[i].city, city);
        aNode->setData(m_lPlayers[i].release, release);
        aNode->setData(m_lPlayers[i].version, version);
        aNode->setData(m_lPlayers[i].registered, registered);
        aNode->setData(m_lPlayers[i].rating, rating);
        aNode->setData(m_lPlayers[i].groupID, groupID);
        aNode->setData(m_lPlayers[i].gameID, gameID);
        aNode->setData(m_lPlayers[i].gamesPlayed, gamesPlayed);
        aNode->setData(m_lPlayers[i].isKibitz, isKibitz);
        aNode->setData(m_lPlayers[i].gameStarted, gameStarted);
        aNode->setData(m_lPlayers[i].moveNumber, moveNumber);
        aNode->setData(m_lPlayers[i].allowKibitz, allowKibitz);
        aNode->setData(m_lPlayers[i].isOpenForGames, isOpenForGames);

        if (gameNode->data(playerName) != "")
            gameNode->appendRow(aNode);
        else
            invisibleRootItem()->appendRow(aNode);
    }

    endResetModel();
}
