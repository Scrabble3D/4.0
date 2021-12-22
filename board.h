#pragma once

#include <QObject>

#include "letter.h"

enum FieldType {ftStart=0,
                ftDefault=1,
                ftDoubleLetter=2,
                ftTrippleLetter=3,
                ftQuadLetter=4,
                ftDoubleWord=5,
                ftTrippleWord=6,
                ftQuadWord=7,
                ftPiece=8};

enum Dimension {
    dmAbscissa  = 0, //x-axis
    dmOrdinate  = 1, //y-axis
    dmApplicate = 2  //z-axis
};

class board
{
public:
    board();

    void initialize(const QVariantList fieldTypeArray, const bool Is3D);
    FieldType getFieldtype(const uint index);

    void setLetter(Letter &aLetter);
    Letter getLetter(const uint index);
    void placeLetters(); //set state for all lsBoard to lsPlaced on NextPlayer
    void removeLetter(const unsigned int aLetterIndex);
    int getBoardsize() { return m_nBoardsize; }
    int getFieldsize() { return m_nFieldsize; }
    unsigned int PointToWhere(const Point3D nPoint); //convert continuous index into x,y,z coordinates
    Point3D WhereToPoint(const unsigned int aWhere); //converts x,y,z coord into index
    Point3D Pos3D(int nIndex2D); //converts x,y aka 2D index into true x,y,z point depending on active dimension/position
    int Index2D(Point3D aPoint3D);

    Dimension getActiveDimension() { return m_eActiveDimension; }
    void setActiveDimension(Dimension aDimension) { m_eActiveDimension = aDimension; }
    int getActivePosition() { return m_nActivePosition; } //2D plane in 3D; always zero in 2D games
    void setActivePosition(int aPosition) { m_nActivePosition = aPosition; }

private:
    QList<Letter> m_Letters; //initialized with EmptyLetter
    QList<FieldType> m_Fieldtypes;
    int m_nBoardsize; //one side, eg. 15 for classic 15x15
    int m_nFieldsize; //total size, ie. fieldTypeArray.length
    Dimension m_eActiveDimension;
    int m_nActivePosition;
};
