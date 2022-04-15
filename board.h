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
    void initialize(const board* aParentBoard);
    FieldType getFieldtype(const int index);

    void setLetter(Letter &aLetter);
    Letter getLetter(const int index);//@1..n
    Letter getLetter(const Point3D aWhere); //@x,y,z
    Letter getLetter(const int col, const int row); //@ col/row at m_nActivePos, m_eActivDim
    void placeLetters(); //set state for all lsBoard to lsPlaced on NextPlayer
    void setJokerLetter(const int aLetterIndex, const QString aWhat);
    void removeLetter(const int aLetterIndex);
    int getBoardSize() { return m_nBoardSize; }
    int getFieldSize() { return m_nFieldSize; }
    int pointToWhere(const Point3D nPoint); //convert x,y,z coordinates into continuous index 0..m_nFieldSize
    int pointToPlane(const Point3D nPoint); //convert x,y,z coordinates into continuous index at the current plane 0..n_BoardSize
    Point3D whereToPoint(const int aWhere); //converts x,y,z coord into index
    Point3D pos3D(int nIndex2D); //converts x,y aka 2D index into true x,y,z point depending on active dimension/position
    int index2D(Point3D aPoint3D);
    bool is3D() { return m_bIs3D; }
    Dimension getActiveDimension() { return m_eActiveDimension; }
    void setActiveDimension(Dimension aDimension) { m_eActiveDimension = aDimension; }
    int getActivePosition() { return m_nActivePosition; } //2D plane in 3D; always zero in 2D games
    void setActivePosition(int aPosition) { m_nActivePosition = aPosition; }

    QString getWordsAt(const int index); //comma delimited list of words (abscissa, ordinate, applicate) at this position

private:
    QList<Letter> m_Letters; //initialized with EmptyLetter
    QList<FieldType> m_Fieldtypes;
    int m_nBoardSize; //one side, eg. 15 for classic 15x15
    int m_nFieldSize; //total size, ie. fieldTypeArray.length
    Dimension m_eActiveDimension;
    int m_nActivePosition;
    bool m_bIs3D;
};
