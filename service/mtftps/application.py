#!/usr/bin/env python
# -*- coding: utf-8 -*-


import websettings
import logging
import logging.handlers
# from twisted.internet.defer import inlineCallbacks, returnValue
import sys


def _init_log():

    if websettings.OPEN_DEBUG:
        level = logging.DEBUG
    else:
        level = logging.INFO
    handler = logging.handlers.SocketHandler(
        websettings.LOG_SERVER_HOST, websettings.LOG_SERVER_PORT)
    logger = logging.getLogger()
    logger.addHandler(handler)
    logger.setLevel(level)

    # 打印到终端
    if websettings.DEV_MODE:
        ch = logging.StreamHandler(sys.stdout)
        ch.setLevel(level)
        logger.addHandler(ch)


class Application(object):
    instance = None

    def __init__(self):
        Application.instance = self
        self.app_logger = None

    def get_logger(self, name):
        return logging.getLogger(name)

    def init(self):
        _init_log()

        self.app_logger = self.get_logger('app')
        return True

#if __name__ == '__main__':
#    app = Application()
#    reactor.callWhenRunning(app.init)
#    reactor.run()
