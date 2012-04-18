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
* Solver::__init
*
* Init internal data
*
*/
inline void Solver::__init()
{
    if ( _board.isNotNull() )
    {
        __fix();
        __bsIndex();
        __hIndex();
    }
}

/**
 * Solver::__destroy
 *
 * Cleanup internal data
 *
 */
inline void Solver::__destroy()
{
    if ( _board.isNotNull() )
    {
        // cleanup board sample index
        for ( int x = 0; x < _board.getSize(); x++ )
            delete [] _bsIndex[x];
        delete [] _bsIndex;

        // cleanup heuristic index
        delete [] _hIndex;
    }
}

/**
 * Solver::__bsIndex
 *
 * Create index of cells coordinates
 * for a sample(goal) board state
 *
 * Example:
 *
 * Board -> 4 1 6
 *          2 7 5
 *          0 8 3
 * bsIndex[0] = [2,2]
 * bsIndex[1] = [0,0]
 * bsIndex[2] = [0,1]
 * bsIndex[3] = [0,2]
 * bsIndex[4] = [1,0]
 * bsIndex[5] = [1,1]
 * bsIndex[6] = [1,2]
 * bsIndex[7] = [2,0]
 * bsIndex[8] = [2,1]
 *
 */
inline void Solver::__bsIndex()
{
    _bsIndex = new int *[_board.getSize()];
    int magick = 1;
    for ( int x = 0; x < _board.getSize(); x++ )
        _bsIndex[x] = new int[2];
    for ( int x = 0; x < _board.getHeight(); x++ )
    {
        for ( int y = 0; y < _board.getWidth(); y++ )
        {
            int magickValue = magick + y;
            if ( x == _board.getHeight() - 1 && y == _board.getWidth() - 1 )
            {
                magickValue = 0;
            }
            _bsIndex[magickValue][0] = x;
            _bsIndex[magickValue][1] = y;
        }
        magick += _board.getWidth();
    }
}

/**
 * Solver::__hIndex
 *
 * Create index of heuristic costs ('Manhattan distance' to a goal state)
 * The last element is a sum of all distances
 * Should be called after bsIndex created
 *
 * Example:
 *
 * Board -> 4 1 6
 *          2 7 5
 *          0 8 3
 * hIndex[0] = 2
 * hIndex[1] = 1
 * hIndex[2] = 2
 * hIndex[3] = 2
 * hIndex[4] = 1
 * hIndex[5] = 1
 * hIndex[6] = 1
 * hIndex[7] = 2
 * hIndex[8] = 0
 * hIndex[9] = 12
 *
 */
inline void Solver::__hIndex()
{
    int hIndexSize = _board.getSize() + 1;
    _hIndex = new int[hIndexSize];
    int heuristic = 0;
    for ( int i = 0; i < _board.getSize(); i++ )
    {
        int dx, dy = 0;
        _board.getValueXY(i, dx, dy);
        int x = _bsIndex[i][0];
        int y = _bsIndex[i][1];
        int h = abs(dx - x) + abs(dy - y);
        heuristic += h;
        _hIndex[i] = h;
    }
    _hIndex[_board.getSize()] = heuristic;
}

/**
 * Solver::__checksum
 *
 * Calculate board checksum
 * For each cell we calculate count of right side cells with less cost
 *
 */
inline int Solver::__checksum()
{
    int boardArray[_board.getSize()];
    int i = 0;
    int cs = 0;
    for ( int x = 0; x < _board.getHeight(); x++ )
        for ( int y = 0; y < _board.getWidth(); y++ )
            boardArray[i++] = _board.getValue(x, y);
    for ( i = 0; i < _board.getSize() - 1; i++ )
    {
        if ( boardArray[i] == 0 )
        {
            continue;
        }
        for ( int j = i + 1; j < _board.getSize(); j++ )
        {
            if ( boardArray[j] == 0 )
            {
                continue;
            }
            if ( boardArray[j] < boardArray[i] )
            {
                cs++;
            }
        }
    }
    return cs;
}

/**
 * Solver::__fix
 *
 * Swap two last cells in a board if unsolvable
 *
 */
inline void Solver::__fix()
{
    int cs = __checksum();
    int zx, zy = 0;
    _board.getValueXY(0, zx, zy);
    int size = _board.getSize() - 1;
    if ( size & 1 )
    {
        // for boards like 2x2(3), 2x3(5), 3x2(5), ...
        // add row index of zero
        cs += zx;
    }
    if ( ! (_board.getHeight() & 1) )
    {
        if ( ! (_board.getWidth() & 1) )
        {
            // for boards like 2x2, 4x4, ...
            // row index of zero numbered from 1
            cs += 1;
        }
        else
        {
            // for boards like 3x2, 3x4, 5x2, ...
            // add parity of zero row index
            cs += (zx & 1);
        }
    }

    // if parity odd then should swap
    if ( cs & 1 )
    {
        _board.swap(size - 1, size - 2);
    }
}

/**
 * Solver::__stepForward
 *
 * Permutation  board on one step, where 0 is swapped with it's neighbour
 *
 * Return new heuristic sum
 *
 */
inline int Solver::__stepForward(int nbr, int &hNbr, int &hZero, int &hSum)
{

    // swap board tiles
    _board.swap(nbr, 0);
    int size = _board.getSize();

    // save current heuristic
    hNbr = _hIndex[nbr];
    hZero = _hIndex[0];
    hSum = _hIndex[size];

    // recalculate heuristic after swap
    int xsn = _bsIndex[nbr][0];
    int ysn = _bsIndex[nbr][1];
    int xsz = _bsIndex[0][0];
    int ysz = _bsIndex[0][1];

    int xn, yn = 0;
    _board.getValueXY(nbr, xn, yn);
    int xz, yz = 0;
    _board.getValueXY(0, xz, yz);

    _hIndex[0] = abs(xz - xsz) + abs(yz - ysz);
    _hIndex[nbr] = abs(xn - xsn) + abs(yn - ysn);
    _hIndex[size] += _hIndex[nbr] + _hIndex[0] - hNbr - hZero;

    _madeStepsCnt++;

    return _hIndex[size];
}

/**
 * Solver::__stepBack
 *
 * Rollback all changes after step forward
 *
 */
inline void Solver::__stepBack(int nbr, int hNbr, int hZero, int hSum)
{
    _board.swap(nbr, 0);
    _hIndex[nbr] = hNbr;
    _hIndex[0] = hZero;
    _hIndex[_board.getSize()] = hSum;
}


/**
 * Solver::__mStart
 *
 * Store current time at the beginning of solve method
 *
 */
inline void Solver::__mStart()
{
    gettimeofday(&_tv_start, NULL);
}

/**
 * Solver::__mStop
 *
 * Store current time at the end of solve method and calculate difference
 *
 */
inline void Solver::__mStop()
{
    gettimeofday(&_tv_stop, NULL);
    _solution_time =
        (_tv_stop.tv_sec * 1000000 + _tv_stop.tv_usec) - (_tv_start.tv_sec * 1000000 + _tv_start.tv_usec);
}

/**
 * Solver::__getSlnUsec
 *
 * Get solution time in milliseconds
 *
 */
inline double Solver::__getSlnUsec()
{
    return _solution_time;
}

/**
 * Solver::__getSlnTime
 *
 * Get solution time as formatted string (hh:mm:ss)
 *
 */
inline void Solver::__getSlnTime(char *buf)
{
    unsigned int hours, minutes, seconds;
    unsigned int stime = _solution_time / 1000000;

    hours = stime / 3600;
    stime = stime % 3600;
    minutes = stime / 60;
    stime = stime % 60;
    seconds = stime;

    snprintf(buf, 9, "%02u:%02u:%02u", hours, minutes, seconds);
}

