<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>任务审核</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/task/auditPage.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->审核任务&nbsp;<span>当前任务: &nbsp;<font color="blue">${task.taskName }</font></span></h1>
		
		<form id="DetailForm" method="post" action="${ctx}/task/audit">
			
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="edit_table">
			<tbody>
				<tr>
					<th width="20%">审核意见:</th>
					<td>
						<textarea name="suggestion" id="suggestion" rows="4" cols="35" onkeyup="this.value=this.value.slice(0,60)"></textarea>
					</td>
				</tr>
				</tbody>
		</table>
		<br>
		
		<br>
		<table align="center">
			<tr>
				<td>
					<input type="button" id="btn_yes" class="input_yes" value="通 过" title="通过" onclick="audit('yes')">
					<input type="button" id="btn_no" class="input_yes" value="不通过" title="不通过" onclick="audit('no')">
					<input type="button" class="input_no" value="关 闭" title="关闭" onclick="closeWin()">
				</td>
			</tr>
			<tr align="center">
				<td align="center">
					<img style="display:none" id="prompt" src="${ctx}/resources/img/loading3.gif">
				</td>
			</tr>
		</table>
		<input type="hidden" name="taskId" id="taskId" value="${taskId}">
		<input type="hidden" name="flag" id="flag" value="${flag}">
		<input type="hidden" name="token" value="${token}" />
	</form>
	</div>
  </body>
</html>
