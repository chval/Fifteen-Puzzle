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

#ifndef SOLVER_H
#define SOLVER_H

#include "Board.h"
#include <sys/time.h>
#include <stdio.h>

/**
 * Abstract Solver
 *
 */
class Solver
{
public:
    Solver(Board &b, int cpuUnits = 0);
    Solver(const Solver &solver);
    void dumpSolutionStates();
    void dumpSolutionShuffles();
    virtual int solve() = 0;
    virtual ~Solver();
private:
    void __init();                                 // init internal data
    void __destroy();                              // destroy internal data
    void __bsIndex();                              // build board sample(goal) Index
    void __hIndex();                               // build heuristic Index
    int __checksum();                              // calculate checksum
    void __fix();                                  // fix board if incorrect checksum

    struct timeval _tv_start;
    struct timeval _tv_stop;
    double _solution_time;
protected:
    Board _board;                                  // Board object
    static int _slnStepsCnt;                       // count of steps in solution
    double _madeStepsCnt;                          // count of steps made while searching solution
    static std::map<int, std::string> _slnStates;  // solution steps ( step => board state )
    static std::vector<int> _slnShuffles;          // solution shuffles (sequence of empty cell neighbours to shuffle with)
    static int _cpu_units;                         // number of processor units (used in multi-threaded version)
    int **_bsIndex;                                // board sample(goal) Index
    int  *_hIndex;                                 // heuristic Index
    char _algName[64];                             // simple name of algorithm

    int __stepForward(int nbr,
                      int &hNbr,
                      int &hZero,
                      int &hSum);                  // swap empty cell with it's neighbour
    void __stepBack(int nbr,
                    int hNbr,
                    int hZero,
                    int hSum);                     // rollback swap changes

    void __mStart();                               // method start (pinpoint the start time)
    void __mStop();                                // method stop (pinpoint the time of termination)
    double __getSlnUsec();                         // get solution time in milliseconds
    void __getSlnTime(char *buf);                  // get solution time as formatted string
};

#include "Solver.hpp"

#endif // SOLVER_H
