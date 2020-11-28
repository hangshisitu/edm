# author: weide@richinfo.cn 
# date  : 2014/03
# recovery.pl
############################

#!/usr/bin/perl
use strict;
use warnings;
use POSIX qw(strftime);
use Time::HiRes qw/gettimeofday tv_interval/;

my @files = <test[23].2014031[56].log>;
my $btime = "2014-03-14 17:10:00";
my $etime = "2014-03-14 17:10:59";
my $tasklist = '\d+\.\d+\.\d+';
my $typelist = 'open|complain|unsubscribe|forward|click redirect';

my %result;
my %result_by_domain;
my %result_by_hour;
my ($bsec,$bmsec)=gettimeofday();

print "begin time:", $btime, "\tend time:", $etime, "\n";

foreach my $file (@files) {
    
	if ( !open(FD, $file) ) {
		print ("Can not open the file!$!n");
		next;
	}

	print "open the file: $file \n";

    while (my $line=<FD> ) {

		if ( $line =~ /$typelist/i && $line =~ /\($tasklist\)/i ) {

			my @fields = split(/[]\s[)(]/, $line);
            my $time = $fields[1] . " " . $fields[2];

			my $taskid = $fields[9];
			$taskid =~ s/^[\s\.]+//;
            $taskid =~ s/[\s\.]+$//;
			$taskid =~ s/\./,/g; 	
			
			if ( $time lt $btime || $time gt $etime || length($taskid)<5) {
				next;
			}

			my $mode = $fields[12];
            if ( $mode eq "complain" ) {
				$mode = "unsubscribe";
			}

            my $email = lc($fields[11]);
			my $domain = substr($email, index($email, "\@") + 1); 

			if ( $mode eq "open" || $mode eq "click" ) {
                my ($hour) = split(/\:/, $fields[2]);
                $result_by_hour {sprintf("%s,%02d", $taskid, $hour)}{$mode} += 1;
			}

            $result {$taskid}{$mode}{lc($email)} += 1;
            $result_by_domain {sprintf("%s,%s", $taskid, $domain)}{$mode}{lc($email)} += 1;
        }
    }

	close (FD);
}

print ("\n\nResult:\ncorp_id,task_id,template_id,read_count,read_user_count,click_count,click_user_count,unsubscribe_count,forward_count\n");

foreach my $keyfirst (keys %result)
{
	my ($read_count,$read_user_count,$click_count,$click_user_count,$unsubscribe_count,$forward_count) = (0,0,0,0,0,0);

	if (exists $result{$keyfirst}->{"open"}) {
		$read_user_count = scalar keys %{$result{$keyfirst}->{"open"}};
		while( my ($keysecond, $valuesecond) = each %{$result{$keyfirst}->{"open"}} ) {
			$read_count += $valuesecond;
		}
	}

	if (exists $result{$keyfirst}->{"click"} ) {
		$click_user_count = scalar keys %{$result{$keyfirst}->{"click"}};
		while(my ($keysecond, $valuesecond) = each %{$result{$keyfirst}->{"click"}} ) {
			$click_count += $valuesecond;
		}
	}

    if (exists $result{$keyfirst}->{"forward"} ) {
		$forward_count = scalar keys %{$result{$keyfirst}->{"forward"}};
	}

	if (exists $result{$keyfirst}->{"unsubscribe"} ) {
		$unsubscribe_count = scalar keys %{$result{$keyfirst}->{"unsubscribe"}};
	}

    print sprintf("%s,%d,%d,%d,%d,%d,%d\n",$keyfirst, $read_count,$read_user_count,$click_count,$click_user_count,$unsubscribe_count,$forward_count);
}


print ("\n\nResult_by_domain:\ncorp_id,task_id,template_id,email_domain,read_count,read_user_count,click_count,click_user_count,unsubscribe_count,forward_count\n");

foreach my $keyfirst (keys %result_by_domain)
{
	my ($read_count,$read_user_count,$click_count,$click_user_count,$unsubscribe_count,$forward_count) = (0,0,0,0,0,0);

	if (exists $result_by_domain{$keyfirst}->{"open"}) {
		$read_user_count = scalar keys %{$result_by_domain{$keyfirst}->{"open"}};
		while( my ($keysecond, $valuesecond) = each %{$result_by_domain{$keyfirst}->{"open"}} ) {
			$read_count += $valuesecond;
		}
	}

	if (exists $result_by_domain{$keyfirst}->{"click"} ) {
		$click_user_count = scalar keys %{$result_by_domain{$keyfirst}->{"click"}};
		while( my ($keysecond, $valuesecond) = each %{$result_by_domain{$keyfirst}->{"click"}} ) {
			$click_count += $valuesecond;
		}
	}

    if (exists $result_by_domain{$keyfirst}->{"forward"} ) {
		$forward_count = scalar keys %{$result_by_domain{$keyfirst}->{"forward"}};
	}

	if (exists $result_by_domain{$keyfirst}->{"unsubscribe"} ) {
		$unsubscribe_count = scalar keys %{$result_by_domain{$keyfirst}->{"unsubscribe"}};
	}

    print sprintf("%s,%d,%d,%d,%d,%d,%d\n",$keyfirst, $read_count,$read_user_count,$click_count,$click_user_count,$unsubscribe_count,$forward_count);
}

print ("\n\nResult_by_hour:\ncorp_id,task_id,template_id,hour,read_count,click_count\n");

foreach my $keyfirst (keys %result_by_hour)
{
	my ($open, $click) = (0,0);

	if (exists $result_by_hour{$keyfirst}->{"open"}) {
		$open = $result_by_hour{$keyfirst}->{"open"};
	}

	if (exists $result_by_hour{$keyfirst}->{"click"} ) {
		$click = $result_by_hour{$keyfirst}->{"click"}
	}

    print sprintf("%s,%d,%d\n",$keyfirst, $open, $click);
}

my ($esec,$emsec)=gettimeofday();
my $time = $esec - $bsec + ($emsec - $bmsec)/1000000; 
print "time: $time seconds\n\n";
exit(0);