from random import randrange
import json


class Puzzle:
    def __init__(self, *argv):
        self.board = self._create_board(argv)

        self.height = len(self.board)
        self.width = len(self.board[0])
        self.size = self.width * self.height

        self._create_xy_index()
        self._fix_board()

    # must be implemented in a subclass
    def _create_board(self, argv): pass

    def get_board(self):
        return self.board

    def print_board(self):
        for x in range(self.height):
            for y in range(self.width):
                print("{:<4}".format(self.board[x][y]), end="")
            print("\n")

    # calculate board checksum
    def _calc_checksum(self):
        board_flat, cs = [], 0

        # convert board into flat array
        for row in self.board:
            for v in row:
                board_flat.append(v)

        for i in range(self.size):
            if not board_flat[i]:
                continue
            for j in range(i+1, self.size):
                if not board_flat[j]:
                    continue
                if board_flat[j] < board_flat[i]:
                    cs += 1
        return cs

    # create index of each board element coordinates
    def _create_xy_index(self):
        self.xy_index = {}
        for x in range(self.height):
            for y in range(self.width):
                self.xy_index[self.board[x][y]] = [x, y]

    # fix board to a valid(resolvable) puzzle state
    def _fix_board(self):
        cs = self._calc_checksum()

        # row index of zero
        zx = self.xy_index[0][0]

        # for boards like 2x2, 2x3, 3x2, ... (with even size)
        # add row index of zero
        if not self.size & 1:
            cs += zx

        if not self.height & 1:
            if not self.width & 1:
                # if board height is even and board width is even
                # then row index of zero numbered from 1
                cs += 1
            else:
                # if only board height is even
                # add parity of zero row index
                cs += zx & 1

        # if parity odd then should fix (swap last odd and even elements)
        if cs & 1:
            self._swap(self.size - 1, self.size - 2)

    # swap 2 board elements between each other
    def _swap(self, a, b):
        _idx_a = self.xy_index[a]
        self.board[_idx_a[0]][_idx_a[1]] = b
        _idx_b = self.xy_index[b]
        self.board[_idx_b[0]][_idx_b[1]] = a

        self.xy_index[a], self.xy_index[b] = self.xy_index[b], self.xy_index[a]


class RandomPuzzle(Puzzle):
    def __init__(self, width, height):
        if not (width > 1 and height > 1):
            raise ValueError('width and height must be greater than 1')

        super().__init__(width, height)

    # create random board
    def _create_board(self, argv):
        width = argv[0]
        height = argv[1]

        board = []

        # generate list of available board elements
        rand = []

        for i in range(height * width):
            rand.append(i)

        for x in range(height):
            board.append([])

            for y in range(width):
                rid = randrange(len(rand)) if len(rand) else 0

                board[x].append(rand[rid])

                # replace used element with unique
                rand[rid] = rand[-1]
                rand.pop()

        return board


class CustomPuzzle(Puzzle):
    def __init__(self, board_str):
        board = json.loads(board_str)

        if not isinstance(board, list):
            raise ValueError('board data is not array')

        height = len(board)
        width = len(board[0]) if isinstance(board[0], list) else 0

        if not (width > 1 and height > 1):
            raise ValueError('width and height must be greater than 1')

        # init flat board with None
        # to validate if all elements are present
        board_flat = []
        board_size = width * height

        for i in range(board_size):
            board_flat.append(None)

        for x in range(height):
            if not isinstance(board[x], list):
                raise ValueError("board row {} is not array".format(x))
            elif len(board[x]) != width:
                raise ValueError("board row {} has a different width".format(x))

            for y in range(width):
                i = board[x][y]
                if not isinstance(i, int):
                    raise ValueError("board element {} at [{}][{}] is not integer".format(i, x, y))

                if i < board_size:
                    board_flat[i] = True

        for i in range(len(board_flat)):
            if board_flat[i] is None:
                raise ValueError("board element {} is missed".format(i))

        super().__init__(board)

    # create custom board from parsed data
    def _create_board(self, argv):
        board = argv[0]

        return board
