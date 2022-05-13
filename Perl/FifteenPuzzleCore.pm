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

package FifteenPuzzleCore;

use strict;
use warnings;
use threads;
use threads::shared;

use constant INFINITY       => 100500;
use constant WIDTH          => 3;
use constant HEIGHT         => 3;
use constant MAX_FORK_LEVEL => 2;

# Internal variables
my $_board;     # Current board
my $_bIndex;    # Coordinates in a current board state
my $_bsIndex;   # Coordinates in a goal state
my $_hIndex;    # Heuristic costs
my $_nIndex;    # Neighbours

# Package variable (used in multithreaded version)
our $maxForkLevel = MAX_FORK_LEVEL;

my $_movesCount :shared = INFINITY;
my %_moves :shared = ();
my @_shuffles :shared = ();

########################################
# Purpose    : Compress board data
# Access     : Private
# Returns    : String like '1,2,3:4,5,6:7,8,9'
# Parameters : Board reference
#
my $__packBoard = sub {
    my $board = shift || $_board;
    return join ':', map { join ',', @$_ } @$board;
};

########################################
# Purpose    : Decompress board data
# Access     : Private
# Returns    : Board reference
# Parameters : String like '1,2,3:4,5,6:7,8,9'
#
my $__unpackBoard = sub {
    my $board = [split ':', shift];
    for ( 0 .. $#$board ) { push @$board, [split ',', shift @$board] }
    return $board;
};

########################################
# Purpose    : Swap values
# Access     : Private
# Returns    : none
# Parameters : Reference to 1-st value,
#            : Reference to 2-nd value
#
my $__swap = sub {
    my ($a, $b) = @_;
    my $s = $$a; $$a = $$b; $$b = $s;
};

########################################
# Purpose    : Create index of board coordinates in a current state
# Access     : Private
# Usage      : After game board created
# Returns    : None
#
my $__create_bIndex = sub {
    $_bIndex = [];
    for ( my $x = 0; $x < @{$_board}; $x++ ) {
        for ( my $y = 0; $y < @{$_board->[$x]}; $y++ ) {
            $_bIndex->[$_board->[$x][$y]] = [$x, $y];
        }
    }
};

########################################
# Purpose    : Create index of board coordinates in a goal state
# Access     : Private
# Usage      : After game board created
# Returns    : None
#
my $__create_bsIndex = sub {
    $_bsIndex = [];
    my $magick = 1;
    for ( my $x = 0; $x < @{$_board}; $x++ ) {
        for ( my $y = 0; $y < @{$_board->[$x]}; $y++ ) {
            my $magickValue = $y + $magick;
            if ( $x == $#{$_board} && $y == $#{$_board->[$x]} ) {
                $magickValue = 0;
            }
            $_bsIndex->[$magickValue] = [$x, $y];
        }
        $magick += @{$_board->[$x]};
    }
};

########################################
# Purpose    : Create index of heuristic costs ('Manhattan distance' to a goal state)
#            : The last element is sum of all distances
# Access     : Private
# Usage      : After bIndex and bsIndex created
# Returns    : None
#
my $__create_hIndex = sub {
    $_hIndex = [];
    my $h = 0;
    for  (my $i = 0; $i < @{$_bIndex}; $i++ ) {
        my ($dx, $dy) = @{$_bIndex->[$i]};
        my ($x, $y) = @{$_bsIndex->[$i]};
        my $h_ = abs($dx - $x) + abs($dy - $y);
        $h += $h_;
        $_hIndex->[$i] = $h_;
    }
    push @{$_hIndex}, $h;
};

########################################
# Purpose    : Create index of neighbour cells in a current board state
# Access     : Private
# Usage      : After game board created
# Returns    : None
#
my $__create_nIndex = sub {
    $_nIndex = [];
    for ( my $x = 0; $x < @{$_board}; $x++ ) {
        for ( my $y = 0; $y < @{$_board->[$x]}; $y++ ) {

            # Horizontal left
            if ( (my $dy = $y - 1) >= 0 ) {
                push @{$_nIndex->[$_board->[$x][$y]]}, \$_board->[$x][$dy];
            }

            # Vertical up
            if ( (my $dx = $x - 1) >= 0 ) {
                push @{$_nIndex->[$_board->[$x][$y]]}, \$_board->[$dx][$y];
            }

            # Horizontal right
            if ( (my $dy = $y + 1) < @{$_board->[$x]} ) {
                push @{$_nIndex->[$_board->[$x][$y]]}, \$_board->[$x][$dy];
            }

            # Vertical down
            if ( (my $dx = $x + 1) < @{$_board} ) {
                push @{$_nIndex->[$_board->[$x][$y]]}, \$_board->[$dx][$y];
            }
        }
    }
};

########################################
# Purpose    : Calculate board checksum
# Access     : Private
# Returns    : Checksum
#
my $__calcChecksum = sub {
    my ($boardArray, $cs) = ([], 0);
    map { map { push @$boardArray, $_ } @$_ } @$_board;
    for ( my $x = 0; $x < $#$boardArray; $x++ ) {
        next unless $boardArray->[$x];
        for ( my $y = $x + 1; $y < @$boardArray; $y++ ) {
            next unless $boardArray->[$y];
            $cs++ if $boardArray->[$y] < $boardArray->[$x];
        }
    }
    return $cs;
};

########################################
# Purpose    : Swap two last tiles in board if incorrect checksum
# Access     : Private
# Usage      : After game board and bIndex created
# Returns    : None
# Parameters : Checksum
#            : Width
#            : Height
#
my $__fixBoard = sub {
    my ($cs, $w, $h) = @_;

    # Board size (width * height - 1)
    my $size = $#$_bIndex;

    # Row index of zero
    my $zx = $_bIndex->[0][0];
    
    # For boards like 2x2(3), 2x3(5), 3x2(5), ...
    # add row index of zero
    if ( $size & 1 ) {
        $cs += $zx;
    }

    unless ( $h & 1 ) {
        unless ( $w & 1 ) {

            # For boards like 2x2, 4x4, ...
            # row index of zero numbered from 1
            $cs += 1;
        } else {

            # For boards like 3x2, 3x4, 5x2, ...
            # add parity of zero row index
            $cs += ($zx & 1);
        }
    }

    # If parity odd then should fix
    if ( $cs & 1 ) {
        my $ac = \$_bIndex->[$size];
        my $bc = \$_bIndex->[$size - 1];
        my ($ax, $ay) = @{$$ac};
        my ($bx, $by) = @{$$bc};
        my $a = \$_board->[$ax][$ay];
        my $b = \$_board->[$bx][$by];
        $__swap->($a, $b);
        $__swap->($ac, $bc);
    }
};

########################################
# Purpose    : Print board
# Access     : Public
# Returns    : None
# Parameters : Board reference
#
sub printBoard {
    my $board = shift || $_board;
    map { map { printf "%4d", $_ } @$_; print "\n" } @$board;
    print "\n";
}

########################################
# Purpose    : Save solution into file
# Access     : Public
# Returns    : None
# Parameters : Filename
#
sub writeSolution {
    my $fname = shift;
    $fname = $$ . '_' . $fname;
    if ( open SLN, ">$fname" ) {
        print SLN "### Initial state ###\n";
        map { map { printf SLN "%4d", $_ } @$_; print SLN "\n" } @$_board;
        foreach ( 1 .. $_movesCount ) {
            next unless $_moves{$_};
            my $board = $__unpackBoard->($_moves{$_});
            print SLN "\n\n[" . $_ . "]\n\n";
            map { map { printf SLN "%4d", $_ } @$_; print SLN "\n" } @$board;
        }
        close SLN;
        print "See '$fname' for solution\n";
    } else {
        print "Sorry, but '$fname' can't be created\n";
    }
}

########################################
# Purpose    : Print sequence of empty cell neighbours to shuffle with
# Access     : Public
# Usage      : After solution
# Returns    : None
#
sub printShuffles {
    print ("shuffles :\n" , (join ',', @_shuffles), "\n");
}

########################################
# Purpose    : Set or get board
# Access     : Public
# Returns    : Board reference
#
sub board {
    my $b = shift;
    if ( $b && ref $b eq 'ARRAY' ) {
        my $height = @$b;
        my $width = ref $b->[0] eq 'ARRAY' ? @{$b->[0]} : 0; 
        unless ( $width > 1 && $height > 1 ) {
            print "Error, width and height must be greater than 1\n";
            return;
        }
        foreach ( @$b ) {
            unless ( ref $_ eq 'ARRAY') {
                print "Error, bad input board format\n";
                return;
            }
            my $w = @$_;
            unless ( $w == $width ) {
                print "Error, bad board width\n";
                return;
            }
        }
        $_board = $b;
        $__create_bIndex->();
        unless ( (grep { defined $_ } @$_bIndex) ==  @$_bIndex && @$_bIndex == ($width * $height) ) {
            print "Error, bad input board data\n";
            undef $_board;
            return;
        }
        $__fixBoard->($__calcChecksum->(), $width, $height);
        $__create_bsIndex->();
        $__create_hIndex->();
        $__create_nIndex->();
    }
    return $_board;
}

########################################
# Purpose    : Generate a valid board with desired dimension
# Access     : Public
# Returns    : Board reference
# Parameters : Width
#            : Height
#
sub createBoard {
    my ($width, $height) = @_;
    $width  ||= WIDTH;
    $height ||= HEIGHT;
    unless ( $width > 1 && $height > 1 ) {
        print "Error, width and height must be greater than 1\n";
        return;
    }
    my $size = $height * $width - 1;
    my $rand = [];
    for ( 0 .. $size ) { push @$rand, $_ };
    $_board  = [];
    for ( my $x = 0; $x < $height; $x++ ) {
        push @{$_board}, [];
        for ( my $y = 0; $y < $width; $y++ ) {
            my $rid = int(rand @$rand);
            push @{$_board->[$x]}, $rand->[$rid];
            $rand->[$rid] = $rand->[$#$rand];
            pop @$rand;
        }
    }
    $__create_bIndex->();
    $__fixBoard->($__calcChecksum->(), $width, $height);
    $__create_bsIndex->();
    $__create_hIndex->();
    $__create_nIndex->();
    return $_board;
}

########################################
# Realization of IDA* (Iterative Deepening A* ) search algorithm (multithreaded version)
# Performs a series of depth-first searches with succesively increased
# cost-bounds
#
sub multi_IDA_star {
    my $F = $_hIndex->[-1];
    $_movesCount = 0 unless $F;
    print "F => $F ";
    while ( $F ) {
        $F = &multi_DFS($F, 1, 0);
        print "$F ";
    }
    print "\n";
    foreach ( threads->list() ) {
        $_->detach();
    }
    return $_movesCount;
}

########################################
# Depth-first search (DFS) algorithm (multithreaded version)
# Parameters : Total cost
#            : Current cost
#            : Previous tile
#            : Address of variable with boolean value (unique in each iteration, shows if current solution is the best)
#
sub multi_DFS {
    my ($F, $G, $prev, $isBest) = @_;
    my ($zx, $zy) = @{$_bIndex->[0]};
    my ($threads, $minimals) = ([], []);
    my @bestFlags = ();
    my $shuffleWith;
    my @nbrs = ();
    map { push @nbrs, $$_ } @{$_nIndex->[0]};
    my $min = INFINITY;
    my $stepsCnt = INFINITY;
    foreach my $nbr ( @nbrs ) {
        next if $nbr == $prev;
        my ($nx, $ny) = @{$_bIndex->[$nbr]};
        my ($ba, $bb) = (\$_board->[$zx][$zy], \$_board->[$nx][$ny]);
        my ($bia, $bib) = (\$_bIndex->[$$ba], \$_bIndex->[$$bb]);
        my ($nia, $nib) = (\$_nIndex->[0], \$_nIndex->[$nbr]);
        my ($bav, $biav, $niav) = ($$ba, $$bia, $$nia);
        ($$ba, $$bb, $$bia, $$bib, $$nia, $$nib) = ($$bb, $bav, $$bib, $biav, $$nib, $niav);
        my ($ha, $hb, $hs) = ($_hIndex->[$$ba], $_hIndex->[$$bb], $_hIndex->[-1]);
        my ($xa, $ya, $xb, $yb) = (@{$_bsIndex->[$$ba]}, @{$_bsIndex->[$$bb]});
        $_hIndex->[$$ba] = abs($zx - $xa) + abs($zy - $ya);
        $_hIndex->[$$bb] = abs($nx - $xb) + abs($ny - $yb);
        $_hIndex->[-1] += $_hIndex->[$$ba] + $_hIndex->[$$bb] - $ha - $hb;

        my $H = $_hIndex->[-1];
        my $f = $G + $H;
        if ( $f > $F ) {
            ($_hIndex->[$$ba], $_hIndex->[$$bb], $_hIndex->[-1]) = ($ha, $hb, $hs);
            ($$bb, $$ba, $$bib, $$bia, $$nib, $$nia) = ($$ba, $bav, $$bia, $biav, $$nia, $niav);
            $min = $f;
            last;
        }
        unless ( $H ) {
            lock($_movesCount);
            my $reset;
            if ( $G < $_movesCount ) {
                lock(%_moves);
                lock(@_shuffles);
                $_movesCount = $G;
                %_moves = ();
                @_shuffles = ();
                $_moves{$G} = $__packBoard->();
                unshift @_shuffles, $nbr;
                $reset = 1;
                if ( $isBest ) {
                    $$isBest = $G;
                }
            }
            ($_hIndex->[$$ba], $_hIndex->[$$bb], $_hIndex->[-1]) = ($ha, $hb, $hs);
            ($$bb, $$ba, $$bib, $$bia, $$nib, $$nia) = ($$ba, $bav, $$bia, $biav, $$nia, $niav);
            if ( $reset ) {
                lock(%_moves);
                $_moves{$G - 1} = $__packBoard->();
            }
            return $H;
        }
        if ( $G > $maxForkLevel ) {
            my $flag = 0;
            push @bestFlags, \$flag;
            push @$minimals, &multi_DFS($F, $G + 1, $nbr, \$flag);
        } else {
            my $flag :shared = 0;
            push @bestFlags, \$flag;
            push @$threads, threads->create(\&multi_DFS, $F, $G + 1, $nbr, \$flag);
        }
        ($_hIndex->[$$ba], $_hIndex->[$$bb], $_hIndex->[-1]) = ($ha, $hb, $hs);
        ($$bb, $$ba, $$bib, $$bia, $$nib, $$nia) = ($$ba, $bav, $$bia, $biav, $$nia, $niav);
    }
    foreach ( @$threads ) {
        push @$minimals, $_->join();
    }
    foreach ( @$minimals ) {
        $min = $_ if $_ < $min;
    }
    my @shfls = ();
    foreach ( @nbrs ) {
        push @shfls, $_ if $_ != $prev;
    }
    for (my $i = 0; $i < @bestFlags; $i++) {
        if ( ${$bestFlags[$i]} < $stepsCnt ) {
            lock($_movesCount);
            if ( ${$bestFlags[$i]} == $_movesCount ) {
                $stepsCnt = ${$bestFlags[$i]};
                $shuffleWith = $shfls[$i];
            }
        }
    }
    if ( !$min && $shuffleWith ) {
        lock(%_moves);
        lock(@_shuffles);
        $_moves{$G - 1} = $__packBoard->();
        unshift @_shuffles, $shuffleWith;
    }
    if ( $isBest && $shuffleWith ) {
        $$isBest = $stepsCnt;
    }
    return $min;
}

########################################
# Realization of IDA* (Iterative Deepening A* ) search algorithm (single-threaded version)
# Performs a series of depth-first searches with succesively increased
# cost-bounds
#
sub IDA_star {
    my $F = $_hIndex->[-1];
    $_movesCount = 0 unless $F;
    print "F => $F ";
    while ( $F ) {
        $F = &DFS($F, 1, 0);
        print "$F ";
    }
    print "\n";
    return $_movesCount;
}

########################################
# Depth-first search (DFS) algorithm (single-threaded version)
# Parameters : Total cost
#            : Current cost
#            : Previous tile
#            : Address of variable with boolean value (unique in each iteration, shows if current solution is the best)
#
sub DFS {
    my ($F, $G, $prev, $isBest) = @_;
    my ($zx, $zy) = @{$_bIndex->[0]};
    my $min = INFINITY;
    my $shuffleWith;
    my @nbrs = ();
    map { push @nbrs, $$_ } @{$_nIndex->[0]};
    foreach my $nbr ( @nbrs ) {
        next if $nbr == $prev;
        my ($nx, $ny) = @{$_bIndex->[$nbr]};
        my ($ba, $bb) = (\$_board->[$zx][$zy], \$_board->[$nx][$ny]);
        my ($bia, $bib) = (\$_bIndex->[$$ba], \$_bIndex->[$$bb]);
        my ($nia, $nib) = (\$_nIndex->[0], \$_nIndex->[$nbr]);
        my ($bav, $biav, $niav) = ($$ba, $$bia, $$nia);
        ($$ba, $$bb, $$bia, $$bib, $$nia, $$nib) = ($$bb, $bav, $$bib, $biav, $$nib, $niav);
        my ($ha, $hb, $hs) = ($_hIndex->[$$ba], $_hIndex->[$$bb], $_hIndex->[-1]);
        my ($xa, $ya, $xb, $yb) = (@{$_bsIndex->[$$ba]}, @{$_bsIndex->[$$bb]});
        $_hIndex->[$$ba] = abs($zx - $xa) + abs($zy - $ya);
        $_hIndex->[$$bb] = abs($nx - $xb) + abs($ny - $yb);
        $_hIndex->[-1] += $_hIndex->[$$ba] + $_hIndex->[$$bb] - $ha - $hb;

        my $H = $_hIndex->[-1];
        my $f = $G + $H;
        if ( $f > $F ) {
            ($_hIndex->[$$ba], $_hIndex->[$$bb], $_hIndex->[-1]) = ($ha, $hb, $hs);
            ($$bb, $$ba, $$bib, $$bia, $$nib, $$nia) = ($$ba, $bav, $$bia, $biav, $$nia, $niav);
            last if $shuffleWith;
            return $f;
        }
        unless ( $H ) {
            if ( $G < $_movesCount ) {
                $_movesCount = $G;
                %_moves = ();
                @_shuffles = ();
                $_moves{$G} = $__packBoard->();
                unshift @_shuffles, $nbr;
                if ( $isBest ) {
                    $$isBest = 1;
                }
            }
            my $goalBoard = $__packBoard->();
            ($_hIndex->[$$ba], $_hIndex->[$$bb], $_hIndex->[-1]) = ($ha, $hb, $hs);
            ($$bb, $$ba, $$bib, $$bia, $$nib, $$nia) = ($$ba, $bav, $$bia, $biav, $$nia, $niav);
            if ( (scalar keys %_moves) == 1 ) {
                $_moves{$G - 1} = $__packBoard->();
            }
            return $H;
        }
        my $best = 0;
        my $m = &DFS($F, $G + 1, $nbr, \$best);
        if ( $m < $min ) {
            $min = $m;
        }
        if ( $best ) {
            $shuffleWith = $nbr;
        }
        ($_hIndex->[$$ba], $_hIndex->[$$bb], $_hIndex->[-1]) = ($ha, $hb, $hs);
        ($$bb, $$ba, $$bib, $$bia, $$nib, $$nia) = ($$ba, $bav, $$bia, $biav, $$nia, $niav);
    }
    if ( !$min && $shuffleWith ) {
        $_moves{$G - 1} = $__packBoard->();
        unshift @_shuffles, $shuffleWith;
    }
    if ( $isBest && $shuffleWith ) {
        $$isBest = 1;
    }
    return $min;
}
