#include "gamecoursemodel.h"
#include <QDateTime>

gamecoursemodel::gamecoursemodel(QObject *parent)
    : QAbstractTableModel(parent),
      m_lData(0)
{
}

QVariant gamecoursemodel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const int i = m_lData.count() - index.row() - 1;
    if ((i<0) || (i>m_lData.count()-1))
        return QVariant();

    ModelData aData = m_lData[i];

    switch (role) {
      case Qt::DisplayRole:
        if (index.column() == 0) //move #
            return i+1;
        else if (index.column() == 1) //word
            return aData.placedWord;
        else if (index.column() == 2) //value
            return aData.value;
        else if (index.column() == 3) //best value
            return aData.bestValue;
        else if (index.column() == 4) //move time
        {
            QTime aTime(0,0,0);
            aTime = aTime.addSecs(aData.time);
            return aTime.toString("mm:ss");
        }
        break;
      case WhoRole:
        return aData.who;
        break;
      case ConnectedWords:
        return aData.connectedWords;
        break;
      case IsScrabbleRole:
        return aData.isScrabble;
        break;
      case TimeRole:
        return aData.time;
        break;
    }

    return QVariant();

}

QVariant gamecoursemodel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
    {
        if (section == 0)
            return tr("#");
        else if (section == 1)
            return tr("Word");
        else if (section == 2)
            return tr("Value");
        else if (section == 3)
            return tr("Best");
        else if (section == 4)
            return tr("Time");
    }
    return QVariant();
}

int gamecoursemodel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_lData.count();
}

int gamecoursemodel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QHash<int, QByteArray> gamecoursemodel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[WhoRole] = "who";
    roles[ConnectedWords] = "connectedWords";
    roles[IsScrabbleRole] = "isScrabble";
    roles[BestValueRole] = "bonus";
    roles[TimeRole] = "time";
    return roles;
}

void gamecoursemodel::clear()
{
    beginResetModel();
    for (int i=0; i<MaxPlayers; i++)
        m_Total[i] = 0;
    m_lData.clear();
    endResetModel();
}

void gamecoursemodel::addMove(QString PlacedWord, QString ConnectedWords, uint Who, int value, int bestValue, bool IsScrabble, int moveTime)
{
    ModelData aData;
    aData.placedWord = PlacedWord;
    aData.connectedWords = ConnectedWords;
    aData.who = Who;
    m_Total[Who] += value;
    aData.value = value;
    aData.bestValue = bestValue;
    aData.isScrabble = IsScrabble;
    aData.time = moveTime;

    const int i = m_lData.count();
    beginInsertRows(QModelIndex(), i, i);
    m_lData.append(aData);
    endInsertRows();
}

void gamecoursemodel::clearLastMove()
{
    ModelData aData;
    aData.placedWord = "";
    aData.connectedWords = "";
    aData.who = m_lData.last().who;
    m_Total[m_lData.last().who] -= m_lData.last().value;
    aData.value = 0;
    aData.bestValue = 0;
    aData.isScrabble = false;
    aData.time = 0;

    int i = m_lData.count();
    beginRemoveRows(QModelIndex(), i, i);
    m_lData.removeLast();
    endRemoveRows();

    i--;
    beginInsertRows(QModelIndex(), i, i);
    m_lData.append(aData);
    endInsertRows();

}

void gamecoursemodel::addBonus(int player, int value)
{
    m_Total[player] += value;
}

int gamecoursemodel::getScore(const int nPlayer)
{
    return m_Total[nPlayer];
}

int gamecoursemodel::timeTotalPerPlayer(const uint nPlayer)
{
    int nResult = 0;
    for (int i = 0; i < m_lData.count(); i++)
        if (m_lData[i].who == nPlayer)
            nResult += m_lData[i].time;
    return nResult;
}

struct Winner {
    int player;
    int value;
};

bool sortByValue(Winner &p1, Winner &p2) {
    return p1.value > p2.value;
}

void gamecoursemodel::getWinner(QList<int> &result)
{
    QList<Winner> lWinner;
    Winner aWinner;
    for (int i=0; i<MaxPlayers; i++) {
        aWinner.player = i;
        aWinner.value = m_Total[i];
        if (aWinner.value > 0)
            lWinner.append(aWinner);
    }
    std::sort(lWinner.begin(),lWinner.end(), sortByValue);

    if (lWinner.count()>0)
        result.append(lWinner[0].player);

    //draw?
    if (lWinner.count()>1) {
        int i=1;
        while ((lWinner[i].value == lWinner[0].value) && (i<MaxPlayers)) {
            result.append(lWinner[i].player);
            i++;
        }
    }
}
