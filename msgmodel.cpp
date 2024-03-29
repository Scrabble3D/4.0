#include "msgmodel.h"

msgmodel::msgmodel(QObject *parent)
    : QAbstractTableModel(parent),
      m_msglist(0)
{
}

QVariant msgmodel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const unsigned int i = m_msglist.count() - index.row() - 1;

    switch (role) {
      case Qt::DisplayRole:
        if (index.column() == 0)
            return m_msglist[i].When.toString("hh:mm:ss");
        else if (index.column() == 1)
            return m_msglist[i].What;
        break;
      case WhoRole:
        return m_lPlayerNames.indexOf(m_msglist[i].Who);
        break;
    }

    return QVariant();
}

int msgmodel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_msglist.count();
}

int msgmodel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

void msgmodel::playerNames(const QStringList names)
{
   m_lPlayerNames.clear();
   m_lPlayerNames << names;
}

void msgmodel::addMessage(const QString aWhat, const QString aWho)
{
    Message aMessage;
    aMessage.What = aWhat;
    aMessage.Who = aWho;
    aMessage.When = QTime().currentTime();
    const int i = m_msglist.count();
    beginInsertRows(QModelIndex(), i, i);
    m_msglist.append(aMessage);
    endInsertRows();
}

void msgmodel::clear()
{
    beginRemoveRows(QModelIndex(), 0, m_msglist.count()-1);
    m_msglist.clear();
    endRemoveRows();
}

QHash<int, QByteArray> msgmodel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[WhoRole] = "who";
    return roles;
}
