/*
Navicat MySQL Data Transfer

Source Server         : 170
Source Server Version : 50713
Source Host           : 192.168.34.170:3306
Source Database       : edm

Target Server Type    : MYSQL
Target Server Version : 50713
File Encoding         : 65001

Date: 2017-01-13 17:33:31
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for campaign
-- ----------------------------
DROP TABLE IF EXISTS `campaign`;
CREATE TABLE `campaign` (
  `campaign_id` int(11) NOT NULL AUTO_INCREMENT,
  `corp_id` int(11) NOT NULL,
  `user_id` varchar(60) NOT NULL,
  `campaign_name` varchar(20) NOT NULL,
  `campaign_desc` varchar(60) DEFAULT NULL,
  `create_time` datetime NOT NULL,
  `modify_time` datetime NOT NULL,
  PRIMARY KEY (`campaign_id`),
  UNIQUE KEY `idx1` (`user_id`,`campaign_name`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of campaign
-- ----------------------------

-- ----------------------------
-- Table structure for category
-- ----------------------------
DROP TABLE IF EXISTS `category`;
CREATE TABLE `category` (
  `category_id` int(11) NOT NULL AUTO_INCREMENT,
  `corp_id` int(11) NOT NULL,
  `user_id` varchar(60) DEFAULT NULL,
  `category_name` varchar(20) NOT NULL,
  `category_desc` varchar(60) DEFAULT NULL,
  `type` tinyint(1) NOT NULL,
  `create_time` datetime NOT NULL,
  `modify_time` datetime NOT NULL,
  PRIMARY KEY (`category_id`),
  UNIQUE KEY `idx1` (`user_id`,`category_name`,`type`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=123 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of category
-- ----------------------------
INSERT INTO `category` VALUES ('1', '1', 'admin', '促销推广', '', '1', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('2', '1', 'admin', '电子期刊', '', '1', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('3', '1', 'admin', '订单', '', '1', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('4', '1', 'admin', '欢迎信', '', '1', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('5', '1', 'admin', '节日祝福', '', '1', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('6', '1', 'admin', '客户关怀', '', '1', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('7', '1', 'admin', '邀请和通知', '', '1', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('41', '0', 'edm', '红名单', '系统默认红名单类别', '0', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('42', '0', 'edm', '黑名单', '系统默认黑名单类别', '0', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('43', '0', 'edm', '拒收', '系统默认拒收类别', '0', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('44', '0', 'edm', '举报', '系统默认举报类别', '0', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('51', '1', 'admin', '表单填写', '', '2', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('52', '1', 'admin', '表单提交', '', '2', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('53', '1', 'admin', '邮件提示', '', '2', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('101', '1', 'admin', '投递', '系统默认投递类别', '0', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('102', '1', 'admin', '测试', '系统默认测试类别', '0', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('103', '1', 'admin', '退订', '系统默认退订类别', '0', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('104', '1', 'admin', '表单收集', '系统默认表单收集类别', '0', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `category` VALUES ('109', '1', 'admin', '触发计划', '触发计划的子模板，不可单独投递', '1', '2014-09-04 15:48:30', '2014-09-04 15:48:30');
INSERT INTO `category` VALUES ('111', '1', 'admin', '临时文件', '系统默认临时文件类别', '0', '2014-09-04 15:48:30', '2014-09-04 15:48:30');
INSERT INTO `category` VALUES ('113', '1', 'admin', '账单', '系统默认模板类别', '1', '2016-11-22 15:27:38', '2016-11-22 15:27:38');
INSERT INTO `category` VALUES ('116', '1', 'admin', '默认', '类别为系统的缺省模板类别', '1', '2016-11-22 15:27:38', '2016-11-22 15:27:38');
INSERT INTO `category` VALUES ('119', '1', 'admin', '自定义推荐', '系统默认模板类别，用于存放系统的推荐模板', '1', '2016-11-22 15:27:38', '2016-11-22 15:27:38');
INSERT INTO `category` VALUES ('122', '1', 'admin', '临时', null, '0', '2016-11-23 10:10:19', '2016-11-23 10:10:19');

-- ----------------------------
-- Table structure for class_of_service
-- ----------------------------
DROP TABLE IF EXISTS `class_of_service`;
CREATE TABLE `class_of_service` (
  `cos_id` int(11) NOT NULL AUTO_INCREMENT,
  `cos_name` varchar(20) NOT NULL,
  `type` tinyint(1) NOT NULL DEFAULT '0',
  `user_count` int(11) NOT NULL DEFAULT '0',
  `day_send` int(11) NOT NULL DEFAULT '0',
  `week_send` int(11) NOT NULL DEFAULT '0',
  `month_send` int(11) NOT NULL DEFAULT '0',
  `total_send` int(11) NOT NULL DEFAULT '0',
  `start_time` datetime DEFAULT NULL,
  `end_time` datetime DEFAULT NULL,
  `is_remind` varchar(20) DEFAULT NULL,
  `create_time` datetime NOT NULL,
  `status` varchar(20) NOT NULL,
  PRIMARY KEY (`cos_id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of class_of_service
-- ----------------------------
INSERT INTO `class_of_service` VALUES ('1', '套餐A-每天发送10万', '1', '3', '10000', '50000', '100000', '100000', null, null, null, '2016-12-15 00:00:00', '1');
INSERT INTO `class_of_service` VALUES ('2', '套餐B-每天发送20万', '1', '5', '20000', '100000', '200000', '200000', null, null, null, '2016-12-15 00:00:00', '1');
INSERT INTO `class_of_service` VALUES ('3', '套餐C-每天发送50万', '1', '5', '50000', '250000', '500000', '500000', null, null, null, '2016-12-15 00:00:00', '1');
INSERT INTO `class_of_service` VALUES ('4', '套餐D-每天发送100万', '1', '5', '100000', '500000', '1000000', '1000000', null, null, null, '2016-12-15 00:00:00', '1');
INSERT INTO `class_of_service` VALUES ('5', '套餐E-每天发送200万', '1', '10', '200000', '1000000', '2000000', '2000000', null, null, null, '2016-12-15 00:00:00', '1');
INSERT INTO `class_of_service` VALUES ('6', '套餐F-每天发送500万', '1', '10', '500000', '2500000', '5000000', '5000000', null, null, null, '2016-12-15 00:00:00', '1');
INSERT INTO `class_of_service` VALUES ('7', '套餐G-每天发送1000万', '1', '10', '1000000', '5000000', '10000000', '10000000', null, null, null, '2016-12-15 00:00:00', '1');
INSERT INTO `class_of_service` VALUES ('8', '套餐Z-无限制', '1', '10', '-1', '-1', '-1', '-1', null, null, null, '2016-12-15 00:00:00', '1');

-- ----------------------------
-- Table structure for copy_recipient_trigger
-- ----------------------------
DROP TABLE IF EXISTS `copy_recipient_trigger`;
CREATE TABLE `copy_recipient_trigger` (
  `user_id` varchar(255) DEFAULT NULL,
  `corp_id` int(11) NOT NULL,
  `db_id` int(11) NOT NULL,
  `coll` int(11) NOT NULL,
  `action` tinyint(1) NOT NULL,
  `modify_time` datetime NOT NULL,
  PRIMARY KEY (`corp_id`,`db_id`,`coll`,`action`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of copy_recipient_trigger
-- ----------------------------

-- ----------------------------
-- Table structure for copy_tag_trigger
-- ----------------------------
DROP TABLE IF EXISTS `copy_tag_trigger`;
CREATE TABLE `copy_tag_trigger` (
  `user_id` varchar(255) DEFAULT NULL,
  `corp_id` int(11) NOT NULL,
  `db_id` int(11) NOT NULL,
  `coll` int(11) NOT NULL,
  `action` tinyint(1) NOT NULL,
  `modify_time` datetime NOT NULL,
  PRIMARY KEY (`corp_id`,`db_id`,`coll`,`action`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of copy_tag_trigger
-- ----------------------------

-- ----------------------------
-- Table structure for corp
-- ----------------------------
DROP TABLE IF EXISTS `corp`;
CREATE TABLE `corp` (
  `corp_id` int(11) NOT NULL AUTO_INCREMENT,
  `cos_id` int(11) NOT NULL,
  `company` varchar(100) NOT NULL,
  `website` varchar(100) DEFAULT NULL,
  `address` varchar(100) DEFAULT NULL,
  `contact` varchar(20) DEFAULT NULL,
  `email` varchar(64) DEFAULT NULL,
  `telephone` varchar(20) DEFAULT NULL,
  `mobile` varchar(20) DEFAULT NULL,
  `fax` varchar(20) DEFAULT NULL,
  `zip` varchar(6) DEFAULT NULL,
  `industry` varchar(20) DEFAULT NULL,
  `email_quantity` varchar(20) DEFAULT NULL,
  `send_quantity` varchar(20) DEFAULT NULL,
  `understand` varchar(20) DEFAULT NULL,
  `promise` varchar(20) DEFAULT NULL,
  `agreement` varchar(20) DEFAULT NULL,
  `way` varchar(20) DEFAULT NULL,
  `audit_path` varchar(60) DEFAULT NULL,
  `formal_id` varchar(20) DEFAULT NULL,
  `tested_id` varchar(20) DEFAULT NULL,
  `status` tinyint(1) NOT NULL,
  `create_time` datetime NOT NULL,
  `expire_time` datetime DEFAULT NULL,
  `sender_validate` tinyint(1) NOT NULL DEFAULT '1',
  `join_api` tinyint(1) DEFAULT '0',
  `parent_id` int(11) NOT NULL DEFAULT '0',
  `corp_path` varchar(500) DEFAULT NULL,
  `manager_id` varchar(60) DEFAULT NULL,
  `modify_time` datetime DEFAULT NULL,
  `open_sms` tinyint(1) DEFAULT '0',
  PRIMARY KEY (`corp_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of corp
-- ----------------------------
INSERT INTO `corp` VALUES ('1', '9', 'admin', 'system admin', null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, 'vip', 'test', '1', '2014-12-16 15:00:00', null, '1', '0', '0', 'admin', null, '2016-11-22 15:27:12', '0');

-- ----------------------------
-- Table structure for customenterprise
-- ----------------------------
DROP TABLE IF EXISTS `customenterprise`;
CREATE TABLE `customenterprise` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `status` int(11) NOT NULL,
  `createTime` datetime NOT NULL,
  `logoUrl` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of customenterprise
-- ----------------------------

-- ----------------------------
-- Table structure for deliver_ability
-- ----------------------------
DROP TABLE IF EXISTS `deliver_ability`;
CREATE TABLE `deliver_ability` (
  `ip` varchar(32) NOT NULL,
  `domain` varchar(128) NOT NULL,
  `ability` int(11) NOT NULL,
  `default_ability` int(11) NOT NULL,
  `dimension` char(1) NOT NULL COMMENT '维度',
  PRIMARY KEY (`ip`,`domain`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of deliver_ability
-- ----------------------------

-- ----------------------------
-- Table structure for deliver_quantity
-- ----------------------------
DROP TABLE IF EXISTS `deliver_quantity`;
CREATE TABLE `deliver_quantity` (
  `from_domain` varchar(128) NOT NULL,
  `rcpt_domain` varchar(128) NOT NULL,
  `quantity` int(11) NOT NULL,
  `seconds` int(11) NOT NULL,
  `status` int(11) NOT NULL DEFAULT '0',
  `create_time` datetime DEFAULT NULL,
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`from_domain`,`rcpt_domain`,`seconds`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of deliver_quantity
-- ----------------------------

-- ----------------------------
-- Table structure for domain_map
-- ----------------------------
DROP TABLE IF EXISTS `domain_map`;
CREATE TABLE `domain_map` (
  `domain` char(60) NOT NULL,
  `map_domain` char(60) NOT NULL,
  UNIQUE KEY `idx1` (`domain`,`map_domain`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of domain_map
-- ----------------------------
INSERT INTO `domain_map` VALUES ('126.com', '163.com');
INSERT INTO `domain_map` VALUES ('139.com', '139.com');
INSERT INTO `domain_map` VALUES ('163.com', '163.com');
INSERT INTO `domain_map` VALUES ('163.net', 'tom.com');
INSERT INTO `domain_map` VALUES ('188.com', '163.com');
INSERT INTO `domain_map` VALUES ('189.cn', '189.cn');
INSERT INTO `domain_map` VALUES ('21cn.com', '21cn.com');
INSERT INTO `domain_map` VALUES ('21cn.net', '21cn.com');
INSERT INTO `domain_map` VALUES ('263.com', '263.com');
INSERT INTO `domain_map` VALUES ('263.net', '263.com');
INSERT INTO `domain_map` VALUES ('263.net.cn', '263.com');
INSERT INTO `domain_map` VALUES ('2911.net', '2911.net');
INSERT INTO `domain_map` VALUES ('51.com', '51.com');
INSERT INTO `domain_map` VALUES ('eyou.com', 'eyou.com');
INSERT INTO `domain_map` VALUES ('foxmail.com', 'qq.com');
INSERT INTO `domain_map` VALUES ('gmail.com', 'gmail.com');
INSERT INTO `domain_map` VALUES ('google.com', 'gmail.com');
INSERT INTO `domain_map` VALUES ('hotmail.com', 'hotmail.com');
INSERT INTO `domain_map` VALUES ('live.cn', 'hotmail.com');
INSERT INTO `domain_map` VALUES ('msn.com', 'hotmail.com');
INSERT INTO `domain_map` VALUES ('qq.com', 'qq.com');
INSERT INTO `domain_map` VALUES ('shangmail.com', 'shangmail.com');
INSERT INTO `domain_map` VALUES ('sina.cn', 'sina.com');
INSERT INTO `domain_map` VALUES ('sina.com', 'sina.com');
INSERT INTO `domain_map` VALUES ('sina.com.cn', 'sina.com');
INSERT INTO `domain_map` VALUES ('sogou.com', 'sogou.com');
INSERT INTO `domain_map` VALUES ('sohu.com', 'sohu.com');
INSERT INTO `domain_map` VALUES ('sohu.net', 'sohu.com');
INSERT INTO `domain_map` VALUES ('tom.com', 'tom.com');
INSERT INTO `domain_map` VALUES ('tom.com.cn', 'tom.com');
INSERT INTO `domain_map` VALUES ('vip.126.com', '163.com');
INSERT INTO `domain_map` VALUES ('vip.163.com', '163.com');
INSERT INTO `domain_map` VALUES ('vip.qq.com', 'qq.com');
INSERT INTO `domain_map` VALUES ('vip.sina.com', 'sina.com');
INSERT INTO `domain_map` VALUES ('vip.sina.com.cn', 'sina.com');
INSERT INTO `domain_map` VALUES ('vip.sohu.com', 'sohu.com');
INSERT INTO `domain_map` VALUES ('vip.tom.com', 'tom.com');
INSERT INTO `domain_map` VALUES ('wo.com.cn', 'wo.com.cn');
INSERT INTO `domain_map` VALUES ('x263.net', '263.com');
INSERT INTO `domain_map` VALUES ('xilu.com', 'yahoo.com.cn');
INSERT INTO `domain_map` VALUES ('yahoo.cn', 'yahoo.com.cn');
INSERT INTO `domain_map` VALUES ('yahoo.com', 'yahoo.com.cn');
INSERT INTO `domain_map` VALUES ('yahoo.com.cn', 'yahoo.com.cn');
INSERT INTO `domain_map` VALUES ('yahoo.hk', 'yahoo.com.cn');
INSERT INTO `domain_map` VALUES ('yeah.net', '163.com');
INSERT INTO `domain_map` VALUES ('ymail.com', 'yahoo.com.cn');

-- ----------------------------
-- Table structure for file
-- ----------------------------
DROP TABLE IF EXISTS `file`;
CREATE TABLE `file` (
  `file_id` int(11) NOT NULL AUTO_INCREMENT,
  `corp_id` int(11) NOT NULL,
  `user_id` varchar(60) NOT NULL,
  `file_name` varchar(20) NOT NULL,
  `file_desc` varchar(60) DEFAULT NULL,
  `file_path` varchar(200) NOT NULL,
  `suff` varchar(8) NOT NULL,
  `file_size` int(11) NOT NULL,
  `email_count` int(11) DEFAULT NULL,
  `status` tinyint(1) NOT NULL,
  `create_time` datetime NOT NULL,
  `modify_time` datetime NOT NULL,
  PRIMARY KEY (`file_id`),
  KEY `idx1` (`user_id`,`modify_time`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of file
-- ----------------------------

-- ----------------------------
-- Table structure for filter
-- ----------------------------
DROP TABLE IF EXISTS `filter`;
CREATE TABLE `filter` (
  `filter_id` int(11) NOT NULL AUTO_INCREMENT,
  `corp_id` int(11) NOT NULL,
  `user_id` varchar(60) NOT NULL,
  `filter_name` varchar(20) NOT NULL,
  `filter_desc` varchar(60) DEFAULT NULL,
  `props` text,
  `create_time` datetime NOT NULL,
  `modify_time` datetime NOT NULL,
  PRIMARY KEY (`filter_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of filter
-- ----------------------------

-- ----------------------------
-- Table structure for form
-- ----------------------------
DROP TABLE IF EXISTS `form`;
CREATE TABLE `form` (
  `form_id` int(11) NOT NULL AUTO_INCREMENT,
  `corp_id` int(11) NOT NULL,
  `user_id` varchar(60) NOT NULL,
  `form_name` varchar(20) NOT NULL,
  `form_desc` varchar(60) NOT NULL,
  `type` tinyint(1) NOT NULL,
  `tag_id` int(11) NOT NULL,
  `email_confirm` tinyint(1) NOT NULL,
  `email_prompt` tinyint(1) NOT NULL,
  `input_path` varchar(200) DEFAULT NULL,
  `submit_path` varchar(200) DEFAULT NULL,
  `confirm_path` varchar(200) DEFAULT NULL,
  `prompt_path` varchar(200) DEFAULT NULL,
  `rand_code` varchar(32) DEFAULT NULL,
  `create_time` datetime NOT NULL,
  `modify_time` datetime NOT NULL,
  `confirm_subject` varchar(100) DEFAULT NULL,
  `prompt_subject` varchar(100) DEFAULT NULL,
  `status` tinyint(1) DEFAULT NULL,
  PRIMARY KEY (`form_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of form
-- ----------------------------

-- ----------------------------
-- Table structure for ip_region
-- ----------------------------
DROP TABLE IF EXISTS `ip_region`;
CREATE TABLE `ip_region` (
  `nip1` bigint(20) NOT NULL,
  `sip1` char(16) NOT NULL,
  `nip2` bigint(20) NOT NULL,
  `sip2` char(16) NOT NULL,
  `region` varchar(60) DEFAULT NULL,
  `isp` varchar(60) DEFAULT NULL,
  PRIMARY KEY (`nip1`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of ip_region
-- ----------------------------

-- ----------------------------
-- Table structure for label
-- ----------------------------
DROP TABLE IF EXISTS `label`;
CREATE TABLE `label` (
  `label_id` int(11) NOT NULL AUTO_INCREMENT,
  `parent_id` int(11) NOT NULL,
  `category_id` int(11) NOT NULL,
  `label_name` varchar(20) NOT NULL,
  `label_desc` varchar(60) DEFAULT NULL,
  PRIMARY KEY (`label_id`),
  UNIQUE KEY `idx1` (`label_name`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=28 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label
-- ----------------------------
INSERT INTO `label` VALUES ('1', '0', '1', '游戏', '');
INSERT INTO `label` VALUES ('2', '0', '1', '房地产', '');
INSERT INTO `label` VALUES ('3', '0', '1', '家电', '');
INSERT INTO `label` VALUES ('4', '0', '1', '饮品', '');
INSERT INTO `label` VALUES ('5', '0', '1', '服饰', '');
INSERT INTO `label` VALUES ('6', '0', '1', '电商', '');
INSERT INTO `label` VALUES ('7', '0', '1', '金融', '');
INSERT INTO `label` VALUES ('8', '0', '1', '教育', '');
INSERT INTO `label` VALUES ('9', '0', '1', '汽车服务', '');
INSERT INTO `label` VALUES ('10', '0', '1', '医疗', '');
INSERT INTO `label` VALUES ('11', '0', '1', '美容纤体', '');
INSERT INTO `label` VALUES ('12', '0', '1', '媒体', '');
INSERT INTO `label` VALUES ('13', '0', '1', '3C', '');
INSERT INTO `label` VALUES ('14', '0', '1', '酒店旅游', '');
INSERT INTO `label` VALUES ('15', '0', '1', '移动业务', '');
INSERT INTO `label` VALUES ('16', '0', '2', '学生', '');
INSERT INTO `label` VALUES ('17', '0', '2', '白领', '');
INSERT INTO `label` VALUES ('18', '0', '2', '公务员', '');
INSERT INTO `label` VALUES ('19', '0', '2', '商旅人士', '');
INSERT INTO `label` VALUES ('20', '0', '2', '居家人士', '');
INSERT INTO `label` VALUES ('21', '0', '2', '农民工', '');
INSERT INTO `label` VALUES ('22', '0', '3', '少年（18岁以下）', '');
INSERT INTO `label` VALUES ('23', '0', '3', '青年（18~44岁）', '');
INSERT INTO `label` VALUES ('24', '0', '3', '中年（45~59岁）', '');
INSERT INTO `label` VALUES ('25', '0', '3', '老年（60岁以上）', '');
INSERT INTO `label` VALUES ('26', '0', '4', '男', '');
INSERT INTO `label` VALUES ('27', '0', '4', '女', '');

-- ----------------------------
-- Table structure for label_category
-- ----------------------------
DROP TABLE IF EXISTS `label_category`;
CREATE TABLE `label_category` (
  `category_id` int(11) NOT NULL AUTO_INCREMENT,
  `category_name` varchar(20) NOT NULL,
  PRIMARY KEY (`category_id`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_category
-- ----------------------------
INSERT INTO `label_category` VALUES ('1', '行业兴趣');
INSERT INTO `label_category` VALUES ('2', '社会身份');
INSERT INTO `label_category` VALUES ('3', '年龄段');
INSERT INTO `label_category` VALUES ('4', '性别');

-- ----------------------------
-- Table structure for label_data
-- ----------------------------
DROP TABLE IF EXISTS `label_data`;
CREATE TABLE `label_data` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_1
-- ----------------------------
DROP TABLE IF EXISTS `label_data_1`;
CREATE TABLE `label_data_1` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_1
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_10
-- ----------------------------
DROP TABLE IF EXISTS `label_data_10`;
CREATE TABLE `label_data_10` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_10
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_11
-- ----------------------------
DROP TABLE IF EXISTS `label_data_11`;
CREATE TABLE `label_data_11` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_11
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_12
-- ----------------------------
DROP TABLE IF EXISTS `label_data_12`;
CREATE TABLE `label_data_12` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_12
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_13
-- ----------------------------
DROP TABLE IF EXISTS `label_data_13`;
CREATE TABLE `label_data_13` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_13
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_14
-- ----------------------------
DROP TABLE IF EXISTS `label_data_14`;
CREATE TABLE `label_data_14` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_14
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_15
-- ----------------------------
DROP TABLE IF EXISTS `label_data_15`;
CREATE TABLE `label_data_15` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_15
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_16
-- ----------------------------
DROP TABLE IF EXISTS `label_data_16`;
CREATE TABLE `label_data_16` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_16
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_17
-- ----------------------------
DROP TABLE IF EXISTS `label_data_17`;
CREATE TABLE `label_data_17` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_17
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_18
-- ----------------------------
DROP TABLE IF EXISTS `label_data_18`;
CREATE TABLE `label_data_18` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_18
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_19
-- ----------------------------
DROP TABLE IF EXISTS `label_data_19`;
CREATE TABLE `label_data_19` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_19
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_2
-- ----------------------------
DROP TABLE IF EXISTS `label_data_2`;
CREATE TABLE `label_data_2` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_2
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_20
-- ----------------------------
DROP TABLE IF EXISTS `label_data_20`;
CREATE TABLE `label_data_20` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_20
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_21
-- ----------------------------
DROP TABLE IF EXISTS `label_data_21`;
CREATE TABLE `label_data_21` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_21
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_22
-- ----------------------------
DROP TABLE IF EXISTS `label_data_22`;
CREATE TABLE `label_data_22` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_22
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_23
-- ----------------------------
DROP TABLE IF EXISTS `label_data_23`;
CREATE TABLE `label_data_23` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_23
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_24
-- ----------------------------
DROP TABLE IF EXISTS `label_data_24`;
CREATE TABLE `label_data_24` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_24
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_25
-- ----------------------------
DROP TABLE IF EXISTS `label_data_25`;
CREATE TABLE `label_data_25` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_25
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_26
-- ----------------------------
DROP TABLE IF EXISTS `label_data_26`;
CREATE TABLE `label_data_26` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_26
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_27
-- ----------------------------
DROP TABLE IF EXISTS `label_data_27`;
CREATE TABLE `label_data_27` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_27
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_3
-- ----------------------------
DROP TABLE IF EXISTS `label_data_3`;
CREATE TABLE `label_data_3` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_3
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_4
-- ----------------------------
DROP TABLE IF EXISTS `label_data_4`;
CREATE TABLE `label_data_4` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_4
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_5
-- ----------------------------
DROP TABLE IF EXISTS `label_data_5`;
CREATE TABLE `label_data_5` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_5
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_6
-- ----------------------------
DROP TABLE IF EXISTS `label_data_6`;
CREATE TABLE `label_data_6` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_6
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_7
-- ----------------------------
DROP TABLE IF EXISTS `label_data_7`;
CREATE TABLE `label_data_7` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_7
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_8
-- ----------------------------
DROP TABLE IF EXISTS `label_data_8`;
CREATE TABLE `label_data_8` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_8
-- ----------------------------

-- ----------------------------
-- Table structure for label_data_9
-- ----------------------------
DROP TABLE IF EXISTS `label_data_9`;
CREATE TABLE `label_data_9` (
  `label_id` int(11) NOT NULL COMMENT '类型ID',
  `mailbox` varchar(256) NOT NULL,
  `open` int(11) DEFAULT NULL COMMENT '打开数',
  `click` int(11) DEFAULT NULL COMMENT '点击数',
  `score` int(11) DEFAULT NULL,
  `create_time` datetime DEFAULT NULL COMMENT '任务时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`label_id`,`mailbox`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_data_9
-- ----------------------------

-- ----------------------------
-- Table structure for label_history
-- ----------------------------
DROP TABLE IF EXISTS `label_history`;
CREATE TABLE `label_history` (
  `corp_id` int(11) NOT NULL,
  `task_id` int(11) NOT NULL,
  `template_id` int(11) NOT NULL DEFAULT '0',
  `label_ids` varchar(255) NOT NULL,
  `status` varchar(20) NOT NULL,
  PRIMARY KEY (`corp_id`,`task_id`,`template_id`),
  KEY `idx1` (`task_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of label_history
-- ----------------------------

-- ----------------------------
-- Table structure for login_history
-- ----------------------------
DROP TABLE IF EXISTS `login_history`;
CREATE TABLE `login_history` (
  `history_id` int(11) NOT NULL AUTO_INCREMENT,
  `corp_id` int(11) NOT NULL,
  `user_id` varchar(60) NOT NULL,
  `login_time` datetime NOT NULL,
  `login_ip` varchar(20) DEFAULT NULL,
  `region` varchar(60) DEFAULT NULL,
  `client` varchar(60) DEFAULT NULL,
  `result` tinyint(1) DEFAULT NULL,
  PRIMARY KEY (`history_id`),
  KEY `idx1` (`user_id`,`login_time`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of login_history
-- ----------------------------
INSERT INTO `login_history` VALUES ('1', '1', 'admin', '2017-01-13 17:25:43', '192.168.31.21', null, 'Chrome 49.0.2623.110', '0');

-- ----------------------------
-- Table structure for menu
-- ----------------------------
DROP TABLE IF EXISTS `menu`;
CREATE TABLE `menu` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '主键',
  `parent_id` int(11) DEFAULT NULL COMMENT '父节点',
  `name` varchar(30) DEFAULT NULL COMMENT '菜单功能名称',
  `url` varchar(200) DEFAULT NULL,
  `modify_time` datetime DEFAULT NULL,
  `data_cate` varchar(30) DEFAULT NULL COMMENT '节点英文目录名',
  `sort` int(4) DEFAULT NULL COMMENT '节点英文目录名',
  `type` int(1) DEFAULT NULL COMMENT '菜单类型：0：必有 1：可配置',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=25 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of menu
-- ----------------------------
INSERT INTO `menu` VALUES ('1', '0', '数据管理', null, '2015-02-05 00:00:00', 'datasource', '1', '0');
INSERT INTO `menu` VALUES ('2', '0', '邮件管理', null, '2015-02-05 00:00:00', 'mailing', '2', '0');
INSERT INTO `menu` VALUES ('3', '0', '数据报告', null, '2015-02-05 00:00:00', 'report', '3', '0');
INSERT INTO `menu` VALUES ('4', '0', '账户管理', null, '2015-02-05 00:00:00', 'account', '4', '0');
INSERT INTO `menu` VALUES ('5', '1', '收件人', 'datasource/tag/page', '2015-02-05 00:00:00', null, '11', '0');
INSERT INTO `menu` VALUES ('6', '1', '属性', 'datasource/prop/page', '2015-02-05 00:00:00', null, '12', '0');
INSERT INTO `menu` VALUES ('7', '1', '过滤器', 'datasource/filter/page', '2015-02-05 00:00:00', null, '13', '0');
INSERT INTO `menu` VALUES ('8', '1', '筛选', 'datasource/selection/page', '2015-02-05 00:00:00', null, '14', '0');
INSERT INTO `menu` VALUES ('9', '1', '表单', 'datasource/form/page', '2015-02-05 00:00:00', null, '15', '0');
INSERT INTO `menu` VALUES ('10', '2', '模板', 'mailing/template/page', '2015-02-05 00:00:00', null, '21', '0');
INSERT INTO `menu` VALUES ('11', '2', '任务', 'mailing/task/page', '2015-02-05 00:00:00', null, '22', '0');
INSERT INTO `menu` VALUES ('12', '2', '活动', 'mailing/campaign/page', '2015-02-05 00:00:00', null, '23', '0');
INSERT INTO `menu` VALUES ('13', '2', '审核', 'mailing/audit/page', '2015-02-05 00:00:00', null, '24', '0');
INSERT INTO `menu` VALUES ('14', '3', '总览', 'report/local/sum', '2015-02-05 00:00:00', null, '31', '0');
INSERT INTO `menu` VALUES ('15', '3', '任务', 'report/task/page', '2015-02-05 00:00:00', null, '32', '0');
INSERT INTO `menu` VALUES ('16', '3', 'API', 'report/task/page?m=join', '2015-02-05 00:00:00', null, '33', '0');
INSERT INTO `menu` VALUES ('17', '3', '活动', 'report/campaign/page', '2015-02-05 00:00:00', null, '34', '0');
INSERT INTO `menu` VALUES ('18', '3', '触发计划', 'report/touch/page', '2015-02-05 00:00:00', null, '35', '0');
INSERT INTO `menu` VALUES ('19', '4', '账户', 'account/profile/view', '2015-02-05 00:00:00', null, '41', '1');
INSERT INTO `menu` VALUES ('20', '4', '资料', 'account/profile/add', '2015-02-05 00:00:00', null, '42', '1');
INSERT INTO `menu` VALUES ('21', '4', '子账号', 'account/user/page', '2015-02-05 00:00:00', null, '43', '0');
INSERT INTO `menu` VALUES ('22', '4', '发件人', 'account/sender/page', '2015-02-05 00:00:00', null, '44', '1');
INSERT INTO `menu` VALUES ('23', '4', '历史', 'account/history/page', '2015-02-05 00:00:00', null, '45', '1');
INSERT INTO `menu` VALUES ('24', '4', '密码', 'account/password/add', '2015-02-05 00:00:00', null, '46', '1');

-- ----------------------------
-- Table structure for plan
-- ----------------------------
DROP TABLE IF EXISTS `plan`;
CREATE TABLE `plan` (
  `plan_id` int(11) NOT NULL AUTO_INCREMENT,
  `corp_id` int(11) NOT NULL,
  `user_id` varchar(60) NOT NULL,
  `cron` varchar(20) NOT NULL,
  `begin_time` date DEFAULT NULL,
  `end_time` date DEFAULT NULL,
  PRIMARY KEY (`plan_id`),
  KEY `idx1` (`begin_time`,`end_time`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of plan
-- ----------------------------

-- ----------------------------
-- Table structure for prop
-- ----------------------------
DROP TABLE IF EXISTS `prop`;
CREATE TABLE `prop` (
  `prop_id` int(11) NOT NULL AUTO_INCREMENT,
  `corp_id` int(11) NOT NULL,
  `user_id` varchar(60) DEFAULT NULL,
  `prop_name` varchar(20) NOT NULL,
  `prop_desc` varchar(60) DEFAULT NULL,
  `type` varchar(20) DEFAULT NULL,
  `length` bigint(20) DEFAULT NULL,
  `required` tinyint(1) NOT NULL,
  `default_value` varchar(20) DEFAULT NULL,
  `create_time` datetime NOT NULL,
  `modify_time` datetime NOT NULL,
  `relation` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`prop_id`),
  UNIQUE KEY `idx1` (`user_id`,`prop_name`) USING BTREE,
  KEY `idx2` (`user_id`,`modify_time`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of prop
-- ----------------------------
INSERT INTO `prop` VALUES ('1', '1', 'admin', 'email', '系统默认邮箱', 'String', null, '0', '', '2013-05-10 10:00:00', '2013-05-10 10:00:00', '0');
INSERT INTO `prop` VALUES ('2', '1', 'admin', 'true_name', '系统默认姓名', 'String', null, '0', '', '2013-05-10 10:00:00', '2013-05-10 10:00:00', '0');
INSERT INTO `prop` VALUES ('3', '1', 'admin', 'nick_name', '系统默认昵称', 'String', null, '0', '', '2013-05-10 10:00:00', '2013-05-10 10:00:00', '0');
INSERT INTO `prop` VALUES ('4', '1', 'admin', 'gender', '系统默认性别', 'String', null, '0', '', '2013-05-10 10:00:00', '2013-05-10 10:00:00', '0');
INSERT INTO `prop` VALUES ('5', '1', 'admin', 'job', '系统默认职业', 'String', null, '0', '', '2013-05-10 10:00:00', '2013-05-10 10:00:00', '0');
INSERT INTO `prop` VALUES ('6', '1', 'admin', 'age', '系统默认年龄', 'Integer', null, '0', '', '2013-05-10 10:00:00', '2013-05-10 10:00:00', '0');
INSERT INTO `prop` VALUES ('7', '1', 'admin', 'birthday', '系统默认生日', 'Date (yyyy-MM-dd)', null, '0', '', '2013-05-10 10:00:00', '2013-05-10 10:00:00', '0');

-- ----------------------------
-- Table structure for recipient
-- ----------------------------
DROP TABLE IF EXISTS `recipient`;
CREATE TABLE `recipient` (
  `corp_id` int(11) NOT NULL,
  `db_id` int(11) NOT NULL,
  PRIMARY KEY (`corp_id`,`db_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of recipient
-- ----------------------------

-- ----------------------------
-- Table structure for region
-- ----------------------------
DROP TABLE IF EXISTS `region`;
CREATE TABLE `region` (
  `region_id` int(11) NOT NULL,
  `country_name` varchar(60) DEFAULT 'CN',
  `province_name` varchar(60) NOT NULL,
  `city_name` varchar(60) NOT NULL,
  `district_name` varchar(60) DEFAULT NULL,
  PRIMARY KEY (`region_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of region
-- ----------------------------
INSERT INTO `region` VALUES ('999', 'CN', '局域网', '局域网', '');
INSERT INTO `region` VALUES ('110000', 'CN', '北京', '', '');
INSERT INTO `region` VALUES ('110100', 'CN', '北京', '市辖区', '');
INSERT INTO `region` VALUES ('110200', 'CN', '北京', '市县', '');
INSERT INTO `region` VALUES ('120000', 'CN', '天津', '', '');
INSERT INTO `region` VALUES ('120100', 'CN', '天津', '市辖区', '');
INSERT INTO `region` VALUES ('120200', 'CN', '天津', '市县', '');
INSERT INTO `region` VALUES ('130000', 'CN', '河北', '', '');
INSERT INTO `region` VALUES ('130100', 'CN', '河北', '石家庄', '');
INSERT INTO `region` VALUES ('130200', 'CN', '河北', '唐山', '');
INSERT INTO `region` VALUES ('130300', 'CN', '河北', '秦皇岛', '');
INSERT INTO `region` VALUES ('130400', 'CN', '河北', '邯郸', '');
INSERT INTO `region` VALUES ('130500', 'CN', '河北', '邢台', '');
INSERT INTO `region` VALUES ('130600', 'CN', '河北', '保定', '');
INSERT INTO `region` VALUES ('130700', 'CN', '河北', '张家口', '');
INSERT INTO `region` VALUES ('130800', 'CN', '河北', '承德', '');
INSERT INTO `region` VALUES ('130900', 'CN', '河北', '沧州', '');
INSERT INTO `region` VALUES ('131000', 'CN', '河北', '廊坊', '');
INSERT INTO `region` VALUES ('131100', 'CN', '河北', '衡水', '');
INSERT INTO `region` VALUES ('140000', 'CN', '山西', '', '');
INSERT INTO `region` VALUES ('140100', 'CN', '山西', '太原', '');
INSERT INTO `region` VALUES ('140200', 'CN', '山西', '大同', '');
INSERT INTO `region` VALUES ('140300', 'CN', '山西', '阳泉', '');
INSERT INTO `region` VALUES ('140400', 'CN', '山西', '长治', '');
INSERT INTO `region` VALUES ('140500', 'CN', '山西', '晋城', '');
INSERT INTO `region` VALUES ('140600', 'CN', '山西', '朔州', '');
INSERT INTO `region` VALUES ('140700', 'CN', '山西', '晋中', '');
INSERT INTO `region` VALUES ('140800', 'CN', '山西', '运城', '');
INSERT INTO `region` VALUES ('140900', 'CN', '山西', '忻州', '');
INSERT INTO `region` VALUES ('141000', 'CN', '山西', '临汾', '');
INSERT INTO `region` VALUES ('141100', 'CN', '山西', '吕梁', '');
INSERT INTO `region` VALUES ('150000', 'CN', '内蒙古', '', '');
INSERT INTO `region` VALUES ('150100', 'CN', '内蒙古', '呼和浩特', '');
INSERT INTO `region` VALUES ('150200', 'CN', '内蒙古', '包头', '');
INSERT INTO `region` VALUES ('150300', 'CN', '内蒙古', '乌海', '');
INSERT INTO `region` VALUES ('150400', 'CN', '内蒙古', '赤峰', '');
INSERT INTO `region` VALUES ('150500', 'CN', '内蒙古', '通辽', '');
INSERT INTO `region` VALUES ('150600', 'CN', '内蒙古', '鄂尔多斯', '');
INSERT INTO `region` VALUES ('150700', 'CN', '内蒙古', '呼伦贝尔', '');
INSERT INTO `region` VALUES ('150800', 'CN', '内蒙古', '巴彦淖尔', '');
INSERT INTO `region` VALUES ('150900', 'CN', '内蒙古', '乌兰察布', '');
INSERT INTO `region` VALUES ('152200', 'CN', '内蒙古', '兴安盟', '');
INSERT INTO `region` VALUES ('152500', 'CN', '内蒙古', '锡林郭勒盟', '');
INSERT INTO `region` VALUES ('152600', 'CN', '内蒙古', '乌兰察布盟', '');
INSERT INTO `region` VALUES ('152900', 'CN', '内蒙古', '阿拉善盟', '');
INSERT INTO `region` VALUES ('210000', 'CN', '辽宁', '', '');
INSERT INTO `region` VALUES ('210100', 'CN', '辽宁', '沈阳', '');
INSERT INTO `region` VALUES ('210200', 'CN', '辽宁', '大连', '');
INSERT INTO `region` VALUES ('210300', 'CN', '辽宁', '鞍山', '');
INSERT INTO `region` VALUES ('210400', 'CN', '辽宁', '抚顺', '');
INSERT INTO `region` VALUES ('210500', 'CN', '辽宁', '本溪', '');
INSERT INTO `region` VALUES ('210600', 'CN', '辽宁', '丹东', '');
INSERT INTO `region` VALUES ('210700', 'CN', '辽宁', '锦州', '');
INSERT INTO `region` VALUES ('210800', 'CN', '辽宁', '营口', '');
INSERT INTO `region` VALUES ('210900', 'CN', '辽宁', '阜新', '');
INSERT INTO `region` VALUES ('211000', 'CN', '辽宁', '辽阳', '');
INSERT INTO `region` VALUES ('211100', 'CN', '辽宁', '盘锦', '');
INSERT INTO `region` VALUES ('211200', 'CN', '辽宁', '铁岭', '');
INSERT INTO `region` VALUES ('211300', 'CN', '辽宁', '朝阳', '');
INSERT INTO `region` VALUES ('211400', 'CN', '辽宁', '葫芦岛', '');
INSERT INTO `region` VALUES ('220000', 'CN', '吉林', '', '');
INSERT INTO `region` VALUES ('220100', 'CN', '吉林', '长春', '');
INSERT INTO `region` VALUES ('220200', 'CN', '吉林', '吉林', '');
INSERT INTO `region` VALUES ('220300', 'CN', '吉林', '四平', '');
INSERT INTO `region` VALUES ('220400', 'CN', '吉林', '辽源', '');
INSERT INTO `region` VALUES ('220500', 'CN', '吉林', '通化', '');
INSERT INTO `region` VALUES ('220600', 'CN', '吉林', '白山', '');
INSERT INTO `region` VALUES ('220700', 'CN', '吉林', '松原', '');
INSERT INTO `region` VALUES ('220800', 'CN', '吉林', '白城', '');
INSERT INTO `region` VALUES ('222400', 'CN', '吉林', '延边朝鲜族自治州', '');
INSERT INTO `region` VALUES ('230000', 'CN', '黑龙江', '', '');
INSERT INTO `region` VALUES ('230100', 'CN', '黑龙江', '哈尔滨', '');
INSERT INTO `region` VALUES ('230200', 'CN', '黑龙江', '齐齐哈尔', '');
INSERT INTO `region` VALUES ('230300', 'CN', '黑龙江', '鸡西', '');
INSERT INTO `region` VALUES ('230400', 'CN', '黑龙江', '鹤岗', '');
INSERT INTO `region` VALUES ('230500', 'CN', '黑龙江', '双鸭山', '');
INSERT INTO `region` VALUES ('230600', 'CN', '黑龙江', '大庆', '');
INSERT INTO `region` VALUES ('230700', 'CN', '黑龙江', '伊春', '');
INSERT INTO `region` VALUES ('230800', 'CN', '黑龙江', '佳木斯', '');
INSERT INTO `region` VALUES ('230900', 'CN', '黑龙江', '七台河', '');
INSERT INTO `region` VALUES ('231000', 'CN', '黑龙江', '牡丹江', '');
INSERT INTO `region` VALUES ('231100', 'CN', '黑龙江', '黑河', '');
INSERT INTO `region` VALUES ('231200', 'CN', '黑龙江', '绥化', '');
INSERT INTO `region` VALUES ('232700', 'CN', '黑龙江', '大兴安岭地区', '');
INSERT INTO `region` VALUES ('310000', 'CN', '上海', '', '');
INSERT INTO `region` VALUES ('310100', 'CN', '上海', '市辖区', '');
INSERT INTO `region` VALUES ('310200', 'CN', '上海', '市县', '');
INSERT INTO `region` VALUES ('320000', 'CN', '江苏', '', '');
INSERT INTO `region` VALUES ('320100', 'CN', '江苏', '南京', '');
INSERT INTO `region` VALUES ('320200', 'CN', '江苏', '无锡', '');
INSERT INTO `region` VALUES ('320300', 'CN', '江苏', '徐州', '');
INSERT INTO `region` VALUES ('320400', 'CN', '江苏', '常州', '');
INSERT INTO `region` VALUES ('320500', 'CN', '江苏', '苏州', '');
INSERT INTO `region` VALUES ('320600', 'CN', '江苏', '南通', '');
INSERT INTO `region` VALUES ('320700', 'CN', '江苏', '连云港', '');
INSERT INTO `region` VALUES ('320800', 'CN', '江苏', '淮安', '');
INSERT INTO `region` VALUES ('320900', 'CN', '江苏', '盐城', '');
INSERT INTO `region` VALUES ('321000', 'CN', '江苏', '扬州', '');
INSERT INTO `region` VALUES ('321100', 'CN', '江苏', '镇江', '');
INSERT INTO `region` VALUES ('321200', 'CN', '江苏', '泰州', '');
INSERT INTO `region` VALUES ('321300', 'CN', '江苏', '宿迁', '');
INSERT INTO `region` VALUES ('330000', 'CN', '浙江', '', '');
INSERT INTO `region` VALUES ('330100', 'CN', '浙江', '杭州', '');
INSERT INTO `region` VALUES ('330200', 'CN', '浙江', '宁波', '');
INSERT INTO `region` VALUES ('330300', 'CN', '浙江', '温州', '');
INSERT INTO `region` VALUES ('330400', 'CN', '浙江', '嘉兴', '');
INSERT INTO `region` VALUES ('330500', 'CN', '浙江', '湖州', '');
INSERT INTO `region` VALUES ('330600', 'CN', '浙江', '绍兴', '');
INSERT INTO `region` VALUES ('330700', 'CN', '浙江', '金华', '');
INSERT INTO `region` VALUES ('330800', 'CN', '浙江', '衢州', '');
INSERT INTO `region` VALUES ('330900', 'CN', '浙江', '舟山', '');
INSERT INTO `region` VALUES ('331000', 'CN', '浙江', '台州', '');
INSERT INTO `region` VALUES ('331100', 'CN', '浙江', '丽水', '');
INSERT INTO `region` VALUES ('340000', 'CN', '安徽', '', '');
INSERT INTO `region` VALUES ('340100', 'CN', '安徽', '合肥', '');
INSERT INTO `region` VALUES ('340200', 'CN', '安徽', '芜湖', '');
INSERT INTO `region` VALUES ('340300', 'CN', '安徽', '蚌埠', '');
INSERT INTO `region` VALUES ('340400', 'CN', '安徽', '淮南', '');
INSERT INTO `region` VALUES ('340500', 'CN', '安徽', '马鞍山', '');
INSERT INTO `region` VALUES ('340600', 'CN', '安徽', '淮北', '');
INSERT INTO `region` VALUES ('340700', 'CN', '安徽', '铜陵', '');
INSERT INTO `region` VALUES ('340800', 'CN', '安徽', '安庆', '');
INSERT INTO `region` VALUES ('341000', 'CN', '安徽', '黄山', '');
INSERT INTO `region` VALUES ('341100', 'CN', '安徽', '滁州', '');
INSERT INTO `region` VALUES ('341200', 'CN', '安徽', '阜阳', '');
INSERT INTO `region` VALUES ('341300', 'CN', '安徽', '宿州', '');
INSERT INTO `region` VALUES ('341400', 'CN', '安徽', '巢湖', '');
INSERT INTO `region` VALUES ('341500', 'CN', '安徽', '六安', '');
INSERT INTO `region` VALUES ('341600', 'CN', '安徽', '亳州', '');
INSERT INTO `region` VALUES ('341700', 'CN', '安徽', '池州', '');
INSERT INTO `region` VALUES ('341800', 'CN', '安徽', '宣城', '');
INSERT INTO `region` VALUES ('350000', 'CN', '福建', '', '');
INSERT INTO `region` VALUES ('350100', 'CN', '福建', '福州', '');
INSERT INTO `region` VALUES ('350200', 'CN', '福建', '厦门', '');
INSERT INTO `region` VALUES ('350300', 'CN', '福建', '莆田', '');
INSERT INTO `region` VALUES ('350400', 'CN', '福建', '三明', '');
INSERT INTO `region` VALUES ('350500', 'CN', '福建', '泉州', '');
INSERT INTO `region` VALUES ('350600', 'CN', '福建', '漳州', '');
INSERT INTO `region` VALUES ('350700', 'CN', '福建', '南平', '');
INSERT INTO `region` VALUES ('350800', 'CN', '福建', '龙岩', '');
INSERT INTO `region` VALUES ('350900', 'CN', '福建', '宁德', '');
INSERT INTO `region` VALUES ('360000', 'CN', '江西', '', '');
INSERT INTO `region` VALUES ('360100', 'CN', '江西', '南昌', '');
INSERT INTO `region` VALUES ('360200', 'CN', '江西', '景德镇', '');
INSERT INTO `region` VALUES ('360300', 'CN', '江西', '萍乡', '');
INSERT INTO `region` VALUES ('360400', 'CN', '江西', '九江', '');
INSERT INTO `region` VALUES ('360500', 'CN', '江西', '新余', '');
INSERT INTO `region` VALUES ('360600', 'CN', '江西', '鹰潭', '');
INSERT INTO `region` VALUES ('360700', 'CN', '江西', '赣州', '');
INSERT INTO `region` VALUES ('360800', 'CN', '江西', '吉安', '');
INSERT INTO `region` VALUES ('360900', 'CN', '江西', '宜春', '');
INSERT INTO `region` VALUES ('361000', 'CN', '江西', '抚州', '');
INSERT INTO `region` VALUES ('361100', 'CN', '江西', '上饶', '');
INSERT INTO `region` VALUES ('370000', 'CN', '山东', '', '');
INSERT INTO `region` VALUES ('370100', 'CN', '山东', '济南', '');
INSERT INTO `region` VALUES ('370200', 'CN', '山东', '青岛', '');
INSERT INTO `region` VALUES ('370300', 'CN', '山东', '淄博', '');
INSERT INTO `region` VALUES ('370400', 'CN', '山东', '枣庄', '');
INSERT INTO `region` VALUES ('370500', 'CN', '山东', '东营', '');
INSERT INTO `region` VALUES ('370600', 'CN', '山东', '烟台', '');
INSERT INTO `region` VALUES ('370700', 'CN', '山东', '潍坊', '');
INSERT INTO `region` VALUES ('370800', 'CN', '山东', '济宁', '');
INSERT INTO `region` VALUES ('370900', 'CN', '山东', '泰安', '');
INSERT INTO `region` VALUES ('371000', 'CN', '山东', '威海', '');
INSERT INTO `region` VALUES ('371100', 'CN', '山东', '日照', '');
INSERT INTO `region` VALUES ('371200', 'CN', '山东', '莱芜', '');
INSERT INTO `region` VALUES ('371300', 'CN', '山东', '临沂', '');
INSERT INTO `region` VALUES ('371400', 'CN', '山东', '德州', '');
INSERT INTO `region` VALUES ('371500', 'CN', '山东', '聊城', '');
INSERT INTO `region` VALUES ('371600', 'CN', '山东', '滨州', '');
INSERT INTO `region` VALUES ('371700', 'CN', '山东', '荷泽', '');
INSERT INTO `region` VALUES ('410000', 'CN', '河南', '', '');
INSERT INTO `region` VALUES ('410100', 'CN', '河南', '郑州', '');
INSERT INTO `region` VALUES ('410200', 'CN', '河南', '开封', '');
INSERT INTO `region` VALUES ('410300', 'CN', '河南', '洛阳', '');
INSERT INTO `region` VALUES ('410400', 'CN', '河南', '平顶山', '');
INSERT INTO `region` VALUES ('410500', 'CN', '河南', '安阳', '');
INSERT INTO `region` VALUES ('410600', 'CN', '河南', '鹤壁', '');
INSERT INTO `region` VALUES ('410700', 'CN', '河南', '新乡', '');
INSERT INTO `region` VALUES ('410800', 'CN', '河南', '焦作', '');
INSERT INTO `region` VALUES ('410900', 'CN', '河南', '濮阳', '');
INSERT INTO `region` VALUES ('411000', 'CN', '河南', '许昌', '');
INSERT INTO `region` VALUES ('411100', 'CN', '河南', '漯河', '');
INSERT INTO `region` VALUES ('411200', 'CN', '河南', '三门峡', '');
INSERT INTO `region` VALUES ('411300', 'CN', '河南', '南阳', '');
INSERT INTO `region` VALUES ('411400', 'CN', '河南', '商丘', '');
INSERT INTO `region` VALUES ('411500', 'CN', '河南', '信阳', '');
INSERT INTO `region` VALUES ('411600', 'CN', '河南', '周口', '');
INSERT INTO `region` VALUES ('411700', 'CN', '河南', '驻马店', '');
INSERT INTO `region` VALUES ('420000', 'CN', '湖北', '', '');
INSERT INTO `region` VALUES ('420100', 'CN', '湖北', '武汉', '');
INSERT INTO `region` VALUES ('420200', 'CN', '湖北', '黄石', '');
INSERT INTO `region` VALUES ('420300', 'CN', '湖北', '十堰', '');
INSERT INTO `region` VALUES ('420500', 'CN', '湖北', '宜昌', '');
INSERT INTO `region` VALUES ('420600', 'CN', '湖北', '襄樊', '');
INSERT INTO `region` VALUES ('420700', 'CN', '湖北', '鄂州', '');
INSERT INTO `region` VALUES ('420800', 'CN', '湖北', '荆门', '');
INSERT INTO `region` VALUES ('420900', 'CN', '湖北', '孝感', '');
INSERT INTO `region` VALUES ('421000', 'CN', '湖北', '荆州', '');
INSERT INTO `region` VALUES ('421100', 'CN', '湖北', '黄冈', '');
INSERT INTO `region` VALUES ('421200', 'CN', '湖北', '咸宁', '');
INSERT INTO `region` VALUES ('421300', 'CN', '湖北', '随州', '');
INSERT INTO `region` VALUES ('422800', 'CN', '湖北', '恩施土家族苗族自治州', '');
INSERT INTO `region` VALUES ('429000', 'CN', '湖北', '直辖行政单位', '');
INSERT INTO `region` VALUES ('430000', 'CN', '湖南', '', '');
INSERT INTO `region` VALUES ('430100', 'CN', '湖南', '长沙', '');
INSERT INTO `region` VALUES ('430200', 'CN', '湖南', '株洲', '');
INSERT INTO `region` VALUES ('430300', 'CN', '湖南', '湘潭', '');
INSERT INTO `region` VALUES ('430400', 'CN', '湖南', '衡阳', '');
INSERT INTO `region` VALUES ('430500', 'CN', '湖南', '邵阳', '');
INSERT INTO `region` VALUES ('430600', 'CN', '湖南', '岳阳', '');
INSERT INTO `region` VALUES ('430700', 'CN', '湖南', '常德', '');
INSERT INTO `region` VALUES ('430800', 'CN', '湖南', '张家界', '');
INSERT INTO `region` VALUES ('430900', 'CN', '湖南', '益阳', '');
INSERT INTO `region` VALUES ('431000', 'CN', '湖南', '郴州', '');
INSERT INTO `region` VALUES ('431100', 'CN', '湖南', '永州', '');
INSERT INTO `region` VALUES ('431200', 'CN', '湖南', '怀化', '');
INSERT INTO `region` VALUES ('431300', 'CN', '湖南', '娄底', '');
INSERT INTO `region` VALUES ('433100', 'CN', '湖南', '湘西土家族苗族自治州', '');
INSERT INTO `region` VALUES ('440000', 'CN', '广东', '', '');
INSERT INTO `region` VALUES ('440200', 'CN', '广东', '韶关', '');
INSERT INTO `region` VALUES ('440400', 'CN', '广东', '珠海', '');
INSERT INTO `region` VALUES ('440500', 'CN', '广东', '汕头', '');
INSERT INTO `region` VALUES ('440600', 'CN', '广东', '佛山', '');
INSERT INTO `region` VALUES ('440700', 'CN', '广东', '江门', '');
INSERT INTO `region` VALUES ('440800', 'CN', '广东', '湛江', '');
INSERT INTO `region` VALUES ('440900', 'CN', '广东', '茂名', '');
INSERT INTO `region` VALUES ('441000', 'CN', '广东', '广州', '');
INSERT INTO `region` VALUES ('441200', 'CN', '广东', '肇庆', '');
INSERT INTO `region` VALUES ('441300', 'CN', '广东', '惠州', '');
INSERT INTO `region` VALUES ('441400', 'CN', '广东', '梅州', '');
INSERT INTO `region` VALUES ('441500', 'CN', '广东', '汕尾', '');
INSERT INTO `region` VALUES ('441600', 'CN', '广东', '河源', '');
INSERT INTO `region` VALUES ('441700', 'CN', '广东', '阳江', '');
INSERT INTO `region` VALUES ('441800', 'CN', '广东', '清远', '');
INSERT INTO `region` VALUES ('441900', 'CN', '广东', '东莞', '');
INSERT INTO `region` VALUES ('442000', 'CN', '广东', '中山', '');
INSERT INTO `region` VALUES ('443000', 'CN', '广东', '深圳', '');
INSERT INTO `region` VALUES ('445100', 'CN', '广东', '潮州', '');
INSERT INTO `region` VALUES ('445200', 'CN', '广东', '揭阳', '');
INSERT INTO `region` VALUES ('445300', 'CN', '广东', '云浮', '');
INSERT INTO `region` VALUES ('450000', 'CN', '广西', '', '');
INSERT INTO `region` VALUES ('450100', 'CN', '广西壮族', '南宁', '');
INSERT INTO `region` VALUES ('450200', 'CN', '广西壮族', '柳州', '');
INSERT INTO `region` VALUES ('450300', 'CN', '广西壮族', '桂林', '');
INSERT INTO `region` VALUES ('450400', 'CN', '广西壮族', '梧州', '');
INSERT INTO `region` VALUES ('450500', 'CN', '广西壮族', '北海', '');
INSERT INTO `region` VALUES ('450600', 'CN', '广西壮族', '防城港', '');
INSERT INTO `region` VALUES ('450700', 'CN', '广西壮族', '钦州', '');
INSERT INTO `region` VALUES ('450800', 'CN', '广西壮族', '贵港', '');
INSERT INTO `region` VALUES ('450900', 'CN', '广西壮族', '玉林', '');
INSERT INTO `region` VALUES ('451000', 'CN', '广西壮族', '百色', '');
INSERT INTO `region` VALUES ('451100', 'CN', '广西壮族', '贺州', '');
INSERT INTO `region` VALUES ('451200', 'CN', '广西壮族', '河池', '');
INSERT INTO `region` VALUES ('451300', 'CN', '广西壮族', '来宾', '');
INSERT INTO `region` VALUES ('451400', 'CN', '广西壮族', '崇左', '');
INSERT INTO `region` VALUES ('460000', 'CN', '海南', '', '');
INSERT INTO `region` VALUES ('460100', 'CN', '海南', '海口', '');
INSERT INTO `region` VALUES ('460200', 'CN', '海南', '三亚', '');
INSERT INTO `region` VALUES ('469000', 'CN', '海南', '直辖县级行政单位', '');
INSERT INTO `region` VALUES ('500000', 'CN', '重庆', '', '');
INSERT INTO `region` VALUES ('500100', 'CN', '重庆', '市辖区', '');
INSERT INTO `region` VALUES ('500200', 'CN', '重庆', '市县', '');
INSERT INTO `region` VALUES ('500300', 'CN', '重庆', '市', '');
INSERT INTO `region` VALUES ('510000', 'CN', '四川', '', '');
INSERT INTO `region` VALUES ('510100', 'CN', '四川', '成都', '');
INSERT INTO `region` VALUES ('510300', 'CN', '四川', '自贡', '');
INSERT INTO `region` VALUES ('510400', 'CN', '四川', '攀枝花', '');
INSERT INTO `region` VALUES ('510500', 'CN', '四川', '泸州', '');
INSERT INTO `region` VALUES ('510600', 'CN', '四川', '德阳', '');
INSERT INTO `region` VALUES ('510700', 'CN', '四川', '绵阳', '');
INSERT INTO `region` VALUES ('510800', 'CN', '四川', '广元', '');
INSERT INTO `region` VALUES ('510900', 'CN', '四川', '遂宁', '');
INSERT INTO `region` VALUES ('511000', 'CN', '四川', '内江', '');
INSERT INTO `region` VALUES ('511100', 'CN', '四川', '乐山', '');
INSERT INTO `region` VALUES ('511200', 'CN', '四川', '万县', '');
INSERT INTO `region` VALUES ('511300', 'CN', '四川', '南充', '');
INSERT INTO `region` VALUES ('511400', 'CN', '四川', '眉山', '');
INSERT INTO `region` VALUES ('511500', 'CN', '四川', '宜宾', '');
INSERT INTO `region` VALUES ('511600', 'CN', '四川', '广安', '');
INSERT INTO `region` VALUES ('511700', 'CN', '四川', '达州', '');
INSERT INTO `region` VALUES ('511800', 'CN', '四川', '雅安', '');
INSERT INTO `region` VALUES ('511900', 'CN', '四川', '巴中', '');
INSERT INTO `region` VALUES ('512000', 'CN', '四川', '资阳', '');
INSERT INTO `region` VALUES ('512300', 'CN', '四川', '涪陵地区', '');
INSERT INTO `region` VALUES ('512500', 'CN', '四川', '宜宾地区', '');
INSERT INTO `region` VALUES ('513000', 'CN', '四川', '达川地区', '');
INSERT INTO `region` VALUES ('513100', 'CN', '四川', '雅安地区', '');
INSERT INTO `region` VALUES ('513200', 'CN', '四川', '阿坝藏族羌族自治州', '');
INSERT INTO `region` VALUES ('513300', 'CN', '四川', '甘孜藏族自治州', '');
INSERT INTO `region` VALUES ('513400', 'CN', '四川', '凉山彝族自治州', '');
INSERT INTO `region` VALUES ('513500', 'CN', '四川', '黔江地区', '');
INSERT INTO `region` VALUES ('513600', 'CN', '四川', '广安地区', '');
INSERT INTO `region` VALUES ('513700', 'CN', '四川', '巴中地区', '');
INSERT INTO `region` VALUES ('520000', 'CN', '贵州', '', '');
INSERT INTO `region` VALUES ('520100', 'CN', '贵州', '贵阳', '');
INSERT INTO `region` VALUES ('520200', 'CN', '贵州', '六盘水', '');
INSERT INTO `region` VALUES ('520300', 'CN', '贵州', '遵义', '');
INSERT INTO `region` VALUES ('520400', 'CN', '贵州', '安顺', '');
INSERT INTO `region` VALUES ('522200', 'CN', '贵州', '铜仁地区', '');
INSERT INTO `region` VALUES ('522300', 'CN', '贵州', '黔西南布依族苗族自治州', '');
INSERT INTO `region` VALUES ('522400', 'CN', '贵州', '毕节地区', '');
INSERT INTO `region` VALUES ('522600', 'CN', '贵州', '黔东南苗族侗族自治州', '');
INSERT INTO `region` VALUES ('522700', 'CN', '贵州', '黔南布依族苗族自治州', '');
INSERT INTO `region` VALUES ('530000', 'CN', '云南', '', '');
INSERT INTO `region` VALUES ('530100', 'CN', '云南', '昆明', '');
INSERT INTO `region` VALUES ('530300', 'CN', '云南', '曲靖', '');
INSERT INTO `region` VALUES ('530400', 'CN', '云南', '玉溪', '');
INSERT INTO `region` VALUES ('530500', 'CN', '云南', '保山', '');
INSERT INTO `region` VALUES ('530600', 'CN', '云南', '昭通', '');
INSERT INTO `region` VALUES ('530700', 'CN', '云南', '丽江', '');
INSERT INTO `region` VALUES ('530800', 'CN', '云南', '思茅', '');
INSERT INTO `region` VALUES ('530900', 'CN', '云南', '临沧', '');
INSERT INTO `region` VALUES ('532300', 'CN', '云南', '楚雄彝族自治州', '');
INSERT INTO `region` VALUES ('532500', 'CN', '云南', '红河哈尼族彝族自治州', '');
INSERT INTO `region` VALUES ('532600', 'CN', '云南', '文山壮族苗族自治州', '');
INSERT INTO `region` VALUES ('532800', 'CN', '云南', '西双版纳傣族自治州', '');
INSERT INTO `region` VALUES ('532900', 'CN', '云南', '大理白族自治州', '');
INSERT INTO `region` VALUES ('533100', 'CN', '云南', '德宏傣族景颇族自治州', '');
INSERT INTO `region` VALUES ('533300', 'CN', '云南', '怒江傈僳族自治州', '');
INSERT INTO `region` VALUES ('533400', 'CN', '云南', '迪庆藏族自治州', '');
INSERT INTO `region` VALUES ('540000', 'CN', '西藏', '', '');
INSERT INTO `region` VALUES ('540100', 'CN', '西藏', '拉萨', '');
INSERT INTO `region` VALUES ('542100', 'CN', '西藏', '昌都地区', '');
INSERT INTO `region` VALUES ('542200', 'CN', '西藏', '山南地区', '');
INSERT INTO `region` VALUES ('542300', 'CN', '西藏', '日喀则地区', '');
INSERT INTO `region` VALUES ('542400', 'CN', '西藏', '那曲地区', '');
INSERT INTO `region` VALUES ('542500', 'CN', '西藏', '阿里地区', '');
INSERT INTO `region` VALUES ('542600', 'CN', '西藏', '林芝地区', '');
INSERT INTO `region` VALUES ('610000', 'CN', '陕西', '', '');
INSERT INTO `region` VALUES ('610100', 'CN', '陕西', '西安', '');
INSERT INTO `region` VALUES ('610200', 'CN', '陕西', '铜川', '');
INSERT INTO `region` VALUES ('610300', 'CN', '陕西', '宝鸡', '');
INSERT INTO `region` VALUES ('610400', 'CN', '陕西', '咸阳', '');
INSERT INTO `region` VALUES ('610500', 'CN', '陕西', '渭南', '');
INSERT INTO `region` VALUES ('610600', 'CN', '陕西', '延安', '');
INSERT INTO `region` VALUES ('610700', 'CN', '陕西', '汉中', '');
INSERT INTO `region` VALUES ('610800', 'CN', '陕西', '榆林', '');
INSERT INTO `region` VALUES ('610900', 'CN', '陕西', '安康', '');
INSERT INTO `region` VALUES ('611000', 'CN', '陕西', '商洛', '');
INSERT INTO `region` VALUES ('620000', 'CN', '甘肃', '', '');
INSERT INTO `region` VALUES ('620100', 'CN', '甘肃', '兰州', '');
INSERT INTO `region` VALUES ('620200', 'CN', '甘肃', '嘉峪关', '');
INSERT INTO `region` VALUES ('620300', 'CN', '甘肃', '金昌', '');
INSERT INTO `region` VALUES ('620400', 'CN', '甘肃', '白银', '');
INSERT INTO `region` VALUES ('620500', 'CN', '甘肃', '天水', '');
INSERT INTO `region` VALUES ('620600', 'CN', '甘肃', '武威', '');
INSERT INTO `region` VALUES ('620700', 'CN', '甘肃', '张掖', '');
INSERT INTO `region` VALUES ('620800', 'CN', '甘肃', '平凉', '');
INSERT INTO `region` VALUES ('620900', 'CN', '甘肃', '酒泉', '');
INSERT INTO `region` VALUES ('621000', 'CN', '甘肃', '庆阳', '');
INSERT INTO `region` VALUES ('621100', 'CN', '甘肃', '定西', '');
INSERT INTO `region` VALUES ('621200', 'CN', '甘肃', '陇南', '');
INSERT INTO `region` VALUES ('622900', 'CN', '甘肃', '临夏回族自治州', '');
INSERT INTO `region` VALUES ('623000', 'CN', '甘肃', '甘南藏族自治州', '');
INSERT INTO `region` VALUES ('630000', 'CN', '青海', '', '');
INSERT INTO `region` VALUES ('630100', 'CN', '青海', '西宁', '');
INSERT INTO `region` VALUES ('632100', 'CN', '青海', '海东地区', '');
INSERT INTO `region` VALUES ('632200', 'CN', '青海', '海北藏族自治州', '');
INSERT INTO `region` VALUES ('632300', 'CN', '青海', '黄南藏族自治州', '');
INSERT INTO `region` VALUES ('632500', 'CN', '青海', '海南藏族自治州', '');
INSERT INTO `region` VALUES ('632600', 'CN', '青海', '果洛藏族自治州', '');
INSERT INTO `region` VALUES ('632700', 'CN', '青海', '玉树藏族自治州', '');
INSERT INTO `region` VALUES ('632800', 'CN', '青海', '海西蒙古族藏族自治州', '');
INSERT INTO `region` VALUES ('640000', 'CN', '宁夏', '', '');
INSERT INTO `region` VALUES ('640100', 'CN', '宁夏', '银川', '');
INSERT INTO `region` VALUES ('640200', 'CN', '宁夏', '石嘴山', '');
INSERT INTO `region` VALUES ('640300', 'CN', '宁夏', '吴忠', '');
INSERT INTO `region` VALUES ('640400', 'CN', '宁夏', '固原', '');
INSERT INTO `region` VALUES ('640500', 'CN', '宁夏', '中卫', '');
INSERT INTO `region` VALUES ('650000', 'CN', '新疆', '', '');
INSERT INTO `region` VALUES ('650100', 'CN', '新疆', '乌鲁木齐', '');
INSERT INTO `region` VALUES ('650200', 'CN', '新疆', '克拉玛依', '');
INSERT INTO `region` VALUES ('652100', 'CN', '新疆', '吐鲁番地区', '');
INSERT INTO `region` VALUES ('652200', 'CN', '新疆', '哈密地区', '');
INSERT INTO `region` VALUES ('652300', 'CN', '新疆', '昌吉回族自治州', '');
INSERT INTO `region` VALUES ('652700', 'CN', '新疆', '博尔塔拉蒙古自治州', '');
INSERT INTO `region` VALUES ('652800', 'CN', '新疆', '巴音郭楞蒙古自治州', '');
INSERT INTO `region` VALUES ('652900', 'CN', '新疆', '阿克苏地区', '');
INSERT INTO `region` VALUES ('653000', 'CN', '新疆', '克孜勒苏柯尔克孜自治州', '');
INSERT INTO `region` VALUES ('653100', 'CN', '新疆', '喀什地区', '');
INSERT INTO `region` VALUES ('653200', 'CN', '新疆', '和田地区', '');
INSERT INTO `region` VALUES ('654000', 'CN', '新疆', '伊犁哈萨克自治州', '');
INSERT INTO `region` VALUES ('654200', 'CN', '新疆', '塔城地区', '');
INSERT INTO `region` VALUES ('654300', 'CN', '新疆', '阿勒泰地区', '');
INSERT INTO `region` VALUES ('659000', 'CN', '新疆', '省直辖行政单位', '');
INSERT INTO `region` VALUES ('710000', 'CN', '台湾', '', '');
INSERT INTO `region` VALUES ('810000', 'CN', '香港', '', '');
INSERT INTO `region` VALUES ('820000', 'CN', '澳门', '', '');

-- ----------------------------
-- Table structure for region_country
-- ----------------------------
DROP TABLE IF EXISTS `region_country`;
CREATE TABLE `region_country` (
  `country_code` int(11) NOT NULL,
  `country_name` varchar(60) DEFAULT NULL,
  PRIMARY KEY (`country_code`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of region_country
-- ----------------------------

-- ----------------------------
-- Table structure for region_province
-- ----------------------------
DROP TABLE IF EXISTS `region_province`;
CREATE TABLE `region_province` (
  `province_code` int(11) NOT NULL,
  `province_name` varchar(60) NOT NULL,
  PRIMARY KEY (`province_code`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of region_province
-- ----------------------------
INSERT INTO `region_province` VALUES ('110000', '北京');
INSERT INTO `region_province` VALUES ('120000', '天津');
INSERT INTO `region_province` VALUES ('130000', '河北');
INSERT INTO `region_province` VALUES ('140000', '山西');
INSERT INTO `region_province` VALUES ('150000', '内蒙古');
INSERT INTO `region_province` VALUES ('210000', '辽宁');
INSERT INTO `region_province` VALUES ('220000', '吉林');
INSERT INTO `region_province` VALUES ('230000', '黑龙江');
INSERT INTO `region_province` VALUES ('310000', '上海');
INSERT INTO `region_province` VALUES ('320000', '江苏');
INSERT INTO `region_province` VALUES ('330000', '浙江');
INSERT INTO `region_province` VALUES ('340000', '安徽');
INSERT INTO `region_province` VALUES ('350000', '福建');
INSERT INTO `region_province` VALUES ('360000', '江西');
INSERT INTO `region_province` VALUES ('370000', '山东');
INSERT INTO `region_province` VALUES ('410000', '河南');
INSERT INTO `region_province` VALUES ('420000', '湖北');
INSERT INTO `region_province` VALUES ('430000', '湖南');
INSERT INTO `region_province` VALUES ('440000', '广东');
INSERT INTO `region_province` VALUES ('450000', '广西');
INSERT INTO `region_province` VALUES ('460000', '海南');
INSERT INTO `region_province` VALUES ('500000', '重庆');
INSERT INTO `region_province` VALUES ('510000', '四川');
INSERT INTO `region_province` VALUES ('520000', '贵州');
INSERT INTO `region_province` VALUES ('530000', '云南');
INSERT INTO `region_province` VALUES ('540000', '西藏');
INSERT INTO `region_province` VALUES ('610000', '陕西');
INSERT INTO `region_province` VALUES ('620000', '甘肃');
INSERT INTO `region_province` VALUES ('630000', '青海');
INSERT INTO `region_province` VALUES ('640000', '宁夏');
INSERT INTO `region_province` VALUES ('650000', '新疆');

-- ----------------------------
-- Table structure for resource
-- ----------------------------
DROP TABLE IF EXISTS `resource`;
CREATE TABLE `resource` (
  `resource_id` varchar(20) NOT NULL,
  `resource_name` varchar(20) NOT NULL,
  `sender_list` varchar(1024) DEFAULT NULL,
  `helo_list` varchar(1024) DEFAULT NULL,
  `skip_list` varchar(1024) DEFAULT NULL,
  `modify_time` datetime NOT NULL,
  PRIMARY KEY (`resource_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of resource
-- ----------------------------
INSERT INTO `resource` VALUES ('api', 'API投递通道', '', '', '192.168.34.170:80', '2013-05-10 10:00:00');
INSERT INTO `resource` VALUES ('normal', '普通通道', 'service@dmdeliver.com,service@dmdeliver.net,service@dmsend.net,service@mtinsure.net,service@mtdelivery.net,service@mtdeliver.net,service@mtdeliver.com,mtservice.asia,caixunmt.com', 'dmdeliver.com,dmdeliver.net,dmsend.net,mtinsure.net,mtdelivery.net,mtdeliver.net,mtdeliver.com,mtservice.asia,caixunmt.com', '192.168.34.170:80', '2013-05-10 10:00:00');
INSERT INTO `resource` VALUES ('test', '测试通道', 'service@mtair.net,service@mtsolomo.com', 'mtair.net,mtsolomo.com', '192.168.34.170:80', '2013-05-10 10:00:00');

-- ----------------------------
-- Table structure for result
-- ----------------------------
DROP TABLE IF EXISTS `result`;
CREATE TABLE `result` (
  `corp_id` int(11) NOT NULL,
  `task_id` int(11) NOT NULL,
  `template_id` int(11) NOT NULL,
  `email_count` int(11) DEFAULT '0',
  `sent_count` int(11) DEFAULT '0',
  `reach_count` int(11) DEFAULT '0',
  `read_count` int(11) DEFAULT '0',
  `read_user_count` int(11) DEFAULT '0',
  `click_count` int(11) DEFAULT '0',
  `click_user_count` int(11) DEFAULT '0',
  `unsubscribe_count` int(11) DEFAULT '0',
  `forward_count` int(11) DEFAULT '0',
  `soft_bounce_count` int(11) DEFAULT '0',
  `hard_bounce_count` int(11) DEFAULT '0',
  `none_exist_count` int(11) DEFAULT '0',
  `none_exist_count1` int(11) DEFAULT '0',
  `dns_fail_count` int(11) DEFAULT '0',
  `spam_garbage_count` int(11) DEFAULT '0',
  `begin_send_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `end_send_time` datetime DEFAULT NULL,
  PRIMARY KEY (`corp_id`,`task_id`,`template_id`),
  KEY `idx1` (`corp_id`,`task_id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of result
-- ----------------------------

-- ----------------------------
-- Table structure for result_by_browser
-- ----------------------------
DROP TABLE IF EXISTS `result_by_browser`;
CREATE TABLE `result_by_browser` (
  `corp_id` int(11) NOT NULL,
  `task_id` int(11) NOT NULL,
  `template_id` int(11) NOT NULL,
  `browser` tinyint(1) NOT NULL,
  `open_count` int(11) NOT NULL,
  `click_count` int(11) NOT NULL,
  UNIQUE KEY `idx1` (`corp_id`,`task_id`,`template_id`,`browser`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of result_by_browser
-- ----------------------------

-- ----------------------------
-- Table structure for result_by_domain
-- ----------------------------
DROP TABLE IF EXISTS `result_by_domain`;
CREATE TABLE `result_by_domain` (
  `corp_id` int(11) NOT NULL,
  `task_id` int(11) NOT NULL,
  `template_id` int(11) NOT NULL,
  `email_domain` varchar(64) NOT NULL,
  `email_count` int(11) DEFAULT '0',
  `sent_count` int(11) DEFAULT '0',
  `reach_count` int(11) DEFAULT '0',
  `read_count` int(11) DEFAULT '0',
  `read_user_count` int(11) DEFAULT '0',
  `click_count` int(11) DEFAULT '0',
  `click_user_count` int(11) DEFAULT '0',
  `unsubscribe_count` int(11) DEFAULT '0',
  `forward_count` int(11) DEFAULT '0',
  `soft_bounce_count` int(11) DEFAULT '0',
  `hard_bounce_count` int(11) DEFAULT '0',
  `none_exist_count` int(11) DEFAULT '0',
  `none_exist_count1` int(11) DEFAULT '0',
  `dns_fail_count` int(11) DEFAULT '0',
  `spam_garbage_count` int(11) DEFAULT '0',
  PRIMARY KEY (`task_id`,`template_id`,`email_domain`,`corp_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of result_by_domain
-- ----------------------------

-- ----------------------------
-- Table structure for result_by_hour
-- ----------------------------
DROP TABLE IF EXISTS `result_by_hour`;
CREATE TABLE `result_by_hour` (
  `corp_id` int(11) NOT NULL,
  `task_id` int(11) NOT NULL,
  `template_id` int(11) NOT NULL,
  `hour` int(11) NOT NULL,
  `read_count` int(11) DEFAULT '0',
  `read_user_count` int(11) DEFAULT '0',
  `click_count` int(11) DEFAULT '0',
  `click_user_count` int(11) DEFAULT '0',
  PRIMARY KEY (`task_id`,`template_id`,`hour`,`corp_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of result_by_hour
-- ----------------------------

-- ----------------------------
-- Table structure for result_by_lang
-- ----------------------------
DROP TABLE IF EXISTS `result_by_lang`;
CREATE TABLE `result_by_lang` (
  `corp_id` int(11) NOT NULL,
  `task_id` int(11) NOT NULL,
  `template_id` int(11) NOT NULL,
  `lang` tinyint(1) NOT NULL,
  `open_count` int(11) NOT NULL,
  `click_count` int(11) NOT NULL,
  UNIQUE KEY `idx1` (`corp_id`,`task_id`,`template_id`,`lang`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of result_by_lang
-- ----------------------------

-- ----------------------------
-- Table structure for result_by_os
-- ----------------------------
DROP TABLE IF EXISTS `result_by_os`;
CREATE TABLE `result_by_os` (
  `corp_id` int(11) NOT NULL,
  `task_id` int(11) NOT NULL,
  `template_id` int(11) NOT NULL,
  `os` tinyint(1) NOT NULL,
  `open_count` int(11) NOT NULL,
  `click_count` int(11) NOT NULL,
  UNIQUE KEY `idx1` (`corp_id`,`task_id`,`template_id`,`os`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of result_by_os
-- ----------------------------

-- ----------------------------
-- Table structure for result_by_region
-- ----------------------------
DROP TABLE IF EXISTS `result_by_region`;
CREATE TABLE `result_by_region` (
  `corp_id` int(11) NOT NULL,
  `task_id` int(11) NOT NULL,
  `template_id` int(11) NOT NULL,
  `region_id` int(11) NOT NULL,
  `read_count` int(11) DEFAULT '0',
  `read_user_count` int(11) DEFAULT '0',
  `click_count` int(11) DEFAULT '0',
  `click_user_count` int(11) DEFAULT '0',
  `unsubscribe_count` int(11) DEFAULT '0',
  `forward_count` int(11) DEFAULT '0',
  PRIMARY KEY (`task_id`,`template_id`,`region_id`,`corp_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of result_by_region
-- ----------------------------

-- ----------------------------
-- Table structure for result_by_url
-- ----------------------------
DROP TABLE IF EXISTS `result_by_url`;
CREATE TABLE `result_by_url` (
  `corp_id` int(11) NOT NULL,
  `task_id` int(11) NOT NULL,
  `template_id` int(11) NOT NULL,
  `url` varchar(255) NOT NULL,
  `click_count` int(11) DEFAULT '0',
  PRIMARY KEY (`task_id`,`template_id`,`url`,`corp_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of result_by_url
-- ----------------------------

-- ----------------------------
-- Table structure for robot
-- ----------------------------
DROP TABLE IF EXISTS `robot`;
CREATE TABLE `robot` (
  `corp_id` int(11) NOT NULL,
  `email` varchar(64) NOT NULL,
  UNIQUE KEY `idx1` (`corp_id`,`email`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of robot
-- ----------------------------

-- ----------------------------
-- Table structure for role
-- ----------------------------
DROP TABLE IF EXISTS `role`;
CREATE TABLE `role` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `role_name` varchar(30) DEFAULT NULL COMMENT '角色名称',
  `role_desc` varchar(100) DEFAULT NULL COMMENT '角色描述',
  `type` int(11) NOT NULL COMMENT '角色类型，如：0-->前台普通角色；1-->前台管理员；2-->后台角色',
  `create_time` datetime DEFAULT NULL COMMENT '创建时间',
  `modify_time` datetime DEFAULT NULL COMMENT '修改时间',
  `operator` varchar(20) DEFAULT NULL COMMENT '操作员',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=15 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of role
-- ----------------------------
INSERT INTO `role` VALUES ('1', '超级管理员', '超级管理员', '2', '2015-02-09 10:11:01', '2015-02-09 10:11:01', 'admin');
INSERT INTO `role` VALUES ('2', '企业管理员', '具有全量功能的默认管理员角色', '1', '2015-02-09 10:11:01', '2015-03-01 14:47:10', 'admin');
INSERT INTO `role` VALUES ('3', '群发人员', '负责任务创建与群发投递的专用角色', '0', '2015-02-09 10:11:01', '2015-03-01 15:23:48', 'admin');
INSERT INTO `role` VALUES ('4', '高级运营投递专员', '高级运营投递专员', '3', '2015-02-09 10:11:01', '2015-02-09 10:11:01', 'admin');
INSERT INTO `role` VALUES ('5', '后台监控专员', '后台监控专员', '3', '2015-02-09 10:11:01', '2015-02-09 10:11:01', 'admin');
INSERT INTO `role` VALUES ('11', '一级管理员', '一级管理员', '1', '2016-12-26 13:42:09', '2016-12-26 13:42:09', 'admin');
INSERT INTO `role` VALUES ('12', '二级管理员', '二级管理员', '1', '2016-12-26 13:42:57', '2016-12-26 13:42:57', 'admin');
INSERT INTO `role` VALUES ('13', '一级操作员', '一级操作员', '0', '2016-12-26 13:43:30', '2016-12-26 13:43:30', 'admin');
INSERT INTO `role` VALUES ('14', '二级操作员', '二级操作员', '0', '2016-12-26 13:44:03', '2016-12-26 13:44:03', 'admin');

-- ----------------------------
-- Table structure for role_menu
-- ----------------------------
DROP TABLE IF EXISTS `role_menu`;
CREATE TABLE `role_menu` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `role_id` int(11) DEFAULT NULL COMMENT 'role表主键',
  `menu_id` int(11) DEFAULT NULL COMMENT 'menu表主键',
  `func_auth` varchar(60) DEFAULT NULL COMMENT '菜单功能增删查改权限（1,2,3,4）=查询、增加、修改、删除',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=1524 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of role_menu
-- ----------------------------
INSERT INTO `role_menu` VALUES ('1', '1', '0', '0');
INSERT INTO `role_menu` VALUES ('2', '1', '1', '0');
INSERT INTO `role_menu` VALUES ('3', '1', '2', '0');
INSERT INTO `role_menu` VALUES ('4', '1', '3', '0');
INSERT INTO `role_menu` VALUES ('5', '1', '4', '0');
INSERT INTO `role_menu` VALUES ('6', '1', '5', '0');
INSERT INTO `role_menu` VALUES ('7', '1', '6', '0');
INSERT INTO `role_menu` VALUES ('8', '1', '7', '0');
INSERT INTO `role_menu` VALUES ('9', '1', '8', '0');
INSERT INTO `role_menu` VALUES ('10', '1', '9', '0');
INSERT INTO `role_menu` VALUES ('11', '1', '10', '0');
INSERT INTO `role_menu` VALUES ('12', '1', '11', '0');
INSERT INTO `role_menu` VALUES ('13', '1', '12', '0');
INSERT INTO `role_menu` VALUES ('14', '1', '13', '0');
INSERT INTO `role_menu` VALUES ('15', '1', '14', '0');
INSERT INTO `role_menu` VALUES ('16', '1', '15', '6');
INSERT INTO `role_menu` VALUES ('17', '1', '16', '0');
INSERT INTO `role_menu` VALUES ('18', '1', '17', '0');
INSERT INTO `role_menu` VALUES ('19', '1', '18', '0');
INSERT INTO `role_menu` VALUES ('20', '1', '19', '0');
INSERT INTO `role_menu` VALUES ('21', '1', '20', '0');
INSERT INTO `role_menu` VALUES ('22', '1', '21', '0');
INSERT INTO `role_menu` VALUES ('23', '1', '22', '0');
INSERT INTO `role_menu` VALUES ('24', '1', '23', '0');
INSERT INTO `role_menu` VALUES ('25', '1', '24', '0');
INSERT INTO `role_menu` VALUES ('1288', '2', '23', '0');
INSERT INTO `role_menu` VALUES ('1289', '2', '24', '0');
INSERT INTO `role_menu` VALUES ('1287', '2', '22', '0');
INSERT INTO `role_menu` VALUES ('1290', '2', '21', '0');
INSERT INTO `role_menu` VALUES ('1286', '2', '20', '0');
INSERT INTO `role_menu` VALUES ('1285', '2', '19', '0');
INSERT INTO `role_menu` VALUES ('1284', '2', '4', '0');
INSERT INTO `role_menu` VALUES ('1283', '2', '18', '0');
INSERT INTO `role_menu` VALUES ('1282', '2', '17', '0');
INSERT INTO `role_menu` VALUES ('1281', '2', '16', '0');
INSERT INTO `role_menu` VALUES ('1280', '2', '15', '0');
INSERT INTO `role_menu` VALUES ('1279', '2', '14', '0');
INSERT INTO `role_menu` VALUES ('1278', '2', '3', '0');
INSERT INTO `role_menu` VALUES ('1277', '2', '12', '0');
INSERT INTO `role_menu` VALUES ('1276', '2', '13', '0');
INSERT INTO `role_menu` VALUES ('1275', '2', '11', '0');
INSERT INTO `role_menu` VALUES ('1274', '2', '10', '0');
INSERT INTO `role_menu` VALUES ('1273', '2', '2', '0');
INSERT INTO `role_menu` VALUES ('1272', '2', '9', '0');
INSERT INTO `role_menu` VALUES ('1271', '2', '8', '0');
INSERT INTO `role_menu` VALUES ('1270', '2', '7', '0');
INSERT INTO `role_menu` VALUES ('1269', '2', '6', '0');
INSERT INTO `role_menu` VALUES ('1268', '2', '5', '0');
INSERT INTO `role_menu` VALUES ('1267', '2', '1', '0');
INSERT INTO `role_menu` VALUES ('1266', '3', '24', '0');
INSERT INTO `role_menu` VALUES ('1265', '3', '23', '0');
INSERT INTO `role_menu` VALUES ('1264', '3', '22', '0');
INSERT INTO `role_menu` VALUES ('1263', '3', '20', '0');
INSERT INTO `role_menu` VALUES ('1262', '3', '19', '0');
INSERT INTO `role_menu` VALUES ('1261', '3', '4', '0');
INSERT INTO `role_menu` VALUES ('1260', '3', '18', '0');
INSERT INTO `role_menu` VALUES ('1259', '3', '17', '0');
INSERT INTO `role_menu` VALUES ('1258', '3', '16', '0');
INSERT INTO `role_menu` VALUES ('1257', '3', '15', '0');
INSERT INTO `role_menu` VALUES ('1256', '3', '14', '0');
INSERT INTO `role_menu` VALUES ('1255', '3', '3', '0');
INSERT INTO `role_menu` VALUES ('1254', '3', '12', '0');
INSERT INTO `role_menu` VALUES ('1253', '3', '11', '1,2,3,4,5,6');
INSERT INTO `role_menu` VALUES ('1252', '3', '10', '0');
INSERT INTO `role_menu` VALUES ('1251', '3', '2', '0');
INSERT INTO `role_menu` VALUES ('1250', '3', '9', '0');
INSERT INTO `role_menu` VALUES ('1249', '3', '8', '0');
INSERT INTO `role_menu` VALUES ('1248', '3', '7', '0');
INSERT INTO `role_menu` VALUES ('1247', '3', '6', '0');
INSERT INTO `role_menu` VALUES ('1246', '3', '5', '0');
INSERT INTO `role_menu` VALUES ('1245', '3', '1', '0');
INSERT INTO `role_menu` VALUES ('1416', '8', '24', '0');
INSERT INTO `role_menu` VALUES ('1417', '11', '1', '0');
INSERT INTO `role_menu` VALUES ('1418', '11', '5', '0');
INSERT INTO `role_menu` VALUES ('1419', '11', '6', '0');
INSERT INTO `role_menu` VALUES ('1420', '11', '7', '0');
INSERT INTO `role_menu` VALUES ('1421', '11', '8', '0');
INSERT INTO `role_menu` VALUES ('1422', '11', '2', '0');
INSERT INTO `role_menu` VALUES ('1423', '11', '10', '0');
INSERT INTO `role_menu` VALUES ('1424', '11', '11', '0');
INSERT INTO `role_menu` VALUES ('1425', '11', '13', '0');
INSERT INTO `role_menu` VALUES ('1426', '11', '3', '0');
INSERT INTO `role_menu` VALUES ('1427', '11', '14', '0');
INSERT INTO `role_menu` VALUES ('1428', '11', '15', '0');
INSERT INTO `role_menu` VALUES ('1429', '11', '16', '0');
INSERT INTO `role_menu` VALUES ('1430', '11', '17', '0');
INSERT INTO `role_menu` VALUES ('1431', '11', '18', '0');
INSERT INTO `role_menu` VALUES ('1432', '11', '21', '0');
INSERT INTO `role_menu` VALUES ('1433', '11', '4', '0');
INSERT INTO `role_menu` VALUES ('1434', '11', '19', '0');
INSERT INTO `role_menu` VALUES ('1435', '11', '20', '0');
INSERT INTO `role_menu` VALUES ('1436', '11', '22', '0');
INSERT INTO `role_menu` VALUES ('1437', '11', '23', '0');
INSERT INTO `role_menu` VALUES ('1438', '11', '24', '0');
INSERT INTO `role_menu` VALUES ('1439', '12', '1', '0');
INSERT INTO `role_menu` VALUES ('1440', '12', '5', '0');
INSERT INTO `role_menu` VALUES ('1441', '12', '6', '0');
INSERT INTO `role_menu` VALUES ('1442', '12', '7', '0');
INSERT INTO `role_menu` VALUES ('1443', '12', '8', '0');
INSERT INTO `role_menu` VALUES ('1444', '12', '2', '0');
INSERT INTO `role_menu` VALUES ('1445', '12', '10', '0');
INSERT INTO `role_menu` VALUES ('1446', '12', '11', '0');
INSERT INTO `role_menu` VALUES ('1447', '12', '13', '0');
INSERT INTO `role_menu` VALUES ('1448', '12', '3', '0');
INSERT INTO `role_menu` VALUES ('1449', '12', '14', '0');
INSERT INTO `role_menu` VALUES ('1450', '12', '15', '0');
INSERT INTO `role_menu` VALUES ('1451', '12', '16', '0');
INSERT INTO `role_menu` VALUES ('1452', '12', '17', '0');
INSERT INTO `role_menu` VALUES ('1453', '12', '18', '0');
INSERT INTO `role_menu` VALUES ('1454', '12', '21', '0');
INSERT INTO `role_menu` VALUES ('1455', '12', '4', '0');
INSERT INTO `role_menu` VALUES ('1456', '12', '19', '0');
INSERT INTO `role_menu` VALUES ('1457', '12', '20', '0');
INSERT INTO `role_menu` VALUES ('1458', '12', '22', '0');
INSERT INTO `role_menu` VALUES ('1459', '12', '23', '0');
INSERT INTO `role_menu` VALUES ('1460', '12', '24', '0');
INSERT INTO `role_menu` VALUES ('1461', '13', '1', '0');
INSERT INTO `role_menu` VALUES ('1462', '13', '5', '0');
INSERT INTO `role_menu` VALUES ('1463', '13', '6', '0');
INSERT INTO `role_menu` VALUES ('1464', '13', '7', '0');
INSERT INTO `role_menu` VALUES ('1465', '13', '8', '0');
INSERT INTO `role_menu` VALUES ('1466', '13', '2', '0');
INSERT INTO `role_menu` VALUES ('1467', '13', '10', '0');
INSERT INTO `role_menu` VALUES ('1468', '13', '11', '0');
INSERT INTO `role_menu` VALUES ('1469', '13', '3', '0');
INSERT INTO `role_menu` VALUES ('1470', '13', '14', '0');
INSERT INTO `role_menu` VALUES ('1471', '13', '15', '0');
INSERT INTO `role_menu` VALUES ('1472', '13', '16', '0');
INSERT INTO `role_menu` VALUES ('1473', '13', '17', '0');
INSERT INTO `role_menu` VALUES ('1474', '13', '18', '0');
INSERT INTO `role_menu` VALUES ('1475', '13', '4', '0');
INSERT INTO `role_menu` VALUES ('1476', '13', '19', '0');
INSERT INTO `role_menu` VALUES ('1477', '13', '20', '0');
INSERT INTO `role_menu` VALUES ('1478', '13', '22', '0');
INSERT INTO `role_menu` VALUES ('1479', '13', '23', '0');
INSERT INTO `role_menu` VALUES ('1480', '13', '24', '0');
INSERT INTO `role_menu` VALUES ('1481', '14', '1', '0');
INSERT INTO `role_menu` VALUES ('1482', '14', '5', '0');
INSERT INTO `role_menu` VALUES ('1483', '14', '6', '0');
INSERT INTO `role_menu` VALUES ('1484', '14', '7', '0');
INSERT INTO `role_menu` VALUES ('1485', '14', '8', '0');
INSERT INTO `role_menu` VALUES ('1486', '14', '2', '0');
INSERT INTO `role_menu` VALUES ('1487', '14', '10', '0');
INSERT INTO `role_menu` VALUES ('1488', '14', '11', '0');
INSERT INTO `role_menu` VALUES ('1489', '14', '3', '0');
INSERT INTO `role_menu` VALUES ('1490', '14', '14', '0');
INSERT INTO `role_menu` VALUES ('1491', '14', '15', '0');
INSERT INTO `role_menu` VALUES ('1492', '14', '16', '0');
INSERT INTO `role_menu` VALUES ('1493', '14', '17', '0');
INSERT INTO `role_menu` VALUES ('1494', '14', '18', '0');
INSERT INTO `role_menu` VALUES ('1495', '14', '4', '0');
INSERT INTO `role_menu` VALUES ('1496', '14', '19', '0');
INSERT INTO `role_menu` VALUES ('1497', '14', '20', '0');
INSERT INTO `role_menu` VALUES ('1498', '14', '22', '0');
INSERT INTO `role_menu` VALUES ('1499', '14', '23', '0');
INSERT INTO `role_menu` VALUES ('1500', '14', '24', '0');

-- ----------------------------
-- Table structure for selection
-- ----------------------------
DROP TABLE IF EXISTS `selection`;
CREATE TABLE `selection` (
  `selection_id` int(11) NOT NULL AUTO_INCREMENT,
  `corp_id` int(11) NOT NULL,
  `user_id` varchar(60) NOT NULL,
  `include_ids` varchar(255) NOT NULL,
  `active_cnd` varchar(20) NOT NULL,
  `in_cnd` varchar(20) NOT NULL,
  `includes` varchar(512) NOT NULL,
  `exclude_ids` varchar(255) DEFAULT NULL,
  `excludes` varchar(512) DEFAULT NULL,
  `filter_id` int(11) DEFAULT NULL,
  `filter` varchar(255) DEFAULT NULL,
  `email_count` int(11) NOT NULL,
  `type` tinyint(1) NOT NULL,
  `file_id` varchar(30) DEFAULT NULL,
  `tag_id` int(11) DEFAULT NULL,
  `status` tinyint(1) NOT NULL,
  `create_time` datetime NOT NULL,
  `modify_time` datetime NOT NULL,
  PRIMARY KEY (`selection_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of selection
-- ----------------------------

-- ----------------------------
-- Table structure for sender
-- ----------------------------
DROP TABLE IF EXISTS `sender`;
CREATE TABLE `sender` (
  `corp_id` int(11) NOT NULL,
  `user_id` varchar(60) NOT NULL,
  `sender_email` varchar(64) NOT NULL,
  `sender_name` varchar(20) NOT NULL,
  `required` tinyint(1) NOT NULL,
  `rand_code` varchar(32) DEFAULT NULL,
  `status` tinyint(1) NOT NULL,
  `create_time` datetime NOT NULL,
  `modify_time` datetime NOT NULL,
  PRIMARY KEY (`corp_id`,`user_id`,`sender_email`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of sender
-- ----------------------------

-- ----------------------------
-- Table structure for smtp
-- ----------------------------
DROP TABLE IF EXISTS `smtp`;
CREATE TABLE `smtp` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `quarter` int(11) NOT NULL,
  `hour` int(11) NOT NULL,
  `day` int(11) NOT NULL,
  `ip` varchar(256) NOT NULL,
  `size` int(11) NOT NULL,
  `create_time` datetime DEFAULT NULL,
  `quarter_count` int(11) DEFAULT '0',
  `hour_count` int(11) DEFAULT '0',
  `day_count` int(11) DEFAULT '0',
  `quarter_time` datetime DEFAULT '0000-00-00 00:00:00',
  `hour_time` datetime DEFAULT '0000-00-00 00:00:00',
  `day_time` datetime DEFAULT '0000-00-00 00:00:00',
  UNIQUE KEY `pk_id` (`id`) USING BTREE,
  UNIQUE KEY `index_ip` (`ip`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of smtp
-- ----------------------------

-- ----------------------------
-- Table structure for stat_havior
-- ----------------------------
DROP TABLE IF EXISTS `stat_havior`;
CREATE TABLE `stat_havior` (
  `Corp_id` int(11) NOT NULL,
  `Task_id` int(11) NOT NULL,
  `Template_id` int(11) NOT NULL,
  `email` char(64) NOT NULL DEFAULT '',
  `havior` tinyint(4) NOT NULL DEFAULT '0',
  `osid` tinyint(4) DEFAULT NULL,
  `browserid` tinyint(4) DEFAULT NULL,
  `slangid` tinyint(4) DEFAULT NULL,
  `open` tinyint(4) DEFAULT '0',
  `click` tinyint(4) DEFAULT '0',
  `updateTime` datetime DEFAULT NULL,
  PRIMARY KEY (`Task_id`,`Template_id`,`email`,`havior`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of stat_havior
-- ----------------------------

-- ----------------------------
-- Table structure for tag
-- ----------------------------
DROP TABLE IF EXISTS `tag`;
CREATE TABLE `tag` (
  `tag_id` int(11) NOT NULL AUTO_INCREMENT,
  `db_id` int(11) NOT NULL,
  `category_id` int(11) NOT NULL,
  `corp_id` int(11) NOT NULL,
  `user_id` varchar(60) DEFAULT NULL,
  `tag_name` varchar(20) NOT NULL,
  `tag_desc` varchar(60) DEFAULT NULL,
  `email_count` int(11) NOT NULL,
  `create_time` datetime NOT NULL,
  `modify_time` datetime NOT NULL,
  PRIMARY KEY (`tag_id`),
  UNIQUE KEY `idx1` (`user_id`,`tag_name`) USING BTREE,
  KEY `idx2` (`user_id`,`modify_time`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=104 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of tag
-- ----------------------------
INSERT INTO `tag` VALUES ('1', '0', '41', '0', 'robot_edm', '红名单', '系统默认红名单标签', '0', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `tag` VALUES ('2', '0', '42', '0', 'robot_edm', '黑名单', '系统默认黑名单标签', '0', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `tag` VALUES ('3', '0', '43', '0', 'robot_edm', '拒收', '系统默认拒收标签', '0', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `tag` VALUES ('4', '0', '44', '0', 'robot_edm', '举报', '系统默认举报标签', '0', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `tag` VALUES ('101', '0', '103', '1', 'admin', '默认退订', '系统默认退订标签', '0', '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `tag` VALUES ('103', '0', '104', '1', 'admin', '数据收集', '', '0', '2016-11-23 10:05:25', '2016-11-23 10:05:25');

-- ----------------------------
-- Table structure for task
-- ----------------------------
DROP TABLE IF EXISTS `task`;
CREATE TABLE `task` (
  `task_id` int(11) NOT NULL AUTO_INCREMENT,
  `corp_id` int(11) NOT NULL,
  `user_id` varchar(60) NOT NULL,
  `template_id` int(11) NOT NULL,
  `task_name` varchar(32) NOT NULL,
  `subject` varchar(512) NOT NULL,
  `sender_name` varchar(64) DEFAULT NULL,
  `sender_email` varchar(64) DEFAULT NULL,
  `receiver` varchar(20) DEFAULT NULL,
  `email_count` int(11) NOT NULL,
  `audit_path` varchar(60) DEFAULT NULL,
  `moderate_path` varchar(60) DEFAULT NULL,
  `suggestion` varchar(60) DEFAULT NULL,
  `resource_id` varchar(20) NOT NULL,
  `type` tinyint(1) DEFAULT NULL,
  `job_time` datetime DEFAULT NULL,
  `task_status` tinyint(1) NOT NULL,
  `create_time` datetime NOT NULL,
  `modify_time` datetime NOT NULL,
  `delivery_time` datetime DEFAULT NULL,
  `bind_corp_id` int(11) DEFAULT NULL,
  `label_status` tinyint(1) DEFAULT '0',
  `campaign_id` int(11) DEFAULT NULL,
  `plan_id` int(11) DEFAULT NULL,
  `plan_referer` varchar(30) DEFAULT NULL,
  `priority` tinyint(1) DEFAULT '0',
  PRIMARY KEY (`task_id`),
  UNIQUE KEY `idx2` (`plan_id`,`plan_referer`) USING BTREE,
  KEY `idx1` (`user_id`,`create_time`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of task
-- ----------------------------

-- ----------------------------
-- Table structure for task_cnd
-- ----------------------------
DROP TABLE IF EXISTS `task_cnd`;
CREATE TABLE `task_cnd` (
  `task_id` int(11) NOT NULL,
  `email_list` text,
  `include_ids` varchar(255) DEFAULT NULL,
  `exclude_ids` varchar(255) DEFAULT NULL,
  `file_id` varchar(30) DEFAULT NULL,
  `includes` varchar(512) DEFAULT NULL,
  `excludes` varchar(512) DEFAULT NULL,
  UNIQUE KEY `idx1` (`task_id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of task_cnd
-- ----------------------------

-- ----------------------------
-- Table structure for task_setting
-- ----------------------------
DROP TABLE IF EXISTS `task_setting`;
CREATE TABLE `task_setting` (
  `task_id` int(11) NOT NULL,
  `ad` varchar(20) NOT NULL,
  `send_code` varchar(20) NOT NULL,
  `sms` varchar(20) NOT NULL,
  `replier` varchar(256) DEFAULT NULL,
  `rand` varchar(20) NOT NULL,
  `robot` text,
  `unsubscribe_id` int(11) DEFAULT NULL,
  `label_ids` text,
  `tag_ids` varchar(255) DEFAULT NULL,
  `unsubscribe` varchar(60) DEFAULT NULL,
  `template` varchar(60) DEFAULT NULL,
  `campaign` varchar(20) DEFAULT NULL,
  UNIQUE KEY `idx1` (`task_id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of task_setting
-- ----------------------------

-- ----------------------------
-- Table structure for template
-- ----------------------------
DROP TABLE IF EXISTS `template`;
CREATE TABLE `template` (
  `template_id` int(11) NOT NULL AUTO_INCREMENT,
  `corp_id` int(11) NOT NULL,
  `user_id` varchar(60) NOT NULL,
  `category_id` int(11) NOT NULL,
  `template_name` varchar(60) DEFAULT NULL,
  `file_path` varchar(200) NOT NULL,
  `subject` varchar(100) DEFAULT NULL,
  `sender_name` varchar(20) DEFAULT NULL,
  `sender_email` varchar(64) DEFAULT NULL,
  `reveiver` varchar(20) DEFAULT NULL,
  `robot_email` varchar(64) DEFAULT NULL,
  `is_rand` tinyint(1) DEFAULT NULL,
  `attrs` text,
  `create_time` datetime NOT NULL,
  `modify_time` datetime NOT NULL,
  PRIMARY KEY (`template_id`),
  KEY `idx1` (`user_id`,`modify_time`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=69 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of template
-- ----------------------------
INSERT INTO `template` VALUES ('1', '1', 'admin', '1', '推广', '/WEB-INF/static/template/1/1.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('2', '1', 'admin', '1', '三八节日促销', '/WEB-INF/static/template/1/2.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('3', '1', 'admin', '1', '特惠', '/WEB-INF/static/template/1/3.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('4', '1', 'admin', '2', '电子期刊1', '/WEB-INF/static/template/1/4.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('5', '1', 'admin', '2', '电子期刊2', '/WEB-INF/static/template/1/5.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('6', '1', 'admin', '2', '电子期刊3', '/WEB-INF/static/template/1/6.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('7', '1', 'admin', '3', '常用订单', '/WEB-INF/static/template/1/7.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('8', '1', 'admin', '3', '复杂订单', '/WEB-INF/static/template/1/8.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('9', '1', 'admin', '3', '简单订单', '/WEB-INF/static/template/1/9.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('10', '1', 'admin', '4', '欢迎信', '/WEB-INF/static/template/1/10.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('11', '1', 'admin', '5', '情人节', '/WEB-INF/static/template/1/11.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('12', '1', 'admin', '5', '三八节日祝福', '/WEB-INF/static/template/1/12.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('13', '1', 'admin', '5', '新年快乐1', '/WEB-INF/static/template/1/13.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('14', '1', 'admin', '5', '新年快乐2', '/WEB-INF/static/template/1/14.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('15', '1', 'admin', '5', '元宵节', '/WEB-INF/static/template/1/15.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('16', '1', 'admin', '6', '生日祝福', '/WEB-INF/static/template/1/16.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('17', '1', 'admin', '7', '通知', '/WEB-INF/static/template/1/17.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('18', '1', 'admin', '7', '邀请函', '/WEB-INF/static/template/1/18.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-07-18 15:17:43');
INSERT INTO `template` VALUES ('51', '1', 'admin', '51', '订阅（橙色主题）', '/WEB-INF/static/template/1/51.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('52', '1', 'admin', '51', '订阅（蓝色主题）', '/WEB-INF/static/template/1/52.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('53', '1', 'admin', '51', '订阅（绿色主题）', '/WEB-INF/static/template/1/53.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('54', '1', 'admin', '51', '注册（橙色主题）', '/WEB-INF/static/template/1/54.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('55', '1', 'admin', '51', '注册（蓝色主题）', '/WEB-INF/static/template/1/55.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('56', '1', 'admin', '51', '注册（绿色主题）', '/WEB-INF/static/template/1/56.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('57', '1', 'admin', '52', '订阅成功（橙色主题）', '/WEB-INF/static/template/1/57.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('58', '1', 'admin', '52', '订阅成功（蓝色主题）', '/WEB-INF/static/template/1/58.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('59', '1', 'admin', '52', '订阅成功（绿色主题）', '/WEB-INF/static/template/1/59.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('60', '1', 'admin', '52', '注册成功（橙色主题）', '/WEB-INF/static/template/1/60.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('61', '1', 'admin', '52', '注册成功（蓝色主题）', '/WEB-INF/static/template/1/61.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('62', '1', 'admin', '52', '注册成功（绿色主题）', '/WEB-INF/static/template/1/62.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('63', '1', 'admin', '53', '订阅邮件（橙色主题）', '/WEB-INF/static/template/1/63.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('64', '1', 'admin', '53', '订阅邮件（蓝色主题）', '/WEB-INF/static/template/1/64.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('65', '1', 'admin', '53', '订阅邮件（绿色主题）', '/WEB-INF/static/template/1/65.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('66', '1', 'admin', '53', '注册邮件（橙色主题）', '/WEB-INF/static/template/1/66.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('67', '1', 'admin', '53', '注册邮件（蓝色主题）', '/WEB-INF/static/template/1/67.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');
INSERT INTO `template` VALUES ('68', '1', 'admin', '53', '注册邮件（绿色主题）', '/WEB-INF/static/template/1/68.html', '', '', '', '', '', null, null, '2013-05-10 10:00:00', '2013-05-10 10:00:00');

-- ----------------------------
-- Table structure for template_adapter
-- ----------------------------
DROP TABLE IF EXISTS `template_adapter`;
CREATE TABLE `template_adapter` (
  `template_id` int(11) NOT NULL,
  `join_phone` tinyint(1) NOT NULL,
  `join_sms` tinyint(1) NOT NULL,
  `phone_path` varchar(200) DEFAULT NULL,
  `sms_path` varchar(200) DEFAULT NULL,
  `phone_attrs` text,
  PRIMARY KEY (`template_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of template_adapter
-- ----------------------------

-- ----------------------------
-- Table structure for template_setting
-- ----------------------------
DROP TABLE IF EXISTS `template_setting`;
CREATE TABLE `template_setting` (
  `corp_id` int(11) NOT NULL COMMENT '企业id',
  `user_id` varchar(60) NOT NULL COMMENT '用户id',
  `parent_id` int(11) NOT NULL COMMENT '主模板id',
  `template_id` int(11) NOT NULL COMMENT '触发计划模板id',
  `touch_urls` text NOT NULL COMMENT '触点链接 # 以逗号分隔',
  `after_minutes` int(11) NOT NULL COMMENT '计划*分钟后投递',
  `subject` varchar(200) NOT NULL COMMENT '邮件主题',
  `device_type` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`parent_id`,`template_id`,`device_type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of template_setting
-- ----------------------------

-- ----------------------------
-- Table structure for touch
-- ----------------------------
DROP TABLE IF EXISTS `touch`;
CREATE TABLE `touch` (
  `task_id` int(11) NOT NULL COMMENT '触发计划任务id',
  `corp_id` int(11) NOT NULL COMMENT '企业id',
  `user_id` varchar(60) NOT NULL COMMENT '用户id',
  `template_id` int(11) NOT NULL COMMENT '触发计划模板id',
  `parent_id` int(11) NOT NULL COMMENT '主任务id',
  `template_name` varchar(20) NOT NULL COMMENT '模板名称',
  `subject` varchar(512) NOT NULL COMMENT '邮件主题',
  `sender_name` varchar(64) DEFAULT NULL COMMENT '发件人名称',
  `sender_email` varchar(64) DEFAULT NULL COMMENT '发件人邮件',
  `receiver` varchar(20) DEFAULT NULL COMMENT '收件人',
  `touch_count` int(11) NOT NULL COMMENT '触发总数',
  `create_time` datetime NOT NULL COMMENT '创建时间',
  PRIMARY KEY (`task_id`,`corp_id`,`template_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of touch
-- ----------------------------

-- ----------------------------
-- Table structure for touch_setting
-- ----------------------------
DROP TABLE IF EXISTS `touch_setting`;
CREATE TABLE `touch_setting` (
  `task_id` int(11) NOT NULL,
  `parent_id` int(11) NOT NULL,
  `template_id` int(11) NOT NULL,
  `parent_name` varchar(100) NOT NULL,
  `template_name` varchar(255) NOT NULL,
  `touch_urls` text NOT NULL,
  `subject` varchar(200) NOT NULL,
  `after_minutes` int(11) NOT NULL DEFAULT '0',
  `device_type` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`task_id`,`template_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of touch_setting
-- ----------------------------

-- ----------------------------
-- Table structure for triger
-- ----------------------------
DROP TABLE IF EXISTS `triger`;
CREATE TABLE `triger` (
  `task_id` int(11) NOT NULL AUTO_INCREMENT,
  `corp_id` int(11) NOT NULL,
  `template_id` int(11) NOT NULL DEFAULT '0',
  `task_name` varchar(60) DEFAULT NULL,
  `sender_name` varchar(64) DEFAULT NULL,
  `sender_email` varchar(64) DEFAULT NULL,
  `subject` varchar(512) DEFAULT NULL,
  `email_count` int(11) DEFAULT '0',
  `resource_id` varchar(30) DEFAULT NULL,
  `create_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`task_id`,`corp_id`),
  KEY `idx1` (`corp_id`,`task_name`) USING BTREE,
  KEY `idx2` (`corp_id`,`create_time`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of triger
-- ----------------------------

-- ----------------------------
-- Table structure for triger_api
-- ----------------------------
DROP TABLE IF EXISTS `triger_api`;
CREATE TABLE `triger_api` (
  `triger_corp_id` int(11) NOT NULL AUTO_INCREMENT,
  `corp_id` int(11) NOT NULL,
  `user_id` varchar(60) DEFAULT NULL,
  `triger_name` varchar(20) NOT NULL,
  `status` tinyint(1) DEFAULT '0',
  PRIMARY KEY (`triger_corp_id`),
  UNIQUE KEY `idx1` (`corp_id`,`user_id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of triger_api
-- ----------------------------

-- ----------------------------
-- Table structure for unsubscribe
-- ----------------------------
DROP TABLE IF EXISTS `unsubscribe`;
CREATE TABLE `unsubscribe` (
  `corp_id` int(11) NOT NULL,
  `task_id` int(11) NOT NULL,
  `template_id` int(11) NOT NULL,
  `email` char(64) NOT NULL,
  `update_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`corp_id`,`task_id`,`template_id`,`email`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of unsubscribe
-- ----------------------------

-- ----------------------------
-- Table structure for user
-- ----------------------------
DROP TABLE IF EXISTS `user`;
CREATE TABLE `user` (
  `user_id` varchar(60) NOT NULL,
  `corp_id` int(11) NOT NULL,
  `role_id` int(11) NOT NULL,
  `password` char(32) NOT NULL,
  `email` varchar(64) DEFAULT NULL,
  `true_name` varchar(20) DEFAULT NULL,
  `phone` varchar(20) DEFAULT NULL,
  `job` varchar(20) DEFAULT NULL,
  `rand_code` varchar(32) DEFAULT NULL,
  `status` tinyint(1) NOT NULL,
  `create_time` datetime NOT NULL,
  `expire_time` datetime DEFAULT NULL,
  `moderate` int(11) NOT NULL DEFAULT '0',
  `open_sms` tinyint(1) DEFAULT '0',
  `update_time` datetime DEFAULT NULL,
  `manager_id` varchar(60) DEFAULT NULL,
  `permission_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`user_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of user
-- ----------------------------
INSERT INTO `user` VALUES ('admin', '1', '1', 'e10adc3949ba59abbe56e057f20f883e', null, null, null, null, null, '1', '2014-12-16 15:00:00', null, '0', '0', '2014-12-16 15:00:00', null, null);

-- ----------------------------
-- View structure for view_delivery_task
-- ----------------------------
DROP VIEW IF EXISTS `view_delivery_task`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`%` SQL SECURITY DEFINER VIEW `view_delivery_task` AS select `t`.`task_id` AS `task_id`,`t`.`corp_id` AS `corp_id`,`t`.`user_id` AS `user_id`,`t`.`template_id` AS `template_id`,`t`.`task_name` AS `task_name`,`t`.`subject` AS `subject`,`t`.`sender_name` AS `sender_name`,`t`.`sender_email` AS `sender_email`,`t`.`receiver` AS `receiver`,`t`.`email_count` AS `email_count`,`t`.`audit_path` AS `audit_path`,`t`.`moderate_path` AS `moderate_path`,`t`.`suggestion` AS `suggestion`,`t`.`resource_id` AS `resource_id`,`t`.`type` AS `type`,`t`.`job_time` AS `job_time`,`t`.`task_status` AS `task_status`,`t`.`create_time` AS `create_time`,`t`.`modify_time` AS `modify_time`,`t`.`delivery_time` AS `delivery_time`,`t`.`bind_corp_id` AS `bind_corp_id`,`t`.`label_status` AS `label_status`,`t`.`campaign_id` AS `campaign_id`,`t`.`plan_id` AS `plan_id`,`t`.`plan_referer` AS `plan_referer`,`t`.`priority` AS `priority` from `task` `t` where ((`t`.`plan_referer` = _utf8'None') and (if(isnull(`t`.`plan_id`),0,(select count(`t2`.`task_id`) AS `count(t2.task_id)` from `task` `t2` where ((`t2`.`plan_id` = `t`.`plan_id`) and (`t2`.`plan_referer` <> _utf8'None') and (`t2`.`task_status` in (24,25,26,27))))) > 0)) union select `t3`.`task_id` AS `task_id`,`t3`.`corp_id` AS `corp_id`,`t3`.`user_id` AS `user_id`,`t3`.`template_id` AS `template_id`,`t3`.`task_name` AS `task_name`,`t3`.`subject` AS `subject`,`t3`.`sender_name` AS `sender_name`,`t3`.`sender_email` AS `sender_email`,`t3`.`receiver` AS `receiver`,`t3`.`email_count` AS `email_count`,`t3`.`audit_path` AS `audit_path`,`t3`.`moderate_path` AS `moderate_path`,`t3`.`suggestion` AS `suggestion`,`t3`.`resource_id` AS `resource_id`,`t3`.`type` AS `type`,`t3`.`job_time` AS `job_time`,`t3`.`task_status` AS `task_status`,`t3`.`create_time` AS `create_time`,`t3`.`modify_time` AS `modify_time`,`t3`.`delivery_time` AS `delivery_time`,`t3`.`bind_corp_id` AS `bind_corp_id`,`t3`.`label_status` AS `label_status`,`t3`.`campaign_id` AS `campaign_id`,`t3`.`plan_id` AS `plan_id`,`t3`.`plan_referer` AS `plan_referer`,`t3`.`priority` AS `priority` from (`task` `t3` join `result` `r2` on(((`t3`.`task_id` = `r2`.`task_id`) and (`t3`.`corp_id` = `r2`.`corp_id`)))) where isnull(`t3`.`plan_id`) ;
DROP TRIGGER IF EXISTS `update_task_status`;
DELIMITER ;;
CREATE TRIGGER `update_task_status` AFTER UPDATE ON `result` FOR EACH ROW if new.email_count = new.sent_count then
update task set task_status = 27 where task_id = new.task_id;
end if
;;
DELIMITER ;
