����ǰ����ƽ̨��lib32��lib64���������ӣ�
ln -s lib32 lib

iconv���ʹ�÷�����
1������lib32��lib64
2���⿪libiconv.so.2.5.0.tgzѹ�����õ�libiconv.so.2.5.0��tar zxvf libiconv.so.2.5.0.tgz
3������������libiconv.so.2 -> libiconv.so.2.5.0��ln -s libiconv.so.2.5.0 libiconv.so.2
4������������libiconv.so -> libiconv.so.2.5.0��ln -s libiconv.so.2.5.0 libiconv.so

oracle���ʹ�÷�����
1������lib32��lib64
2���⿪libclntsh.so.11.1.tgzѹ�����õ�libclntsh.so.11.1��tar zxvf libclntsh.so.11.1.tgz
3���⿪libocci.so.11.1.tgzѹ�����õ�libocci.so.11.1��tar zxvf libocci.so.11.1.tgz
4������������libclntsh.so -> libclntsh.so.11.1��ln -s libclntsh.so.11.1 libclntsh.so
5������������libocci.so -> libocci.so.11.1��ln -s libocci.so.11.1 libocci.so

