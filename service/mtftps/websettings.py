#!/usr/bin/env python
# -*- coding: utf8 -*-

# 开启debug
OPEN_DEBUG = False

# 是否打印到终端
DEV_MODE = True

# 日志:
# 1) LOG_SERVER_HOST 不要修改，固定填 localhost
# 2) LOG_SERVER_PORT 填一个没有在使用的端口即可，web通过这个端口把日志发到一个server上来保存日志
# 3) LOG_PATH 是日志保存路径，路径必须存在，'.' 表示当前目录，可以修改成 '/home/tdzx/v3/log/asyncweb'
LOG_SERVER_HOST = 'localhost'
LOG_SERVER_PORT = 9009
LOG_PATH = '.'

#接口接入IP配置(按','分隔)
# API_LIST = '127.0.0.1,192.168.18.123,192.168.86.133'

# mtarget 注册用户名
MTARGET_USER_NAME = 'admin'

# mtarget 注册用户密码
MTARGET_USER_PASS = 'admin'

# 外部账单投递接口url，可以配置多个按’,’分隔，配置多个时候程序将从第一个地
# 址开始发送成功则返回（只做一次成功发送），失败则轮询后面多个地址直到成
# 功为止，如果都不成功则返回失败
RCV_PHP_URL = 'http://192.168.34.103:8085/recv.php'

# 用户ftp文件存储目录(目录名最后应加斜杠/)
FTP_FILE_PATH = '/home/iedm/data_ftp_task/'
