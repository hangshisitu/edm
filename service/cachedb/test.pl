#!perl

use DBI;
use MIME::Base64;
srand;

my @domain = ("qq.com","163.com","126.com","139.com","gmail.com","sohu.com","sina.com","hotmail.com", "richinfo.cn");
@basestr = ('a'..'z');
my $ngxserver = "192.168.18.156";
my $db = "edm";
my $host = "127.0.0.1:3306";
my $user = "web";
my $pwd = "iedm";

my @taskid;
$sql = "select CONCAT('\&ta=',task_id,'\&te=',template_id,'\&c=', corp_id) from task"; # limit XXX

my $dbh = DBI->connect("dbi:mysql:$db:$host",$user,$pwd) or die "failed to connect mysql,$!";
my $sth = $dbh->prepare($sql);
$sth->execute();

my @data;
while (@data = $sth->fetchrow() ) {
	push @taskid, $data[0];
}

$sth->finish();
$dbh->disconnect();

my $count = 1000;
while($count > 0) {
	ngxopen();
	ngxopen();
	ngxopen();
	ngxclick();
	ngxopen();
	ngxclick();
	ngxopen();
	select(undef, undef, undef, 0.1); 
}

sub ngxopen()
{
	my $item = $taskid[int rand @taskid];
    $randdomain = join '', map { $basestr[int rand @basestr] } 0..2;
    $randsender = join '', map { $basestr[int rand @basestr] } 0..2;
	$url = "http://www.$randdomain.com";
	$sender = "$randsender\@$domain[int rand @domain]";
	my $request = "http://$ngxserver/test2.php?u=$url$item&la=".int(rand(16))."&e=$sender";

	system("curl '$request'");
}

sub ngxclick()
{
	my $item = $taskid[int rand @taskid];
	$randdomain = join '', map { $basestr[int rand @basestr] } 0..2;
    $randsender = join '', map { $basestr[int rand @basestr] } 0..2;
	$url = encode_base64("http://www.$randdomain.com");
	$sender = encode_base64("$randsender\@$domain[int rand @domain]");
	chomp($url);
	chomp($sender);
	my $request = "http://$ngxserver/test3.php?u=$url$item&la=".int(rand(16))."&e=$sender";

	system("curl '$request'");
}

