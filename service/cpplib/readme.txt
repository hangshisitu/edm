rfc=Richinfo Foundation Class

rfc����ʹ�÷�����
1����rfc����checkout������Ŀ¼��svn co $RFC_SVN_PATH $(RFC_PATH)
2������$(RFC_PATH)/buildĿ¼��ִ��make����
3�����ӱ������
RFC_INCLUDE_FLAGS = -I$(RFC_PATH)
4���������Ӳ���
RFC_LINK_FLAGS = -L$(RFC_PATH)/build -lrfc_network -lrfc_system -lrfc_protocolbuf -lrfc_stream -lrfc_charset -lrfc_base
����Ҫ�������Ӳ��� SYSTEM_LINK_FLAGS = -lpthread -lresolv


rfc�ڲ�lib������ϵ��
librfc_base.a�� ���������⣬�������ڲ�lib
librfc_charset.a������librfc_base.a
librfc_checksum.a������librfc_base.a��librfc_stream.a
librfc_mutex.a������librfc_base.a��ͬʱ����ϵͳ�ײ��libpthread
librfc_network.a������librfc_base.a��SocketStream����librfc_stream.a��DNSResolver����librfc_system.a��ͬʱ����ϵͳ�ײ��libpthread libresolv
librfc_protocolbuf.a������librfc_base.a��librfc_stream.a
librfc_stream.a������librfc_base.a
librfc_system.a������librfc_base.a��librfc_mutex.a��FileHandle��������librfc_stream.a��ͬʱ����ϵͳ�ײ��libpthread
librfc_web.a������librfc_base.a

TODO:
110901��class MutexBase ɾ�� m_nWaitingLockCount����Ϊ����ԭ�Ӳ���
110901������ⲻ��Ҫ����mutex


