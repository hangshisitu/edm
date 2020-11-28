<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>任务统计-汇总</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/report/getDateRange.js?${version}"></script>
	<script type="text/javascript" src="${ctx}/resources/script/report/plan/plan.report.total.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="position:relative">
	<h1 class="title">汇总&nbsp;&nbsp;<span>对所选任务进行合计</span></h1>
		<div class="ribbon clearfix">
			<div class="note">
				<p>共<span>${planTotal}</span>个任务</p>
			</div>
			<ul class="btn">
				<li>
					<a class="btn_icon btn_icon_down" href="javaScript:exportpPlanCollect('${ctx}/plan/planReport/exportpPlanCollect?checkedIds=${checkedIds}');">导出</a>
				</li>
				<li>
					<a class="btn_icon btn_icon_imp" href="javaScript:goBack('${ctx}/plan/planReport/reportList');">返回</a>
				</li>
			</ul>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th>发送总数</th>
					<th>成功总数</th>
					<th>弹回总数</th>
					<th>打开总人数</th>
					<th>打开总次数</th>
					<th>点击总人数</th>
					<th>点击总次数</th>
					<th>成功率</th>
					<th>打开率</th>
					<th>点击率</th>
				</tr>
				<tr>
					<td>${result.sentCount}</td>
					<td>${result.reachCount}</td>
					<td>${result.softBounceCount + result.hardBounceCount}</td>
					<td>${result.readUserCount}</td>
					<td>${result.readCount}</td>
					<td>${result.clickUserCount}</td>
					<td>${result.clickCount}</td>
					<td>${result.resultReach}%</td>
					<td>${result.resultRead}%</td>
					<td>${result.resultClick}%</td>
				</tr>
			</tbody>
		</table>
	
		<c:if test="${result==null}">
			<div class="no_data">没有匹配的数据</div>
		</c:if>
		<input type="hidden" name="msg" id="msg" value="${message}">
	</div>
	<script>
// 		$(function() {
// 			var sentCount = defaultValue("${result.sentCount}");
// 			var reachCount = defaultValue("${result.reachCount}");
// 			var readUserCount = defaultValue("${result.readUserCount}");
// 			var clickUserCount = defaultValue("${result.clickUserCount}");
			
// 			var afterNoneCount = defaultValue("${result.afterNoneCount}");
// 		  	var reach = (sentCount - afterNoneCount)==0 ? 0 : reachCount * 100 / (sentCount - afterNoneCount);
// 		  	var read = reachCount==0 ? 0 :readUserCount * 100 / reachCount;
// 		  	var click = readUserCount==0 ? 0 : clickUserCount * 100 / readUserCount;
		     	
// 			$("#reachRate").text(reach.toFixed(2) + " %");
// 		   	$("#readRate").text(read.toFixed(2) + " %");
// 		   	$("#clickRate").text(click.toFixed(2) + " %");
		   	
// 		});
		
// 		function defaultValue(value) {
// 			return defaultVal = value=="" ? 0 : value;
// 		}
	</script>
  </body>
</html>
