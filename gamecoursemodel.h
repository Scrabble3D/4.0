#pragma once

#include <QAbstractTableModel>

class gamecoursemodel : public QAbstractTableModel
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

    explicit gamecoursemodel(QObject *parent = nullptr);
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
    void getScores(QList<int>& result);
    int timeTotalPerPlayer(const uint nPlayer);
    int timePerMove(const uint nMove) { if (nMove < m_lData.count()) return m_lData[nMove].time; else return 0; }
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
