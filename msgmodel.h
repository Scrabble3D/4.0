#pragma once

#include <QAbstractTableModel>
#include <QTime>

struct Message
{
    QTime When;
    QString What;
    QString Who; //empty for system messages, otherwise player names
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
    void playerNames(const QStringList names);
    void clear();

public slots:
    void addMessage(const QString aWhat, const QString aWho = "");

protected:
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:
    QVector<Message> m_msglist;
    QStringList m_lPlayerNames;
};
