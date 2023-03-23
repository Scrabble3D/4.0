#pragma once

#include <QAbstractListModel>
#include <QCoreApplication>

class locList : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ModelRoles {
        NativeRole = Qt::UserRole + 1,
        EnglishRole,
        FileNameRole,
        AvailableVersionRole,
        InstalledVersionRole,
        AuthorRole,
        IsLoadedRole,
    };

    explicit locList(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;

    Q_INVOKABLE QVariantMap get(int index) const;
    int indexOf(const QString fileName); //helper function for load/delete
    void loadingFinished(const QString fileName);
    bool deleteLocFile(const QString fileName);
    void updateList();
    QStringList canUpdate() { return m_CanUpdate; }

signals:
    void onClearDictionary();
    void onMessage(const QString aMessage, const QString aWho = "");

protected:
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:

    struct locListData {
        QString NativeName;
        QString EnglishName;
        QString FileName;
        QString AvailableVersion;
        QString InstalledVersion;
        QString Author;
        //calculated values
        int InstalledVersionNumber;
        int AvailableVersionNumber;
    };

    QTranslator *m_pTranslator;
    int m_nLoadedLocFile; //index of currently active dic
    QList<locListData> m_LocFiles;

    QStringList m_CanUpdate;
};
