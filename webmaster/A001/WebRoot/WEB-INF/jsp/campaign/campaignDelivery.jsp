<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><html>
  <head>
    <title>活动统计-投递历史</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/js/highcharts/highcharts.js"></script>
	<script type="text/javascript" src="${ctx}/resources/script/campaign/campaignDelivery.js?${version}"></script>
 	<script>
 		var cid = 2;
 	</script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="position:relative">
		<h1 class="title">历史&nbsp;&nbsp;&nbsp;<span>针对活动下投递完成的任务提供历史查询</span></h1>
		
		<form id="ListForm" method="post" action="${ctx}/campaign/delivery?campaignId=${campaign.campaignId}">
		<ul class="tab clearfix">
			<li><a href="${ctx}/campaign/overview?campaignId=${campaign.campaignId}">概览</a></li>
			<li><a href="${ctx}/campaign/send?campaignId=${campaign.campaignId}">发送</a></li>
			<li><a href="${ctx}/campaign/open?campaignId=${campaign.campaignId}">打开</a></li>
			<li><a href="${ctx}/campaign/click?campaignId=${campaign.campaignId}">点击</a></li>
			<li><a href="${ctx}/campaign/domain?campaignId=${campaign.campaignId}">域名</a></li>
			<li><a href="${ctx}/campaign/region?campaignId=${campaign.campaignId}">地域</a></li>
			<%-- <li><a href="${ctx}/campaign/client?campaignId=${campaign.campaignId}">终端</a></li> --%>
			<li  class="now"><a href="${ctx}/campaign/delivery?campaignId=${campaign.campaignId}">历史</a></li>
			<li><a href="${ctx}/campaign/compare?campaignId=${campaign.campaignId}">对比</a></li>
			<c:if test="${campaign.touchCount gt 0}">
				<li><a href="${ctx}/campaign/touch?campaignId=${campaign.campaignId}">触发</a></li>
			</c:if>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
			</div>
			<ul class="btn">
				<li>
					<a id="order_list" class="but_select" href="javascript:void(0)" style="width:90px;">按投递时间排序</a>
					<input id="order" type="hidden" value="${orderby}">
				</li>
				<li><a class="btn_icon btn_icon_down" href="javascript:exportCsv('${ctx}/campaign/exportCsv?campaignId=${campaign.campaignId}&flag=delivery');">导出</a></li>
			</ul>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th>活动名称</th>
					<th>活动描述</th>
					<th>投递次数</th>
					<th>更新时间</th>
				</tr>
				<tr>
					<td title="${campaign.campaignName}">${campaign.campaignName}</td>
					<td title="${campaign.campaignDesc}">${campaign.campaignDesc}</td>
					<td>${taskNum}</td>
					<td>${latestTime}</td>
				</tr>
			</tbody>
		</table>
		
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th>任务名称</th>
					<th>发送总数</th>
					<th>成功数</th>
					<th>弹回数</th>
					<th>打开人数</th>
					<th>点击人数</th>
					<th>退订人数</th>
					<th>转发人数</th>
					<th>无效人数</th>
					<th>成功率</th>
					<th>打开率</th>
					<th>点击率</th>
					<th style="width:13%">投递时间</th>
				</tr>
				<c:forEach items="${pagination.recordList}" var="result" varStatus="i">
					<tr>
					<td title="${result.taskName}">
						${result.taskName}
					</td>
					<td>
						<c:if test="${result.sentCount eq null}">0</c:if>
						<c:if test="${result.sentCount ne null}">${result.sentCount}</c:if>
					</td>
					<td>
						<c:if test="${result.reachCount eq null}">0</c:if>
						<c:if test="${result.reachCount ne null}">${result.reachCount}</c:if>
					</td>
					<td>
						<c:if test="${(result.softBounceCount+result.hardBounceCount) eq null}">0</c:if>
						<c:if test="${(result.softBounceCount+result.hardBounceCount) ne null}">${result.softBounceCount+result.hardBounceCount}</c:if>
					</td>
					<td>
						<c:if test="${result.readUserCount eq null}">0</c:if>
						<c:if test="${result.readUserCount ne null}">${result.readUserCount}</c:if>
					</td>
					<td>
						<c:if test="${result.clickUserCount eq null}">0</c:if>
						<c:if test="${result.clickUserCount ne null}">${result.clickUserCount}</c:if>
					</td>
					<td>
						<c:if test="${result.unsubscribeCount eq null}">0</c:if>
						<c:if test="${result.unsubscribeCount ne null}">${result.unsubscribeCount}</c:if>
					</td>
					<td>
						<c:if test="${result.forwardCount eq null}">0</c:if>
						<c:if test="${result.forwardCount ne null}">${result.forwardCount}</c:if>
					</td>
					<td>
						<c:if test="${result.afterNoneCount eq null}">0</c:if>
						<c:if test="${result.afterNoneCount ne null}">${result.afterNoneCount}</c:if>
					</td>
					<td>
						<c:if test="${result.sentCount-result.afterNoneCount==0}">0%</c:if>
						<c:if test="${result.sentCount-result.afterNoneCount!=0}">
							<fmt:formatNumber value="${result.reachCount/(result.sentCount-result.afterNoneCount)*100}" pattern="#.##"/>%
						</c:if>
					</td>
					<td>
						<c:if test="${result.reachCount==0}">0%</c:if>
						<c:if test="${result.reachCount!=0}">
							<fmt:formatNumber value="${result.readUserCount/result.reachCount*100}" pattern="#.##"/>%
						</c:if>
						
					</td>
					<td>
						<c:if test="${result.readUserCount==0}">0%</c:if>
						<c:if test="${result.readUserCount!=0}">
							<fmt:formatNumber value="${result.clickUserCount/result.readUserCount*100}" pattern="#.##"/>%
						</c:if>
					</td>
					
					<td>
						<c:if test="${result.deliveryTime ne null}">
							<fmt:formatDate value="${result.deliveryTime}" pattern="yyyy-MM-dd HH:mm:ss"/>
						</c:if>
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
	</form>
		<input type="hidden" id="listUrl" value="${ctx}/campaign/delivery?campaignId=${campaign.campaignId}">
		<input type="hidden" name="msg" id="msg" value="${message}">
	<br />
	<br />
	<br />
	</div>
	<div class='select_div'></div>
  </body>
</html>
