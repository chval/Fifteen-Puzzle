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
 * IDA_Star::__DFS
 *
 * Depth-first search (DFS) algorithm with cost limits
 * F = total cost
 * G = current cost
 *
 * isBest - Address of variable with boolean value
 *     (unique in each iteration, shows if current solution is the best)
 *
 * Return minimal found cost
 *
 */
inline int IDA_Star::__DFS(int F, int G, int prev, int *isBest)
{
    int min = std::numeric_limits<int>::max();
    int shuffleWith = 0;
    int nbrs[NEIGHBOURS_CNT_MAX];
    int cnt = _board.getNeigbours(0, nbrs);
    for ( int i = 0; i < cnt; i++ )
    {
        int nbr  = nbrs[i];
        if ( nbr == prev )
        {
            continue;
        }

        int hn, hz, hs = 0;

        int H = __stepForward(nbr, hn, hz, hs);
        int f = G + H;
        if ( f > F )
        {

            // step back, this is wrong way
            __stepBack(nbr, hn, hz, hs);

            return f;
        }
        if ( ! H )
        {
            _slnFoundCnt++;
            if ( G < _slnStepsCnt )
            {

                // Yahoo!!! This is best solution
                _slnStepsCnt = G;
                _slnStates.clear();
                _slnShuffles.clear();

                // save board in a goal state
                _slnStates[G] = _board.toString();
                _slnShuffles.push_back(nbr);
                if ( isBest )
                {
                    *isBest = 1;
                }
            }

            __stepBack(nbr, hn, hz, hs);

            if ( _slnStates.size() == 1 )
            {

                // save previous board state
                _slnStates[G - 1] = _board.toString();
            }

            return H;
        }

        int best = 0;

        // recursive call
        int m = __DFS(F, G + 1, nbr, &best);
        if ( m < min )
        {
            min = m;
        }
        if ( best )
        {
            shuffleWith = nbr;
        }

        __stepBack(nbr, hn, hz, hs);
    }

    // save current board state if best solution was found
    if ( ! min && shuffleWith )
    {
        _slnStates[G - 1] = _board.toString();
        _slnShuffles.push_back(shuffleWith);
    }
    if ( isBest && shuffleWith )
    {
        *isBest = 1;
    }
    return min;
}

