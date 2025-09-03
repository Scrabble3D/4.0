#pragma once

#include <QAbstractListModel>

class dictionarymodel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ModelRoles {
        NativeRole = Qt::UserRole + 1,
        EnglishRole,
        FilenameRole,
        AvailableVersionRole,
        InstalledVersionRole,
        AuthorRole,
        LicenseRole,
        ReleaseRole,
        CommentRole,
        CategoriesRole,
        IsLoadedRole,
    };

    explicit dictionarymodel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;

    Q_INVOKABLE QVariantMap get(int index) const;
    int indexOf(const QString fileName); //helper function for load/delete

    void deleteDic(const QString fileName);
    void updateList(); //called from GamePlay on refresh version
    void doReset() { beginResetModel(); endResetModel(); } //update categories in network mode
    const QStringList canUpdate() { return m_CanUpdate; }

signals:
    void onClearDictionary();
    void onMessage(const QString aMessage, const QString aWho = "");

public slots:
    void doLoadingFinished(QString fileName);

protected:
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:
    struct dicListData {
        QString NativeName;
        QString EnglishName;
        int InstalledVersionNumber;
        int AvailableVersionNumber;
        QString InstalledVersion;
        QString AvailableVersion;
        QString FileName;
        QString Author;
        QString StandardCategory;
        QString License;
        QString Release;
        QString Comment;
        QStringList Categories; // checked status get/set via dictionary
        QString LetterDistribution;
    };

    int m_nLoadedDictionary = -1; //index of currently selected dic
    QList<dicListData> m_Dictionaries;

    void getInfo(dicListData *aData); //used also in downloadmanager::update()

    QStringList m_CanUpdate; //TODO: dictionary: update - smarter solution needed
};
