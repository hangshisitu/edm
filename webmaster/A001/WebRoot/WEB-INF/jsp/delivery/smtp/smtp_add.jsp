<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<title>SMTP	设置</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
		<meta http-equiv="Content-Language" content="zh-cn" />
		<meta http-equiv="pragma" content="no-cache" />
		<meta http-equiv="cache-control" content="no-cache" />
		<meta http-equiv="expires" content="0" />
		<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
		<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
		<script type="text/javascript" src="${ctx}/resources/script/common/jquery.form-min.js?${version}"></script>
		<script type="text/javascript" src="${ctx}/resources/script/delivery/delivery.smtp.js?${version}"></script>	
	</head>
	
	<body>
	<div id="main" class="clearfix">
		<h1 class="title">投递配置&nbsp;<span>新增配置信息</span></h1>
		<form id="addIP" name="addIP" method="post"/>
			<input type="hidden" name="token" value="${token}" />
			<input type="hidden" id="id" name="id" value="${smtp.id}" />
			<input type="hidden" id="createTime" name="createTime" value="${smtp.createTime}" />
			<div class="sys_blueline">
				<ul class="smtp_wrapper clearfix">
					<li><strong>IP</strong></li>
					<li>
						<textarea name="ip" id="ip" maxlength="159" style="width:324px;"/>${smtp.ip}</textarea>
						&nbsp;<span class="red">*</span>
						&nbsp;<span class="smtp_tips">(多个ip请用英文分号隔开，每次提交不能超过10个)</span>
					</li>
					<li><strong>频率</strong></li>
					<li>
						<div class="rate_desc">
							<span class="rate_title">15分钟</span>
							<input type="text" name="quarter" id="quarter" class="rate_sum" value="${smtp.quarter}" maxlength="8"/>
							&nbsp;<span class="red">*</span>
							&nbsp;<span class="smtp_tips">(单位：封)</span>
						</div>
					</li>
					<li>
						<div class="rate_desc">
							<span class="rate_title">1小时</span>
							<input type="text" name="hour" id="hour" class="rate_sum" value="${smtp.hour}" maxlength="8"/>
							&nbsp;<span class="red">*</span>
							&nbsp;<span class="smtp_tips">(单位：封)</span>
						</div>
					</li>
					<li>
						<div class="rate_desc">
							<span class="rate_title">1天</span>
							<input type="text" name="day" id="day" class="rate_sum" value="${smtp.day}" maxlength="8"/>
							&nbsp;<span class="red">*</span>
							&nbsp;<span class="smtp_tips">(单位：封)</span>
						</div>
					</li>
					<li><strong>邮件大小</strong></li>
					<li>
						<input type="text" name="size" id="size" value="${smtp.size}" maxlength="8"/>
						&nbsp;<span class="red">*</span>
						&nbsp;<span class="smtp_tips">(单位：KB)</span>
					</li>
				</ul>
			</div>
			<table align="center">
				<tbody><tr>
					<td>
						<input type="submit"  value="提交" class="input_yes" id="btn_submit">
						<input type="button" onclick="location.href='/delivery/smtp/smtpList'"  value="返 回" class="input_no">
					</td>
				</tr>
				<tr>
					<td align="center">
						<img src="/resources/img/loading3.gif" id="prompt" style="display:none">
					</td>
				</tr>
			</tbody></table>
			<input type="hidden" name="id" value=""/>
		</form>
	</div>
	</body>
</html>
