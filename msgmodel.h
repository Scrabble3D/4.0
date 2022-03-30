#pragma once

#include <QAbstractTableModel>
#include <QTime>

struct Message
{
    QTime When;
    QString What;
    int Who; //-1 for system messages, 0..3 players
};

class msgmodel: public QAbstractTableModel
{
    Q_OBJECT

public:
    enum ModelRoles {
        WhoRole = Qt::UserRole + 1
    };

    explicit msgmodel(QObject *parent = nullptr);;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;

    void addMessage(const QString aWhat, const int aWho = -1);
    void clear();

protected:
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:
    QVector<Message> m_msglist;
};
