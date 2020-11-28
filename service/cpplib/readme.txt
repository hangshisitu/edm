rfc=Richinfo Foundation Class

rfc类库的使用方法：
1，将rfc代码checkout到本地目录，svn co $RFC_SVN_PATH $(RFC_PATH)
2，进入$(RFC_PATH)/build目录，执行make编译
3，增加编译参数
RFC_INCLUDE_FLAGS = -I$(RFC_PATH)
4，增加连接参数
RFC_LINK_FLAGS = -L$(RFC_PATH)/build -lrfc_network -lrfc_system -lrfc_protocolbuf -lrfc_stream -lrfc_charset -lrfc_base
按需要增加连接参数 SYSTEM_LINK_FLAGS = -lpthread -lresolv


rfc内部lib依赖关系：
librfc_base.a： 最基础的类库，不依赖内部lib
librfc_charset.a：依赖librfc_base.a
librfc_checksum.a：依赖librfc_base.a和librfc_stream.a
librfc_mutex.a：依赖librfc_base.a，同时依赖系统底层库libpthread
librfc_network.a：依赖librfc_base.a，SocketStream依赖librfc_stream.a，DNSResolver依赖librfc_system.a，同时依赖系统底层库libpthread libresolv
librfc_protocolbuf.a：依赖librfc_base.a和librfc_stream.a
librfc_stream.a：依赖librfc_base.a
librfc_system.a：依赖librfc_base.a和librfc_mutex.a，FileHandle依赖依赖librfc_stream.a，同时依赖系统底层库libpthread
librfc_web.a：依赖librfc_base.a

TODO:
110901：class MutexBase 删除 m_nWaitingLockCount，因为不是原子操作
110901：网络库不需要依赖mutex


