#!/bin/bash

IS_LOG_SERVER_RUNNING=no
LOG_SERVER_PID_FILE_PATH=logserver.pid
WEB_PID_FILE_PATH=web.pid
PY=/usr/local/python2.7/bin/python

if [ -f $LOG_SERVER_PID_FILE_PATH ]; then
    LOG_SERVER_PID=`head -n 1 $LOG_SERVER_PID_FILE_PATH | tr -d ' \t\r\n'`
    if [ -n "$LOG_SERVER_PID" ]; then
        if (( "$LOG_SERVER_PID" > 0 )); then
            if ps -f -p $LOG_SERVER_PID | grep logserver >/dev/null 2>&1; then
                IS_LOG_SERVER_RUNNING=yes
            fi
        fi
    fi
fi

# is running web ?
if [ -f $WEB_PID_FILE_PATH ]; then
    RUNNING_WEB=no
    while read line; do
        PID=`echo $line | tr -d ' \t\r\n'`
        if [ -n "$PID" ]; then
            if (( "$PID" > 0 )); then
                if ps -f -p $PID | grep 'main.py' >/dev/null 2>&1; then
                    echo "web process $PID is running"
                    RUNNING_WEB=yes
                fi
            fi
        fi
    done <$WEB_PID_FILE_PATH
    if [ $RUNNING_WEB = yes ]; then
        echo "kill running web process, and then start."
        exit 1
    fi
fi

if [ $IS_LOG_SERVER_RUNNING = 'no' ]; then
    echo "start logserver ..."
    nohup ${PY} logserver.py >/dev/null 2>&1 &
    echo $!>logserver.pid
fi

rm -f $WEB_PID_FILE_PATH
for (( i=9220; i<=9221; i++ )); do
    echo "start web at port $i ..."
    nohup ${PY} main.py -p $i >>out.log 2>&1 &
    echo $!>>$WEB_PID_FILE_PATH
done

echo "done"
