<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>任务模板查看</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">
	<link rel="shortcut icon" href="${ctx}/resources/img/favicon.ico" type="image/x-icon" />    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/trigger/templateDetail.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->任务&nbsp;<span>查看任务模板信息</span></h1>
		
		<form id="DetailForm" method="post">
		<ul class="tab clearfix">
			<li class="now"><a href="javascript:void(0)">任务模板</a></li>
		</ul>	
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="edit_table">
			<tbody>
				<tr>
					<th width="20%">模板类别:</th>
					<td>
						<c:if test="${template.categoryName ne null}">${template.categoryName}</c:if>
						<c:if test="${template.categoryName eq null}">默认类别</c:if>
					</td>
				</tr>
				<tr>
					<th>创建时间:</th>
					<td>
						<fmt:formatDate value="${template.createTime}" pattern="yyyy-MM-dd HH:mm"/>
					</td>
				</tr>
				<tr>
					<th>修改时间:</th>
					<td>
						<fmt:formatDate value="${template.modifyTime}" pattern="yyyy-MM-dd HH:mm"/>
					</td>
				</tr>
				<tr>
					<th>发件人:</th>
					<td>
						${trigger.senderName}
					</td>
				</tr>
				<tr>
					<th>主   题:</th>
					<td>
						${trigger.subject}
					</td>
				</tr>
				<tr>
					<th>发件人邮箱地址:</th>
					<td>
						${trigger.senderEmail}
					</td>
				</tr>
				<%--<tr>
					<th>网络发件人地址:</th>
					<td>
						${robot}
					</td>
				</tr>
				<tr>
					<th>加入随机数:</th>
					<td>
						<c:if test="${rand eq true}">是</c:if>
						<c:if test="${rand ne true}">否</c:if>
					</td>
				</tr>
				--%><%--<tr>
					<th>回复邮箱:</th>
					<td>
						${template.isRand}
					</td>
				</tr>
			--%></tbody>
		</table>
		<br>
		<div align="center">
			${template.content}
		</div>
		<br>
		<table align="center">
			<tr>
				<td>
					<%--<input onmouseover="this.style.cursor='pointer';this.style.cursor='hand';this.style.color='blue'" onmouseout="this.style.color=''" type="button" value="关 闭" onclick="closeWin()">
					--%><input type="button" class="input_no" value="关 闭" title="关闭" onclick="closeWin()">
				</td>
			</tr>
		</table>
		<input type="hidden" name="msg" id="msg" value="${message}">
	</form>
	</div>
	<br/>
	<br/>
  </body>
</html>
