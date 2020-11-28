<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>任务统计-对比</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/js/highcharts/highcharts.js"></script>
	<script type="text/javascript" src="${ctx}/resources/script/report/report.stat.compare.js?${version}"></script>
 	<script type="text/javascript">
 	
	var names = ["发送总数", "成功总数", "打开人数", "点击人数", "退订人数"];
	var temp = [
	        <c:forEach items="${resultList}" var="r" varStatus="status">
	           	{name:'${r.taskName}',data:[${r.sentCount},${r.reachCount},${r.readUserCount},${r.clickUserCount},${r.unsubscribeCount}]}<c:if test='${status.last==false}'>,</c:if>
	        </c:forEach>
	]; 
	
	var datas = [];
	for(var i=0;i<temp.length;i++) {
		datas[i] = temp[temp.length-i-1];
	}

	
 	</script>
 
  </head>
 
  <body>
    <div id="main" class="clearfix" style="position:relative">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->任务对比</h1>
		
		<form id="ListForm1" method="get">
		<div class="ribbon clearfix">
			<div class="note">
				<p>共<font color="blue">${fn:length(resultList)}</font>个任务</p>
			</div>
			<ul class="btn">
				<li>
					<input type="button" class="groovybutton" value="返 回" onclick="goBack()">
				</li>
			</ul>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th>任务名称</th>
					<th>发送总数</th>
					<th>成功数</th>
					<th>弹回数</th>
					<th>打开人数</th>
					<th>打开次数</th>
					<th>点击人数</th>
					<th>点击次数</th>
					<th>退订人数</th>
					<th>转发次数</th>
					<th>无效人数</th>
					<th>成功率</th>
					<th>打开率</th>
					<th>点击率</th>
				</tr>
				<c:forEach items="${resultList}" var="result">
					<tr>
						<td title="${result.taskName}"><span class="color_grid"></span>${result.taskName}</td>
						<td>${result.sentCount}</td>
						<td>${result.reachCount}</td>
						<td>${result.softBounceCount+result.hardBounceCount}</td>
						<td>${result.readUserCount}</td>
						<td>${result.readCount}</td>
						<td>${result.clickUserCount}</td>
						<td>${result.clickCount}</td>
						<td>${result.unsubscribeCount}</td>
						<td>${result.forwardCount}</td>
						<td>${result.afterNoneCount}</td>
						<td>
						<c:if test="${result.sentCount-result.afterNoneCount==0}">0.00%</c:if>
						<c:if test="${result.sentCount-result.afterNoneCount!=0}">
							<fmt:formatNumber value="${result.reachCount/(result.sentCount-result.afterNoneCount)*100}" pattern="#.##"/>%
						</c:if>
					</td>
					<td>
						<c:if test="${result.reachCount==0}">0.00%</c:if>
						<c:if test="${result.reachCount!=0}">
							<fmt:formatNumber value="${result.readUserCount/result.reachCount*100}" pattern="#.##"/>%
						</c:if>
					</td>
					<td>
						<c:if test="${result.readUserCount==0}">0.00%</c:if>
						<c:if test="${result.readUserCount!=0}">
							<fmt:formatNumber value="${result.clickUserCount/result.readUserCount*100}" pattern="#.##"/>%
						</c:if>
					</td>
					</tr>
				</c:forEach>
			</tbody>
		</table>
		<div class="charts">
			<div id="stat_compare_column" style="width: 910px; height: 380px; margin: 0 auto;"></div>
		</div>
		<input type="hidden" name="type" id="type" value="${type}">
		
		<input type="hidden" name="userId" id="userId" value="${userId}">
		<input type="hidden" name="corpId" id="corpId" value="${corpId}">
		<input type="hidden" name="fromFlag" id="fromFlag" value="${fromFlag}">
	</form>
		<input type="hidden" name="msg" id="msg" value="${message}">
		<input type="hidden" name="taskUrl" id="taskUrl" value="${ctx}/taskReport/list">
		<input type="hidden" name="customerUrl" id="customerUrl" value="${ctx}/customerReport/taskList">
		<input type="hidden" name="apiGroupsByUserUrl" id="apiGroupsByUserUrl" value="${ctx}/customerReport/apiGroupsTaskStatList">
		<input type="hidden" name="apiGroupsByAllUrl" id="apiGroupsByAllUrl" value="${ctx}/apiReport/apiGroupsList">
		<br />
	<br />
	<br />
	</div>
  </body>
</html>
