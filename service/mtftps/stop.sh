#!/bin/bash

WEB_PID_FILE_PATH=web.pid

IS_LOG_SERVER_RUNNING=no
LOG_SERVER_PID_FILE_PATH=logserver.pid

# 停止 logserver
if [ -f $LOG_SERVER_PID_FILE_PATH ]; then
    LOG_SERVER_PID=`head -n 1 $LOG_SERVER_PID_FILE_PATH | tr -d ' \t\r\n'`
    if [ -n "$LOG_SERVER_PID" ]; then
        if (( "$LOG_SERVER_PID" > 0 )); then
            if ps -f -p $LOG_SERVER_PID | grep logserver >/dev/null 2>&1; then
                echo "kill logserver pid=$LOG_SERVER_PID ..."
                kill $LOG_SERVER_PID 2>/dev/null
            fi
        fi
    fi
fi

if [ -f $WEB_PID_FILE_PATH ]; then
    while read line; do
        PID=`echo $line | tr -d ' \t\r\n'`
        if [ -n "$PID" ]; then
            if (( "$PID" > 0 )); then
                if ps -f -p $PID | grep 'main.py' >/dev/null 2>&1; then
                    echo "kill web pid=$PID ..."
                    kill $PID 2>/dev/null
                fi
            fi
        fi
    done <$WEB_PID_FILE_PATH
fi

echo "done"
