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
		${result.sentCount}, 
	    ${result.reachCount},
	    ${result.readUserCount},
	    ${result.clickUserCount},
	    ${result.beforeNoneCount},
	    ${result.afterNoneCount}
	];

	var a = datas[0] == 0 ? 0 : 100;
   	var b = datas[1] == 0 ? 0 : datas[1] * 100 / (datas[0] - datas[5]);
   	var c = datas[1] == 0 ? 0 : datas[2] * 100 / datas[1];
   	var d = datas[2] == 0 ? 0 : datas[3] * 100 / datas[2];
	
	var percents = [
		["发送总数", a.toFixed(2)],
		["成功总数", b.toFixed(2)],
		["打开人数", c.toFixed(2)],
		["点击人数", d.toFixed(2)]
	];
 	</script>
 
  </head>
  
  <body>
    <div id="main" class="clearfix" style="position:relative">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->历史发送总览&nbsp;<span></span></h1>
		
		<form id="ListForm1" action="${ctx}/historyReport/apiTrigger" method="post">
			<%--<ul class="search clearfix m_b00 no_display">--%>
				<ul class="search clearfix m_b10">
<!-- 				<li>触发类型</li> -->
<!-- 				<li style="width:10%"> -->
<!-- 					<select id="search_triggerName" -->
<!-- 							name="search_triggerName" style="width:100%;background-color: #F5F5F5"> -->
<!-- 								<option value="">全部</option> -->
<%-- 								<option <c:if test="${search_triggerName eq '注册验证'}">selected="selected"</c:if> value="注册验证">注册验证</option> --%>
<%-- 								<option <c:if test="${search_triggerName eq '注册成功'}">selected="selected"</c:if> value="注册成功">注册成功</option> --%>
<%-- 								<option <c:if test="${search_triggerName eq '订单确认'}">selected="selected"</c:if> value="订单确认">订单确认</option> --%>
<%-- 								<option <c:if test="${search_triggerName eq '成功下单'}">selected="selected"</c:if> value="成功下单">成功下单</option> --%>
<%-- 								<c:forEach items="${triggerNameList}" var="triggerName"> --%>
<%-- 									<option <c:if test="${search_triggerName eq triggerName}">selected="selected"</c:if> value="${triggerName}">${triggerName}</option> --%>
<%-- 								</c:forEach> --%>
<!-- 					</select> -->
<!-- 				</li> -->
				
				<li>触发时间</li>
				<li>
					<input maxlength="10" class="date" id="d4311"  name="search_beginTimeStr1" value="${search_beginTimeStr1}" class="Wdate" type="text" onFocus="WdatePicker({maxDate:'#F{$dp.$D(\'d4312\')}'})"/>
				</li>
				<li style="width:3%">到</li>
				<li>
					 <input maxlength="10"  class="date" id="d4312" name="search_endTimeStr1" value="${search_endTimeStr1}" class="Wdate" type="text" onFocus="WdatePicker({minDate:'#F{$dp.$D(\'d4311\')}'})"/>
				</li>
				<li style="width:10%">
					<select id="search_cycle1" onchange="getDateRange(this.value,'d4311','d4312')"
							name="search_cycle1" style="width:100%;background-color: #F5F5F5">
								<option value="">选择周期</option>
								<option <c:if test="${search_cycle1==6}">selected="selected"</c:if> value="6">近一周</option>
								<option <c:if test="${search_cycle1==30}">selected="selected"</c:if> value="30">近一月</option>
								<option <c:if test="${search_cycle1==91}">selected="selected"</c:if> value="91">近一季度</option>
								<option <c:if test="${search_cycle1==365}">selected="selected"</c:if> value="365">近一年</option>
					</select>
				</li>
				<li>
					<input type="button" class="groovybutton" value="查 询" title="查询" onclick="search1()">
					</li>
				<li>
					<%--<input type="button" class="groovybutton" value="重 置" title="重置" onclick="resetParam3()">
				--%></li>
			</ul>
			
		<ul class="tab clearfix">
			<li><a href="${ctx}/historyReport/list">投递发送类</a></li>
			<li class="now"><a href="${ctx}/historyReport/apiTrigger">API触发类</a></li>
			<li><a href="${ctx}/historyReport/apiGroups">API群发类</a></li>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p>历史发送总览</p>
			</div>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
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
				</tr>
				<tr>
					<td>
						<c:if test="${result eq null}">0</c:if>
						<c:if test="${result ne null }">${result.sentCount}</c:if>
					</td>
					<td>
						<c:if test="${result eq null}">0</c:if>
						<c:if test="${result ne null }">${result.reachCount}</c:if>
					</td>
					<td>
						<c:if test="${result eq null}">0</c:if>
						<c:if test="${result ne null }">${result.softBounceCount+result.hardBounceCount}</c:if>
					</td>
					<td>
						<c:if test="${result eq null}">0</c:if>
						<c:if test="${result ne null }">${result.afterNoneCount}</c:if>
					</td>
					<td>
						<c:if test="${result eq null}">0</c:if>
						<c:if test="${result ne null }">${result.readUserCount}</c:if>
					</td>
					<td>
						<c:if test="${result eq null}">0</c:if>
						<c:if test="${result ne null }">${result.readCount}</c:if>
					</td>
					<td>
						<c:if test="${result eq null}">0</c:if>
						<c:if test="${result ne null }">${result.clickUserCount}</c:if>
					</td>
					<td>
						<c:if test="${result eq null}">0</c:if>
						<c:if test="${result ne null }">${result.clickCount}</c:if>
					</td>
					<td>
						<c:if test="${result eq null}">0</c:if>
						<c:if test="${result ne null }">${result.unsubscribeCount}</c:if>
					</td>
					<td>
						<c:if test="${result eq null}">0</c:if>
						<c:if test="${result ne null }">${result.forwardCount}</c:if>
					</td>
				</tr>
			</tbody>
		</table>
		<c:if test="${result ne null}">
			<div class="charts">
				<div id="stat_funnel" style="width: 480px; height: 240px; margin: 0 auto;"></div>
			</div>
		</c:if>
		<c:if test="${result eq null}">
			<div class="no_data">没有匹配的数据</div>
		</c:if>
	</form>
	
		<input type="hidden" name="msg" id="msg" value="${message}">
		<br />
		<br />
	</div>
  </body>
</html>
