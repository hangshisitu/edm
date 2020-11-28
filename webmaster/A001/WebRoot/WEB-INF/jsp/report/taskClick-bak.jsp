<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>任务统计-点击</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/js/highcharts/highcharts.js"></script>
	<script type="text/javascript" src="${ctx}/resources/script/report/report.stat.click.js?${version}"></script>
 	<script type="text/javascript">
 	
	var datas = [
	             <c:forEach items="${hourMap}" var="h" varStatus="status">   
	            	 ${h.value}<c:if test='${status.last==false}'>,</c:if>
	      		 </c:forEach>   
	];

 	</script>
 
  </head>
  
  <body>
    <div id="main" class="clearfix" style="position:relative">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->点击</h1>
		
		<form id="ListForm" method="post">
		<ul class="tab clearfix">
			<li><a href="${ctx}/taskDetailStat/overview?taskId=${task.taskId}">概览</a></li>
			<li><a href="${ctx}/taskDetailStat/send?taskId=${task.taskId}">发送</a></li>
			<li><a href="${ctx}/taskDetailStat/open?taskId=${task.taskId}">打开</a></li>
			<li class="now"><a href="${ctx}/taskDetailStat/click?taskId=${task.taskId}">点击</a></li>
			<li><a href="${ctx}/taskDetailStat/domain?taskId=${task.taskId}">域名</a></li>
			<li><a href="${ctx}/taskDetailStat/region?taskId=${task.taskId}">地域</a></li>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p>当前任务：<font color="blue">${task.taskName}</font></p>
			</div>
		</div>
		<c:if test="${task ne null}">
		<div class="charts">
			<div id="stat_line" style="width: 840px; height: 240px; margin: 0 auto;"></div>
		</div>
		</c:if>
		<c:if test="${task eq null}">
			<div class="no_data">暂无数据图显示</div>
		</c:if>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr id="logger_0">
					<th>链接地址</th>
					<th>点击次数</th>
					<th>占比</th>
				</tr>
			</tbody>
		</table>
	</form>
		<input type="hidden" name="msg" id="msg" value="${message}">
		<input type="hidden" name="url" id="url" value="${ctx}/taskDetailStat/clickLogList?taskId=${task.taskId}">
		<br />
	<br />
	<br />
	</div>
  </body>
</html>
