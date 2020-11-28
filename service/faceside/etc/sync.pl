use strict;
use DBI;
use POSIX qw(strftime);
use Time::HiRes qw/gettimeofday tv_interval/;
use File::Basename;

my $period = 305;
my $path   = '/home/weide/iedm_version/faceside/report';
my $host   = "127.0.0.1:3306";
my $db     = "edm";
my $user   = "web";
my $pwd    = "iedm";
my $cache_path = '/home/weide/iedm_version/faceside/cache';

my $CFJH_id_min = 100000000;
my $CFJH_id_max = 999999999;

## DB DOMAIN
my $sqlstm_get_domain = 'select lower(domain) as domain, map_domain  from domain_map';

my $sqlstm_format = q{insert into result 
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
	on duplicate key update 
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

my $sqlstm_format_domain = q{insert into result_by_domain 
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
	on duplicate key update 
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

    ### DB DOMAIN
    my %DB_DOMAIN;
    my $dbh = DBI->connect("dbi:mysql:$db:$host",$user,$pwd, {PrintError=>0, RaiseError=>1, AutoCommit=>0});
    if ( $dbh ) {
        my $sth = $dbh->prepare($sqlstm_get_domain);
        $sth->execute();

        my @data;
        while (@data = $sth->fetchrow() ) {
            $DB_DOMAIN{$data[0]} = $data[1];
        }

        $sth->finish();
        $dbh->disconnect();
    }


	foreach my $file (@files) {

		if ( !open(FD, $file) ) {
			next;
		}

		print nowstr(), "read '$file'\n";

		while (my $line=<FD> ) {

			my ($corp_id, $task_id, $temp_id, $user, $domain, $status) 
				= $line =~ /^(\d+)\.(\d+)\.(\d+)\|([^\|]+)@([^\|]+)\|(ok|domain not exist|spam|not found|retry|fail)\s*$/;

			if (defined($corp_id) 
				and defined($task_id) 
				and defined($temp_id) 
				and defined($domain)
				and defined($status)) {
				if ($task_id<$CFJH_id_min || $task_id>$CFJH_id_max) {
					$temp_id = 1;
				}

                ## DB_DOMAIN
                if ( %DB_DOMAIN ) {
                    if (exists $DB_DOMAIN{lc($domain)}) {
                        $domain = $DB_DOMAIN{$domain}
                    } else{
                        $domain = 'other';
                    }
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

		my $dbh = DBI->connect("dbi:mysql:$db:$host",$user,$pwd, {PrintError=>0, RaiseError=>1, AutoCommit=>0});
		if ( !$dbh ) {
			print nowstr(), "failed to connect mysql:$DBI::errstr";
			goto TAIL_OF_WHILE;
		}

		$dbh->{mysql_auto_reconnect} = 1;

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

			if (exists $REPORT{$taskid}{"ok"}) {
				$reach_count += $REPORT{$taskid}{"ok"};
			}

			if (exists $REPORT{$taskid}{"domain not exist"}) {
				$dns_fail_count += $REPORT{$taskid}{"domain not exist"};
			}
			
			if (exists $REPORT{$taskid}{"spam"}) {
				$spam_garbage_count += $REPORT{$taskid}{"spam"};
			}
			
			if (exists $REPORT{$taskid}{"not found"}) {
				$none_exist_count1 += $REPORT{$taskid}{"not found"};
			}
			
			if (exists $REPORT{$taskid}{"retry"}) {
				$soft_bounce_count += $REPORT{$taskid}{"retry"};
			}
			
			if (exists $REPORT{$taskid}{"fail"}) {
				$hard_bounce_count += $REPORT{$taskid}{"fail"};
			}

			$hard_bounce_count =  $hard_bounce_count+$dns_fail_count+$spam_garbage_count+$none_exist_count1;
			$email_count = $reach_count + $hard_bounce_count + $soft_bounce_count;

			my @grp = split(/\./, $taskid);
			my $sqlstr = sprintf($sqlstm_format, 
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

				if (exists $REPORT_DOMAIN{$taskid}{$domain}{"ok"}) {
					$reach_count += $REPORT_DOMAIN{$taskid}{$domain}{"ok"};
				}

				if (exists $REPORT_DOMAIN{$taskid}{$domain}{"domain not exist"}) {
					$dns_fail_count += $REPORT_DOMAIN{$taskid}{$domain}{"domain not exist"};
				}
				
				if (exists $REPORT_DOMAIN{$taskid}{$domain}{"spam"}) {
					$spam_garbage_count += $REPORT_DOMAIN{$taskid}{$domain}{"spam"};
				}
				
				if (exists $REPORT_DOMAIN{$taskid}{$domain}{"not found"}) {
					$none_exist_count1 += $REPORT_DOMAIN{$taskid}{$domain}{"not found"};
				}
				
				if (exists $REPORT_DOMAIN{$taskid}{$domain}{"retry"}) {
					$soft_bounce_count += $REPORT_DOMAIN{$taskid}{$domain}{"retry"};
				}
				
				if (exists $REPORT_DOMAIN{$taskid}{$domain}{"fail"}) {
					$hard_bounce_count += $REPORT_DOMAIN{$taskid}{$domain}{"fail"};
				}

				$hard_bounce_count =  $hard_bounce_count+$dns_fail_count+$spam_garbage_count+$none_exist_count1;
				$email_count = $reach_count + $hard_bounce_count + $soft_bounce_count;

				my @grp = split(/\./, $taskid);
				my $sqlstr = sprintf($sqlstm_format_domain, 
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
