<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
	    <title>任务统计-触发</title>
	    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	    <meta http-equiv="Content-Language" content="zh-cn" />
		<meta http-equiv="pragma" content="no-cache">
		<meta http-equiv="cache-control" content="no-cache">
		<meta http-equiv="expires" content="0">    
		
		<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
		<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
		<script type="text/javascript" src="${ctx}/resources/script/report/touch/report.stat.touch.js?${version}"></script>

	</head>
	
	<body>
	<div id="wrapper" class="clearfix">
		<div id="main" class="clearfix posfix">
		  <div class="posfix">
			<h1 class="title">触发&nbsp;&nbsp;<span>提供与任务关联的触发计划的数据报告</span></h1>
			<form id="ListForm" method="get">
				<ul class="tab clearfix">
					<li><a href="${ctx}/plan/planDetailStat/planReportView?planId=${task.planId}">概览</a></li>
					<li><a href="${ctx}/plan/planDetailStat/deliveryHistory?planId=${task.planId}">投递历史</a></li>
					<li class="now"><a href="${ctx}/plan/planDetailStat/planReportTouch?planId=${task.planId}&templateId=${task.templateId}">触发</a></li>
				</ul>
				<div class="ribbon clearfix">
					<div class="note">
						<p>任务名称：<span>${task.taskName}</span></p>
					</div>
					<ul class="btn">
						<li><a class="btn_icon btn_icon_down" href="javaScript:exportTouch('${ctx}/plan/planDetailStat/exportPlanTouch?planId=${task.planId}&templateId=${task.templateId}');">导出报告</a></li>
					</ul>
				</div>
				<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
					<tbody>
						<tr class="">
							<th>邮件主题</th>
							<th width="12%">周期时间</th>
							<th width="12%">投递次数</th>
							<th width="28%">有效期</th>
						</tr>
						<tr class="">
							<th title="${task.subject}" style="border: 0;">${task.subject}</th>
							<th style="border: 0;">
								${plan.translateCron}
							</th>
							<th style="border: 0;">
								${plan.taskCount}
							</th>
							<th style="border: 0;">
								<c:if test="${plan.beginTime ne null}">
									<fmt:formatDate value="${plan.beginTime}" pattern="yyyy-MM-dd HH:mm"/>
								</c:if>至
								<c:if test="${plan.endTime ne null}"><fmt:formatDate value="${plan.endTime}" pattern="yyyy-MM-dd HH:mm"/></c:if>
								<c:if test="${plan.endTime eq null ||plan.endTime == ''}">现在</c:if>
							</th>
						</tr>
					</tbody>
				</table>
				<c:if test="${fn:length(domainList) == 0}">
					<div class="no_data">没有匹配的数据！</div>
				</c:if>
				<c:if test="${fn:length(domainList) > 0}">
					<div class="trig_title">当前任务共${fn:length(domainList)}个关联的触发计划：</div>
						<c:forEach items="${domainList}" var="domain">
							<div class="trig_links">
								<p>触点链接：</p>
								<ul>
									<c:forEach items="${domain.urlList}" var="touchUrl">
										<li><a href="${touchUrl}" target="blank">${touchUrl}</a></li>
									</c:forEach>
								</ul>
								<span class="highlight" style="display:none">展开所有链接>></span>
							</div>
							<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
								<tbody>
									<tr>
										<th>模板名称</th>
										<th>触发总数</th>
										<th>成功数</th>
										<th>打开人数</th>
										<th>打开次数</th>
										<th>点击人数</th>
										<th>点击次数</th>
										<th>退订人数</th>
										<th>转发人数</th>
										<th>触发率</th>
										<th>成功率</th>
										<th>打开率</th>
										<th>点击率</th>
										<th width="14%">操作</th>
									</tr>
									<tr>
										<td title="${domain.templateName}">${domain.templateName}</td>
										<td>${domain.touchCount}</td>
										<td>${domain.reachCount}</td>
										<td>${domain.readUserCount}</td>
										<td>${domain.readCount}</td>
										<td>${domain.clickUserCount}</td>
										<td>${domain.clickCount}</td>
										<td>${domain.unsubscribeCount}</td>
										<td>${domain.forwardCount}</td>
										<td>${domain.touchRate}%</td>
										<td>${domain.resultReach}%</td>
										<td>${domain.resultRead}%</td>
										<td>${domain.resultClick}%</td>
										<td class="edit">
											<ul class="f_ul">
												<li><a href="javaScript:view('${ctx}/task/view?corpId=${domain.corpId}&touchId=${domain.touchId}&templateId=${domain.templateId}&action=touch')">预览</a></li>
												<li><a href="javaScript:exportRecipients('${ctx}/taskDetailStat/exportRecipients?taskId=${domain.touchId}&templateId=${domain.templateId}')">导出</a></li>
											</ul>
										</td>
									</tr>
								</tbody>
							</table>
						</c:forEach>
					</div>
				</c:if>
			</form>
		</div>
	</div>
	<div class="select_div"></div>
	</body>
</html>