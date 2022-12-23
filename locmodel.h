#pragma once

#include <QAbstractTableModel>
#include <QCoreApplication>

struct locListData {
    // data in *.conf
    QString NativeName;
    QString EnglishName;
    QString AvailableVersion;
    QString FileName;
    // data loaded from files
    QString InstalledVersion;
    QString Author;
    //calculated values
    int InstalledVersionNumber;
    int AvailableVersionNumber;
//    QString Release;
//    QString Comment;
};

class locList : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum ModelRoles {
        IsLoadedRole = Qt::UserRole + 1,
        AuthorRole,
        LicenseRole,
        FileNameRole
    };
    explicit locList(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    void setIsLoaded(const QString fileName);
    bool removeLocFile(const QString fileName);
    void updateList();
    QStringList canUpdate() { return m_CanUpdate; }

protected:
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:

    QTranslator *m_pTranslator;
    QObject* m_pParent;
    int m_nCurrentLocFile; //index of currently active dic
    QList<locListData> m_LocFiles;
    QStringList m_CanUpdate;
};
