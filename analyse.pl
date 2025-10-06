#!/usr/bin/perl

# Obtained from ed discusson board
use strict;
use warnings;

my $bitcmps = 0;
my $naccess = 0;
my $lines = 0;

my $bmin = 1_000_000_000; # Use underscores for readability
my $bmax = 0; 
my $nmin = 1_000_000_000; 
my $nmax = 0;

while (my $line = <>) {
    # Match the line format: ... --> ... b123 n45 s6
    if ($line =~ /: b(\d+) n(\d+)/) {
        my $b = $1;
        my $n = $2;

        $bitcmps += $b;
        $naccess += $n;
        $lines++;

        if ($b < $bmin) { $bmin = $b; }
        if ($b > $bmax) { $bmax = $b; }

        if ($n < $nmin) { $nmin = $n; }
        if ($n > $nmax) { $nmax = $n; }
    }
}

# Avoid division by zero if the file is empty
if ($lines > 0) {
    print "Minimum Bit Comparisons: " . $bmin . "\n";
    print "Average Bit Comparisons: " . ($bitcmps / $lines) . "\n";
    print "Maximum Bit Comparisons: " . $bmax . "\n";
    print "\n";
    print "Minimum Node Accesses: " . $nmin . "\n";
    print "Average Node Accesses: " . ($naccess / $lines) . "\n";
    print "Maximum Node Accesses: " . $nmax . "\n";
} else {
    print "No valid comparison data found.\n";
}
