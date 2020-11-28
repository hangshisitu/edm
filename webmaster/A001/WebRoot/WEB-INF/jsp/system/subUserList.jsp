<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>子账号列表</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/system/subUserList.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->系统管理&nbsp;<span></span></h1>
		
		<form id="ListForm" action="${ctx}/subUser/subUserList" method="get">
			<%--<ul class="search clearfix m_b00 no_display">--%>
				<ul class="search clearfix m_b10">
				<li>账号</li>
				<li>
					<input maxlength="20" type="text" id="search_userId" name="search_userId" value="${search_userId}" />
				</li>
				<li>姓名</li>
				<li>
					<input maxlength="20" type="text" id="search_trueName" name="search_trueName" value="${search_trueName}" />
				</li>
				<li>
					<input type="button" class="groovybutton" value="查 询" title="查询" onclick="search()" />
				</li>
				<li>
					<%--<input type="button" class="groovybutton" value="重 置" title="重置" onclick="resetParam()">
				--%></li>
			</ul>
			
		<ul class="tab clearfix">
			<li><a href="${ctx}/history/myHistory">登录信息</a></li>
			<li><a href="${ctx}/password/edit">修改密码</a></li>
			<sec:authorize ifAnyGranted="ROLE_ADM">
				<li class="now"><a href="${ctx}/subUser/subUserList">子账号管理</a></li>
				<li><a href="${ctx}/history/subUserHistory">子账号日志</a></li>
			</sec:authorize>
			<li><a href="${ctx}/custom/page">企业定制</a></li>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p>共<span><font color="blue">${pagination.recordCount}</font></span>个子账号</p>
			</div>
			<ul class="btn">
				<li>
					<input type="button" class="groovybutton" value="创建子账号" title="创建子账号" onclick="createSubUser('${ctx}/subUser/add')">
				</li>
				
			</ul>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th>账号</th>
					<th>姓名</th>
					<th>状态</th>
					<th>创建时间</th>
					<th>操作</th>
				</tr>
				<c:forEach items="${pagination.recordList}" var="user" varStatus="i">
				<tr>
					<td title="${user.userId}">${user.userId}</td>
					<td>${user.trueName}</td>
					<td>
						<c:if test="${user.status==1}">正常</c:if>
						<c:if test="${user.status==2}">冻结</c:if>
					</td>
					<td>
						<fmt:formatDate value="${user.createTime}" pattern="yyyy-MM-dd HH:mm"/>
					</td>
					
					<td class="edit">
						<ul class="f_ul">
							<li><a href="javascript:editSubUser('${ctx}/subUser/edit?userId=${user.userId}')">修改</a></li>
							<%--<li><a href="javascript:deleteSubUser('${ctx}/subUser/delete?userId=${user.userId}','${user.userId}')">删除</a></li>--%>
							<c:if test="${user.status==1}"><li><a href="javascript:freeze('${ctx}/subUser/freeze?userId=${user.userId}','${user.userId}')">冻结</a></li></c:if>
							<c:if test="${user.status==2}"><li><a href="javascript:unfreeze('${ctx}/subUser/unfreeze?userId=${user.userId}','${user.userId}')">解冻</a></li></c:if>
						</ul>
					</td>
				</tr>
			</c:forEach>
					
			</tbody>
		</table>
		
			<c:if test="${pagination.recordList==null||fn:length(pagination.recordList)<1}">
				<div class="no_data">没有匹配的子账号</div>
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
