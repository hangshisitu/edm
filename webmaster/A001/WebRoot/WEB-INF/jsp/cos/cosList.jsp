<%@ page language="java" pageEncoding="UTF-8"%>
<%@ page contentType="text/html;charset=UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>套餐信息列表</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/cos/cosList.js?${version}"></script>

  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		<h1 class="title">套餐管理&nbsp;<span></span></h1>
		
		<form id="ListForm" action="${ctx}/cos/cosList" method="get">
				<ul class="search clearfix m_b10">
				<li>套餐名称</li>
				<li>
					<input maxlength="20" type="text" id="search_cosName" name="search_cosName" value="${search_cosName}" />
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
					<p>共<span><font color="blue">${pagination.recordCount}</font></span>个套餐</p>
				</div>
				<sec:authorize ifAnyGranted="ROLE_ADM">
				<ul class="btn">
					<li>
						<input type="button" class="groovybutton" value="创建套餐" title="创建套餐" onclick="customCos('/cos/cos')" />
					</li>
					
				</ul>
				</sec:authorize>
			</div>
			<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
				<tbody>
					<tr>
						<th>套餐</th>
						<th>类型</th>
						<th>总发送量</th>
						<th>单日最大发送量</th>
						<th>每周最大发送量</th>
						<th>每月最大发送量</th>
						<th>允许子账户数量</th>
						<th>状态</th>
						<th>创建时间</th>
						<th>操作</th>
					</tr>
					<c:forEach items="${pagination.recordList}" var="cos" varStatus="i">
					<tr>
						<td title="${cos.cosName}">${cos.cosName}</td>	
						<td>
						   <c:choose>
						     <c:when test="${cos.type == 1}">正式</c:when>
						     <c:when test="${cos.type == 0}">测试</c:when>
						     <c:otherwise>无效</c:otherwise>
						   </c:choose>
						</td>					
						<td title="${cos.totalSend}">${cos.totalSend}</td>
						<td title="${cos.daySend}">${cos.daySend}</td>
						<td title="${cos.weekSend}">${cos.weekSend}</td>
						<td title="${cos.monthSend}">${cos.monthSend}</td>
						<td title="${cos.userCount}">${cos.userCount}</td>												
						<td>
						    <c:choose>
						       <c:when test="${cos.status == 1}">有效</c:when>
						       <c:when test="${cos.status == 0}">冻结</c:when>
						       <c:otherwise>无效</c:otherwise>
						    </c:choose>
						</td>
						<td>
							<fmt:formatDate value="${cos.createTime}" pattern="yyyy-MM-dd HH:mm"/>
						</td>
						<td class="edit">
							<ul class="f_ul">
							    <li><a href="javascript:view('${ctx}/cos/view?cosId=${cos.cosId}')">查看</a></li>
								<sec:authorize ifAnyGranted="ROLE_ADM">
								    <%-- <li><a href="javascript:edit('${ctx}/account/edit?accountType=${user.roleId}&userId=${user.userId}','${user.userId}')">修改</a></li> --%>								
                                    <%-- <li><a href="javascript:del('${ctx}/account/delete?userId=${user.userId}','${user.userId}')">删除</a></li> --%>
								    <c:choose >
						            <c:when test="${cos.cosId != 1}">
								    <c:choose>
								      <c:when test="${cos.status == 1}">
								        <li><a href="javascript:freeze('${ctx}/cos/freeze?cosId=${cos.cosId}','${cos.cosId}')">冻结</a></li>
								      </c:when>
								      <c:when test="${cos.status == 0}">
								        <li><a href="javascript:unfreeze('${ctx}/cos/unfreeze?cosId=${cos.cosId}','${cos.cosId}')">激活</a></li>
								      </c:when>
								    </c:choose>
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
		<input type="hidden" id="currentPage" name="currentPage" value="${pagination.currentPage}" />
		<%--总页数--%>
		<input type="hidden" id="totalPageCount" value="${pagination.pageCount}" />
		<input type="hidden" name="msg" id="msg" value="${message}" />
		<input type="hidden" name="token" value="${token}" />
	</form>
	<br />
	<br />
	<br />
	</div>

  </body>
</html>
