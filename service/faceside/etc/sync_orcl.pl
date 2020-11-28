use strict;
use DBI;
use POSIX qw(strftime);
use Time::HiRes qw/gettimeofday tv_interval/;
use File::Basename;

my $period = 30;
my $path   = '/home/weide/iedm_version/faceside/report';
my $connstr = "dbi:Oracle:192.168.18.156:1512/edm";
my $user   = "edm";
my $pwd    = "edm";
my $cache_path = '/home/weide/iedm_version/faceside/cache';

my $CFJH_id_min = 100000000;
my $CFJH_id_max = 999999999;

my $sqlstm_format = q{merge into result t1
    using (select %d as a,%d as b, %d as c from dual) t2 
    on ( t1.corp_id=t2.a and t1.task_id=t2.b and t1.template_id=t2.c) 
    when not matched then
    insert
    (corp_id,task_id,template_id,
    email_count,
    sent_count,
    reach_count,
    soft_bounce_count,
    hard_bounce_count,
    none_exist_count1,
    dns_fail_count,
    spam_garbage_count,
    end_send_time) 
    values (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,now()) 
    when matched then 
    update set  
    email_count = email_count + %d,
    sent_count = sent_count + %d,
    reach_count = reach_count + %d,
    soft_bounce_count = soft_bounce_count + %d,
    hard_bounce_count = hard_bounce_count + %d,
    none_exist_count1 = none_exist_count1 + %d,
    dns_fail_count = dns_fail_count + %d,
    spam_garbage_count = spam_garbage_count + %d,
    end_send_time = now()
};

my $sqlstm_format_domain = q{merge into result_by_domain t1
    using (select %d as a,%d as b, %d as c, '%s' as d from dual) t2 
    on ( t1.corp_id=t2.a and t1.task_id=t2.b and t1.template_id=t2.c and t1.email_domain=t2.d) 
    when not matched then
    insert
    (corp_id,task_id,template_id,email_domain,
    email_count,
    sent_count,
    reach_count,
    soft_bounce_count,
    hard_bounce_count,
    none_exist_count1,
    dns_fail_count,
    spam_garbage_count) 
    values (%d,%d,%d,'%s',%d,%d,%d,%d,%d,%d,%d,%d) 
    when matched then 
    update set   
    email_count = email_count + %d,
    sent_count = sent_count + %d,
    reach_count = reach_count + %d,
    soft_bounce_count = soft_bounce_count + %d,
    hard_bounce_count = hard_bounce_count + %d,
    none_exist_count1 = none_exist_count1 + %d,
    dns_fail_count = dns_fail_count + %d,
    spam_garbage_count = spam_garbage_count + %d
};

sub nowstr() {
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)=localtime();
    return sprintf("[%04d-%02d-%02d %02d:%02d:%02d] ",$year+1900,$mon+1,$mday,$hour,$min,$sec);
}

while (1) {

	my @files;
	push(@files, glob "$path/*.side");

	my %REPORT;
	my %REPORT_DOMAIN;

	foreach my $file (@files) {

		if ( !open(FD, $file) ) {
			next;
		}

		print nowstr(), "read '$file'\n";

		while (my $line=<FD> ) {

			my ($corp_id, $task_id, $temp_id, $user, $domain, $status) 
				= $line =~ /^(\d+)\.(\d+)\.(\d+)\|([^\|]+)@([^\|]+)\|(OK|DNS_FAIL|SPAM|NONE_EXIST|RETRY|FAIL)\s*$/;

			if (defined($corp_id) 
				and defined($task_id) 
				and defined($temp_id) 
				and defined($domain)
				and defined($status)) {
				if ($task_id<$CFJH_id_min || $task_id>$CFJH_id_max) {
					$temp_id = 1;
				}

				my $taskid = sprintf("%d.%d.%d", $corp_id, $task_id, $temp_id);
				$REPORT{$taskid}{$status} += 1;
				$REPORT_DOMAIN{$taskid}{$domain}{$status} += 1;

				#cache task
				my $taskfile = $cache_path . sprintf("/%d.%d.%d.task", $corp_id, $task_id, $temp_id);
                open(FILE, ">>$taskfile");
				if (-z $taskfile) {
					print FILE "<\$MAILLIST>\n";
				}
				print FILE sprintf("%s\@%s,\n", $user, $domain);
				close (FILE);
			}
		}

		close (FD);
	}

    my ($total, $successful) = (0,0);

	if (%REPORT and %REPORT_DOMAIN) {

		my $dbh = DBI->connect($connstr, $user,$pwd, {PrintError=>0, RaiseError=>1, AutoCommit=>0});
		if ( !$dbh ) {
			print nowstr(), "failed to connect :$DBI::errstr";
			goto TAIL_OF_WHILE;
		}

		# update result
		foreach my $taskid (keys %REPORT){

			my ($email_count,
				$sent_count,
				$reach_count,
				$soft_bounce_count,
				$hard_bounce_count,
				$none_exist_count1,
				$dns_fail_count,
				$spam_garbage_count) = (0,0,0,0,0,0,0,0);

			if (exists $REPORT{$taskid}{"OK"}) {
				$reach_count += $REPORT{$taskid}{"OK"};
			}

			if (exists $REPORT{$taskid}{"DNS_FAIL"}) {
				$dns_fail_count += $REPORT{$taskid}{"DNS_FAIL"};
			}
			
			if (exists $REPORT{$taskid}{"SPAM"}) {
				$spam_garbage_count += $REPORT{$taskid}{"SPAM"};
			}
			
			if (exists $REPORT{$taskid}{"NONE_EXIST"}) {
				$none_exist_count1 += $REPORT{$taskid}{"NONE_EXIST"};
			}
			
			if (exists $REPORT{$taskid}{"RETRY"}) {
				$soft_bounce_count += $REPORT{$taskid}{"RETRY"};
			}
			
			if (exists $REPORT{$taskid}{"FAIL"}) {
				$hard_bounce_count += $REPORT{$taskid}{"FAIL"};
			}

			$hard_bounce_count =  $hard_bounce_count+$dns_fail_count+$spam_garbage_count+$none_exist_count1;
			$email_count = $reach_count + $hard_bounce_count + $soft_bounce_count;

			my @grp = split(/\./, $taskid);
			my $sqlstr = sprintf($sqlstm_format, 
					$grp[0], $grp[1], $grp[2],
					$grp[0], $grp[1], $grp[2],
					$email_count,
					$email_count,
					$reach_count,
					$soft_bounce_count,
					$hard_bounce_count,
					$none_exist_count1,
					$dns_fail_count,
					$spam_garbage_count,
					$email_count,
					$email_count,
					$reach_count,
					$soft_bounce_count,
					$hard_bounce_count,
					$none_exist_count1,
					$dns_fail_count,
					$spam_garbage_count); 
			
			$total++;
			eval { $dbh->do($sqlstr);};

			if ( !$@ ){
				$successful++;
			}else{
				print sprintf("(result)DBI err=%d, errstr=%s\n",$DBI::err,$DBI::errstr);
			}
		}

		$dbh->commit();

	   # update result_by_domain
		foreach my $taskid (keys %REPORT_DOMAIN)
		{
			foreach my $domain (keys %{$REPORT_DOMAIN{$taskid}})
			{
				my ($email_count,
					$sent_count,
					$reach_count,
					$soft_bounce_count,
					$hard_bounce_count,
					$none_exist_count1,
					$dns_fail_count,
					$spam_garbage_count) = (0,0,0,0,0,0,0,0);

				if (exists $REPORT_DOMAIN{$taskid}{$domain}{"OK"}) {
					$reach_count += $REPORT_DOMAIN{$taskid}{$domain}{"OK"};
				}

				if (exists $REPORT_DOMAIN{$taskid}{$domain}{"DNS_FAIL"}) {
					$dns_fail_count += $REPORT_DOMAIN{$taskid}{$domain}{"DNS_FAIL"};
				}
				
				if (exists $REPORT_DOMAIN{$taskid}{$domain}{"SPAM"}) {
					$spam_garbage_count += $REPORT_DOMAIN{$taskid}{$domain}{"SPAM"};
				}
				
				if (exists $REPORT_DOMAIN{$taskid}{$domain}{"NONE_EXIST"}) {
					$none_exist_count1 += $REPORT_DOMAIN{$taskid}{$domain}{"NONE_EXIST"};
				}
				
				if (exists $REPORT_DOMAIN{$taskid}{$domain}{"RETRY"}) {
					$soft_bounce_count += $REPORT_DOMAIN{$taskid}{$domain}{"RETRY"};
				}
				
				if (exists $REPORT_DOMAIN{$taskid}{$domain}{"FAIL"}) {
					$hard_bounce_count += $REPORT_DOMAIN{$taskid}{$domain}{"FAIL"};
				}

				$hard_bounce_count =  $hard_bounce_count+$dns_fail_count+$spam_garbage_count+$none_exist_count1;
				$email_count = $reach_count + $hard_bounce_count + $soft_bounce_count;

				my @grp = split(/\./, $taskid);
				my $sqlstr = sprintf($sqlstm_format_domain, 
						$grp[0], $grp[1], $grp[2],$domain,
						$grp[0], $grp[1], $grp[2],$domain,
						$email_count,
						$email_count,
						$reach_count,
						$soft_bounce_count,
						$hard_bounce_count,
						$none_exist_count1,
						$dns_fail_count,
						$spam_garbage_count,
						$email_count,
						$email_count,
						$reach_count,
						$soft_bounce_count,
						$hard_bounce_count,
						$none_exist_count1,
						$dns_fail_count,
						$spam_garbage_count); 
				
				$total++;
				eval { $dbh->do($sqlstr);};

				if ( !$@ ){
					$successful++;
				}else{
					print nowstr(), sprintf("(domain)DBI err=%d, errstr=%s\n",$DBI::err,$DBI::errstr);
				}
			}
		}

		$dbh->commit();
		$dbh->disconnect();
	}

	my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)=localtime();
	my $backup = sprintf("%s/%04d%02d%02d",$path, $year+1900,$mon+1,$mday);
	mkdir $backup;

	foreach my $file (@files) {
		my($fname, $dir, $ext)=fileparse($file);
		rename ($file, sprintf("%s/%s%s",$backup,$fname,$ext));
	}

TAIL_OF_WHILE:

	eval{
		print nowstr(), 
		"total: $total\tsuccessful: $successful\t",
		sprintf("(%0.3f%%)\n\n",$successful/$total*100);
	};

	sleep $period;
}

exit(0);
