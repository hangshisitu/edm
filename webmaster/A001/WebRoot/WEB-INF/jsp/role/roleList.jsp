<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>角色管理</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/role/roleList.js?${version}"></script>
  </head>
  <body>
    <div id="main" class="clearfix" style="">
		<h1 class="title">角色管理&nbsp;<span>管理前台的账户角色</span></h1>
		<form id="ListForm" action="${ctx}/role/roleList" method="get">
		<div class="ribbon clearfix">
			<div class="note">
				<p>共<span><font color="blue">${pagination.recordCount}</font></span>个账号角色</p>
			</div>
			<!--  农信银版本需要暂时屏蔽
			<sec:authorize ifAnyGranted="ROLE_ADM">
			<ul class="btn">
				<li>
					<input type="button" class="groovybutton" value="创建角色" title="创建角色" onclick="createAccount('${ctx}/role/createRole')">
				</li>
			</ul>
			</sec:authorize>
			-->
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table role_table">
			<tbody>
				<tr>
					<th>角色名称</th>
					<th>角色描述</th>
					<th>功能范围</th>
					<th>更新时间</th>
					<th>操作</th>
				</tr>
				<c:forEach items="${pagination.recordList}" var="role" varStatus="i">
				<c:if test="${role.id !=4 && role.id !=6}"><!-- 屏蔽企业只读人员和运营人员 -->
				<tr>
					<td>${role.roleName}</td>
					<td title="${role.roleDesc}">${role.roleDesc}</td>
					<td>
						<div class="moreInfo J_hook_more clearfix">
						    <c:forEach items="${role.menuFuncList}" var="menu" varStatus="i">
						       <c:if test="${i.count == 1}">
						          <div class="txt f_l">${menu}</div>
						       </c:if>
						    </c:forEach>
							<c:if test="${fn:length(role.menuFuncList) != 1}">
							<span class="arrow f_l">▼</span>
							</c:if>
							<ul class="none">
							    <c:forEach items="${role.menuFuncList}" var="menu" varStatus="i">
							      <li>${menu}</li>
							    </c:forEach>
							</ul>
						</div>
					</td>
					<td><fmt:formatDate value="${role.modifyTime}" pattern="yyyy-MM-dd HH:mm"/></td>
					<sec:authorize ifAnyGranted="ROLE_ADM">
					<td class="edit">
					    <!-- 初始化的6个角色不可删除 -->
					    <!--  农信银版本需要暂时屏蔽
					    <ul class="f_ul">
						<li><a href="javascript:createRole('${ctx}/role/editRole?roleId=${role.id}')">修改</a></li>
						<c:if test="${role.id > 6}">
						   <li><a href="javascript:delRole('${ctx}/role/delRole?roleId=${role.id}')">删除</a></li>								
						</c:if>
						</ul> 
						-->
						
				    </td>
				    </sec:authorize>					
				 </tr>
				 </c:if>
				</c:forEach>
								
			</tbody>
		</table>
		
			<c:if test="${pagination.recordList==null||fn:length(pagination.recordList)<1}">
				<div class="no_data">没有匹配的账号</div>
			</c:if>
	
		
		<div class="ribbon clearfix">
			<%@ include file="/WEB-INF/jsp/common/page.jsp"%> 
		</div>
		
		<%--当前页--%>
		<input type="hidden" id="currentPage" name="currentPage" value="${pagination.currentPage}">
		<%--总页数--%>
		<input type="hidden" id="totalPageCount" value="${pagination.pageCount}">
		<input type="hidden" name="msg" id="msg" value="${message}">
		<input type="hidden" name="token" value="${token}" />
		<br />
		<br />
		<br />
	</form>
	</div>
  </body>
</html>
