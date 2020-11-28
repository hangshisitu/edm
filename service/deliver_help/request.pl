#!/usr/bin/perl
# By weide@richinfo.cn  2013/08

#use strict;
use Socket;
use threads;

my $interval = 2;
my $nthreads = 5;
my $addr = '127.0.0.1';
my $port = '29999';

my @from = (
"139edm.com",
"mail.139.com",
"n20svrg.139.com",
"n21svrg.139.com",
"n22svrg.139.com",
"n23svrg.139.com",
"n24svrg.139.com",
"mtsvr0.com",
"mtsvr1.com",
"mtsvr2.com",
"mtsvr3.com",
"mtsvr4.com",
"mtsvr5.com",
"mtsvr6.com",
"mtsvr7.com",
"mtsvr8.com",
"mtsvr9.com",
"mtair0.net",
"mtair1.net",
"mtair2.net",
"mtair3.net",
"mtair4.net",
"mtair5.net",
"mtair6.net",
"mtair7.net",
"mtair8.net",
"mtair9.net",
"mtedm0.cn",
"mtedm1.cn",
"mtedm2.cn",
"mtedm3.cn",
"mtedm4.cn",
"mtedm5.cn",
"mtedm6.cn",
"mtedm7.cn",
"mtedm8.cn",
"mtedm9.cn"
);

my @rcpt = (
"qq.com",
"163.com",
"gmail.com",
"126.com",
"sohu.com",
"139.com",
"aliyun.com",
"sina.com",
"richinfo.cn"
);

my @status = ('s','s','s','s','s','s','f','s','f','s','s','s','s','s','s');

sub send_message()
{
    my $buffer = undef;
    my $dest = sockaddr_in($port, inet_aton($addr));
    #print $_[0];
    socket(SOCK,PF_INET,SOCK_STREAM,6) or die "Can't create socket: $!";
    connect(SOCK,$dest) or die "Can't connect: $!";
    syswrite(SOCK, $_[0], length($_[0]));
    select(undef, undef, undef, $interval);
    sysread(SOCK, $buffer, 256);
    close SOCK;

    #my @response = split(/\n/,$buffer);
    #print $response[0] ."\n\n";
    #if ($response[0] =~ /HTTP\/\d\.\d\s+[1-5]\d\d\s+.*/i); 
}

sub request_1001_1002_1003()
{
    while(1)
    {
        my $request="GET /do?cmd=100".(int(rand(2))+1)."&ip=192.168.0.".int(rand(255))." HTTP/1.1\r\n";
        &send_message( $request);
    }
}


sub request_2001()
{
    while(1)
    {
        my $request="GET /do?cmd=2001&ip=192.168.0.".int(rand(255))."&from=".$from[int rand(@from)]."&rcpt=".$rcpt[int rand(@rcpt)]."&status=".$status[int rand @status]." HTTP/1.1\r\n";
        &send_message( $request);
    }	    
}

sub request_2002()
{
    while(1)
    {
        my($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time() - (3600*24)*int(rand(7))); 
        my $date = sprintf("%04d%02d%02d",$year+1990,$mon+1,$mday);
        my $request="GET /do?cmd=2002&ip=192.168.0.".int(rand(255))."&from=".$from[int rand(@from)]."&rcpt=".$rcpt[int rand(@rcpt)]."&date=".$date." HTTP/1.1\r\n";
        &send_message( $request);
    }
}

sub request_2003()
{
    while(1)
    {
        my $request="GET /do?cmd=2003&from=".$from[int rand(@from)]."&rcpt=".$rcpt[int rand(@rcpt)]." HTTP/1.1\r\n";
        &send_message( $request);
    }
}

for (my $n=0; $n<$nthreads; $n++)
{
    threads->new(\&request_2001);
}

for (my $n=0; $n<1; $n++)
{
    threads->new(\&request_2002);
}

for (my $n=0; $n<$nthreads; $n++)
{
    threads->new(\&request_2003);
}

for (my $n=0; $n<3; $n++)
{
    threads->new(\&request_1001_1002_1003);
}

foreach my $thread (threads->list(threads::all)){
    $thread->join();
}

