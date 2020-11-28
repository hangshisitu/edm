#!/usr/bin/perl
# By weide@richinfo.cn  2013/08

#use strict;
#use Socket;
use threads;
use Net::SMTP;

my $interval = 2;
my $nthreads = 5;
my $server = '127.0.0.1:2527';

sub post_smtp()
{
	my $helo = 'localhost';
	my ($sender, $rcpt) = @_;
	my $smtp = Net::SMTP->new($server,Hello=>$helo,Debug=>0);
	$smtp->mail($sender);
	$smtp->to($rcpt);
	$smtp->data();

	if ( defined($file) ) {
		open( FD, $file ) || die("Can not open the file!$!n");
		while($line=<FD>){
			$smtp->datasend($line);
		}

		close ($file);
	}
	else {
		my $message = "From: $sender\r\n".
					  "To: $rcpt\r\n".
					  "Subject: ~Oh,Apple\r\n\r\n".
					  "message\r\n";
		$smtp->datasend($message);
	}

	$smtp->dataend();
	$smtp->quit();
}

sub thread_post_smtp()
{
	my ($num, $count) = @_;
	while($count>0){
		my $sender = "sender" .  (int(rand($count))+1) . "\@test" .  (int(rand($num*2))+1) . ".com";
		my $rcpt = "user" .  (int(rand($count))+1) . "\@target" .  (int(rand($num*2))+1) . ".com";
		&post_smtp($sender,$rcpt);
		$count--;
	}
}

for (my $n=0; $n<$nthreads; $n++){
    threads->new(\&thread_post_smtp, $n, 100000);
}

foreach my $thread (threads->list(threads::all)){
    $thread->join();
}

