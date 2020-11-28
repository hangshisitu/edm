<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>登录信息</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/system/history.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->系统管理&nbsp;<span></span></h1>
		
		<form id="ListForm" action="${ctx}/history/myHistory" method="get">
			<%--<ul class="search clearfix m_b10 no_display">--%>
				<ul class="search clearfix m_b10">
				
				<li>登录时间</li>
				<li>
					<input maxlength="10" class="date" id="d4311" id="search_beginTimeStr" name="search_beginTimeStr" value="${search_beginTimeStr}" class="Wdate" type="text" onFocus="WdatePicker({maxDate:'#F{$dp.$D(\'d4312\')}'})"/>
				</li>
				<li>到</li>
				<li>
					 <input maxlength="10"  class="date" id="d4312"  id="search_endTimeStr" name="search_endTimeStr" value="${search_endTimeStr}" class="Wdate" type="text" onFocus="WdatePicker({minDate:'#F{$dp.$D(\'d4311\')}'})"/>
				</li>
				<li>
					<input type="button" class="groovybutton" value="查 询" title="查询" onclick="search()">
				</li>
				<li><%--
					<input type="button" class="groovybutton" value="重 置" title="重置" onclick="resetParam()">
				--%></li>
			</ul>
			
		<ul class="tab clearfix">
			<li  class="now"><a href="${ctx}/history/myHistory">登录信息</a></li>
			<li><a href="${ctx}/password/edit">修改密码</a></li>
			<sec:authorize ifAnyGranted="ROLE_ADM">
				<li><a href="${ctx}/subUser/subUserList">子账号管理</a></li>
				<li><a href="${ctx}/history/subUserHistory">子账号日志</a></li>
			</sec:authorize>
			<li><a href="${ctx}/custom/page">企业定制</a></li>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p>共<span><font color="blue">${pagination.recordCount}</font></span>个登录信息</p>
			</div>
			<ul class="btn">
			</ul>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th>账号</th>
					<th>登录IP</th>
					<th>登录城市</th>
					<th>登录时间</th>
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
				<div class="no_data">没有匹配的日志</div>
			</c:if>
	
		
		<div class="ribbon clearfix">
			<%@ include file="/WEB-INF/jsp/common/page.jsp"%> 
		</div>
		
		<%--当前页--%>
		<input type="hidden" id="currentPage" name="currentPage" value="${pagination.currentPage}">
		<%--总页数--%>
		<input type="hidden" id="totalPageCount" value="${pagination.pageCount}">
		<input type="hidden" name="msg" id="msg" value="${message}">
	</form>
	<br />
	<br />
	<br />
	</div>
  </body>
</html>
