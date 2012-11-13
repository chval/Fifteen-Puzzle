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
#define ID_4x4_60      "[[2,5,15,13],[4,0,6,11],[3,14,1,9],[7,12,10,8]]"
#define ID_4x4_62      "[[15,9,0,8],[6,3,13,14],[7,1,11,5],[12,4,10,2]]"
#define ID_4x4_63      "[[12,3,13,9],[0,4,6,10],[8,2,5,15],[7,14,11,1]]"
#define ID_4x4_58      "[[9,11,0,1],[4,14,8,12],[15,10,7,13],[5,6,2,3]]"

#include "IDA_Star.h"
#include <boost/thread/thread.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>

std::string head = "\
*************************************\n\
**       Copyright (c) 2012        **\n\
** The Fifteen Puzzle solver v0.65 **\n\
*************************************\n\
";

// variables for command line parameters
std::string custom_puzzle;
int width;
int height;
bool multi;
int cpu_units;
bool json = false;
bool needSolve = true;


/**
 * Run solver
 *
 */
void run()
{
    Board *board;

    if ( custom_puzzle.length() )
    {
        board = new Board(custom_puzzle);
    }
    else
    {
        board = new Board(width, height);
    }

    if ( board->isNotNull() )
    {
        if ( ! multi )
        {
            cpu_units = 1;
        }

        Solver *solver = new IDA_Star(*board, cpu_units);
        std::cout << "=====> Set puzzle: ";

        if ( json )
        {
            std::cout << "\"" << board->toString() << "\"";
        }

        std::cout << std::endl << std::endl;
        board->print();

        if ( ! needSolve )
        {
            delete solver;
            return;
        }

        solver->solve();
        solver->dumpSolutionShuffles();
        delete solver;
    }
}

/**
 * Print program info and help message to STDOUT
 *
 */
void showHelp(boost::program_options::options_description desc)
{
    std::cout << head << std::endl;
    std::cout << desc << std::endl;
}

/**
 * Main function
 *
 * Collect program options and call run method
 *
 */
int main(int argc, char *argv[])
{
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
    ("help", "show this help message")
    ("no-solution,n", "just create and print random puzzle")
    ("json,j", "print generated puzzle in JSON format")
    ("width,w", boost::program_options::value<int>(&width)->default_value(4), "random puzzle width")
    ("height,h", boost::program_options::value<int>(&height)->default_value(4), "random puzzle height")
    ("set-puzzle,c", boost::program_options::value<std::string>(&custom_puzzle)->default_value(""), "set custom puzzle where arg is JSON array")
    ("multi,m", boost::program_options::value<bool>(&multi)->default_value(true), "use multi-threaded version of algotithm")
    ("cpu-units,u", boost::program_options::value<int>(&cpu_units)->default_value(boost::thread::hardware_concurrency()),
     "used in multi-threaded algorithm\nSet it only if auto detected value is incorrect");

    boost::program_options::variables_map vm;

    try
    {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);
    }
    catch ( const boost::program_options::error& e )
    {
        showHelp(desc);
        return 1;
    }

    if ( vm.count("help") )
    {
        showHelp(desc);
        return 1;
    }

    if ( vm.count("no-solution") )
    {
        needSolve = false;
    }

    if ( vm.count("json") )
    {
        json = true;
    }

    std::cout << head << std::endl;

    run();

    return 0;
}
