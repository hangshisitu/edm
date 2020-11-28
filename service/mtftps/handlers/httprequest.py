#!/usr/bin/env python
# -*- coding: utf-8 -*-

from twisted.internet.protocol import Protocol
from twisted.internet import reactor
from twisted.web.client import Agent, ResponseDone
from zope.interface import implements
from twisted.internet.defer import succeed, Deferred, inlineCallbacks
from twisted.web.iweb import IBodyProducer
from twisted.web.http_headers import Headers
from pprint import pformat

class StringProducer(object):
    implements(IBodyProducer)

    def __init__(self, body):
        self.body = body
        self.length = len(body)

    def startProducing(self, consumer):
        try:
            consumer.write(self.body)
        except Exception, ex:
            print 'ddddfffssafdgaga'

        return succeed(None)

    def pauseProducing(self):
        pass

    def stopProducing(self):
        pass

class HttpResponse(object):
    def __init__(self):
        self.response = None
        self.body = None
        self.body_error = None

class HttpBodyReader(Protocol):
    def __init__(self, finished, hr):
        self.resp_text = ""
        self.finished = finished
        self.http_response = hr

    def dataReceived(self, d):
        self.resp_text += d

    def connectionLost(self, reason):
        if reason.type == ResponseDone:
            self.http_response.body = self.resp_text
        else:
            self.http_response.body_error = reason
        self.finished.callback(self.http_response)

default_agent = Agent(reactor, connectTimeout=60)

def request_cb(response):
    hr = HttpResponse()
    hr.response = response
    finished = Deferred()
    response.deliverBody(HttpBodyReader(finished, hr))
    return finished

def do_get(url, headers=None):
    if headers is None:
        h = Headers()
    else:
        h = Headers(headers)
    d = default_agent.request(
        'GET',
        url,
        h,
        None)
    d.addCallback(request_cb)
    return d

def do_post(url, post_data='', headers=None):
    if headers is None:
        h = Headers()
    else:
        h = Headers(headers)
    d = default_agent.request(
        'POST',
        url,
        h,
        StringProducer(post_data))
    d.addCallback(request_cb)
    return d

@inlineCallbacks
def do_test():
    try:
        resp = yield do_post('http://www.baidu.com')
    except Exception, ex:
        print "**", ex
        reactor.stop()
        return
    response = resp.response
    print 'Response version:', response.version
    print 'Response code:', response.code
    print 'Response phrase:', response.phrase
    print 'Response headers:'
    print pformat(list(response.headers.getAllRawHeaders()))
    print resp.body
    reactor.stop()

if __name__ == '__main__':
    reactor.callWhenRunning(do_test)
    reactor.run()
