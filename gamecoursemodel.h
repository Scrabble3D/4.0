#pragma once

#include <QAbstractTableModel>

class gamecoursemodel : public QAbstractTableModel
{
    Q_OBJECT

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
    void addBonus(int player, int value);
    int getScore(const int nPlayer);
    int timeTotalPerPlayer(const uint nPlayer);
    int timePerMove(const int nMove) { if (nMove < m_lData.count()) return m_lData[nMove].time; else return 0; }
    void getWinner(QList<int> &result);
    void clear();

protected:
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:
    static const int MaxPlayers = 4;

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
    int m_Total[MaxPlayers];
};
