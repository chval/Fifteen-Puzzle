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

#define ID_3x3_31      "[[8,6,7],[2,5,4],[3,0,1]]"
#define ID_3x3_31_2    "[[6,4,7],[8,5,0],[3,2,1]]"
#define ID_3x4_32      "[[6,11,1],[0,5,9],[2,4,8],[7,3,10]]"
#define ID_3x4_37      "[[5,4,0],[3,8,7],[6,10,11],[9,1,2]]"
#define ID_4x4_44      "[[5,7,15,11],[2,0,3,14],[10,9,4,1],[6,13,12,8]]"
#define ID_4x4_56      "[[0,12,8,9],[2,15,6,1],[4,5,7,14],[10,13,11,3]]"
#define ID_4x4_56_2    "[[3,9,6,15],[10,0,8,4],[7,12,5,1],[11,2,14,13]]"
#define ID_4x4_50      "[[10,7,0,6],[14,13,9,15],[2,1,8,3],[5,12,4,11]]"
#define ID_4x4_59      "[[14,13,12,10],[6,2,8,5],[3,11,7,4],[15,9,0,1]]"
#define ID_4x4_55      "[[12,6,5,11],[10,9,13,7],[2,1,3,15],[0,4,14,8]]"
#define ID_4x4_55_2    "[[7,5,9,10],[6,2,1,14],[12,11,13,4],[3,8,0,15]]"
#define ID_4x4_57      "[[8,2,13,15],[4,12,9,10],[3,11,7,0],[6,1,5,14]]"

#include "IDA_Star.h"
#include <iostream>

std::string head = "\
*************************************\n\
**   Fifteen puzzle project 2012   **\n\
*************************************\n\
Version 0.6\n\
This is free software\n\n\
";

void run()
{
    Board *mBoard = new Board(4, 4);
    Solver *mSolver = new IDA_Star(*mBoard, 2); // test multi-threaded version
    std::cout << "=====> Randomly generated puzzle:" << std::endl << std::endl;
    mBoard->print();
    mSolver->solve();
    mSolver->dumpSolutionShuffles();
    Board *board = new Board(*mBoard);
    delete mSolver;
    std::cout << std::endl << std::endl;
    Solver *solver = new IDA_Star(*board); // test singe threaded version
    solver->solve();
    solver->dumpSolutionShuffles();
    delete solver;
}

int main()
{
    std::cout << head;
    run();
    return 0;
}
