<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<title>API触发统计</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
		<meta http-equiv="Content-Language" content="zh-cn" />
		<meta http-equiv="pragma" content="no-cache">
		<meta http-equiv="cache-control" content="no-cache">
		<meta http-equiv="expires" content="0">
	
		<%@ include file="/WEB-INF/jsp/common/css.jsp"%>
		<%@ include file="/WEB-INF/jsp/common/script.jsp"%>
		<script type="text/javascript" src="${ctx}/resources/script/js/highcharts/highcharts.js"></script>
		<script type="text/javascript" src="${ctx}/resources/script/js/highcharts/modules/funnel.js"></script>
		<script type="text/javascript" src="${ctx}/resources/script/report/report.stat.sum.js?${version}"></script>
		<script type="text/javascript" src="${ctx}/resources/script/report/getDateRange.js?${version}"></script>
		<script type="text/javascript" src="${ctx}/resources/script/report/historyReportList.js?${version}"></script>
		<script type="text/javascript">
			var datas = [
				${resultTotal.sentCount}, 
			    ${resultTotal.reachCount},
			    ${resultTotal.readUserCount},
			    ${resultTotal.clickUserCount},
			    ${resultTotal.beforeNoneCount},
			    ${resultTotal.afterNoneCount}
			];
		
			var a = datas[0] == 0 ? 0 : 100;
// 		   	var b = datas[1] == 0 ? 0 : datas[1] * 100 / (datas[0] - datas[5]);
// 		   	var c = datas[1] == 0 ? 0 : datas[2] * 100 / datas[1];
// 		   	var d = datas[2] == 0 ? 0 : datas[3] * 100 / datas[2];
			
			var percents = [
				["发送总数", a.toFixed(2)],
				["成功总数", ${resultTotal.resultReach}],
				["打开人数", ${resultTotal.resultRead}],
				["点击人数", ${resultTotal.resultClick}]
			];
	 	</script>
	</head>
	
	<body>
		<div id="main" class="clearfix" style="position: relative">
			<h1 class="title">
				历史发送总览&nbsp;<span></span>
			</h1>
	
			<form id="ListForm1" action="${ctx}/historyReport/apiDelivery" method="get">
				<%--<ul class="search clearfix m_b00 no_display">--%>
				<ul class="tab clearfix">
					<li>
						<a href="${ctx}/historyReport/list">投递发送类</a>
					</li>
					<li class="now">
						<a href="${ctx}/historyReport/apiDelivery">API投递</a>
					</li>
				</ul>
				<div class="ribbon clearfix">
					<div class="note">
						<p>历史发送总览</p>
					</div>
					<ul class="btn">
						<li>
							<a class="btn_icon btn_icon_down" href="javascript:exportApiDelivery('${ctx}/historyReport/exportApiDelivery');">导出</a>
						</li>
					</ul>
				</div>
				<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
					<tbody>
						<tr>
							<th>API类型</th>
							<th>发送总数</th>
							<th>成功总数</th>
							<th>弹回总数</th>
							<th>无效人数</th>
							<th>打开人数</th>
							<th>打开次数</th>
							<th>点击人数</th>
							<th>点击次数</th>
							<th>退订总数</th>
							<th>转发总数</th>
							<th>成功率</th>
							<th>打开率</th>
							<th>点击率</th>
						</tr>
						<tr>
							<td>触发任务</td>
							<td>${resultTrigger.sentCount}</td>
							<td>${resultTrigger.reachCount}</td>
							<td>${resultTrigger.softBounceCount+resultTrigger.hardBounceCount}</td>
							<td>${resultTrigger.afterNoneCount}</td>
							<td>${resultTrigger.readUserCount}</td>
							<td>${resultTrigger.readCount}</td>
							<td>${resultTrigger.clickUserCount}</td>
							<td>${resultTrigger.clickCount}</td>
							<td>${resultTrigger.unsubscribeCount}</td>
							<td>${resultTrigger.forwardCount}</td>
							<td>${resultTrigger.resultReach}%</td>
							<td>${resultTrigger.resultRead}%</td>
							<td>${resultTrigger.resultClick}%</td>
						</tr>
						<tr>
							<td>群发任务</td>
							<td>${resultGroups.sentCount}</td>
							<td>${resultGroups.reachCount}</td>
							<td>${resultGroups.softBounceCount+resultGroups.hardBounceCount}</td>
							<td>${resultGroups.afterNoneCount}</td>
							<td>${resultGroups.readUserCount}</td>
							<td>${resultGroups.readCount}</td>
							<td>${resultGroups.clickUserCount}</td>
							<td>${resultGroups.clickCount}</td>
							<td>${resultGroups.unsubscribeCount}</td>
							<td>${resultGroups.forwardCount}</td>
							<td>${resultGroups.resultReach}%</td>
							<td>${resultGroups.resultRead}%</td>
							<td>${resultGroups.resultClick}%</td>
						</tr>
						<tr>
							<td>合计</td>
							<td>${resultTotal.sentCount}</td>
							<td>${resultTotal.reachCount}</td>
							<td>${resultTotal.softBounceCount+resultTotal.hardBounceCount}</td>
							<td>${resultTotal.afterNoneCount}</td>
							<td>${resultTotal.readUserCount}</td>
							<td>${resultTotal.readCount}</td>
							<td>${resultTotal.clickUserCount}</td>
							<td>${resultTotal.clickCount}</td>
							<td>${resultTotal.unsubscribeCount}</td>
							<td>${resultTotal.forwardCount}</td>
							<td>${resultTotal.resultReach}%</td>
							<td>${resultTotal.resultRead}%</td>
							<td>${resultTotal.resultClick}%</td>
						</tr>
					</tbody>
				</table>
	
				<!-- 展示图需完善 -->
				<c:if test="${resultTotal ne null}">
					<div class="charts">
						<div id="stat_funnel"
							style="width: 480px; height: 240px; margin: 0 auto;"></div>
					</div>
				</c:if>
				<c:if test="${resultTotal eq null}">
					<div class="no_data">没有匹配的数据</div>
				</c:if>
			</form>
	
			<input type="hidden" name="msg" id="msg" value="${message}"> <br />
				<br />
		</div>
	</body>
</html>
