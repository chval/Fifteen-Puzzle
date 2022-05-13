use strict;
use warnings;

use Test::More;

use FifteenPuzzleCore;

my $board_1 = &FifteenPuzzleCore::createBoard(2, 2);
isnt($board_1, undef, "Minimal board size is 2x2");

my $board_2 = &FifteenPuzzleCore::createBoard(1, 2);
is($board_2, undef, "Board width must be > 1");

my $board_3 = &FifteenPuzzleCore::createBoard(2, 1);
is($board_3, undef, "Board height must be > 1");

note "Generate default board " . FifteenPuzzleCore::WIDTH . "x" . FifteenPuzzleCore::HEIGHT;
my $board_4 = &FifteenPuzzleCore::createBoard();
isnt($board_4, undef, "Default board created");
is( scalar(@$board_4), FifteenPuzzleCore::HEIGHT, "Default board height" );
is( scalar(@{$board_4->[0]}), FifteenPuzzleCore::WIDTH, "Default board width" );

my ($width, $height) = ( int(rand(10)+2), int(rand(10)+2));
note "Generate board $width x $height";
my $board_5 = &FifteenPuzzleCore::createBoard($width, $height);
is( scalar(@$board_5), $height, "Custom board height" );
is( scalar(@{$board_5->[0]}), $width, "Custom board width" );

my $board_6 = &FifteenPuzzleCore::board();
is($board_6, undef, "Board can't be created from nothing");

my $board_7 = &FifteenPuzzleCore::board({foo => 'bar'});
is($board_7, undef, "Board can't be created from hash");

my $board_8 = &FifteenPuzzleCore::board([1,2,3]);
is($board_8, undef, "Board can't be created from one dimensional array");

my $board_9 = &FifteenPuzzleCore::board([[1],[2]]);
is($board_9, undef, "Board can't be created from only one element in a row");

my $board_10 = &FifteenPuzzleCore::board([[1, 2],[3, 4]]);
is($board_10, undef, "Board can't be created without an empty cell");

my $board_11 = &FifteenPuzzleCore::board([[1, 2],[5, 0]]);
is($board_11, undef, "Board can't be created from cells with invalid values");

my $board_12 = &FifteenPuzzleCore::board([[1, 2],[3, 0]]);
isnt($board_12, undef, "Create board from manual custom set");

my $board_13 = &FifteenPuzzleCore::board([[1, 3],[2, 0]]);
isnt($board_13, undef, "Create board from elements with parity odd");
is($board_13->[0][1], 2, "Board pre-last element was swapped");
is($board_13->[1][0], 3, "Board last element was swapped");

done_testing;
