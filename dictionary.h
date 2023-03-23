#pragma once

#include <QObject>
#include <QVariant>

class dictionary : public QObject
{
    Q_OBJECT

public:
    explicit dictionary(QObject* parent);

    // parent needed to emit wordcountchange notification
    void loadDictionary(const QString fileName, const QString categories);

    // shuffle letters and return possible words
    QString variation(const QString chars);
    // returns a comma delimited string of unknown (or excluded) words
    QStringList checkWords(const QString placedWord, QString connectedWords);

    // returns the word at index, if category is enabled
    QString getWord(const uint index);
    // return the meaning of the word
    QString getMeanings(const QString aWords);
    // returns word, meaning, category, incl. excluded for index
    QVariantMap wordByIndex(const uint index);
    // returns index of word
    int indexByWord(const QString sFind);
    int count() const { return m_Words.count(); }
    // used in dictionaries.data() to update categories
    QString fileName() { return m_sFileName; }
    // the m_sChars are used for variation()
    // set at newgame() based on letter distribution incl. ? for blank tiles
    void setLetterSet(QString aChars) { m_sChars = aChars; }
    QString getLetterSet() { return m_sChars; }

    // getter/setter used in ConfigDictionary
    void setCategoryChecked(const QString catName, const bool isChecked);
    bool getCategoryChecked(const QString catName) const;

signals:
    void onLoadingFinished(QString fileName);
    void onProgress(int nProgress);
    void onDownload(QString fileName);

public slots:
    void doClear();

private:
    struct dicEntry {
        QString word;
        QString meaning;
        int category;
    };

    struct catInfo {
        QString name;
        int value;
        bool enabled;
    };
    QList<dicEntry> m_Words;

    QList<catInfo> m_CategoryNames;

    //all different chars A..Z etc. contained in the dictionary
    QString m_sChars;
    QString m_sFileName;
    //iterate over words to collect the m_sChars
    void charsFromWords();
    //true if word is found and category enabled;
    //otherwise index of partially words is returned,
    //eg. 100="Foo" returns 100 but false for "Fo"
    enum searchResult {srMatch, srDisabled, srNotFound};
    searchResult isWordInDictionary(QString sFind, int *index);
    void doLoad(QString fileName, QString categories);
};

