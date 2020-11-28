#!/usr/bin/env python
# -*- coding: utf-8 -*-

import time
import uuid
import hashlib
from twisted.internet import reactor
import httprequest
from twisted.internet.defer import inlineCallbacks, DeferredList

# 测试单副本接口

_SC_HOST = "localhost" #'192.168.18.123' #'192.168.21.56'
_SC_PORT = 9220

# =
_BI_URL = "http://%s:%d/mtftps?file=" % (_SC_HOST, _SC_PORT)



@inlineCallbacks
def main():
    @inlineCallbacks
    def job_test():

        param = '201411202000387.xml'
        res = yield httprequest.do_get(_BI_URL+param)

        #response = res.getresponse()

        if res is not None:
            xml_data = res.body
            print xml_data


    #dl = DeferredList(job_test)
    for i in xrange(1):
        print i
        yield job_test()

    reactor.stop()

if __name__ == '__main__':
    reactor.callWhenRunning(main)
    reactor.run()

