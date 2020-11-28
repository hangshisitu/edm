#!/usr/bin/perl
# nohup @appname 
use strict;
use Socket;

if ((@ARGV != 2) {
    print "Usage : $0 [CSV_FILE] [LABELS]"."\n";
    exit;
}

my $file = $ARGV[0];
my $labels = $ARGV[1];

my $addr = '127.0.0.1';
my $port = '5758';

open(FD,$file)||die("Can not open the file!$!n");

my $succeed = 0;
my $failed = 0;
my $line;

while($line=<FD>)
{
	$line =~ s/^\s+//;
    $line =~ s/\s+$//;
    $line =~ s/\r+$//;
    $line =~ s/\n+$//;

    if ( length($line) < 1){
    	next;
    }
	my $buffer = undef;
	my $request = "GET /insert?label=".$labels."&taskid=".$line." HTTP/1.1\r\n";
	my $dest = sockaddr_in($port, inet_aton($addr));

	socket(SOCK,PF_INET,SOCK_STREAM,6) or die "Can't create socket: $!";
	connect(SOCK,$dest) or die "Can't connect: $!";
	syswrite(SOCK, $request, length($request));
	sysread(SOCK, $buffer, 256);
	close SOCK; 

	my @response = split(/\n/,$buffer);
	if ($response[0] =~ /HTTP\/\d\.\d\s+[1-5]\d\d\s+.*/i) {
		my @retval = split(/\s+/,$response[0]);
		if ( $retval[1] == 200 ){
			$succeed = $succeed + 1;
			next;
		}
	}

	$failed = $failed + 1;
	print "┌->".$request;
	print "└->".$response[0]."\n";
}

close(FD);

print "\n"."Succeed:".$succeed." Failed:".$failed."\n\n";
