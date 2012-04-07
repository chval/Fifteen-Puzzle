#!/usr/bin/perl
#
# Copyright 2011,2012 The Fifteen Puzzle Project, <blackchval@gmail.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.

# THIS SOFTWARE IS PROVIDED BY THE FIFTEEN PUZZLE PROJECT AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE FIFTEEN PUZZLE PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

use strict;
use warnings;
use Getopt::Long;
use Time::HiRes;
use Config;
use FifteenPuzzleCore;

my $help;
my $boardWidth;
my $boardHeight;
my $multi;
my $maxForkLevel;
my $saveSolution;
my $noSolution;
my $customBoard;

$| = 1;

&GetOptions(
    'w|width=i'         => \$boardWidth,
    'h|height=i'        => \$boardHeight,
    'm|multi'           => \$multi,
    'l|max-fork-level=i'=> \$maxForkLevel,
    'help'              => \$help,
    's|save-solution'   => \$saveSolution,
    'n|no-solution'     => \$noSolution,
    'c|set-puzzle=s'    => \$customBoard
);

if ( $help ) {
    while ( <DATA> ) { print }
    exit 0;
}

while ( <DATA> ) { print; last if $_ eq "\n" }

my $board;
unless ( $customBoard ) {
    $board = &FifteenPuzzleCore::createBoard($boardWidth, $boardHeight);
    print "=====> Randomly generated puzzle:\n\n" if $board;
} else {
    $board = &FifteenPuzzleCore::board(eval $customBoard);
    print "=====> Custom puzzle:\n\n" if $board;
}
&FifteenPuzzleCore::printBoard();

if ( $noSolution ) {
    exit 0;
}

if ( $board ) {
    my $moves = 0;
    my $timeStart = Time::HiRes::time();

    if ( $multi && $Config{'useithreads'} ) {
        print "> Using multithreaded version\n";
        $FifteenPuzzleCore::maxForkLevel = $maxForkLevel if $maxForkLevel;
        print "> Max fork level = " . $FifteenPuzzleCore::maxForkLevel . "\n\n";
        $moves = &FifteenPuzzleCore::multi_IDA_star();
    } else {
        print "> Using single-threaded version\n\n";
        $moves = &FifteenPuzzleCore::IDA_star();
    }

    my $timeStop = Time::HiRes::time();
    printf( "=====> %.8f sec\n", $timeStop - $timeStart );
    print "moves    : " . $moves . "\n";

    if ( $moves && $saveSolution ) {
        &FifteenPuzzleCore::writeSolution('solution.txt');
    } elsif ( $moves ) {
        &FifteenPuzzleCore::printShuffles();
    }
}
exit 0;

__DATA__
*******************************
**   Fifteen puzzle solver   **
*******************************
Version 1.4
Author <blackchval@gmail.com> 2011, 2012
Using: IDA*, DFS, Manhattan distance

Usage: puzzle.pl [options] ...

 --help                           show this help message
 -w N, --width=N                  random puzzle width (default is 3)
 -h N, --height=N                 random puzzle height (default is 3)
 --multi                          use multi-threaded version of algotithm
 -l N, --max-fork-level=N         used in multi-threaded algorithm (default is 2)
                                  Recommended value is number of processor cores
 -s, --save-solution              save solution into file
 -n, --no-solution                just create and print random puzzle
 -c PUZZLE, --set-puzzle=PUZZLE   set custom where PUZZLE format is "[[1,2,3],[4,5,6],[7,8,0]]"
