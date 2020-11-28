<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
	    <title>活动统计-触发</title>
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
					<li><a href="${ctx}/campaign/overview?campaignId=${campaign.campaignId}">概览</a></li>
					<li><a href="${ctx}/campaign/send?campaignId=${campaign.campaignId}">发送</a></li>
					<li><a href="${ctx}/campaign/open?campaignId=${campaign.campaignId}">打开</a></li>
					<li><a href="${ctx}/campaign/click?campaignId=${campaign.campaignId}">点击</a></li>
					<li><a href="${ctx}/campaign/domain?campaignId=${campaign.campaignId}">域名</a></li>
					<li><a href="${ctx}/campaign/region?campaignId=${campaign.campaignId}">地域</a></li>
					<%-- <li><a href="${ctx}/campaign/client?campaignId=${campaign.campaignId}">终端</a></li> --%>
					<li><a href="${ctx}/campaign/delivery?campaignId=${campaign.campaignId}">历史</a></li>
					<li><a href="${ctx}/campaign/compare?campaignId=${campaign.campaignId}">对比</a></li>
					<li class="now"><a href="${ctx}/campaign/touch?campaignId=${campaign.campaignId}">触发</a></li>
				</ul>
				<div class="ribbon clearfix">
					<ul class="btn">
						<li><a class="btn_icon btn_icon_down" href="javaScript:exportTouch('${ctx}/campaign/exportCsv?campaignId=${campaign.campaignId}&flag=touch&templateId=110');">导出报告</a></li>
					</ul>
				</div>
				<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
					<tbody>
						<tr class="">
							<th>活动名称</th>
							<th width="28%">活动描述</th>
							<th width="12%">投递次数</th>
							<th>更新时间</th>
						</tr>
						<tr class="">
							<th title="${campaign.campaignName}" style="border: 0;">${campaign.campaignName}</th>
							<th title="${campaign.campaignDesc}" style="border: 0;">${campaign.campaignDesc}</th>
							<th style="border: 0;">${taskNum}</th>
							<th style="border: 0;">
								${latestTime}
							</th>
						</tr>
					</tbody>
				</table>
				<c:if test="${domainMap==null || fn:length(domainMap)==0}">
					<div class="no_data">没有匹配的数据！</div>
				</c:if>
				<c:if test="${fn:length(domainMap) > 0}">
					<div class="trig_title">当前活动共 ${taskNum} 个投递任务，${fn:length(domainList)} 个关联的触发计划：</div>
						<c:forEach items="${domainMap}" var="map">
							<div class="trig_links">
								<c:forEach items="${taskIds}" var="taskId">
									<c:set var="endStr" value="_${taskId}"></c:set>
									<c:set var="tn" value="${map.key}"></c:set>
									<c:if test="${fn:contains(tn, endStr)}">
										<c:set var="taskName" value="${fn:substringBefore(tn, endStr)}"></c:set>
									</c:if>
								</c:forEach>
								<p>任务来源：${taskName}</p>
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
									<c:forEach items="${map.value}" var="domain">
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
									</c:forEach>
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