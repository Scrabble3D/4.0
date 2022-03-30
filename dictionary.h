#pragma once

#include <QAbstractTableModel>

class dicFile
{

public:
    explicit dicFile(QObject* parent);

    // parent needed to emit wordcountchange notification
    bool loadDictionary(const QString fileName);

    // shuffle letters and return possible words
    QString variation(const QString chars);
    // returns a comma delimited string of unknown (or excluded) words
    QStringList checkWords(const QString placedWord, QString connectedWords);

    // returns the word at index, if category is enabled
    QString getWord(const uint index);
    // returns word, meaning, category, and excluded for index
    QVariantMap wordByIndex(const uint index);
    // returns index of word
    int indexByWord(const QString word);
    int count() const { return m_Words.count(); }
    // used in dictionaries.data() to update categories
    QString fileName() { return m_sFileName; }
    // the m_sChars are used for variation()
    // set at newgame() based on letter distribution incl. ? for blank tiles
    void setLetterSet(QString aChars) { m_sChars = aChars; }
    QString getLetterSet() { return m_sChars; }
    //returns whether word is contained and category is enabled
    bool containsWord(QString word);

    QString categoryNames();
    void setCategoryChecked(const QString catName, const bool isChecked);
    bool getCategoryChecked(const QString catName) const;
private:
    struct CatInfo {
        QString name;
        int value;
        bool enabled;
    };
    QStringList m_Words;
    QStringList m_Meanings;
    QList<int> m_Categories;
    QList<CatInfo> m_CategoryNames;
    //emit progress
    QObject* m_pParent;
    //all different chars contained in the dictionary
    QString m_sChars;
    QString m_sFileName;
    //iterate over words to collect the m_sChars
    void charsFromWords();
    //true if word is found and category enabled;
    //otherwise index of partially words is returned,
    //eg. 100="Foo" returns 100 but false for "Fo"
    bool isWordInDictionary(QString sWord, int *index);
};

class dicList : public QAbstractTableModel
{
public:
    enum ModelRoles {
        IsLoadedRole = Qt::UserRole + 1,
        AuthorRole,
        LicenseRole
    };

    explicit dicList(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    bool loadFrom(QString fileName);
    dicFile *dictionary;
    QString currentDicName() const;
    QVariantMap selectedDicInfo(const int index) const;
    void setCategory(const QString catName, const bool isChecked) {dictionary->setCategoryChecked(catName, isChecked);}
    bool getCategory(const QString catName) const {return dictionary->getCategoryChecked(catName); }
    void updateList();

protected:
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

private:

    struct dicListData {
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

    QString versionToString(const int aValue);
    QObject* m_pParent;
    int m_nCurrentDictionary; //index of currently active dic
    void getInfo(dicListData *aData);
    QList<dicListData> m_Dictionaries;
};
