<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>未激活账号列表</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
		<script type="text/javascript" src="${ctx}/resources/script/account/accountList.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		<h1 class="title">账号管理&nbsp;<span></span></h1>
		
		<form id="ListForm" action="${ctx}/account/accountList" method="get">
				<ul class="search clearfix m_b10">
				<li>账号</li>
				<li>
					<input maxlength="20" type="text" id="search_userId" name="search_userId" value="${search_userId}" />
				</li>
				<li>公司名称</li>
				<li>
					<input maxlength="50" type="text" id="search_companyName" name="search_companyName" value="${search_companyName}" />
				</li>
				<li>角色</li>
				<li>
					<select id="search_roleId" name="search_roleId" style="width:100%;background-color: #F5F5F5">
						<option value="">--请选择--</option>
						<c:forEach items="${roleList}" var="role" varStatus="i">
						  <option value="${role.id}"
						     <c:if test="${search_roleId == role.id}">selected="selected"</c:if> >${role.roleName}</option>
						</c:forEach>		
					</select>
				</li>
				<li>创建时间</li>
				<li>
					<input maxlength="10" class="date" id="d4311" id="search_beginTimeStr" name="search_beginTimeStr" 
					   value="${search_beginTimeStr}" class="Wdate" type="text" onFocus="WdatePicker({maxDate:'#F{$dp.$D(\'d4312\')}'})"/>
				</li>
				<li>到</li>
				<li>
					 <input maxlength="10"  class="date" id="d4312"  id="search_endTimeStr" name="search_endTimeStr" 
					   value="${search_endTimeStr}" class="Wdate" type="text" onFocus="WdatePicker({minDate:'#F{$dp.$D(\'d4311\')}'})"/>
				</li>
				<li>
					<input type="button" class="groovybutton" value="查 询" title="查询" onclick="search()" />
				</li>
				<li></li>
			</ul>

			<div class="ribbon clearfix">
				<div class="note">
					<p>共<span><font color="blue">${pagination.recordCount}</font></span>个账号</p>
				</div>
				<sec:authorize ifAnyGranted="ROLE_ADM">
				<ul class="btn">
					<li>
						<input type="button" class="groovybutton" value="创建账号" title="创建账号" onclick="createAccount('${ctx}/account/createAccount?flag=activate')">
					</li>
					
				</ul>
				</sec:authorize>
			</div>
			<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
				<tbody>
					<tr>
						<th>账号</th>
						<th>公司名称</th>
						<th>角色</th>
						<th>姓名</th>
						<th>投递通道</th>
						<th>状态</th>
						<th>创建时间</th>
						<th>更新时间</th>
						<th>操作</th>
					</tr>
					<c:forEach items="${pagination.recordList}" var="user" varStatus="i">
					<tr>
						<td title="${user.userId}">${user.userId}</td>						
						<td title="${user.company}">${user.company}</td>
						<td title="${user.roleName}">${user.roleName}</td>
						<td title="${user.trueName}">${user.trueName}</td>
						<td>
						  <c:forEach items="${resourceList}" var="res" varStatus="status">
							  <c:if test="${user.formalId==res.resourceId}">${res.resourceName}</c:if>
						  </c:forEach>
						</td>
						<td>
						   <c:choose>
						     <c:when test="${user.status == 1}">有效</c:when>
						     <c:when test="${user.status == 2}">冻结</c:when>
						     <c:otherwise>无效</c:otherwise>
						   </c:choose>
						</td>
						<td>
							<fmt:formatDate value="${user.createTime}" pattern="yyyy-MM-dd HH:mm"/>
						</td>
						<td>
							<fmt:formatDate value="${user.updateTime}" pattern="yyyy-MM-dd HH:mm"/>
						</td>
						
						<td class="edit">
							<ul class="f_ul">
								<li><a href="javascript:view('${ctx}/account/view?userId=${user.userId}')">查看</a></li>
								
								<sec:authorize ifAnyGranted="ROLE_ADM">
								    <li><a href="javascript:edit('${ctx}/account/edit?accountType=${user.roleId}&userId=${user.userId}','${user.userId}')">修改</a></li>
								    <li><a href="javascript:del('${ctx}/account/delete?userId=${user.userId}','${user.userId}')">删除</a></li> 
								    <c:choose>
								      <c:when test="${user.status == 1}">
								        <li><a href="javascript:freeze('${ctx}/account/freeze?userId=${user.userId}','${user.userId}')">冻结</a></li>
								      </c:when>
								      <c:when test="${user.status == 2}">
								        <li><a href="javascript:activate('${ctx}/account/activate?userId=${user.userId}','${user.userId}')">激活</a></li>
								      </c:when>
								    </c:choose>
									
								</sec:authorize>
							</ul>
						</td>
					</tr>
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
	</form>
	<br />
	<br />
	<br />
	</div>
  </body>
</html>
