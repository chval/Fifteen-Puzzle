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

#include "IDA_Star.h"
#include <stdio.h>
#include <string.h>

// declaration of static variables (should be seen from all threads)
int IDA_Star::_slnFoundCnt = 0;
boost::mutex IDA_Star::_sln_mutex;

/**
 * IDA_Star constructor
 *
 * Base class Solver initialized with board
 *
 */
IDA_Star::IDA_Star(Board &board, int cpuUnits) : Solver(board, cpuUnits)
{
    // Initialized in a base class
    if ( cpuUnits > 1 )
    {
        strcpy(_algName, "IDA* multi-threaded");
    }
    else
    {
        strcpy(_algName, "IDA*");
    }
}

/**
 * IDA_Star copy constructor
 *
 * Duplicate solver internal data
 *
 */
IDA_Star::IDA_Star(const Solver &solver) : Solver(solver)
{
    // Copied in a base class
}

/**
 * IDA_Star::solve
 *
 * Realization of IDA* (Iterative Deepening A* ) search algorithm
 * Performs a series of depth-first searches with succesively increased cost-bounds
 *
 * Return minimal count of moves needed for solution
 *
 */
int IDA_Star::solve()
{
    if ( ! _board.isNotNull() )
    {
        return 0;
    }
    Solver::_slnStepsCnt = std::numeric_limits<int>::max();
    IDA_Star::_slnFoundCnt = 0;

    Logger::getInstance().append(_algName).append(" started, ").timestamp();

    __mStart();

    // heuristic sum
    int F = _hIndex[_board.getSize()];
    if ( ! F )
    {

        // already in a goal state
        Solver::_slnStepsCnt = 0;
    }
    Logger::getInstance() << "F => " << F << " ";

    while ( F )
    {
        if ( Solver::_cpu_units > 1 )
        {
            __DFS_Multi(F, 1, 0, &F, NULL);
        }
        else
        {
            F = __DFS(F, 1, 0, NULL);
        }
        Logger::getInstance() << F << " ";
    }

    __mStop();

    Logger::getInstance().endl();
    Logger::getInstance().append(_algName).append(" stopped, ").timestamp();
    Logger::getInstance().append("-----------------------").endl();
    Logger::getInstance().append("Minimal steps count: ")
    .append(Solver::_slnStepsCnt).endl();
    Logger::getInstance().append("Made permutations  : ")
    .append(this->_madeStepsCnt).endl();
    Logger::getInstance().append("Found solutions    : ")
    .append(IDA_Star::_slnFoundCnt).endl();
    char buf[128];
    snprintf(buf, 128, "Solution time      : %.6f sec", __getSlnUsec()/1000000);
    Logger::getInstance().append(buf).append(" (");
    __getSlnTime(buf);
    Logger::getInstance().append(buf).append(")").endl();
    return Solver::_slnStepsCnt;
}
