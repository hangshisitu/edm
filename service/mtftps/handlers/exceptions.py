#!/usr/bin/env python
# -*- coding: utf-8 -*-

class RequestErrorException(Exception):
    def __init__(self, error_code=None, error_desc=None, ex_catch=None):
        Exception.__init__(self, "request error, error_code=%s, error_desc=%s" %
                           (str(error_code), error_desc))
        self.error_code = error_code
        self.error_desc = error_desc
        self.ex_catch = ex_catch
