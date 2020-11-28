<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>修改密码</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/system/passwordEdit.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		
		<form id="DetailForm" method="post">
		<h3 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->系统管理&nbsp;<span></span></h3>
		<ul class="tab clearfix">
			<li><a href="${ctx}/history/myHistory">登录信息</a></li>
			<li  class="now"><a href="${ctx}/password/edit">修改密码</a></li>
			<sec:authorize ifAnyGranted="ROLE_ADM">
				<li><a href="${ctx}/subUser/subUserList">子账号管理</a></li>
				<li><a href="${ctx}/history/subUserHistory">子账号日志</a></li>
			</sec:authorize>
			<li><a href="${ctx}/custom/page">企业定制</a></li>
		</ul>
		<br/>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="edit_table">
			<tbody>
				<tr>
					<th width="20%">原密码<font color="red">*</font></th>
					<td>
						<input type="password" id="oldPassword" name="oldPassword" style="width:220px; height: 25px;border: #DCDCDC 1px solid;ime-mode:disabled;" maxlength="16">
						<span id="oldPassword_span"></span>
					</td>
					
				</tr>
				<tr>
					<th width="20%">新密码<font color="red">*</font></th>
					<td>
						<input id="newPassword" name="newPassword" type="password" style="width:220px; height: 25px;border: #DCDCDC 1px solid;ime-mode:disabled;" maxlength="16">
						<span id="newPassword_span"></span>
					</td>
				</tr>
				<tr>
					<th width="20%">确认新密码<font color="red">*</font></th>
					<td>
						<input id="confirmNewPassword" name="confirmNewPassword" type="password" style="width:220px; height: 25px;border: #DCDCDC 1px solid;ime-mode:disabled;" maxlength="16">
						<span id="confirmNewPassword_span"></span>
					</td>
				</tr>
				</tbody>
		</table>
		
		<br>
		
		<br>
		<table align="left">
			<tr align="right">
				<td align="right">
					<input type="button" class="input_yes" value="保 存" title="保存" onclick="savePassword('${ctx}/password/save')">
					<input type="button" class="input_no" value="返 回" title="返回" onclick="cancle('${ctx}/history/myHistory')">
				</td>
			</tr>
			<tr>
				<td align="center">
					<img style="display:none" id="prompt" src="${ctx}/resources/img/loading3.gif">
				</td>
			</tr>
		</table>
		<input type="hidden" name="msg" id="msg" value="${message}">
		<input type="hidden" name="token" value="${token}" />
	</form>
	</div>
  </body>
</html>
