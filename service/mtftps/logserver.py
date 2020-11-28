#!/usr/bin/env python
# -*- coding: utf-8 -*-

from twisted.internet.protocol import Factory, Protocol
import struct
import pickle
import logging
from twisted.internet import reactor
import websettings
from logging.handlers import TimedRotatingFileHandler


class LogProtocol(Protocol):
    def __init__(self, peer_addr):
        self.input_buffer = ""
        self.peer_addr = peer_addr

    def dataReceived(self, data):
        if self.input_buffer:
            data = self.input_buffer + data
            self.input_buffer = ""
        while True:
            if len(data) < 4:
                self.input_buffer = data
                return
            slen = struct.unpack('>L', data[0:4])[0]
            if slen <= len(data) - 4:
                chunk = data[4:4 + slen]
                data = data[4 + slen:]
                obj = pickle.loads(chunk)
                record = logging.makeLogRecord(obj)
                name = record.name
                logger = logging.getLogger(name)
                logger.handle(record)
            else:
                self.input_buffer = data
                return


class ProtocolFactory(Factory):
    def buildProtocol(self, addr):
        return LogProtocol(addr)


def init_log(log_path, is_debug=True):
    log_map = {
        'mtftps':'mtftps.log'
    }
    #'billcenter':'billcenter.log',

    level = logging.DEBUG if is_debug else logging.INFO
    for k in log_map:
        filename = log_path + "/" + log_map[k]
        handler = TimedRotatingFileHandler(filename, when="midnight")
        formatter = logging.Formatter('%(asctime)s %(process)d %(levelname)s %(message)s')
        handler.setFormatter(formatter)

        logger = logging.getLogger(k)
        logger.addHandler(handler)
        logger.setLevel(level)


def main():
    init_log(websettings.LOG_PATH)
    pf = ProtocolFactory()
    reactor.listenTCP(websettings.LOG_SERVER_PORT, pf)
    reactor.run()


if __name__ == '__main__':
    main()
