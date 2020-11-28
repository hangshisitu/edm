#!/usr/bin/perl -w
#author: weide 2014/5/21

use DBI;
use Digest::file qw(digest_file_hex);

my $connstr = "dbi:Oracle:edm";
# my $connstr = "dbi:mysql:edm:127.0.0.1:3306";
my $user = "web";
my $pwd  = "iedm";
my $file = "yxtask.txt";

my $tmpfile = "$file.tmp";
open(FD,">$tmpfile") or die("Can not open the file!$!n");

my $sqlstr = qq{select corp_id,task_id,template_id from task 
 where task_status in (21,22,23,24) 
 and priority > 0};

my $dbh = DBI->connect($connstr,$user,$pwd) 
    or die "failed to connect mysql,$!";

my $sth = $dbh->prepare($sqlstr);
$sth->execute();

my @data;
while (@data = $sth->fetchrow() ) {
    print FD $data[0],".",$data[1],".",$data[2],"\n";
}

$sth->finish();
$dbh->disconnect();
close(FD);

if (!-e $file || 
    digest_file_hex($file, "MD5") 
    ne digest_file_hex($tmpfile, "MD5")) {
    rename($tmpfile, $file);
    print("reload -> '$file'\n");
} else{
    unlink($tmpfile);
}


