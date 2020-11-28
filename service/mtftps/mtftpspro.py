#!/usr/bin/env python
# -*- coding: utf-8 -*-

from handlers.exceptions import RequestErrorException
from twisted.internet.defer import inlineCallbacks, returnValue
from handlers import httprequest
import websettings
import hashlib
import time
import uuid
import re


# 读ftp路径文件失败
RC_READ_FILE_ERROR = 905
# 解析数据文件失败--文件格式失败
RC_PARSER_DATA_ERROR = 906
# mt用户登陆信息未配置
RC_USER_ERROR = 907
# 账单数据投递失败
RC_MT_REQ_ERROR = 908
# 数据转换编码失败
RC_FILE_TRANS_CHARSET_ERROR = 909

# mt系统账单数据投递模板
XML_TEMP = """
<?xml version="1.0" encoding="utf-8"?>
<requestdata>
<head>
    <reqcode>%s</reqcode>
    <cmdcode>1</cmdcode>
    <user>%s</user>
    <tiaourl>http://tiaourl.com</tiaourl>
    <163ad>true</163ad>
    <passwrd>%s</passwrd>
    <timestamp>%s</timestamp>
    <skey>%s</skey>
    <subject>%s</subject> 
    <task_des>%s</task_des>
    <sender>%s</sender>
    <replier>%s</replier>
    <nickname>%s</nickname>
    <isradom>false</isradom>
    <channel>cy_trigger</channel>
    <trigger>1</trigger>
    <service_id>334</service_id>
    <courl>http://www.people.com.cn</courl>
    <sms>13</sms>
</head>
<body>
    <modula>%s</modula>
    <maillist>%s</maillist>
    <modula_qvga>%s</modula_qvga>
    <appendix>%s</appendix>
    <appendixname>%s</appendixname>
</body>
</requestdata>
"""

# sx账单投递xml解析字段
xml_item_list = ['tos', 'toname', 'brand', 'from', 'title', 'appendix', 'appendixname', 'info', 'infowap']

# mtarget请求应答包解析字段
mt_par_item = ['reqcode', 'retcode', 'taskid', 'corpid']

# 需要校验备选标签列表
fd_item_list = {'infowap': 'modula_qvga', 'appendix': 'appendix', 'appendixname': 'appendixname'}


def search_item(i_str, tmp_str):

    search_fun = re.compile(r'\s*<%s>(.+)</%s>\s*' % (i_str, i_str), re.MULTILINE | re.IGNORECASE | re.DOTALL)
    end_pos = tmp_str.find('</' + i_str + '>') + len(i_str) + 3
    start_pos = tmp_str.find('<' + i_str + '>') - 1
    ret_str = search_fun.findall(tmp_str, start_pos, end_pos)
    if len(ret_str) > 0:
        return ret_str[0]
    else:
        return None


def make_send_data(xml_tmp, item_list):
    timestamp = time.strftime('%Y%m%d%H%M%S')
    req_code = uuid.uuid4().get_hex()
    req_code = req_code.upper()

    if websettings.MTARGET_USER_NAME and websettings.MTARGET_USER_PASS:
        m = hashlib.md5()
        m.update('%s' % websettings.MTARGET_USER_PASS)
        password = m.hexdigest().upper()
        user = websettings.MTARGET_USER_NAME

        m.update('%s%s%s' % (websettings.MTARGET_USER_NAME, websettings.MTARGET_USER_PASS, timestamp))
        s_key = m.hexdigest().upper()
    else:
        raise RequestErrorException(RC_USER_ERROR, u'用户配置信息不存在')

    send_xml = xml_tmp % (req_code, user, password,
                      timestamp, s_key, item_list['title'],
                      u'山西业务受理单任务', item_list['from'],
                      item_list['from'], item_list['brand'],
                      item_list['info'], item_list['tos'],
                      item_list['infowap'], item_list['appendix'],
                      item_list['appendixname'])

    send_xml = rm_item(send_xml, fd_item_list, item_list)
    return send_xml


def rm_item(xml_data, find_item_dict, item_list):

    for i in find_item_dict:
        find_item = find_item_dict[i]
        if len(item_list[i]) == 0:
            s_str = '<' + find_item + '>'
            e_str = '</' + find_item + '>'
            rp_str = xml_data[xml_data.find(s_str): (xml_data.find(e_str)+len(e_str))]
            xml_data = xml_data.replace(rp_str, '')

    return xml_data


@inlineCallbacks
def req_mta(req_data):
    http_srv_list = websettings.RCV_PHP_URL.split(',')
    count = 0
    for i in http_srv_list:
        try:
            rsp = yield httprequest.do_post(i, req_data)
            count += 1
        except Exception:
            count += 1
            if count >= len(http_srv_list):
                raise RequestErrorException(RC_MT_REQ_ERROR, u'投递接口连接失败')
            else:
                continue

        rsp_xml = rsp.body

        if websettings.OPEN_DEBUG:
            print rsp_xml

        rsp_list = {}
        for i in mt_par_item:
            rsp_list[i] = search_item(i, rsp_xml)

        returnValue(rsp_list)


class MtFtpsPro(object):
    """
    山西投递数据处理
    """

    def __init__(self, filename):
        self.filename = filename

    @inlineCallbacks
    def req_handler(self):

        rsp_list = {}
        if self.filename:
            try:
                fd = open(self.filename, 'r')
                file_data = fd.read()
            except Exception:
                raise RequestErrorException(RC_READ_FILE_ERROR, u'文件不存在')

            for i in xml_item_list:
                item_data = search_item(i, file_data)
                if item_data is not None:
                    if i == 'tos':
                        to_datas = ''
                        item_data = item_data.replace('\t', '')
                        while item_data.find('<to>') >= 0:
                            tmp_str = item_data[item_data.find('<to>')+len('<to>'):item_data.find('</to>')]
                            rm_str = item_data[item_data.find('<to>'):(item_data.find('</to>') + len('</to>'))]
                            item_data = item_data.replace(rm_str, '')
                            if len(to_datas) > 0:
                                to_datas += '\n'
                            to_datas += tmp_str

                        item_data = to_datas.replace(' ', '')

                    try:
                        item_data = unicode(item_data, 'gbk')
                    except UnicodeError:
                        try:
                            item_data = unicode(item_data, 'utf8')
                        except UnicodeError:
                            raise RequestErrorException(RC_FILE_TRANS_CHARSET_ERROR, u'数据文件编码错误')

                    rsp_list[i] = item_data
                else:
                    rsp_list[i] = ''
        else:
            raise RequestErrorException(RC_READ_FILE_ERROR, u'文件不存在')

        if len(rsp_list) > 0:
            send_xml = make_send_data(XML_TEMP, rsp_list)
            send_xml = send_xml.encode('utf8')
            if websettings.OPEN_DEBUG:
                print send_xml
                #fd = open('/home/chull/wf/data/test.xml', 'w')
                #fd.write(send_xml)
                #fd.close()

            rsp_list = yield req_mta(send_xml)
            returnValue(rsp_list)
        else:
            raise RequestErrorException(RC_PARSER_DATA_ERROR, u'账单xml数据解析错误')