#!/bin/bash

DB_CONSTR="edm/edm@edm"
OUT_FILE="yxtask.txt"
DB_SQLSTR='select corp_id||','||task_id||','||template_id from task where task_status in (21,22,23,24) and priority > 0;'

betafile="$OUT_FILE.beta"
sqlfile="dbexport.sql"
echo "set head off" > $sqlfile
echo "set headsep off" >> $sqlfile
echo "set newp none" >> $sqlfile
echo "set sqlblanklines OFF" >> $sqlfile
echo "set trimspool ON" >> $sqlfile
echo "set termout off" >> $sqlfile
echo "set feedback off" >> $sqlfile
echo "set linesize 2000" >> $sqlfile
echo "spool $betafile" >> $sqlfile
echo "$DB_SQLSTR" >> $sqlfile
echo "spool off" >> $sqlfile
echo "exit" >> $sqlfile

sqlplus -S $DB_CONSTR @$sqlfile;

if [[ "`md5sum $betafile 2>>/dev/null |awk '{print $1}'`" != "`md5sum $OUT_FILE 2>>/dev/null |awk '{print $1}'`" ]]
then
    echo "reload $OUT_FILE"
    cp "$betafile" "$OUT_FILE"
fi

rm "$betafile" "$sqlfile"
