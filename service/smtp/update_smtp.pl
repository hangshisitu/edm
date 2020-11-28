#!/usr/bin/perl -w
use DBI;

if (! @ARGV) {
	exit(0);
}

my $fieldname;

if ($ARGV[0] eq 'q') {
	$fieldname = 'quarter_count';
}
elsif ($ARGV[0] eq 'h') {
	$fieldname = 'hour_count';
}
elsif ($ARGV[0] eq 'd') {
	$fieldname = 'day_count';
}
else{
	exit(0);
}

#my $connstr = "dbi:Oracle:edm";
my $connstr = "dbi:mysql:edm:127.0.0.1:3306";
my $user = "web";
my $pwd  = "iedm";
my $sqlstr = qq{update smtp set $fieldname = 0};
my $dbh = DBI->connect($connstr,$user,$pwd) or die "failed to connect mysql,$!";
$dbh->do($sqlstr);
#$sth->finish();
$dbh->disconnect();
