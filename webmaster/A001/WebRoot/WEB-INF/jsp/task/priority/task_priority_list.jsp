<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<title>任务优先级</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
		<meta http-equiv="Content-Language" content="zh-cn" />
		<meta http-equiv="pragma" content="no-cache" />
		<meta http-equiv="cache-control" content="no-cache" />
		<meta http-equiv="expires" content="0" />
		<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
		<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
		<script type="text/javascript" src="${ctx}/resources/script/task/task.priority.js?${version}"></script>
	</head>
	
	<body>
		<div id="main" class="clearfix">
			<h1 class="title">投递优先级&nbsp;<span></span></h1>
			<form id="ListForm" action="${ctx}/task/priority/taskPriorityList" method="GET">
					<ul class="search clearfix m_b10">
					<li>任务名称</li>
					<li>
						<input maxlength="30" type="text" id="search_taskName" name="search_taskName" value="${search_taskName}">
					</li>
					<li>机构名称</li>
					<li>
						<input maxlength="30" type="text" id="search_corpPath" name="search_corpPath" value="${search_corpPath}">
					</li>
					<li>客户账号</li>
					<li>
						<input maxlength="20" type="text" id="search_userId" name="search_userId" value="${search_userId}">
					</li>
					<li>投递通道</li>
					<li style="width:10%">
						<select id="search_resourceId" name="search_resourceId" style="width:100%;background-color: #F5F5F5">
							<option value="">请选择</option>
							<c:forEach items="${resourceList}" var="res">
									<option <c:if test="${search_resourceId==res.resourceId}">selected="selected"</c:if> value="${res.resourceId}">${res.resourceName}
									</option>
							</c:forEach>
						</select>
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
							<th>发送人数</th>
							<th>投递通道</th>
							<th>优先级状态</th>
							<th>创建时间</th>
							<th>操作</th>
						</tr>
						<c:forEach items="${pagination.recordList}" var="task" varStatus="i">
							<tr>
								<c:set var="taskName" value="${task.taskName}"></c:set>
								<c:if test="${task.type == 1}">
									<c:set var="taskName" value="（测试）${task.taskName}"></c:set>
								</c:if>
								<td title="${taskName}"><a data-id='${task.taskId}' class='list_select' href="javascript:void(0)">${taskName}</a></td>
								<td>
									<c:choose>
										<c:when test="${task.type == 4}">API群发</c:when>
										<c:otherwise>单项任务</c:otherwise>
									</c:choose>
								</td>
								<td title="${task.campaignName}">${task.campaignName}</td>
								<td title="${task.corpPath}">${task.corpPath}</td>
								<td>${task.userId}</td>
								<td>${task.emailCount}</td>
								<td>
									<c:forEach items="${resourceList}" var="res">
										<c:if test="${task.resourceId==res.resourceId}">${res.resourceName}</c:if>
									</c:forEach>
								</td>
								<td>
									${priorityMap[task.priority]}
								</td>
								<td>
									<fmt:formatDate value="${task.createTime}" pattern="yyyy-MM-dd HH:mm"/>
								</td>
								<td class="edit">
									<sec:authorize ifAnyGranted="ROLE_ADM">
										<ul class="f_ul">
											<c:choose>
												<c:when test="${task.priority == 0}">
													<li><a href="javascript:switchPriority('${ctx}/task/priority/switchPriority?taskId=${task.taskId}&priority=1', '优先 ${task.taskName}')">优先</a></li>
												</c:when>
												<c:otherwise>
													<li><a href="javascript:switchPriority('${ctx}/task/priority/switchPriority?taskId=${task.taskId}&priority=0', '撤消 ${task.taskName}')">撤消</a></li>
												</c:otherwise>
											</c:choose>
										</ul>
									</sec:authorize>
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
				    receiverName:'${d.receiverName }',
				    unsubscribe:'${d.setting}',
				    sendCode:'${d.way }',
				    campaign:'${d.campaignName}'
			  	}<c:if test='${status.last==false}'>,</c:if>
			  </c:forEach>
			}
		</script>
	</body>
</html>
