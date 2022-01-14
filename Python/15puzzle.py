#!/usr/bin/env python3

import sys
import argparse

import FifteenPuzzleCore

intro = """
*************************
** Fifteen puzzle game **
*************************

Author <blackchval@gmail.com> 2021
Uses: IDA*, DFS, Manhattan distance
"""


def main():
    parser = argparse.ArgumentParser(description=intro, formatter_class=argparse.RawTextHelpFormatter, add_help=False)
    parser.add_argument('--help', action='help', help='show this help message and exit')
    parser.add_argument('-w', '--width', type=int, default=3, help='random puzzle width (default is 3)')
    parser.add_argument('-h', '--height', type=int, default=3, help='random puzzle height (default is 3)')
    parser.add_argument('--multi', help='use multi-threaded version of algorithm', action='store_true')
    parser.add_argument('-l', '--max-threads', type=int, default=2,
                        help='used in multi-threaded algorithm to limit number of threads (default is 2)')
    parser.add_argument('-s', '--save-solution', help='save solution into file', action='store_true')
    parser.add_argument('-n', '--no-solution', help='just create and print random puzzle', action='store_true')
    parser.add_argument('-c', '--set-puzzle', type=str, help='set custom puzzle in format "[[1,2,3],[4,5,6],[7,8,0]]"',
                        dest='custom_board')

    args = parser.parse_args()

    # show help if called without arguments
    """if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(0)"""

    if not args.custom_board:
        puzzle = FifteenPuzzleCore.RandomPuzzle(args.width, args.height)
        print("=====> Randomly generated puzzle:")
    else:
        puzzle = FifteenPuzzleCore.CustomPuzzle(args.custom_board)
        print("=====> Custom puzzle:")

    puzzle.print_board()

    if args.no_solution:
        sys.exit(0)


if __name__ == '__main__':
    main()
