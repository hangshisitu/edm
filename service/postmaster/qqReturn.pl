#!/usr/bin/perl -w
use strict;
use warnings;
use LWP::Simple;
use MIME::Base64;
use POSIX qw(strftime);

my @files;
my $path = "/home/weide/iedm_version/PostMaster/filter";
my $forward = "http://192.168.18.156:80";

if ( @ARGV ) {
    foreach my $arg (@ARGV){
        push(@files, glob $arg);
    }
}
else {
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)=localtime(time - 24*3600);
    @files = glob $path.sprintf("/%04d%02d%02d/*.eml",$year+1900,$mon+1,$mday);
}

my $count = 0;
my $message = undef;

foreach my $file (@files) {

    if ( !open(FD, $file) ) {
        print ("Can not open the file!$!n");
        next;
    }

	print "open the file: $file \n";

    my $flags = 0;
	my $boundary = undef; #'QQ_MAIL_RETURN';

    while (my $line=<FD> ) {

        if ( defined($boundary) && $line =~ /^--$boundary/ ) {
            $flags ++;
            next;
        }

		if ($flags == 0) {
			if ( $line =~ /^From:/i && !($line =~ /postmaster@/i ) ) {
				last;
			}

			if ( !defined($boundary) && (($boundary) = $line =~ /boundary[\s]*=([^;]+)/)) {
				$boundary =~ s/^[\s\"\']+//;
				$boundary =~ s/[\s\r\n\"\']+$//;
			}
		}
        elsif ($flags == 2) {
            if ( $line =~ /^To:\s*/i ) {
                $line =~ s/[\s\r\n>,:.\"]+$//;
				my @fields = split(/[\s<]/, $line);
				#get( $forward . "/?e=" . encode_base64($fields[-1]));
				$message .= ($fields[-1] . "\r\n");
                last;
            }
        }
        elsif ($flags == 3) {
            last;
        }
    }

    close (FD);
}

if ( defined ($message) ) {
    system("curl $forward -d '$message' ");
}

exit(0);
