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

            if ( shuffleWith )
            {

                // solution was found in previous neighbours, should save it
                break;
            }

            return f;
        }

        if ( ! H )
        {
            IDA_Star::_slnFoundCnt++;

            if ( G < Solver::_slnStepsCnt )
            {

                // Yahoo!!! This is best solution
                Solver::_slnStepsCnt = G;
                Solver::_slnStates.clear();
                Solver::_slnShuffles.clear();

                // save board in a goal state
                Solver::_slnStates[G] = _board.toString();
                Solver::_slnShuffles.push_back(nbr);

                if ( isBest )
                {
                    *isBest = 1;
                }
            }

            __stepBack(nbr, hn, hz, hs);

            if ( Solver::_slnStates.size() == 1 )
            {

                // save previous board state
                Solver::_slnStates[G - 1] = _board.toString();
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

            // swapping with this neighbour is best solution
            shuffleWith = nbr;
        }

        __stepBack(nbr, hn, hz, hs);
    }

    // save current board state if best solution was found
    if ( ! min && shuffleWith )
    {
        Solver::_slnStates[G - 1] = _board.toString();
        Solver::_slnShuffles.push_back(shuffleWith);
    }

    if ( isBest && shuffleWith )
    {

        // this branch is the best
        *isBest = 1;
    }

    return min;
}

/**
 * IDA_Star::__DFS_Multi
 *
 * Multi-threaded version
 *
 */
inline void IDA_Star::__DFS_Multi(int F, int G, int prev, int *min, int *isBest)
{
    boost::thread_group *nbrWorkers = NULL;

    // each thread should have it's own Solver
    IDA_Star *nbrSolvers[NEIGHBOURS_CNT_MAX];
    int minimals[NEIGHBOURS_CNT_MAX];
    int bestFlags[NEIGHBOURS_CNT_MAX];
    int nbrs[NEIGHBOURS_CNT_MAX];
    int nbrsCnt = _board.getNeigbours(0, nbrs);

    // uniquely processed neighbours (without previous)
    int realNbrs[NEIGHBOURS_CNT_MAX];
    int realNbrsCnt = 0;
    int shuffleWith = 0;

    int localMin = std::numeric_limits<int>::max();
    int localSlnStepsCnt = std::numeric_limits<int>::max();

    for ( int i = 0; i < nbrsCnt; i++ )
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

            // maybe solution was found in previous neighbours, we should check it
            localMin = f;
            break;
        }

        if ( ! H )
        {

            // critical section
            boost::mutex::scoped_lock lock(IDA_Star::_sln_mutex);
            IDA_Star::_slnFoundCnt++;

            if ( G < Solver::_slnStepsCnt )
            {

                // Yahoo!!! This is best solution
                Solver::_slnStepsCnt = G;
                Solver::_slnStates.clear();
                Solver::_slnShuffles.clear();

                // save board in a goal state
                Solver::_slnStates[G] = _board.toString();
                Solver::_slnShuffles.push_back(nbr);

                if ( isBest )
                {

                    // here we are saving best solution cost (not just boolean flag)
                    *isBest = G;
                }
            }

            __stepBack(nbr, hn, hz, hs);

            if ( Solver::_slnStates.size() == 1 )
            {

                // save previous board state
                Solver::_slnStates[G - 1] = _board.toString();
            }

            *min = H;

            if ( nbrWorkers )
            {

                // if some threads are running, sould be interrupted
                nbrWorkers->join_all();
                for ( int i = 0; i < realNbrsCnt; i++ )
                {
                    delete nbrSolvers[i];
                }
                delete nbrWorkers;
            }
            return;
        }

        bestFlags[realNbrsCnt] = std::numeric_limits<int>::max();

        if ( G > Solver::_cpu_units )
        {

            // recursive call
            __DFS_Multi(F, G + 1, nbr, &minimals[realNbrsCnt], &bestFlags[realNbrsCnt]);
        }
        else
        {

            // create a copy of current object
            nbrSolvers[realNbrsCnt] = new IDA_Star(*this);

            if ( ! nbrWorkers )
            {
                nbrWorkers = new boost::thread_group();
            }

            // this neighbour will be processed in a separated thread
            boost::thread *worker = new boost::thread(
                &IDA_Star::__DFS_Multi,
                boost::ref(nbrSolvers[realNbrsCnt]),
                F,
                G + 1,
                nbr,
                &minimals[realNbrsCnt],
                &bestFlags[realNbrsCnt]);
            nbrWorkers->add_thread(worker);
        }

        __stepBack(nbr, hn, hz, hs);
        realNbrs[realNbrsCnt++] = nbr;
    }

    if ( nbrWorkers )
    {
        nbrWorkers->join_all();
        for ( int i = 0; i < realNbrsCnt; i++ )
        {

            // add worker thread made steps into main thread
            this->_madeStepsCnt += nbrSolvers[i]->_madeStepsCnt;
            delete nbrSolvers[i];
        }
        delete nbrWorkers;
    }

    for ( int i = 0; i < realNbrsCnt; i++ )
    {
        if ( minimals[i] < localMin )
        {
            localMin = minimals[i];
        }

        if ( bestFlags[i] < localSlnStepsCnt )
        {

            // critical section
            boost::mutex::scoped_lock lock(IDA_Star::_sln_mutex);
            if ( bestFlags[i] == Solver::_slnStepsCnt )
            {
                localSlnStepsCnt = bestFlags[i];
                shuffleWith = realNbrs[i];
            }
        }
    }

    // save current board state if best solution was found
    if ( ! localMin && shuffleWith )
    {

        // critical section
        boost::mutex::scoped_lock lock(IDA_Star::_sln_mutex);
        Solver::_slnStates[G - 1] = _board.toString();
        Solver::_slnShuffles.push_back(shuffleWith);
    }

    if ( isBest && shuffleWith )
    {

        // this branch is the best
        *isBest = localSlnStepsCnt;
    }

    *min = localMin;
    return;
}

