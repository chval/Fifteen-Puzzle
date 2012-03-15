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

#include "Solver.h"
#include <stdio.h>
#include <sstream>

/**
 * Solver constructor
 *
 */
Solver::Solver(Board &b)
{
    _board = b;
    __init();
}

/**
 * Solver::dumpSolutionStates
 *
 * Print solution states into log
 * Just for debug
 *
 */
void Solver::dumpSolutionStates()
{
    if ( _slnStates.begin() != _slnStates.end() )
    {
        char buf[8];
        Logger::getInstance().append("Solution states:").endl();
        std::map<int, std::string>::iterator it;
        for ( int i = 1; i <= _slnStepsCnt; i++ )
        {
            it = _slnStates.find(i);
            if ( it != _slnStates.end() )
            {
                snprintf(buf, 8, "%3d: ", i);
                Logger::getInstance().append(buf)
                .append(_slnStates[i]).endl();
            }
        }
    }
}

/**
 * Solver::dumpSolutionShuffles
 *
 * Print solution shuffles into log
 * Just for debug
 *
 */
void Solver::dumpSolutionShuffles()
{
    std::vector<int>::reverse_iterator sIt;
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    Logger::getInstance().append("Solution shuffles  :").endl();
    for ( sIt = _slnShuffles.rbegin(); sIt < _slnShuffles.rend(); sIt++ )
    {
        ss << (*sIt) << ", ";
    }
    int pos = ss.tellp();
    ss.seekp(pos - 2);
    ss << std::endl;
    Logger::getInstance() << ss.str();
}

/**
 * Solver destructor
 *
 */
Solver::~Solver()
{
    __destroy();
}
