#!/usr/bin/perl -w
use strict;
use warnings;
use POSIX qw(strftime);

# 设置明细显示的最大行数,
# 超过则显示为other
my $print_item_nums = 15;

my ($reach_text, $soft_text,$hard_text) = ('reach-count','soft-bounce','hard-bounce');
my @files;

if (! @ARGV) {
	print " (~_~)\n";
	exit(0);
}

foreach my $arg (@ARGV){
    push(@files, glob $arg);
}

sub print_something(){

    my ($num) = @_;
    my $text = "";
	for (my $i=0;$i<$num;$i++){
       $text = $text . "*";
    }
	print $text, "\n";
}

sub print_detail_record(\%\%){

    my( $data, $data_sort, $title) = @_;
	my $item_num = 0;
	my ($other_reach_count,
		$other_soft_bounce,
		$other_hard_bounce) = (0,0,0);

	printf "%-35s %-17s  %-18s  %-18s  %-18s\n", 
	"target",$title,$reach_text,$soft_text,$hard_text;
	&print_something(114);

	foreach my $host ( 
		sort { $data_sort->{$b} <=> $data_sort->{$a} } 
		keys %$data_sort ){

		my ($reach_count,$soft_bounce,$hard_bounce) = (0,0,0);

		if (exists $data->{$host}{$reach_text}) {
			$reach_count = $data->{$host}{$reach_text};
		}

		if (exists $data->{$host}{$soft_text}) {
			$soft_bounce = $data->{$host}{$soft_text};
		}

		if (exists $data->{$host}{$hard_text}) {
			$hard_bounce = $data->{$host}{$hard_text};
		}

		my $sums = $reach_count + $soft_bounce + $hard_bounce;

		if (defined($print_item_nums) 
			and $item_num >= $print_item_nums) {

			$other_reach_count += $reach_count;
			$other_soft_bounce += $soft_bounce;
			$other_hard_bounce += $hard_bounce;
			next;
		}

		if ( $sums > 0) {

			$item_num += 1;
			printf "%-35s %-17d  %-18s  %-18s  %-18s\n",
				$host, $sums,
				sprintf("%d (%0.2f%%)", 
					$reach_count, 
					100*$reach_count/$sums),

				sprintf("%d (%0.2f%%)", 
					$soft_bounce, 
					100*$soft_bounce/$sums),

				sprintf("%d (%0.2f%%)", 
					$hard_bounce, 
					100*$hard_bounce/$sums);
		}
	}

	if (defined($print_item_nums) 
		and $item_num >= $print_item_nums) {

		my $other_sums = $other_reach_count 
			+ $other_soft_bounce 
			+ $other_hard_bounce;

        if ($other_sums > 0) {
			printf "%-35s %-17d  %-18s  %-18s  %-18s\n",
				"other", 
				$other_sums,

				sprintf("%d (%0.2f%%)", 
				$other_reach_count, 
				100*$other_reach_count/$other_sums),

				sprintf("%d (%0.2f%%)", 
				$other_soft_bounce, 
				100*$other_soft_bounce/$other_sums),

				sprintf("%d (%0.2f%%)", 
				$other_hard_bounce, 
				100*$other_hard_bounce/$other_sums);
        }
	}
	&print_something(114);
	print "\n";
}

my ($starttime, $endtime);

my (%records, 
%records_ip, 
%records_user, 
%domain_distinct,
%domain_distinct_sort, 
%domain, 
%domain_sort,
%ips, 
%ips_sort, 
%users, 
%users_sort
);

foreach my $file (@files) {

    if ( !open(FD, $file) ) {
        next;
    }

    while (my $line=<FD> ) {
		
        # [2016-04-28 12:15:08][2777888512] Result=OK,Mail=qq@qwe.com(qq@qwe.com),Rcpt=sfsdf@qe.com,Client=127.0.0.1:56835,Remote=127.0.0.1:12580,X-Mid=,Respond=250 OK
		my ($data) = $line =~ /(Result=.*)/;
        if ( !defined($data)) {
            next;
        }

		my ($desc) = $data =~ /Result=([^,]+),/;
		my ($rcpt) = $data =~ /Rcpt=([^,]*),/;
		my ($err) = $data =~ /Respond=(\d\d\d)/;
        my ($client) = $data =~ /Client=([^,:]+)/;
        my ($sender) = $data =~ /,Mail=([^,\(]+)/;

		if (!defined($desc) or !defined($rcpt)) {
			next;
		}

		if (!defined($starttime)) {
			($starttime) = $line =~ /^\[([^\]\[]+)\]/;
		}

		($endtime) = $line =~ /^\[([^\]\[]+)\]/;

		if (!defined($err)) {
			$err = 0;
		}

		my $host = lc(substr($rcpt, index($rcpt, "\@") + 1));
		my $type;

		if ($desc eq 'OK' or $desc eq 'RetryOK') {
			$type = $reach_text;
		} else {
			my $mask = int($err/100);
			$type = ($mask == 4 or $mask == 6) 
				? $soft_text:$hard_text;
		}

		$records{$type}{$rcpt} += 1;
		$domain{$host}{$type} += 1;
		$domain_sort{$host} += 1;

		$records_ip{$type}{$client} += 1;
		$ips{$client}{$type} += 1;
		$ips_sort{$client} += 1;

		$records_user{$type}{$sender} += 1;
		$users{$sender}{$type} += 1;
		$users_sort{$sender} += 1;
    }

    close (FD);
}

my ($mail_reach_count,
$mail_soft_bounce,
$mail_hard_bounce) = (0,0,0);

my ($send_reach_count,
$send_soft_bounce,
$send_hard_bounce) = (0,0,0);

#################

foreach my $rcpt (keys %{$records{$reach_text}}){
	$mail_reach_count += 1;
	$send_reach_count += $records{$reach_text}{$rcpt};

	my $host = lc(substr($rcpt, index($rcpt, "\@") + 1));
	$domain_distinct_sort{$host} += 1;
	$domain_distinct{$host}{$reach_text} += 1;
}

foreach my $rcpt (keys %{$records{$hard_text}}){

	$send_hard_bounce += $records{$hard_text}{$rcpt};

	if (!exists $records{$reach_text}{$rcpt}) {
		$mail_hard_bounce += 1;
		my $host = lc(substr($rcpt, index($rcpt, "\@") + 1));
		$domain_distinct_sort{$host} += 1;
		$domain_distinct{$host}{$hard_text} += 1;
	}
}

foreach my $rcpt (keys %{$records{$soft_text}}){

	$send_soft_bounce += $records{$soft_text}{$rcpt};

	if (!exists $records{$reach_text}{$rcpt} 
	and !exists $records{$hard_text}{$rcpt}) {
		$mail_soft_bounce += 1;
		my $host = lc(substr($rcpt, index($rcpt, "\@") + 1));
		$domain_distinct_sort{$host} += 1;
		$domain_distinct{$host}{$soft_text} += 1;
	}
}

if (defined($starttime) and defined($endtime) ){
    printf "\n%50s ~ %s\n",$starttime,$endtime;
}

my $email_sum = $mail_reach_count+$mail_soft_bounce+$mail_hard_bounce;
printf "\nemail-count: \t%d\n", $email_sum;
if ($email_sum < 1) {
    exit(0);
}

printf "  reach-count:\t %d (%0.2f%%)\n", $mail_reach_count, 100*$mail_reach_count/$email_sum;
printf "  soft-bounce:\t %d (%0.2f%%)\n",  $mail_soft_bounce, 100*$mail_soft_bounce/$email_sum;
printf "  hard-bounce:\t %d (%0.2f%%)\n\n", $mail_hard_bounce, 100*$mail_hard_bounce/$email_sum;
&print_detail_record(\%domain_distinct,\%domain_distinct_sort, "email-count");

my $send_sum = $send_reach_count+$send_soft_bounce+$send_hard_bounce;
printf "\nsent-count: \t%d\n", $send_sum;
printf "  reach-count:\t %d (%0.2f%%)\n", $send_reach_count, 100*$send_reach_count/$send_sum;
printf "  soft-bounce:\t %d (%0.2f%%)\n",  $send_soft_bounce, 100*$send_soft_bounce/$send_sum;
printf "  hard-bounce:\t %d (%0.2f%%)\n\n", $send_hard_bounce, 100*$send_hard_bounce/$send_sum;

printf "domain status:\n";
&print_detail_record(\%domain,\%domain_sort, "send-count");

printf "rcpt status:\n";
&print_detail_record(\%users,\%users_sort, "send-count");

printf "ip status:\n";
&print_detail_record(\%ips,\%ips_sort, "send-count");

printf "\n";
exit(0);
