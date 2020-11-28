<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>任务统计-地域</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/js/highcharts/highcharts.js"></script>
	<script type="text/javascript" src="${ctx}/resources/script/report/report.stat.region.js?${version}"></script>
 	<script type="text/javascript">
 	
 	var nameDatas = [
	             <c:forEach items="${provinceRegionList}" var="pr" varStatus="status">   
	            	 '${pr.provinceName}'<c:if test='${status.last==false}'>,</c:if>
	      		 </c:forEach>   
	];
 	
	var readUserDatas = [
		{
		   	name: "打开人数",
		   	data: [
				   	<c:forEach items="${provinceRegionList}" var="pr" varStatus="status">   
		           	  ${pr.readUserCount}<c:if test='${status.last==false}'>,</c:if>
		     		</c:forEach>   
		   	] 
		},
		{
		   	name: "点击人数",
		   	data: [
		   		<c:forEach items="${provinceRegionList}" var="pr" varStatus="status">   
	           	  ${pr.clickUserCount}<c:if test='${status.last==false}'>,</c:if>
	     		</c:forEach>      
		   	]
		}
	];

 	</script>
 
  </head>
  
  <body>
    <div id="main" class="clearfix" style="position:relative">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->地域</h1>
		
		<form id="ListForm" method="post">
		<ul class="tab clearfix">
			<li><a href="${ctx}/taskDetailStat/overview?taskId=${result.taskId}&taskType=${taskType}">概览</a></li>
			<li><a href="${ctx}/taskDetailStat/send?taskId=${result.taskId}&taskType=${taskType}">发送</a></li>
			<li><a href="${ctx}/taskDetailStat/open?taskId=${result.taskId}&taskType=${taskType}">打开</a></li>
			<li><a href="${ctx}/taskDetailStat/click?taskId=${result.taskId}&taskType=${taskType}">点击</a></li>
			<li><a href="${ctx}/taskDetailStat/domain?taskId=${result.taskId}&taskType=${taskType}">域名</a></li>
			<li class="now"><a href="${ctx}/taskDetailStat/region?taskId=${result.taskId}&taskType=${taskType}">地域</a></li>
			<c:if test="${touchs!=null && fn:length(touchs) > 0}">
				<li><a href="${ctx}/taskDetailStat/touch?taskId=${result.taskId}&templateId=${result.templateId}&taskType=${taskType}">触发</a></li>
			</c:if>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p>当前任务：<font color="blue">${task.taskName}</font></p>
			</div>
			<ul class="btn">
				<li><a class="btn_icon btn_icon_down" href="javascript:exportRegion('${ctx}/taskDetailStat/exportRegion?taskId=${task.taskId}&taskType=${taskType}');">导出</a></li>
			</ul>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th>邮件主题</th>
					<th>当前状态</th>
					<th>投递时间</th>
				</tr>
				<tr>
					<td>
						${result.subject}
					</td>
					<td>
						<c:if test="${result.status eq 0 }">草稿</c:if>
						<c:if test="${result.status eq 3 }">已删除</c:if>
						<c:if test="${result.status eq 11 }">待批示</c:if>
						<c:if test="${result.status eq 12 }">批示不通过</c:if>
						<c:if test="${result.status eq 13 }">待审核</c:if>
						<c:if test="${result.status eq 14 }">审核不通过</c:if>
						<c:if test="${result.status eq 21 }">测试发送</c:if>
						<c:if test="${result.status eq 22 }">待发送</c:if>
						<c:if test="${result.status eq 23 }">队列中</c:if>
						<c:if test="${result.status eq 24 }">发送中</c:if>
						<c:if test="${result.status eq 25 }">已暂停</c:if>
						<c:if test="${result.status eq 26 }">已取消</c:if>
						<c:if test="${result.status eq 27 }">已完成</c:if>
					</td>
					<td>
						<c:if test="${result.deliveryTime ne null}">
							<fmt:formatDate value="${result.deliveryTime}" pattern="yyyy-MM-dd HH:mm"/>
						</c:if>至
						<c:if test="${result.endSendTime ne null}"><fmt:formatDate value="${result.endSendTime}" pattern="yyyy-MM-dd HH:mm"/></c:if>
						<c:if test="${result.endSendTime eq null ||result.endSendTime == ''}">现在</c:if>
					</td>
				</tr>
			</tbody>
		</table>
		<c:if test="${fn:length(provinceRegionList)>0}">
			<div class="charts">
				<div id="stat_region_column" style="width: 800px; height: 300px; margin: 0 auto;"></div>
			</div>
		</c:if>
		<c:if test="${fn:length(provinceRegionList)<1}">
			<div class="no_data">暂无数据图显示</div>
		</c:if>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th>地区</th>
					<th>打开人数</th>
					<th>打开次数</th>
					<th>点击人数</th>
					<th>点击次数</th>
					<th>退订人数</th>
					<th>转发人数</th>
				</tr>
				<c:forEach items="${provinceRegionList}" var="p">
					<tr>
						<td><b>${p.provinceName}</b></td>
						<td><b><font color="#72429A">${p.readUserCount}</font></b></td>
						<td>${p.readCount}</td>
						<td><b><font color="#BBB9D0">${p.clickUserCount}</font></b></td>
						<td>${p.clickCount}</td>
						<td>${p.unsubscribeCount}</td>
						<td>${p.forwardCount}</td>
					</tr>
					<c:forEach items="${cityRegionList}" var="c">
						<c:if test="${p.provinceName eq c.provinceName}">
							<tr>
								<td>&nbsp;&nbsp;&nbsp;&nbsp;${c.cityName}</td>
								<td>${c.readUserCount}</td>
								<td>${c.readCount}</td>
								<td>${c.clickUserCount}</td>
								<td>${c.clickCount}</td>
								<td>${c.unsubscribeCount}</td>
								<td>${c.forwardCount}</td>
							</tr>
						</c:if>
					</c:forEach>
				</c:forEach>
			</tbody>
		</table>
	</form>
		<input type="hidden" name="msg" id="msg" value="${message}">
		<br />
	<br />
	<br />
	</div>
  </body>
</html>
