<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>

<title>账号明细</title>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta http-equiv="Content-Language" content="zh-cn" />
<meta http-equiv="Cache-Control" content="no-store" />
<meta http-equiv="Pragma" content="no-cache" />
<meta http-equiv="Expires" content="0" />
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
<script type="text/javascript" src="${ctx}/resources/script/account/accountDetail.js?${version}"></script>

</head>

<body>
	<form id="DetailForm" method="post">
		基本信息：
		<table border="1">
			<tr align="left">
				<th>用户名：</th>
				<td>${user.userId}</td>
			</tr>
			<tr align="left">
				<th>公司名称：</th>
				<td>${corp.company}</td>
			</tr>
			<tr align="left">
				<th>公司地址：</th>
				<td>${corp.address}</td>
			</tr>
			<tr align="left">
				<th>邮编：</th>
				<td>${corp.zip}</td>
			</tr>
			<tr align="left">
				<th>联系人姓名：</th>
				<td>${corp.contact}</td>
			</tr>
			<tr align="left">
				<th>联系人职位：</th>
				<td>${user.job}</td>
			</tr>
			<tr align="left">
				<th>联系电话：</th>
				<td>${corp.telephone}</td>
			</tr>
			<tr align="left">
				<th>行业：</th>
				<td>${corp.industry}</td>
			</tr>
			<tr align="left">
				<th>您通过什么途径找到我们的：</th>
				<td>${corp.way}</td>
			</tr>
			<tr align="left">
				<th>您的邮件地址数量是多少：</th>
				<td></td>
			</tr>
			<tr align="left">
				<th>预计每月发送数量：</th>
				<td></td>
			</tr>
			<tr align="left">
				<th>注册时间：</th>
				<td>
					<fmt:formatDate value="${user.createTime}" pattern="yyyy-MM-dd HH:mm"/>
				</td>
			</tr>
		</table>
		<br>
		<table align="center">
			<tr>
				<td>
					
					<input type="button" id="btn_acticate" value="激 活" onclick="activate('${ctx}/account/activateAsny?userId=${user.userId}','${user.userId}')">
					<input type="button" value="关 闭" onclick="back()">
				</td>
			</tr>
			<tr>
				<td align="center">
					<img style="display:none" id="prompt" src="${ctx}/resources/img/loading3.gif">
				</td>
			</tr>
		</table>
		<input type="hidden" name="token" value="${token}" />
		<input type="hidden" id="successUrl" name="successUrl" value="${ctx}/account/hadBeenActivatedList" />
		<input type="hidden" id="backUrl" name="backUrl" value="${ctx}/account/waitForActivateList" />
		
	</form>
</body>
</html>
