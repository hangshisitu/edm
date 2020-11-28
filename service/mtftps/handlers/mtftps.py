#!/usr/bin/env python
# -*- coding: utf-8 -*-

from twisted.internet.defer import inlineCallbacks, returnValue
from handlers.basehandler import BaseHandler, BaseResource, ParameterError
from handlers.exceptions import RequestErrorException
from application import Application
import websettings
from mtftpspro import MtFtpsPro
# import json


# 返回码
# 成功
RC_SUCCESS = 200
# 数据投递失败
RC_FAILED = 900
# 接口参数错误
RC_PARAM_ERROR = 910
# 服务器内部错误
RC_UNKNOWN_ERROR = 999

# 应答数据xml模板
RC_XML = """
<?xml version="1.0" encoding="utf-8" ?>
<responsedata>
    <reqcode>%s</reqcode>
    <retcode>%d</retcode>
    <taskid>%s</taskid>
    <retmsg>%s</retmsg>
</responsedata>
"""


def is_mobile_number(user_number):
    """
    :type user_number: str
    """
    if len(user_number) != 11:
        return False
    return user_number.isdigit()


def get_succ_rsp(req_code, ret_code, task_id):
    ret_xml = RC_XML % (req_code, int(ret_code), task_id, u'投递成功')
    return ret_xml

def get_error_rsp(req_code, ret_code, ret_desc):
    ret_xml = RC_XML % (req_code, int(ret_code), -1, ret_desc)
    return ret_xml


class SXEdmApiHandler(BaseHandler):

    def __init__(self, req):
        BaseHandler.__init__(self, req)
        self.filename = websettings.FTP_FILE_PATH
        self.app = Application.instance
        self.logger = None
        self.mobile = None

    def sync_process(self):
        self.logger = self.app.get_logger("mtftps")

    def get_request_log_info(self, req_code='', ret_code='', task_id='', corp_id=''):
        m = ["filename={0}".format(self.filename),
             "reqcd={0}".format(req_code),
             "retcd={0}".format(ret_code),
             "taskid={0}".format(task_id)]
        return m

    @inlineCallbacks
    def async_process(self):

        try:
            req_filename = self.get_query_parameter("file")
        except ParameterError, ex:
            err_info = self.get_request_log_info()
            err_info.append("result=" + ex.em)
            self.logger.error("|".join(err_info))
            res = get_error_rsp('', RC_PARAM_ERROR, ex.em)
            res = res.encode('utf8')
            self.req.write(res)
            return

        if req_filename is not None:
            self.filename  = self.filename + req_filename

        try:
            rsp_list = yield self._async_process()
        except RequestErrorException, ex:
            err_info = self.get_request_log_info()
            err_info.append("result=" + ex.error_desc)
            self.logger.error("|".join(err_info))
            res = get_error_rsp('', ex.error_code, ex.error_desc)
            res = res.encode('utf8')
            self.req.write(res)
            return

        if rsp_list['retcode'] != '200':
            err_info = self.get_request_log_info(rsp_list['reqcode'],
                                             rsp_list['retcode'],
                                             rsp_list['taskid'])
            err_info.append("result=" + u'账单投递失败')
            self.logger.error("|".join(err_info))
            res = get_error_rsp(rsp_list['reqcode'], RC_FAILED, u'账单投递失败')
            res = res.encode('utf8')
            self.req.write(res)
            return

        rsp_info = self.get_request_log_info(rsp_list['reqcode'],
                                             rsp_list['retcode'],
                                             rsp_list['taskid'])
        rsp_info.append("result=succ")
        self.logger.info("|".join(rsp_info))
        rsp_xml = get_succ_rsp(rsp_list['reqcode'],
                               rsp_list['retcode'],
                               rsp_list['taskid'])
        rsp_xml = rsp_xml.encode('utf8')
        self.req.write(rsp_xml)

    @inlineCallbacks
    def _async_process(self):

        try:
            req_pro = MtFtpsPro(self.filename)
            rsp_list = yield req_pro.req_handler()
        except RequestErrorException, ex:
            raise RequestErrorException(ex.error_code, ex.error_desc)

        returnValue(rsp_list)


class SXEdmApiResource(BaseResource):
    handler_type = SXEdmApiHandler
    log_request_error = True
    log_access = True
    support_get_method = True
    logger = "sxedmapi"
