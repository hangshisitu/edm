#!/usr/bin/env python
# -*- coding: utf8 -*-

import re
import xml.etree.ElementTree as ET


class XMLError(Exception):
    def __init__(self, desc):
        Exception.__init__(self, "xml error: " + desc)
        self.desc = desc


def _node_text_to_gbk(node):
    text = node.text
    if type(text) == str and len(text) > 0:
        node.text = unicode(text, 'utf-8').encode("gbk")
    elif text is None:
        node.text = ''
    for c in node.getchildren():
        _node_text_to_gbk(c)


def parse(xml_data, to_gbk=True):
    """
    通用 XML 解释器
    Python 的 xml 库不支持 非 utf8 编码的报文，如果报文是 GBK的，需要先转成 UTF8，再解释
    """
    reg = re.compile(r'\s*<\?xml.+?encoding\s*=\s*["\']?([-_a-zA-Z0-9]+)["\']?',
                     re.IGNORECASE | re.MULTILINE)
    mat = reg.match(xml_data)
    is_gbk = True
    if mat is not None:
        encoding = mat.group(1)
        encoding = encoding.upper()
        is_gbk = encoding == "GBK" or encoding == "GB2312" or encoding == 'GB18030'
        is_utf8 = encoding == "UTF8" or encoding == "UTF-8"
        if not is_utf8:
            xml_data = xml_data[0:mat.regs[1][0]] + "utf-8" + xml_data[mat.regs[1][1]:]
    if is_gbk:
        if type(xml_data) == unicode:
            xml_data = xml_data.encode('utf-8')
        else:
            xml_data = unicode(xml_data, 'gbk').encode('utf-8')
    # 注意：
    # 辽宁的XML，最后面全是 \x00，这样的XML是解释不了的，要处理一下
    if xml_data[-1] == '\x00':
        xml_data = xml_data.replace('\x00', '')
    root = ET.fromstring(xml_data)
    if to_gbk:
        _node_text_to_gbk(root)
    return root


def find_tag_or_raise_exception(base_tag, target_tag_name):
    tgr_tag = base_tag.find(target_tag_name)
    if tgr_tag is None:
        raise XMLError("require xml tag: " + target_tag_name)
    return tgr_tag


def find_tag_text_or_raise_exception(base_tag, target_tag_name):
    tag = find_tag_or_raise_exception(base_tag, target_tag_name)
    return tag.text


def find_tag_text_strip_or_raise_exception(base_tag, target_tag_name):
    tag = find_tag_or_raise_exception(base_tag, target_tag_name)

    try:
        re_tag =  tag.text.strip()
    except Exception, ex:
        re_tag = ''

    return re_tag