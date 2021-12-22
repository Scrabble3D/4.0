#pragma once

#include <QObject>
#include <QVariant>
#include <QPoint>

namespace LetterState {
    Q_NAMESPACE
    enum State {lsEmpty,    //no letter placed
                lsBag,      //in bag
                lsRack,     //on rack, can be changed or dragged
                lsDragging, //on rank, just dragging; need for removing a letter from board by calling in Pieces.DoUpdate
                lsChange,   //on rank, marked for change, can't be dragged
                lsBoard,    //on board, currently placed, can be dragged
                lsPlaced};  //on board placed, can't be dragged
    Q_ENUM_NS(State)
}


class Point3D
{
public:
    constexpr Point3D() noexcept
        : mx(0), my(0), mz(0) {};
    constexpr Point3D(int x, int y, int z) noexcept
        : mx(x), my(y), mz(z) {}
    constexpr inline int x() const noexcept { return mx; };
    constexpr inline int y() const noexcept { return my; };
    constexpr inline int z() const noexcept { return mz; };
    constexpr inline void setX(int x) noexcept { mx = x; };
    constexpr inline void setY(int y) noexcept { my = y; };
    constexpr inline void setZ(int z) noexcept { mz = z; };

private:
    int mx;
    int my;
    int mz;
};

struct Letter
{
    QString What = QString(QChar::Null);
    unsigned int Value = 0;
    bool IsJoker = false;
    bool IsRandom = false;
    unsigned int When = 0;
    unsigned int Who = 0;
    unsigned int RackPos = 0;
    unsigned int Where = 0;
    Point3D Point = Point3D(0,0,0);
    LetterState::State State = LetterState::lsEmpty;

    QVariantList toString()
    {
        QVariantList aList;
        aList.append(What);
        aList.append(Value);
        aList.append(IsJoker);
        aList.append(IsRandom);
        aList.append(When);
        aList.append(Who);
        aList.append(RackPos);
        aList.append(Where);
        aList.append(Point.x());
        aList.append(Point.y());
        aList.append(Point.z());
        aList.append(State);
        return aList;
    };
    void fromString(QStringList aList) {
        What = aList[0];
        Value = aList[1].toUInt();
        IsJoker = aList[2] == "true";
        IsRandom = aList[3] == "true";
        When = aList[4].toUInt();
        Who = aList[5].toUInt();
        RackPos = aList[6].toUInt();
        Where = aList[7].toUInt();
        Point = Point3D(aList[8].toUInt(),aList[9].toUInt(),aList[10].toUInt());
        State = static_cast<LetterState::State>(aList[11].toInt());
    }
/*
    QJsonObject toJson()
    {
        QJsonObject jLetter;
        jLetter["w"] = What;
        jLetter["v"] = int(Value);
        jLetter["j"] = IsJoker;
        jLetter["r"] = IsRandom;
        jLetter["h"] = int(When);
        jLetter["o"] = int(Who);
        jLetter["p"] = int(RackPos);
        jLetter["e"] = int(Where);
        jLetter["x"] = int(Point.x());
        jLetter["y"] = int(Point.y());
        jLetter["z"] = int(Point.z());
        jLetter["s"] = State;
        return jLetter;
    };
    void fromJson(const QJsonObject &jLetter) {
        if (jLetter.contains("w") && jLetter["w"].isString())
            What = jLetter["w"].toString();
        if (jLetter.contains("v") && jLetter["v"].isDouble())
            Value = jLetter["v"].toInt();
        if (jLetter.contains("j") && jLetter["j"].isBool())
            IsJoker = jLetter["j"].toBool();
        if (jLetter.contains("r") && jLetter["r"].isBool())
            IsRandom = jLetter["r"].toBool();
        if (jLetter.contains("h") && jLetter["h"].isDouble())
            When = jLetter["h"].toInt();
        if (jLetter.contains("o") && jLetter["o"].isDouble())
            Who = jLetter["o"].toInt();
        if (jLetter.contains("p") && jLetter["p"].isDouble())
            RackPos = jLetter["p"].toInt();
        if (jLetter.contains("e") && jLetter["e"].isDouble())
            Where = jLetter["e"].toInt();
        if (jLetter.contains("x") && jLetter["x"].isDouble())
            Point.setX(jLetter["x"].toInt());
        if (jLetter.contains("y") && jLetter["y"].isDouble())
            Point.setY(jLetter["y"].toInt());
        if (jLetter.contains("z") && jLetter["z"].isDouble())
            Point.setZ(jLetter["z"].toInt());
        if (jLetter.contains("s") && jLetter["s"].isDouble())
            State = static_cast<LetterState::State>(jLetter["s"].toInt());
    }
*/
    bool IsEmpty()
    {
        bool aResult = true;
        aResult &= What == QString(QChar::Null);
        aResult &= Value == 0;
        aResult &= IsJoker == false;
        aResult &= IsRandom == false;
        aResult &= When == 0;
        aResult &= Who == 0;
        aResult &= RackPos == 0;
        aResult &= Where == 0;
        aResult &= Point.x() == 0;
        aResult &= Point.y() == 0;
        aResult &= Point.z() == 0;
        aResult &= State == LetterState::lsEmpty;
        return aResult;
    }

};

const Letter EmptyLetter = {QString(QChar::Null), 0,
                            false, false,
                            0, 0, 0, 0, Point3D(0,0,0),
                            LetterState::lsEmpty};

//Q_DECLARE_METATYPE(Letter);
