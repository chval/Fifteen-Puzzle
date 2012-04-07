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

#ifndef BOARD_H
#define BOARD_H

#include "Logger.h"
#include <new>
#include <limits>
#include <map>
#include <string>
#include <time.h>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

#define BOARD_WIDTH  3
#define BOARD_HEIGHT 3

#define NEIGHBOURS_CNT_MAX 4

#define ARRAY_START_CHAR         '['
#define ARRAY_END_CHAR           ']'
#define ARRAY_ELEM_SPLITTER_CHAR ','

class Board
{
public:
    Board();
    Board(int width,
          int height);
    Board(std::string json);
    Board(const Board &board);
    int getWidth();
    int getHeight();
    int getSize();
    int getValue(int x, int y);
    void getValueXY(int value, int &x, int &y);
    void swap(int a, int b);
    int getNeigbours(int value, int *nbrs);
    void print();
    std::string toString();             // pack board into JSON array format
    bool isNotNull();                   // check if board was created
    void dumpIndexes();                 // print all indexes into log
    ~Board();
private:
    int _width;
    int _height;
    int _size;                          // flat board size (width * height)
    int **_board;
    int **_bIndex;                      // board Index
    int ***_nIndex;                     // neighbour Index

    void __createRandom();              // create random board
    void __init();                      // init all indexes
    void __destroy();                   // destroy board and indexes
    void __bIndex();                    // build board Index
    void __nIndex();                    // build neighbour Index
    int** __parse(std::string json,
                  int &width,
                  int &height);         // parse string like JSON array into board format
};

#include "Board.hpp"

#endif // BOARD_H
