<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>周期任务发送统计</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/report/getDateRange.js?${version}"></script>
	<script type="text/javascript" src="${ctx}/resources/script/report/plan/plan.report.list.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="position:relative">
	<h1 class="title">周期任务发送统计&nbsp;<span></span></h1>
		<form id="ListForm" action="${ctx}/plan/planReport/reportList" method="get">
			<ul class="search clearfix m_b00">
				<li>任务名称</li>
				<li>
					<input maxlength="20" type="text" id="search_taskName" name="search_taskName" value="${search_taskName}"/>
				</li>
				<li>机构名称</li>
				<li>
					<input maxlength="20" type="text" id="search_corpPath" name="search_corpPath" value="${search_corpPath}"/>
				</li>
				<li>
					<input type="button" class="groovybutton" value="查 询" title="查询" onclick="search()" />
				</li>
			</ul>
		
			<ul class="tab clearfix">
				<li><a href="${ctx}/taskReport/list?type=0">单项任务</a></li>
				<li><a href="${ctx}/taskReport/list?type=1">测试任务</a></li>
<%-- 				<li><a href="${ctx}/taskReport/apiList">API触发</a></li> --%>
				<li class="now"><a href="${ctx}/plan/planReport/reportList">周期任务</a></li>
			</ul>
			<div class="ribbon clearfix">
				<div class="note">
					<p>共<span><font color="blue">${pagination.recordCount}</font></span>个</p>
				</div>
				<ul class="btn">
					<li>
						<input type="button" class="groovybutton" value="汇 总" title="汇 总" onclick="planCollect('${ctx}/plan/planReport/planCollect')" />
					</li>
				</ul>
			</div>
			<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
				<tbody>
					<tr>
						<th width="25"><input type="checkbox" id="all_checkbox"/></th>
						<th>任务名称</th>
						<th>机构名称</th>
						<th>发送总数</th>
						<th>成功数</th>
						<th>弹回数</th>
						<th>打开人数</th>
						<th>打开次数</th>
						<th>点击人数</th>
						<th>点击次数</th>
						<th>退订人数</th>
						<th>转发人数</th>
						<th>触发计划</th>
						<th>状态</th>
						<th style="width:13%">操作</th>
					</tr>
					<c:forEach items="${pagination.recordList}" var="r" varStatus="i">
						<tr>
							<td><input type="checkbox" name="checkedIds" value="${r.planId}" /></td>
							<td title="${r.taskName}">${r.taskName}</td>
							<td title="${r.corpPath}">${r.corpPath}</td>
							<td>${r.sentCount}</td>
							<td>${r.reachCount}</td>
							<td>${r.softBounceCount + r.hardBounceCount}</td>
							<td>${r.readUserCount}</td>
							<td>${r.readCount}</td>
							<td>${r.clickUserCount}</td>
							<td>${r.clickCount}</td>
							<td>${r.unsubscribeCount}</td>
							<td>${r.forwardCount}</td>
							<td>
								<c:if test="${r.touchCount gt 0 }">有</c:if>
								<c:if test="${r.touchCount eq 0 }">无</c:if>
							</td>
							<td>
								<c:if test="${r.status eq 22 }">待发送</c:if>
								<c:if test="${r.status eq 25 }">已暂停</c:if>
								<c:if test="${r.status eq 26 }">已取消</c:if>
								<c:if test="${r.status eq 27 }">已完成</c:if>								
							</td>
							<td class="edit">
								<ul class="f_ul">
									<li><a href="javascript:report('${ctx}/plan/planDetailStat/planReportView?planId=${r.planId}')">报告</a></li>
									<li><a href="javascript:exportOnePlanStat('${ctx}/plan/planReport/exportPlanStat?planId=${r.planId}')">导出</a></li>
								</ul>
							</td>
						</tr>
					</c:forEach>
						
				</tbody>
			</table>
		
			<c:if test="${pagination.recordList==null||fn:length(pagination.recordList)<1}">
				<div class="no_data">没有匹配的数据</div>
			</c:if>
	
		
			<div class="ribbon clearfix">
				<%@ include file="/WEB-INF/jsp/common/page.jsp"%> 
			</div>
		
			<%--当前页--%>
			<input type="hidden" id="currentPage" name="currentPage" value="${pagination.currentPage}">
			<%--总页数--%>
			<input type="hidden" id="totalPageCount" value="${pagination.pageCount}">
			<input type="hidden" name="type" id="type" value="${type}">
			<input type="hidden" name="fromFlag" id="fromFlag" value="task">
		</form>
		<input type="hidden" name="msg" id="msg" value="${message}">
		<input type="hidden" name="listUrl" id="listUrl" value="${ctx}/plan/planReport/reportList">
		<br />
		<br />
		<br />
	</div>
  </body>
</html>
