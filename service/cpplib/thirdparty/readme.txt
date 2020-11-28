编译前根据平台对lib32或lib64创建软连接：
ln -s lib32 lib

iconv库的使用方法：
1，进入lib32或lib64
2，解开libiconv.so.2.5.0.tgz压缩包得到libiconv.so.2.5.0：tar zxvf libiconv.so.2.5.0.tgz
3，创建软连接libiconv.so.2 -> libiconv.so.2.5.0：ln -s libiconv.so.2.5.0 libiconv.so.2
4，创建软连接libiconv.so -> libiconv.so.2.5.0：ln -s libiconv.so.2.5.0 libiconv.so

oracle库的使用方法：
1，进入lib32或lib64
2，解开libclntsh.so.11.1.tgz压缩包得到libclntsh.so.11.1：tar zxvf libclntsh.so.11.1.tgz
3，解开libocci.so.11.1.tgz压缩包得到libocci.so.11.1：tar zxvf libocci.so.11.1.tgz
4，创建软连接libclntsh.so -> libclntsh.so.11.1：ln -s libclntsh.so.11.1 libclntsh.so
5，创建软连接libocci.so -> libocci.so.11.1：ln -s libocci.so.11.1 libocci.so

