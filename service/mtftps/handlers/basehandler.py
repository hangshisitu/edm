#!/usr/bin/env python
# -*- coding: utf-8 -*-

from twisted.internet.defer import inlineCallbacks
import json
from application import Application
from twisted.web.resource import Resource
from twisted.python import log
from twisted.web.server import NOT_DONE_YET
import logging

RC_SUCCESS = (0, u'操作成功')
RC_PARAMETER_ERROR = (1, u'参数错误')
RC_INVALID_METHOD = (10, u"非法方法")
RC_UNKNOWN_ERROR = (999, u'未知错误')


class RequestError(Exception):
    def __init__(self, ec, em):
        self.ec = ec
        self.em = em
        Exception.__init__(self, "request error: %d: %s" % (ec, em))

    def pack_response_str(self):
        return json.dumps({'error_code': self.ec, 'error_msg': self.em})
            #return json.dumps([{"response":[{'error_code':self.ec, 'error_msg':self.em}]}])

class BillCentRqError(RequestError):
    def __init__(self, ec, em):
        self.ec = ec
        self.em = em
        RequestError.__init__(self, int(ec), em)

    def pack_response_str(self):
        return json.dumps({"response":[{'error_code':self.ec, 'error_msg':self.em}]})

class UnknownError(RequestError):
    def __init__(self):
        RequestError.__init__(self, RC_UNKNOWN_ERROR[0], RC_UNKNOWN_ERROR[1])


class InvalidMethod(RequestError):
    def __init__(self):
        RequestError.__init__(self, RC_INVALID_METHOD[0], RC_INVALID_METHOD[1])


unknown_error = UnknownError()
unknown_error_str = unknown_error.pack_response_str()
success_response_str = json.dumps({'ec': RC_SUCCESS[0], 'em': RC_SUCCESS[1]})
invalid_method = InvalidMethod()
invalid_method_str = invalid_method.pack_response_str()


class ParameterError(RequestError):
    def __init__(self, em=None):
        RequestError.__init__(self, RC_PARAMETER_ERROR[0],
                              RC_PARAMETER_ERROR[1] if em is None else em)


class ValidateError(RequestError):
    def __init__(self, desc):
        RequestError.__init__(self, RC_PARAMETER_ERROR[0], desc)


class ValidateInteger(object):
    def __init__(self, min_value=None, max_value=None, in_list=None):
        self.min_value = min_value
        self.max_value = max_value
        self.in_list = in_list

    def validate(self, input_str):
        try:
            input_int = int(input_str)
        except ValueError:
            raise ValidateError(u"非法整数")
        if self.min_value is not None:
            if input_int < self.min_value:
                raise ValidateError(u"小于最小值 %d" % self.min_value)
        if self.max_value is not None:
            if input_int > self.max_value:
                raise ValidateError(u"大于最大值 %d" % self.max_value)
        if self.in_list is not None:
            if input_int not in self.in_list:
                raise ValidateError(u"不合法整数值")
        return input_int


class ValidateString(object):
    def __init__(self, min_len=None, max_len=None):
        self.min_len = min_len
        self.max_len = max_len

    def validate(self, input_str):
        str_len = len(input_str)
        if self.min_len is not None:
            if str_len < self.min_len:
                raise ValidateError(u"小于最小长度 %d" % self.min_len)
        if self.max_len is not None:
            if str_len > self.max_len:
                raise ValidateError(u"超过最大长度 %d" % self.max_len)
        return input_str


class BaseHandler(object):
    def __init__(self, req):
        self.app = Application.instance
        """@type: application.Application"""
        self.req = req
        """@type: twisted.web.http.Request"""

        self.data_map = None

        self.log_fields = None

    def add_log_field(self, k, v):
        if self.log_fields is None:
            self.log_fields = {k: v}
        else:
            self.log_fields[k] = v

    def get_client_ip(self):
        req = self.req
        real_ip = req.getHeader('x-real-ip')
        if real_ip is None:
            return req.getClientIP()
        if type(real_ip) not in (str, unicode):
            return req.getClientIP()
        return real_ip

    def response_success(self, **kwargs):
        self.req.write(self.pack_success_response_json(**kwargs))

    def response_error(self, error, **kwargs):
        self.req.write(self.pack_error_response_json(error, **kwargs))

    @staticmethod
    def pack_response_json(ec, em, **kwargs):
        m = {'ec': ec, 'em': em}
        m.update(kwargs)
        return json.dumps(m)

    @staticmethod
    def pack_success_response_json(**kwargs):
        m = {'ec': RC_SUCCESS[0], 'em': RC_SUCCESS[1]}
        m.update(kwargs)
        return json.dumps(m)

    @staticmethod
    def pack_error_response_json(error, **kwargs):
        m = {'ec': error.ec, 'em': error.em}
        m.update(kwargs)
        return json.dumps(m)

    def get_query_parameter(self, key, value_type=None, _raise=True):
        req = self.req
        value = req.args.get(key)
        if value is None or len(value) == 0:
            if _raise:
                raise ParameterError(u"缺少参数" + key)
            else:
                return
        value = value[0]
        if value_type is None:
            return value
        try:
            return value_type(value)
        except ValueError:
            raise ParameterError(u"参数值类型错误" + key)

    def has_query_parameter(self, key):
        req = self.req
        value = req.args.get(key)
        if value is None or len(value) == 0:
            return False
        return True

    def get_query_validate(self, key, validator, _raise=True):
        req = self.req
        value = req.args.get(key)
        if value is None or len(value) == 0:
            if _raise:
                raise ParameterError(u"缺少参数" + key)
            else:
                return
        value = value[0]
        try:
            return validator.validate(value)
        except ValidateError, error:
            error.em = u'验证%s参数不通过：%s' % (key, error.em)
            raise error

    def decode_post_json(self, validate_list=None):
        data = self.req.content.getvalue()
        try:
            self.data_map = json.loads(data)
        except ValueError:
            raise ParameterError(u'解码json失败')
        if validate_list is not None:
            for item in validate_list:
                key = item['name']
                validator = item.get('validator')
                default = item.get('default')
                value = self.data_map.get(key)
                if value is None:
                    if default is None:
                        raise ParameterError(u'缺少参数：' + key)
                    if callable(default):
                        value = default()
                    else:
                        value = default
                    self.data_map[key] = value
                    use_default = True
                else:
                    use_default = False

                if not use_default and validator is not None:
                    try:
                        self.data_map[key] = validator.validate(value)
                    except ValidateError, ex:
                        raise ParameterError(u'非法参数值 %s: %s' % (key, ex.em))


class BaseResource(Resource):
    isLeaf = True

    handler_type = None
    support_get_method = False
    support_post_method = False
    log_request_error = False
    log_access = False
    logger = None

    _logger_instance = None

    @classmethod
    def _get_logger(cls):
        if cls._logger_instance is None:
            if cls.logger is None:
                cls._logger_instance = logging.getLogger()
            else:
                cls._logger_instance = logging.getLogger(cls.logger)
        return cls._logger_instance

    @classmethod
    def _get_log_fields_str(cls, handler):
        log_fields = handler.log_fields
        if log_fields is None:
            return ''
        return '|' + '|'.join("%s=%s" % (key, val) for (key, val) in log_fields.iteritems())

    @classmethod
    def _log_request_error(cls, handler, error):
        """
        @type handler: BaseHandler
        @param handler:
        @param error:
        @return:
        """
        req = handler.req
        logger = cls._get_logger()
        ip = handler.get_client_ip()
        logger.error("request error: ip=%s|ec=%d|em=%s%s|args=%s|path=%s|method=%s|headers=%s" %
                     (ip, error.ec, error.em, cls._get_log_fields_str(handler), req.args,
                      req.path, req.method, req.getAllHeaders()))

    @classmethod
    def _log_access(cls, handler):
        req = handler.req
        logger = cls._get_logger()
        ip = handler.get_client_ip()
        logger.info("access: ip=%s%s|args=%s|path=%s|method=%s|headers=%s" %
                    (ip, cls._get_log_fields_str(handler), req.args, req.path, req.method, req.getAllHeaders()))

    @classmethod
    def _log_unhandled_exception(cls, handler, exception):
        req = handler.req
        logger = cls._get_logger()
        ip = handler.get_client_ip()
        logger.error("exception: ip=%s%s|args=%s|path=%s|method=%s|headers=%s|exception=%s" %
                     (ip, cls._get_log_fields_str(handler), req.args, req.path, req.method,
                      req.getAllHeaders(), str(exception)))

    @inlineCallbacks
    def base_async_process(self, handler):
        try:
            yield handler.async_process()
        except RequestError, error:
            if self.log_request_error:
                self._log_request_error(handler, error)
            handler.req.write(error.pack_response_str())
        except Exception, ex:
            handler.req.write(unknown_error_str)
            log.err()
            self._log_unhandled_exception(handler, ex)
        else:
            if self.log_access:
                self._log_access(handler)
        handler.req.finish()

    def base_sync_process(self, req):
        handler = self.handler_type(req)
        try:
            sync_result = None
            if hasattr(handler, 'sync_process'):
                sync_result = handler.sync_process()
            if sync_result is None and hasattr(handler, 'async_process'):
                self.base_async_process(handler)
                return NOT_DONE_YET
            if self.log_access:
                self._log_access(handler)
            return success_response_str if sync_result is None else sync_result
        except RequestError, error:
            if self.log_request_error:
                self._log_request_error(handler, error)
            return error.pack_response_str()
        except Exception, ex:
            log.err()
            self._log_unhandled_exception(handler, ex)
            return unknown_error_str

    def render_GET(self, req):
        if not self.support_get_method:
            return invalid_method_str
        return self.base_sync_process(req)

    def render_POST(self, req):
        if not self.support_post_method:
            return invalid_method_str
        return self.base_sync_process(req)
