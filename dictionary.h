#pragma once

#include <QAbstractTableModel>

class dictionary : public QAbstractTableModel
{

public:
    enum ModelRoles {
        IsSelectedRole = Qt::UserRole + 1,
        AuthorRole,
        LicenseRole,
        ReleaseRole,
        CommentRole
    };

    explicit dictionary(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;

    bool loadFrom(QString fileName);
    QString variation(const QString aChars);
    QString getWord(const uint index);
    bool contains(QString aWord) { return m_Words.contains(aWord); }

protected:
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:
    struct modelData {
      QString NativeName;
      QString EnglishName;
      int InstalledVersionNumber;
      QString InstalledVersion;
      QString AvailableVersion;
      QString FileName;
      QString Author;
      QString StandardCategory;
      QString License;
      QString Release;
      QString Comment;
    };
    QStringList m_Words;

    QString m_sChars;
    bool isWordInDictionary(QString sWord, int *index);
    QString versionToString(const int aValue);
    int m_nCurrentDictionary; //index of currently active dic
    void readModelData();
    void getInfo(modelData *aData);
    QList<modelData> m_lData;
};
