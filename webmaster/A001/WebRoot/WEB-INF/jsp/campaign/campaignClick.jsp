<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>活动统计-点击</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/js/highcharts/highcharts.js"></script>
	<script type="text/javascript" src="${ctx}/resources/script/campaign/campaign.stat.click.js?${version}"></script>
 	<script type="text/javascript">
 	
 	var titles = [
 	        <c:forEach items="${fiveList}" var="u" varStatus="status">   
           	 	'${u.url}'<c:if test='${status.last==false}'>,</c:if>
     		 </c:forEach>       
 	];
 	
	var datas = [
	           <c:forEach items="${fiveList}" var="u" varStatus="status">   
	           	 	${u.clickCount}<c:if test='${status.last==false}'>,</c:if>
	     	    </c:forEach> 
	];
	
	var names = [];
 	</script>
 
  </head>
  
  <body>
    <div id="main" class="clearfix" style="position:relative">
		<h1 class="title">点击&nbsp;&nbsp;&nbsp;<span>针对邮件点击的时段和链接提供图表分析</span></h1>
		
		
		<form id="ListForm" method="post">
		<ul class="tab clearfix">
			<li><a href="${ctx}/campaign/overview?campaignId=${campaign.campaignId}">概览</a></li>
			<li><a href="${ctx}/campaign/send?campaignId=${campaign.campaignId}">发送</a></li>
			<li><a href="${ctx}/campaign/open?campaignId=${campaign.campaignId}">打开</a></li>
			<li class="now"><a href="${ctx}/campaign/click?campaignId=${campaign.campaignId}">点击</a></li>
			<li><a href="${ctx}/campaign/domain?campaignId=${campaign.campaignId}">域名</a></li>
			<li><a href="${ctx}/campaign/region?campaignId=${campaign.campaignId}">地域</a></li>
			<%-- <li><a href="${ctx}/campaign/client?campaignId=${campaign.campaignId}">终端</a></li> --%>
			<li><a href="${ctx}/campaign/delivery?campaignId=${campaign.campaignId}">历史</a></li>
			<li><a href="${ctx}/campaign/compare?campaignId=${campaign.campaignId}">对比</a></li>
			<c:if test="${campaign.touchCount gt 0}">
				<li><a href="${ctx}/campaign/touch?campaignId=${campaign.campaignId}">触发</a></li>
			</c:if>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
			</div>
			<ul class="btn">
				<li><a class="btn_icon btn_icon_down" href="javascript:exportCsv('${ctx}/campaign/exportCsv?campaignId=${campaign.campaignId}&flag=click');">导出</a></li>
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
		<c:if test="${fn:length(urlList)>0}">
		<div class="charts">
			<div id="stat_column" style="width: 840px; height: 300px; margin: 0 auto;"></div>
		</div>
		</c:if>
		<c:if test="${fn:length(urlList)<1}">
			<div class="no_data">暂无数据图显示</div>
		</c:if>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr id="logger_0">
					<th>链接地址</th>
					<th>点击次数</th>
					<th>占比</th>
				</tr>
				<c:forEach items="${urlList}" var="u" varStatus="i">
				<tr>
					<td  class="edit"><a href="${u.url}" target="_blank">${u.url}</a></td>
					<td>${u.clickCount}</td>
					<td>
						<c:if test="${totalClick==0}">0.00%</c:if>
						<c:if test="${totalClick!=0}">
							<fmt:formatNumber value="${u.clickCount/totalClick*100}" pattern="#.##"/>%
						</c:if>
					</td>
				</tr>
			</c:forEach>
			</tbody>
		</table>
		<br />
		<br />
	</form>
		<input type="hidden" name="msg" id="msg" value="${message}">
		<br />
	<br />
	<br />
	</div>
  </body>
</html>
