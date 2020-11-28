#!/usr/bin/env python
# -*- coding: utf8 -*-

from twisted.web import server
from twisted.web.resource import Resource
from twisted.internet import reactor
from application import Application
# from twisted.internet.defer import inlineCallbacks
import argparse
from handlers import mtftps


def post_init():
    app = Application()
    init_result = app.init()
    if not init_result:
        reactor.stop()
        return
    app.app_logger.info("main loop ...")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", type=int, help="bind port", required=True)
    args = parser.parse_args()

    root = Resource()
    root.putChild("mtftps", mtftps.SXEdmApiResource())

    reactor.listenTCP(args.p, server.Site(root))
    reactor.callWhenRunning(post_init)

    reactor.run()


if __name__ == '__main__':
    main()
