<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>备案信息</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/record/sina.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->备案管理&nbsp;<span></span></h1>
		
	<form id="ListForm" action="${ctx}/history/myHistory" method="get">
		<ul class="tab clearfix">
			<li  class="now"><a href="${ctx}/sina/list">企业信誉管理</a></li>
			<li><a href="${ctx}/sina/manage">企业信息管理</a></li>
			<li><a href="${ctx}/sina/rule">信誉等级规则</a></li>
		</ul>
	    <div class="ribbon clearfix">
			<div class="note">
				<p>企业账号:<span><font color="blue">${pagination.recordCount}</font></span>个登录信息</p>
			</div>
			<ul class="btn">
			</ul>
		</div>
		
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th>账号</th>
					<th>信誉等级</th>					
					<th>企业ID</th>
					<th>企业名称</th>
					<th>注册时间</th>
					<th>15分钟配额</th>
					<th>每日配额</th>
					<th>每月配额</th>
				</tr>
				<c:forEach items="${pagination.recordList}" var="his" varStatus="i">
				<tr>
					<td>${his.userId}</td>
					<td>${his.loginIp}</td>
					<td>${his.region}</td>
					<td>
						<fmt:formatDate value="${his.loginTime}" pattern="yyyy-MM-dd HH:mm:ss"/>
					</td>
					
				</tr>
			</c:forEach>
					
			</tbody>
		</table>
		
		<c:if test="${pagination.recordList==null||fn:length(pagination.recordList)<1}">
			<div class="no_data">没有相关的信誉信息</div>
		</c:if>
			
	</form>
	<br />
	<br />
	<br />
	</div>
  </body>
</html>
