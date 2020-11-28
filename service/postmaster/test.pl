#!/usr/bin/perl -w
use warnings;
use Net::SMTP;

my $helo   = 'localhost';
my $mail   = 'PostMaster@qq.com';
my $rcpt   = 'test@edm.emailfocus.cn';
my $server = '127.0.0.1:12580';

my $smtp = Net::SMTP->new($server,Hello=>$helo,Debug=>0);
$smtp->mail($mail);
$smtp->to($rcpt);
$smtp->data();

if ( @ARGV ) {

    open( FD, $ARGV[0] )||die("Can not open the file!$!n");

    while($line=<FD>){
        $smtp->datasend($line);
    }

	close ($ARGV[0]);
}
else {
    my $message = "From: $mail\r\n".
                  "To: $rcpt\r\n".
                  "Subject: ~Oh,Apple\r\n\r\n".
                  "message\r\n";
	$smtp->datasend($message);
}

$smtp->dataend();
$smtp->quit();
