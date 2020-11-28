<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>子账号</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/system/subUserAdd.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->子账号&nbsp;<span>${title}子账号</span></h1>
		
		<form id="DetailForm" method="post">
			
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="edit_table">
			<tbody>
				<tr>
					<th width="20%">账号<font color="red">*</font></th>
					<td>
						<input type="text" id="userId" name="userId" style="width:150px; height: 25px;border: #DCDCDC 1px solid;" maxlength="20">
						<span id="userId_span"></span>
					</td>
					
				</tr>
				<tr>
					<th width="20%">姓名<font color="red">*</font></th>
					<td>
						<input id="trueName" name="trueName" type="text" style="width:150px; height: 25px;border: #DCDCDC 1px solid;" maxlength="20">
						<span id="trueName_span"></span>
					</td>
				</tr>
				<tr>
					<th width="20%">密码<font color="red">*</font></th>
					<td>
						<input id="password" name="password" type="password" style="width:150px; height: 25px;border: #DCDCDC 1px solid;" maxlength="16">
						<span id="password_span"></span>
					</td>
				</tr>
				<tr>
					<th width="20%">确认密码<font color="red">*</font></th>
					<td>
						<input id="confirmPassword" name="confirmPassword" type="password" style="width:150px; height: 25px;border: #DCDCDC 1px solid;" maxlength="16">
						<span id="confirmPassword_span"></span>
					</td>
				</tr>
				<tr>
					<th width="20%">联系电话</th>
					<td>
						<input id="phone" name="phone" type="text" style="width:150px; height: 25px;border: #DCDCDC 1px solid;" maxlength="20">
					</td>
				</tr>
				<tr>
					<th width="20%">状态</th>
					<td>
						<input type="radio" name="status" value="1" checked="checked"/>正常
						<input type="radio" name="status" value="2"/>冻结
					</td>
				</tr>
				</tbody>
		</table>
		<br>
		
		<br>
		<table align="center">
			<tr>
				<td>
					<input id="btn_save" type="button" class="input_yes" value="保 存" title="保存" onclick="saveSubUser('${ctx}/subUser/save')">
					<input type="button" class="input_no" value="关 闭" title="关闭" onclick="closeWin()">
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
