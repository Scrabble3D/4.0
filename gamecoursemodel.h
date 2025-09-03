#pragma once

#include <QAbstractTableModel>
#include "letter.h"

class gamecoursemodel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum ModelRoles {
        WhoRole = Qt::UserRole + 1,
        MoveRole,
        WordRole,
        ConnectedWords,
        ValueRole,
        IsScrabbleRole,
        BestValueRole,
        TimeRole,
        BonusWho,
        Bonus,
    };

    explicit gamecoursemodel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    Q_PROPERTY(int selectedMove READ getSelected WRITE setSelected NOTIFY onSelectedMoveChanged)

    void addMove(QString PlacedWord,
                 QString ConnectedWords,
                 uint Who,
                 int value,
                 int bestValue,
                 bool IsScrabble,
                 int moveTime,
                 QList<Letter> rack);
    void clearLastMove(); //legimately challenged
    void addBonus(int player, int value);
    int getScore(const int nPlayer);
    int timeTotalPerPlayer(const uint nPlayer);
    int timePerMove(const int nMove) { if (nMove < m_pData.count()) return m_pData[nMove].time; else return 0; }
    void getWinner(QList<int> &result);
    void clear();
    void zerototal(const uint nPlayer) { m_Total[nPlayer] = 0; }
    QList<Letter> getRack(const int nMove) { return m_pData[nMove].rack; }
    int getBonus(const int nMove) { if (nMove < m_pData.count()) return m_pData[nMove].bonus; else return 0; }
    int getBonusWho(const int nMove) { if (nMove < m_pData.count()) return m_pData[nMove].bonuswho; else return -1; }

signals:
    void onSelectedMoveChanged(int nMove);

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
      int bonuswho;
      int bonus;
      QList<Letter> rack;
    };

    int getSelected() { return m_nSelected; };
    void setSelected(const int nSelected);

    QList<ModelData> m_pData;
    int m_Total[MaxPlayers];
    int m_nSelected = -1;
};
