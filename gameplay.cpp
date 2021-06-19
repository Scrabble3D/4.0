#include "gameplay.h"

Game::Game()
{
    for (int i=0; i<10; i++)
    {
        Letter aLetter;
        aLetter.What='A';
        aLetter.Value=1;
        m_Pieces.append( aLetter );
    }
}

Game::~Game()
{
    m_Pieces.clear();
}

QList<Letter> Game::getPieces()
{
    return( m_Pieces );
}
