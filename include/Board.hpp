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

/**
 * Board::__createRandom
 *
 * Create random board
 * _width, _height and _size should be set before call
 *
 */
inline void Board::__createRandom()
{
    __destroy();
    srand ( time(NULL) );
    int size = _size;
    int boardArray[size];
    for ( int x = 0; x < _size; x++ )
        boardArray[x] = x;
    _board = new int *[_height];
    for ( int x = 0; x < _height; x++ )
        _board[x] = new int[_width];
    for ( int x = 0; x < _height; x++ )
        for ( int y = 0; y < _width; y++ )
        {
            int rid = rand() % size;
            _board[x][y] = boardArray[rid];
            boardArray[rid] = boardArray[--size];
        }
    __init();
}

/**
 * Board::__init
 *
 * Init board internal data
 *
 */
inline void Board::__init()
{
    __bIndex();
    __nIndex();
}

/**
 * Board::__destroy
 *
 * Cleanup board data
 *
 */
inline void Board::__destroy()
{
    if ( _board )
    {

        // cleanup board
        for ( int x = 0; x < _height; x++ )
            delete [] _board[x];
        delete [] _board;
        _board = NULL;

        // cleanup board index
        for ( int x = 0; x < _size; x++ )
            delete [] _bIndex[x];
        delete [] _bIndex;

        // cleanup neighbour index
        for ( int x = 0; x < _size; x++ )
            delete [] _nIndex[x];
        delete [] _nIndex;
    }
}

/**
 * Board::__bIndex
 *
 * Create index of cells coordinates
 *
 * Example:
 *
 * Board -> 4 1 6
 *          2 7 5
 *          0 8 3
 * bIndex[0] = [2,0]
 * ...
 * bIndex[8] = [2,1]
 *
 */
inline void Board::__bIndex()
{
    _bIndex = new int *[_size];
    for ( int x = 0; x < _size; x++ )
        _bIndex[x] = new int[2];
    for ( int x = 0; x < _height; x++ )
        for ( int y = 0; y < _width; y++ )
        {
            _bIndex[_board[x][y]][0] = x;
            _bIndex[_board[x][y]][1] = y;
        }
}

/**
 * Board::__nIndex
 *
 * Create index of neighbour cells
 *
 * Example:
 *
 * Board -> 1 2 3
 *          4 5 6
 *          7 0 8
 * 0 has address 0x100
 * 8 has address 0x200
 * 7 has address 0x300
 * 5 has address 0x400
 * 6 has address 0x500
 * nIndex[0] = [0x300, 0x400, 0x200]
 * ...
 * nIndex[8] = [0x100, 0x500]
 *
 */
inline void Board::__nIndex()
{
    _nIndex = new int **[_size];
    for ( int x = 0; x < _height; x++ )
        for ( int y = 0; y < _width; y++ )
        {
            int cell = _board[x][y];
            _nIndex[cell] = new int *[NEIGHBOURS_CNT_MAX];
            for ( int i = 0; i < NEIGHBOURS_CNT_MAX; i++ )
                _nIndex[cell][i] = 0;
            int cnt = 0;
            int yLeft = y - 1;
            if ( yLeft >= 0 )
                _nIndex[cell][cnt++] = &_board[x][yLeft];
            int xUp = x - 1;
            if ( xUp >= 0 )
                _nIndex[cell][cnt++] = &_board[xUp][y];
            int yRight = y + 1;
            if ( yRight < _width )
                _nIndex[cell][cnt++] = &_board[x][yRight];
            int xDown = x + 1;
            if ( xDown < _height )
                _nIndex[cell][cnt] = &_board[xDown][y];
        }
}

/**
 * Board::__parse
 *
 * Parse string like "[[1,2,3],[4,5,6],[7,8,0]]" and return new board
 *
 */
inline int** Board::__parse(std::string json, int &width, int &height)
{
    int length = json.length();
    if ( ! length )
    {
        Logger::getInstance().errorAt("Board::__parse")
        .explain("param is null")
        .endl();
        return NULL;
    }

    /* step 1
     * collect positions for opening and closing array tokens
     */
    std::vector<int> openTokens;
    std::vector<int> closeTokens;

    for (int i = 1; i< length - 1; i++)
    {
        if ( json[i] == ARRAY_START_CHAR )
        {
            openTokens.push_back(i);
        }
        else if ( json[i] == ARRAY_END_CHAR )
        {
            closeTokens.push_back(i);
        }
    }
    if ( openTokens.size() != closeTokens.size() )
    {
        Logger::getInstance().errorAt("Board::__parse")
        .explain("bad tokens ")
        .append(ARRAY_START_CHAR)
        .append(ARRAY_END_CHAR)
        .append(" balance")
        .endl();
        return NULL;
    }

    /* step 2
     * build map with
     * opening token position => closing token position
     */
    std::map<int, int> tokenMap;
    int prevClosePos = std::numeric_limits<int>::max();
    for (int i = openTokens.size() - 1; i >= 0; i--)
    {
        int openPos = openTokens[i];
        unsigned int j = 0;
        while ( j < closeTokens.size() && closeTokens[j] <= openPos )
        {
            j++;
        }
        if ( closeTokens[j] > openPos )
        {
            if ( closeTokens[j] > prevClosePos )
            {
                std::string s(prevClosePos, ' ');
                s.append("^ is inside another array");
                Logger::getInstance().errorAt("Board::__parse")
                .explain(json).endl()
                .explain(s).endl();
                return NULL;
            }
            tokenMap[openPos] = closeTokens[j];
            prevClosePos = closeTokens[j];
            closeTokens[j] = -1;
        }
        else
        {
            std::string s(openPos, ' ');
            s.append("^ no closing token for");
            Logger::getInstance().errorAt("Board::__parse")
            .explain(json).endl()
            .explain(s).endl();
            return NULL;
        }
    }
    int boardHeight = tokenMap.size();

    /* step 3
     * build map with
     * board row => elements
     */
    std::map<int, std::vector<int> > boardMap;
    std::map<int, int>::iterator tmIt;
    std::string splitter(1, ARRAY_ELEM_SPLITTER_CHAR);
    unsigned int boardWidth = 0;
    for ( tmIt = tokenMap.begin(); tmIt != tokenMap.end(); tmIt++ )
    {
        std::vector<int> parsedValues;
        int startPos = (*tmIt).first + 1;
        int stopPos = (*tmIt).second;
        std::string rawStr(json, startPos, stopPos - startPos);
        std::vector<std::string> rawValues;
        boost::split(rawValues, rawStr, boost::is_any_of(splitter));
        std::vector<std::string>::iterator rIt;
        for ( rIt = rawValues.begin(); rIt != rawValues.end(); rIt++ )
        {
            try
            {
                int n = boost::lexical_cast<int>(*rIt);
                parsedValues.push_back(n);
            }
            catch ( boost::bad_lexical_cast const& )
            {
                Logger::getInstance().errorAt("Board::__parse")
                .explain("row ")
                .append(ARRAY_START_CHAR)
                .append(rawStr)
                .append(ARRAY_END_CHAR)
                .endl()
                .explain("contains illegal integer '")
                .append(*rIt)
                .append("'")
                .endl();
                return NULL;
            }
        }
        if ( ! boardWidth )
        {
            boardWidth = parsedValues.size();
        }
        if ( parsedValues.size() != boardWidth )
        {
            Logger::getInstance().errorAt("Board::__parse")
            .explain("row ")
            .append(ARRAY_START_CHAR)
            .append(rawStr)
            .append(ARRAY_END_CHAR)
            .endl()
            .explain("contains ")
            .append(parsedValues.size())
            .append(" values, but previous width = ")
            .append(boardWidth)
            .endl();
            return NULL;
        }
        boardMap[(*tmIt).first] = parsedValues;
    }

    /* step 4
     * check if board (width x height) has all values filled
     */
    int lexErrors = 0;
    int size = boardWidth * boardHeight;
    if ( size < 4 || boardWidth < 2 || boardHeight < 2 )
    {
        Logger::getInstance().errorAt("Board::__parse")
        .explain("minimal board dimension is 2x2").endl();
        return NULL;
    }
    int boardArr[size];
    for ( int i = 0; i < size; i++ )
    {
        boardArr[i] = i;
    }
    std::map<int, std::vector<int> >::iterator bmIt;
    for ( bmIt = boardMap.begin(); bmIt != boardMap.end(); bmIt++ )
    {
        std::vector<int> r = (*bmIt).second;
        std::vector<int>::iterator rIt;
        for ( rIt = r.begin(); rIt != r.end(); rIt++ )
        {
            if ( *rIt >= 0 && *rIt < size )
            {
                boardArr[*rIt] = -1;
            }
            else
            {
                lexErrors++;
                Logger::getInstance().errorAt("Board::__parse")
                .explain(*rIt)
                .append(" is out of range").endl();
            }
        }
    }
    for ( int i = 0; i < size; i++ )
    {
        if ( boardArr[i] != -1 )
        {
            lexErrors++;
            Logger::getInstance().errorAt("Board::__parse")
            .explain(i)
            .append(" missed").endl();
        }
    }
    if ( lexErrors )
    {
        return NULL;
    }

    // string is valid
    int **board = new int *[boardHeight];
    for ( int i = 0; i < boardHeight; i++ )
        board[i] = new int[boardWidth];
    int x = 0;
    for ( bmIt = boardMap.begin(); bmIt != boardMap.end(); bmIt++ )
    {
        std::vector<int> r = (*bmIt).second;
        for ( unsigned int y = 0; y < boardWidth; y++ )
        {
            board[x][y] = r[y];
        }
        x++;
    }
    width = boardWidth;
    height = boardHeight;
    return board;
}
