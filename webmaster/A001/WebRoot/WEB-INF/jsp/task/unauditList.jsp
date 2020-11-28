<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>待审核任务列表</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/task/unauditList.js?${version}"></script>
  </head>
  <body>
    <div id="main" class="clearfix" style="">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->任务管理&nbsp;<span></span></h1>
		
		<form id="ListForm" action="${ctx}/task/unauditList" method="get">
			<%--<ul class="search clearfix m_b10 no_display">--%>
				<ul class="search clearfix m_b10">
				<li>任务名称</li>
				<li>
					<input maxlength="30" type="text" id="search_taskName" name="search_taskName" value="${search_taskName}">
				</li>
				<li>机构名称</li>
				<li>
					<input maxlength="20" type="text" id="search_corpPath" name="search_corpPath" value="${search_corpPath}">
				</li>
				<li>客户账号</li>
				<li>
					<input maxlength="20" type="text" id="search_userId" name="search_userId" value="${search_userId}">
				</li>
				<li>创建时间</li>
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
				
			</ul>
			
		
		<ul class="tab clearfix">
			<li class="now"><a href="${ctx}/task/unauditList">待审核任务</a></li>
			<li><a href="${ctx}/task/auditedList">已审核任务</a></li>
			<li><a href="${ctx}/task/needlessAuditList">不需要审核任务</a></li>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p>共<span><font color="blue">${pagination.recordCount}</font></span>个任务</p>
			</div>
			<ul class="btn">
			</ul>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th>任务名称</th>
					<th>任务类型</th>
					<th>所属活动</th>
					<th>机构名称</th>
					<th>客户账号</th>
					<th>接收人数</th>
					<th>投递通道</th>
					<th>触发计划</th>
					<th>创建时间</th>
					<th>操作</th>
				</tr>
				<c:forEach items="${pagination.recordList}" var="task" varStatus="i">
					<tr>
						<td title="${task.taskName}"><a data-id='${task.taskId}' class='list_select' href="javascript:void(0)">${task.taskName}</a></td>
						<td>
							<c:choose>
								<c:when test="${task.type == 2}">周期任务</c:when>
								<c:when test="${task.type == 4}">API任务</c:when>
								<c:otherwise>单项任务</c:otherwise>
							</c:choose>
						</td>
						<td title="${task.campaignName}">${task.campaignName}</td>
						<td title="${task.companyName}">${task.companyName}</td>
						<td>${task.userId}</td>
						<td>${task.emailCount}</td>
						<td>
							<c:forEach items="${resourceList}" var="res">
								<c:if test="${task.resourceId==res.resourceId}">${res.resourceName}</c:if>
							</c:forEach>
						</td>
						<td>
							<c:if test="${task.touchCount gt 0 }">有</c:if>
							<c:if test="${task.touchCount eq 0 }">无</c:if>
						</td>
						<td>
							<fmt:formatDate value="${task.createTime}" pattern="yyyy-MM-dd HH:mm"/>
						</td>
						
						<td class="edit">
							<ul class="f_ul">
								<li><a href="javascript:view('${ctx}/task/view?taskId=${task.taskId}')">查看</a></li>
								<sec:authorize ifAnyGranted="ROLE_ADM">
								<li><a href="javascript:auditPage('${ctx}/task/auditPage?taskId=${task.taskId}&flag=yes')">审核</a></li>
								</sec:authorize>
								<%--<li><a href="javascript:auditPage('${ctx}/task/auditPage?taskId=${task.taskId}&flag=no')">不通过</a></li>
							--%></ul>
						</td>
					</tr>
			</c:forEach>
					
			</tbody>
		</table>
		
			<c:if test="${pagination.recordList==null||fn:length(pagination.recordList)<1}">
				<div class="no_data">没有匹配的任务</div>
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
	<br>
	<br>
	<br>
	</div>
<script type="text/javascript">
	var taskData  = {
		<c:forEach items="${details}" var="d" varStatus="status">
		  	${d.taskId}:{
		  	task:'${d.taskName}',
		  	template:'${d.templateName}', 
		    subject:'${d.subject}',
		    senderEmail:'${d.senderEmail}',
		    senderName:'${d.senderName}',
		    robot:'${d.robot}',
		    replier:'${d.replier}',
		    includes:'${d.tag}',
		    excludes:'${d.exculde}',
		    receiverName:'${d.receiverName}',
		    unsubscribe:'${d.setting}',
		    sendCode:'${d.way}',
		    campaign:'${d.campaignName}'
	  	}<c:if test='${status.last==false}'>,</c:if>
	  </c:forEach>
	}
</script>
  </body>
</html>
