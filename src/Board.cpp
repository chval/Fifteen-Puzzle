/*
 * Copyright 2012 The Fifteen Puzzle Project, <blackchval@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.

 * THIS SOFTWARE IS PROVIDED BY THE FIFTEEN PUZZLE PROJECT AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE FIFTEEN PUZZLE PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Board.h"
#include <sstream>
#include <stdio.h>

/**
 * Board constructor
 *
 * Create random board with dimension width x height
 *
 */
Board::Board(int width, int height)
{
    _board = NULL;
    if ( width > 1 && height > 1 )
    {
        _width  = width;
        _height = height;
        _size = width * height;
        __createRandom();
    }
    else
    {
        Logger::getInstance().errorAt("Board::Board")
        .explain("width and height must be greater than 1")
        .endl();
    }
}

/**
 * Board default constructor
 *
 * Create random board with default dimension
 *
 */
Board::Board()
{
    _board  = NULL;
    _width  = BOARD_WIDTH;
    _height = BOARD_HEIGHT;
    _size = _width * _height;
    __createRandom();
}

/**
 * Board constructor
 *
 * Create custom board from JSON array
 *
 */
Board::Board(std::string json)
{
    int width, height = 0;
    int **board = __parse(json, width, height);
    if ( board )
    {
        _board = board;
        _width = width;
        _height = height;
        _size = width * height;
        __init();
    }
    else
    {
        _board = NULL;
    }
}

/**
 * Board::getWidth
 *
 * Return board width
 *
 */
int Board::getWidth()
{
    return _width;
}

/**
 * Board::getHeight
 *
 * Return board height
 *
 */
int Board::getHeight()
{
    return _height;
}

/**
 * Board::getSize
 *
 * Return flat board size (width * height)
 *
 */
int Board::getSize()
{
    return _size;
}

/**
 * Board::getValue
 *
 * Return cell value by it's coordinates
 * This method is unsave!!!
 * Because of Solver class performance it doesn't check input params and board state
 *
 */
int Board::getValue(int x, int y)
{
    return _board[x][y];
}

/**
 * Board::getValueXY
 *
 * Fill X and Y with cell coordinates
 * This method is unsave!!!
 * Because of Solver class performance it doesn't check input params and board state
 *
 */
void Board::getValueXY(int value, int &x, int &y)
{
    x = _bIndex[value][0];
    y = _bIndex[value][1];
}

/**
 * Board::swap
 *
 * Swap two cells
 * This method is unsave!!!
 * Because of Solver class performance it doesn't check input params and board state
 *
 */
void Board::swap(int a, int b)
{
    _board[_bIndex[a][0]][_bIndex[a][1]] = _board[_bIndex[b][0]][_bIndex[b][1]];
    _board[_bIndex[b][0]][_bIndex[b][1]] = a;

    int *bi = _bIndex[a];
    _bIndex[a] = _bIndex[b];
    _bIndex[b] = bi;

    int **ni = _nIndex[a];
    _nIndex[a] = _nIndex[b];
    _nIndex[b] = ni;
}

/**
 * Board::getNeigbours
 *
 * Get cell neighbours
 * This method is unsave!!!
 * Because of Solver class performance it doesn't check input params and board state
 *
 * Return count of neighbours
 *
 */
int Board::getNeigbours(int value, int *nbrs)
{
    int i;
    for ( i = 0; i < NEIGHBOURS_CNT_MAX && _nIndex[value][i]; i++)
    {
        nbrs[i] = *_nIndex[value][i];
    }
    return i;
}

/**
 * Board::print
 *
 * Print board into log
 */
void Board::print()
{
    if ( _board )
    {
        char num_buf[8];
        for ( int i = 0; i < _height; i++ )
        {
            for ( int j = 0; j < _width; j++ )
            {
                snprintf(num_buf, 8, "%4d", _board[i][j]);
                Logger::getInstance() << num_buf;
            }
            Logger::getInstance().endl();
        }
        Logger::getInstance().endl();
    }
}

/**
 * Board::toString
 *
 * Pack board into string like "[[1,2,3],[4,5,6],[7,8,0]]"
 *
 */
std::string Board::toString()
{
    if ( _board )
    {
        std::stringstream ss(std::stringstream::in | std::stringstream::out);
        ss << ARRAY_START_CHAR;
        for ( int x = 0; x < _height; x++ )
        {
            ss << ARRAY_START_CHAR;
            for (int y = 0; y < _width; y++ )
            {
                ss << _board[x][y] << ARRAY_ELEM_SPLITTER_CHAR;
            }
            int pos = ss.tellp();
            ss.seekp(pos - 1);
            ss << ARRAY_END_CHAR << ARRAY_ELEM_SPLITTER_CHAR;
        }
        int pos = ss.tellp();
        ss.seekp(pos - 1);
        ss << ARRAY_END_CHAR;
        return ss.str();
    }
    else
    {
        return NULL;
    }
}


/**
 * Board::isNull
 *
 * Check if board was created successfully
 *
 */
bool Board::isNotNull()
{
    return _board ? true : false;
}

/**
 * Board::dumpIndexes
 *
 * Print internal indexes into log
 * Just for debug
 *
 */
void Board::dumpIndexes()
{
    if ( _board )
    {
        int bs = 32;
        char buf[bs];
        Logger::getInstance().append("Coordinates:").endl();
        for ( int i = 0; i < _size; i++ )
        {
            snprintf(buf, bs, "%2d: ", i);
            Logger::getInstance() << buf;
            int x = _bIndex[i][0];
            int y = _bIndex[i][1];
            snprintf(buf, bs, "%c%2d,%2d%c\n", ARRAY_START_CHAR, x, y, ARRAY_END_CHAR);
            Logger::getInstance() << buf;
        }
        Logger::getInstance().endl().append("Neighbours:").endl();
        for ( int i = 0; i < _size; i++ )
        {
            snprintf(buf, bs, "%2d: %c", i, ARRAY_START_CHAR);
            Logger::getInstance() << buf;
            for ( int n = 0; n < NEIGHBOURS_CNT_MAX && _nIndex[i][n]; n++)
            {
                snprintf(buf, bs, "%p -> %2d, ", _nIndex[i][n], *_nIndex[i][n]);
                Logger::getInstance() << buf;
            }
            snprintf(buf, bs, "\b\b%c\n", ARRAY_END_CHAR);
            Logger::getInstance() << buf;
        }
        Logger::getInstance().endl();
    }
}

/**
 * Board destructor
 *
 */
Board::~Board()
{
    __destroy();
}
