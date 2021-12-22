#include "statmodel.h"
#include <QDebug>

statmodel::statmodel(QObject *parent)
    : QAbstractTableModel(parent),
      m_lData(0)
{
}

QVariant statmodel::data(const QModelIndex &index, int role) const
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
            return "";
        else if (index.column() == 4) //move time
            return "";
        break;
      case WhoRole:
        return aData.who;
        break;
      case ConnectedWords:
        return aData.connectedWords;
        break;
      case BonusRole:
        return aData.bonus;
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

QVariant statmodel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
    {
        if (section == 0)
            return QString("#"); //TODO localize
        else if (section == 1)
            return QString("Word");
        else if (section == 2)
            return QString("Value");
        else if (section == 3)
            return QString("Best");
        else if (section == 4)
            return QString("Time");
    }
    return QVariant();
}

int statmodel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_lData.count();
}

int statmodel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QHash<int, QByteArray> statmodel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[WhoRole] = "who";
    roles[ConnectedWords] = "connectedWords";
    roles[IsScrabbleRole] = "isScrabble";
    roles[BonusRole] = "bonus";
    roles[TimeRole] = "time";
    return roles;
}

void statmodel::clear()
{
    beginResetModel();
    m_lData.clear();
    endResetModel();
}

void statmodel::addMove(QString PlacedWord, QString ConnectedWords, uint Who, int Value, int Bonus, bool IsScrabble, int moveTime)
{
    ModelData aData;
    aData.placedWord = PlacedWord;
    aData.connectedWords = ConnectedWords;
    aData.who = Who;
    aData.value = Value;
    aData.bonus = Bonus;
    aData.isScrabble = IsScrabble;
    aData.time = moveTime;

    const int i = m_lData.count();
    beginInsertRows(QModelIndex(), i, i);
    m_lData.append(aData);
    endInsertRows();
}
