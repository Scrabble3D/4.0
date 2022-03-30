#pragma once

#include <QAbstractTableModel>

class statmodel : public QAbstractTableModel
{

public:
    enum ModelRoles {
        //Display = placed, connected, value
        WhoRole = Qt::UserRole + 1,
        ConnectedWords,
        BestValueRole,
        IsScrabbleRole,
        TimeRole
    };

    explicit statmodel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    void addMove(QString PlacedWord,
                 QString ConnectedWords,
                 uint Who,
                 int value,
                 int bestValue,
                 bool IsScrabble,
                 int moveTime);
    void clear();

protected:
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:
    struct ModelData {
      QString placedWord;
      QString connectedWords;
      uint who;
      int value;
      int bestValue;
      bool isScrabble;
      int time;
    };

    QList<ModelData> m_lData;
};
