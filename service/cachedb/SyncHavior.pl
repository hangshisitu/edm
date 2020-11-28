# author: weide@richinfo.cn 
# date  : 2014/03
# SyncHavior.pl
############################
use strict;
use DBI;
use POSIX qw(strftime);
use Time::HiRes qw/gettimeofday tv_interval/;

my $file;
my $path = '/home/weide/iedm_version/cachedb/data';
my $prefix = 'daily.txt';
my $host = "127.0.0.1:3306";
my $db = "edm";
my $user = "web";
my $pwd = "iedm";

my $ddl_create_sqlstm = q{create table `%s` (
`id`  int(11) not null auto_increment ,
`corp_id`  int(11) not null ,
`task_id`  int(11) not null ,
`template_id`  int(11) not null ,
`email`  char(64) character set utf8 collate utf8_general_ci not null default '' ,
`havior`  tinyint(4) not null default 0 ,
`osid`  tinyint(4) null default null ,
`browserid`  tinyint(4) null default null ,
`slangid`  tinyint(4) null default null ,
`open`  tinyint(4) null default 0 ,
`click`  tinyint(4) null default 0 ,
`updatetime`  datetime null default null ,
`url`  varchar(4096) DEFAULT NULL,
primary key (`id`)
)};

sub now() {
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)=localtime();
    return sprintf("[%04d-%02d-%02d %02d:%02d:%02d] ",$year+1900,$mon+1,$mday,$hour,$min,$sec);
}

if ( -e $ARGV[0] ) {
	$file = $ARGV[0];
} 
else {
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)=localtime(time - 24*3600);
    $file = "$path/$prefix." . sprintf("%04d%02d%02d",$year+1900,$mon+1,$mday);
}

my ($bsec,$bmsec)=gettimeofday();
open(FD,$file) || die("Can not open the file!$!n");

print now(), "open the file '$file'\n";

my $dbh = DBI->connect("dbi:mysql:$db:$host",$user,$pwd, {PrintError=>0, RaiseError => 1}) 
          or die "failed to connect mysql:$DBI::errstr";

$dbh->{mysql_auto_reconnect} = 1;

my $total;
my $successful;
my $line;

while( $line=<FD> ) {

    $line =~ s/^\s+//;
    $line =~ s/\s+$//;
    $line =~ s/\r+$//;
    $line =~ s/\n+$//;

	if ( length($line) < 1){
        next;
    }

    $total ++;
	my $zxvf = 2;

	while($zxvf-- > 0) {

		eval { $dbh->do($line); };

        if ( !$@ ){
			$successful ++;
			last;
		}

		if ($DBI::err == 1146) {
			my ($table) = $DBI::errstr =~ /Table \'\w+\.([\w_]+)\' doesn\'t exist/i;
			print now(), "create table '$table'\n";

			eval { 
				$dbh->do(sprintf($ddl_create_sqlstm, $table)); 
			};

			if ($@){
				print now(), "$DBI::errstr [$line]\n";
			    last;
		    }
		}
		else {
			print now(), "$DBI::errstr [$line]\n";
			last;
		}
	} 
}

close (FD);
$dbh->disconnect();

rename ($file,"$file.bak");

my ($esec,$emsec)=gettimeofday();
my $time = $esec - $bsec + ($emsec - $bmsec)/1000000; 
print now(), "total: $total\tsuccessful: $successful\ttime: $time\n\n";
exit(0);
