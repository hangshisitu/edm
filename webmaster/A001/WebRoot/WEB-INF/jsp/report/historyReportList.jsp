<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>历史发送总览</title>
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
		${domain.sentCount}, 
	    ${domain.reachCount},
	    ${domain.readUserCount},
	    ${domain.clickUserCount},
	    ${domain.beforeNoneCount},
	    ${domain.afterNoneCount}
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
		
		<form id="ListForm1" action="${ctx}/historyReport/list" method="get">
			<%--<ul class="search clearfix m_b00 no_display">--%>
				<ul class="search clearfix m_b10">
				<%-- <li>域名</li>
				<li style="width:10%">
					<select id="search_emailDomain"
							name="search_emailDomain" style="width:100%;background-color: #F5F5F5">
								<option value="">不限</option>
								<option <c:if test="${search_emailDomain=='139.com'}">selected="selected"</c:if> value="139.com">139邮箱</option>
								<option <c:if test="${search_emailDomain=='163.com'}">selected="selected"</c:if> value="163.com">网易邮箱</option>
								<option <c:if test="${search_emailDomain=='gmail.com'}">selected="selected"</c:if> value="gmail.com">Gmail邮箱</option>
								<option <c:if test="${search_emailDomain=='qq.com'}">selected="selected"</c:if> value="qq.com">QQ邮箱</option>
								<option <c:if test="${search_emailDomain=='sina.com'}">selected="selected"</c:if> value="sina.com">新浪邮箱</option>
								<option <c:if test="${search_emailDomain=='sohu.com'}">selected="selected"</c:if> value="sohu.com">搜狐邮箱</option>
					</select>
				</li>
				--%>
				<li>创建时间</li>
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
					<%--<input onmouseover="this.style.cursor='pointer';this.style.cursor='hand';this.style.color='blue'" onmouseout="this.style.color=''"  type="button" value="查 询" onclick="search1()">
					--%><input type="button" class="groovybutton" value="查 询" title="查询" onclick="search1()">
					</li>
				<li>
					<%--<input onmouseover="this.style.cursor='pointer';this.style.cursor='hand';this.style.color='blue'" onmouseout="this.style.color=''"  type="button" value="重 置" onclick="resetParam1()">
					--%><%--<input type="button" class="groovybutton" value="重 置" title="重置" onclick="resetParam1()">
				--%></li>
			</ul>
			
		<ul class="tab clearfix">
			<li class="now"><a href="${ctx}/historyReport/list">投递发送类</a></li>
<%-- 			<li><a href="${ctx}/historyReport/apiTrigger">API触发类</a></li> --%>
			<li><a href="${ctx}/historyReport/apiDelivery">API 投递</a></li>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p>历史发送总览</p>
			</div>
			<ul class="btn">
				<li>
					<input type="button" class="groovybutton" value="导 出" title="导出" onclick="exportHis('${ctx}/historyReport/exportHis')">
				</li>
			</ul>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th>域名</th>
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
					<td>
						139邮箱
					</td>
					<td>
						<c:if test="${domain139 eq null}">0</c:if>
						<c:if test="${domain139 ne null }">${domain139.sentCount}</c:if>
					</td>
					<td>
						<c:if test="${domain139 eq null}">0</c:if>
						<c:if test="${domain139 ne null }">${domain139.reachCount}</c:if>
					</td>
					<td> 
						<c:if test="${domain139 eq null}">0</c:if>
						<c:if test="${domain139 ne null }">${domain139.softBounceCount+domain139.hardBounceCount}</c:if>
					</td>
					<td> 
						<c:if test="${domain139 eq null}">0</c:if>
						<c:if test="${domain139 ne null }">${domain139.afterNoneCount}</c:if>
					</td>
					<td>
						<c:if test="${domain139 eq null}">0</c:if>
						<c:if test="${domain139 ne null }">${domain139.readUserCount}</c:if>
					</td>
					<td>
						<c:if test="${domain139 eq null}">0</c:if>
						<c:if test="${domain139 ne null }">${domain139.readCount}</c:if>
					</td>
					<td>
						<c:if test="${domain139 eq null}">0</c:if>
						<c:if test="${domain139 ne null }">${domain139.clickUserCount}</c:if>
					</td>
					<td>
						<c:if test="${domain139 eq null}">0</c:if>
						<c:if test="${domain139 ne null }">${domain139.clickCount}</c:if>
					</td>
					<td>
						<c:if test="${domain139 eq null}">0</c:if>
						<c:if test="${domain139 ne null }">${domain139.unsubscribeCount}</c:if>
					</td>
					<td>
						<c:if test="${domain139 eq null}">0</c:if>
						<c:if test="${domain139 ne null }">${domain139.forwardCount}</c:if>
					</td>
					<td>
						<c:if test="${domain139 eq null}">0.00%</c:if>
						<c:if test="${domain139 ne null }">${domain139.successRate}</c:if>
					</td>
					<td>
						<c:if test="${domain139 eq null}">0.00%</c:if>
						<c:if test="${domain139 ne null }">${domain139.openRate}</c:if>
					</td>
					<td>
						<c:if test="${domain139 eq null}">0.00%</c:if>
						<c:if test="${domain139 ne null }">${domain139.clickRate}</c:if>
					</td>
				</tr>
				<tr>
					<td>
						网易邮箱
					</td>
					<td>
						<c:if test="${domain163 eq null}">0</c:if>
						<c:if test="${domain163 ne null }">${domain163.sentCount}</c:if>
					</td>
					<td>
						<c:if test="${domain163 eq null}">0</c:if>
						<c:if test="${domain163 ne null }">${domain163.reachCount}</c:if>
					</td>
					<td> 
						<c:if test="${domain163 eq null}">0</c:if>
						<c:if test="${domain163 ne null }">${domain163.softBounceCount+domain163.hardBounceCount}</c:if>
					</td>
					<td> 
						<c:if test="${domain163 eq null}">0</c:if>
						<c:if test="${domain163 ne null }">${domain163.afterNoneCount}</c:if>
					</td>
					<td>
						<c:if test="${domain163 eq null}">0</c:if>
						<c:if test="${domain163 ne null }">${domain163.readUserCount}</c:if>
					</td>
					<td>
						<c:if test="${domain163 eq null}">0</c:if>
						<c:if test="${domain163 ne null }">${domain163.readCount}</c:if>
					</td>
					<td>
						<c:if test="${domain163 eq null}">0</c:if>
						<c:if test="${domain163 ne null }">${domain163.clickUserCount}</c:if>
					</td>
					<td>
						<c:if test="${domain163 eq null}">0</c:if>
						<c:if test="${domain163 ne null }">${domain163.clickCount}</c:if>
					</td>
					<td>
						<c:if test="${domain163 eq null}">0</c:if>
						<c:if test="${domain163 ne null }">${domain163.unsubscribeCount}</c:if>
					</td>
					<td>
						<c:if test="${domain163 eq null}">0</c:if>
						<c:if test="${domain163 ne null }">${domain163.forwardCount}</c:if>
					</td>
					<td>
						<c:if test="${domain163 eq null}">0.00%</c:if>
						<c:if test="${domain163 ne null }">${domain163.successRate}</c:if>
					</td>
					<td>
						<c:if test="${domain163 eq null}">0.00%</c:if>
						<c:if test="${domain163 ne null }">${domain163.openRate}</c:if>
					</td>
					<td>
						<c:if test="${domain163 eq null}">0.00%</c:if>
						<c:if test="${domain163 ne null }">${domain163.clickRate}</c:if>
					</td>
				</tr>
				<tr>
					<td>
						Gmail邮箱
					</td>
					<td>
						<c:if test="${domainGmail eq null}">0</c:if>
						<c:if test="${domainGmail ne null }">${domainGmail.sentCount}</c:if>
					</td>
					<td>
						<c:if test="${domainGmail eq null}">0</c:if>
						<c:if test="${domainGmail ne null }">${domainGmail.reachCount}</c:if>
					</td>
					<td> 
						<c:if test="${domainGmail eq null}">0</c:if>
						<c:if test="${domainGmail ne null }">${domainGmail.softBounceCount+domainGmail.hardBounceCount}</c:if>
					</td>
					<td> 
						<c:if test="${domainGmail eq null}">0</c:if>
						<c:if test="${domainGmail ne null }">${domainGmail.afterNoneCount}</c:if>
					</td>
					<td>
						<c:if test="${domainGmail eq null}">0</c:if>
						<c:if test="${domainGmail ne null }">${domainGmail.readUserCount}</c:if>
					</td>
					<td>
						<c:if test="${domainGmail eq null}">0</c:if>
						<c:if test="${domainGmail ne null }">${domainGmail.readCount}</c:if>
					</td>
					<td>
						<c:if test="${domainGmail eq null}">0</c:if>
						<c:if test="${domainGmail ne null }">${domainGmail.clickUserCount}</c:if>
					</td>
					<td>
						<c:if test="${domainGmail eq null}">0</c:if>
						<c:if test="${domainGmail ne null }">${domainGmail.clickCount}</c:if>
					</td>
					<td>
						<c:if test="${domainGmail eq null}">0</c:if>
						<c:if test="${domainGmail ne null }">${domainGmail.unsubscribeCount}</c:if>
					</td>
					<td>
						<c:if test="${domainGmail eq null}">0</c:if>
						<c:if test="${domainGmail ne null }">${domainGmail.forwardCount}</c:if>
					</td>
					<td>
						<c:if test="${domainGmail eq null}">0.00%</c:if>
						<c:if test="${domainGmail ne null }">${domainGmail.successRate}</c:if>
					</td>
					<td>
						<c:if test="${domainGmail eq null}">0.00%</c:if>
						<c:if test="${domainGmail ne null }">${domainGmail.openRate}</c:if>
					</td>
					<td>
						<c:if test="${domainGmail eq null}">0.00%</c:if>
						<c:if test="${domainGmail ne null }">${domainGmail.clickRate}</c:if>
					</td>
				</tr>
				<tr>
					<td>
						QQ邮箱
					</td>
					<td>
						<c:if test="${domainQQ eq null}">0</c:if>
						<c:if test="${domainQQ ne null }">${domainQQ.sentCount}</c:if>
					</td>
					<td>
						<c:if test="${domainQQ eq null}">0</c:if>
						<c:if test="${domainQQ ne null }">${domainQQ.reachCount}</c:if>
					</td>
					<td> 
						<c:if test="${domainQQ eq null}">0</c:if>
						<c:if test="${domainQQ ne null }">${domainQQ.softBounceCount+domainQQ.hardBounceCount}</c:if>
					</td>
					<td> 
						<c:if test="${domainQQ eq null}">0</c:if>
						<c:if test="${domainQQ ne null }">${domainQQ.afterNoneCount}</c:if>
					</td>
					<td>
						<c:if test="${domainQQ eq null}">0</c:if>
						<c:if test="${domainQQ ne null }">${domainQQ.readUserCount}</c:if>
					</td>
					<td>
						<c:if test="${domainQQ eq null}">0</c:if>
						<c:if test="${domainQQ ne null }">${domainQQ.readCount}</c:if>
					</td>
					<td>
						<c:if test="${domainQQ eq null}">0</c:if>
						<c:if test="${domainQQ ne null }">${domainQQ.clickUserCount}</c:if>
					</td>
					<td>
						<c:if test="${domainQQ eq null}">0</c:if>
						<c:if test="${domainQQ ne null }">${domainQQ.clickCount}</c:if>
					</td>
					<td>
						<c:if test="${domainQQ eq null}">0</c:if>
						<c:if test="${domainQQ ne null }">${domainQQ.unsubscribeCount}</c:if>
					</td>
					<td>
						<c:if test="${domainQQ eq null}">0</c:if>
						<c:if test="${domainQQ ne null }">${domainQQ.forwardCount}</c:if>
					</td>
					<td>
						<c:if test="${domainQQ eq null}">0.00%</c:if>
						<c:if test="${domainQQ ne null }">${domainQQ.successRate}</c:if>
					</td>
					<td>
						<c:if test="${domainQQ eq null}">0.00%</c:if>
						<c:if test="${domainQQ ne null }">${domainQQ.openRate}</c:if>
					</td>
					<td>
						<c:if test="${domainQQ eq null}">0.00%</c:if>
						<c:if test="${domainQQ ne null }">${domainQQ.clickRate}</c:if>
					</td>
				</tr>
				<tr>
					<td>
						新浪邮箱
					</td>
					<td>
						<c:if test="${domainSina eq null}">0</c:if>
						<c:if test="${domainSina ne null }">${domainSina.sentCount}</c:if>
					</td>
					<td>
						<c:if test="${domainSina eq null}">0</c:if>
						<c:if test="${domainSina ne null }">${domainSina.reachCount}</c:if>
					</td>
					<td> 
						<c:if test="${domainSina eq null}">0</c:if>
						<c:if test="${domainSina ne null }">${domainSina.softBounceCount+domainSina.hardBounceCount}</c:if>
					</td>
					<td> 
						<c:if test="${domainSina eq null}">0</c:if>
						<c:if test="${domainSina ne null }">${domainSina.afterNoneCount}</c:if>
					</td>
					<td>
						<c:if test="${domainSina eq null}">0</c:if>
						<c:if test="${domainSina ne null }">${domainSina.readUserCount}</c:if>
					</td>
					<td>
						<c:if test="${domainSina eq null}">0</c:if>
						<c:if test="${domainSina ne null }">${domainSina.readCount}</c:if>
					</td>
					<td>
						<c:if test="${domainSina eq null}">0</c:if>
						<c:if test="${domainSina ne null }">${domainSina.clickUserCount}</c:if>
					</td>
					<td>
						<c:if test="${domainSina eq null}">0</c:if>
						<c:if test="${domainSina ne null }">${domainSina.clickCount}</c:if>
					</td>
					<td>
						<c:if test="${domainSina eq null}">0</c:if>
						<c:if test="${domainSina ne null }">${domainSina.unsubscribeCount}</c:if>
					</td>
					<td>
						<c:if test="${domainSina eq null}">0</c:if>
						<c:if test="${domainSina ne null }">${domainSina.forwardCount}</c:if>
					</td>
					<td>
						<c:if test="${domainSina eq null}">0.00%</c:if>
						<c:if test="${domainSina ne null }">${domainSina.successRate}</c:if>
					</td>
					<td>
						<c:if test="${domainSina eq null}">0.00%</c:if>
						<c:if test="${domainSina ne null }">${domainSina.openRate}</c:if>
					</td>
					<td>
						<c:if test="${domainSina eq null}">0.00%</c:if>
						<c:if test="${domainSina ne null }">${domainSina.clickRate}</c:if>
					</td>
				</tr>
				<tr>
					<td>
						搜狐邮箱
					</td>
					<td>
						<c:if test="${domainSohu eq null}">0</c:if>
						<c:if test="${domainSohu ne null }">${domainSohu.sentCount}</c:if>
					</td>
					<td>
						<c:if test="${domainSohu eq null}">0</c:if>
						<c:if test="${domainSohu ne null }">${domainSohu.reachCount}</c:if>
					</td>
					<td> 
						<c:if test="${domainSohu eq null}">0</c:if>
						<c:if test="${domainSohu ne null }">${domainSohu.softBounceCount+domainSohu.hardBounceCount}</c:if>
					</td>
					<td> 
						<c:if test="${domainSohu eq null}">0</c:if>
						<c:if test="${domainSohu ne null }">${domainSohu.afterNoneCount}</c:if>
					</td>
					<td>
						<c:if test="${domainSohu eq null}">0</c:if>
						<c:if test="${domainSohu ne null }">${domainSohu.readUserCount}</c:if>
					</td>
					<td>
						<c:if test="${domainSohu eq null}">0</c:if>
						<c:if test="${domainSohu ne null }">${domainSohu.readCount}</c:if>
					</td>
					<td>
						<c:if test="${domainSohu eq null}">0</c:if>
						<c:if test="${domainSohu ne null }">${domainSohu.clickUserCount}</c:if>
					</td>
					<td>
						<c:if test="${domainSohu eq null}">0</c:if>
						<c:if test="${domainSohu ne null }">${domainSohu.clickCount}</c:if>
					</td>
					<td>
						<c:if test="${domainSohu eq null}">0</c:if>
						<c:if test="${domainSohu ne null }">${domainSohu.unsubscribeCount}</c:if>
					</td>
					<td>
						<c:if test="${domainSohu eq null}">0</c:if>
						<c:if test="${domainSohu ne null }">${domainSohu.forwardCount}</c:if>
					</td>
					<td>
						<c:if test="${domainSohu eq null}">0.00%</c:if>
						<c:if test="${domainSohu ne null }">${domainSohu.successRate}</c:if>
					</td>
					<td>
						<c:if test="${domainSohu eq null}">0.00%</c:if>
						<c:if test="${domainSohu ne null }">${domainSohu.openRate}</c:if>
					</td>
					<td>
						<c:if test="${domainSohu eq null}">0.00%</c:if>
						<c:if test="${domainSohu ne null }">${domainSohu.clickRate}</c:if>
					</td>
				</tr>
				<tr>
					<td>
						其他
					</td>
					<td>
						<c:if test="${ohterDomain eq null}">0</c:if>
						<c:if test="${ohterDomain ne null }">${ohterDomain.sentCount}</c:if>
					</td>
					<td>
						<c:if test="${ohterDomain eq null}">0</c:if>
						<c:if test="${ohterDomain ne null }">${ohterDomain.reachCount}</c:if>
					</td>
					<td> 
						<c:if test="${ohterDomain eq null}">0</c:if>
						<c:if test="${ohterDomain ne null }">${ohterDomain.softBounceCount+ohterDomain.hardBounceCount}</c:if>
					</td>
					<td> 
						<c:if test="${ohterDomain eq null}">0</c:if>
						<c:if test="${ohterDomain ne null }">${ohterDomain.afterNoneCount}</c:if>
					</td>
					<td>
						<c:if test="${ohterDomain eq null}">0</c:if>
						<c:if test="${ohterDomain ne null }">${ohterDomain.readUserCount}</c:if>
					</td>
					<td>
						<c:if test="${ohterDomain eq null}">0</c:if>
						<c:if test="${ohterDomain ne null }">${ohterDomain.readCount}</c:if>
					</td>
					<td>
						<c:if test="${ohterDomain eq null}">0</c:if>
						<c:if test="${ohterDomain ne null }">${ohterDomain.clickUserCount}</c:if>
					</td>
					<td>
						<c:if test="${ohterDomain eq null}">0</c:if>
						<c:if test="${ohterDomain ne null }">${ohterDomain.clickCount}</c:if>
					</td>
					<td>
						<c:if test="${ohterDomain eq null}">0</c:if>
						<c:if test="${ohterDomain ne null }">${ohterDomain.unsubscribeCount}</c:if>
					</td>
					<td>
						<c:if test="${ohterDomain eq null}">0</c:if>
						<c:if test="${ohterDomain ne null }">${ohterDomain.forwardCount}</c:if>
					</td>
					<td>
						<c:if test="${ohterDomain eq null}">0.00%</c:if>
						<c:if test="${ohterDomain ne null }">${ohterDomain.successRate}</c:if>
					</td>
					<td>
						<c:if test="${ohterDomain eq null}">0.00%</c:if>
						<c:if test="${ohterDomain ne null }">${ohterDomain.openRate}</c:if>
					</td>
					<td>
						<c:if test="${ohterDomain eq null}">0.00%</c:if>
						<c:if test="${ohterDomain ne null }">${ohterDomain.clickRate}</c:if>
					</td>
				</tr>
				<tr style="font-weight:bold">
					<td>
						合计
					</td>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null }">${domain.sentCount}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null }">${domain.reachCount}</c:if>
					</td>
					<td> 
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null }">${domain.softBounceCount+domain.hardBounceCount}</c:if>
					</td>
					<td> 
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null }">${domain.afterNoneCount}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null }">${domain.readUserCount}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null }">${domain.readCount}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null }">${domain.clickUserCount}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null }">${domain.clickCount}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null }">${domain.unsubscribeCount}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null }">${domain.forwardCount}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0.00%</c:if>
						<c:if test="${domain ne null }">${domain.successRate}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0.00%</c:if>
						<c:if test="${domain ne null }">${domain.openRate}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0.00%</c:if>
						<c:if test="${domain ne null }">${domain.clickRate}</c:if>
					</td>
				</tr>
			</tbody>
		</table>
		<c:if test="${hasdomain eq 'Y'}">
			<div class="charts">
				<div id="stat_funnel" style="width: 480px; height: 300px; margin: 0 auto;"></div>
			</div>
		</c:if>
		<c:if test="${hasdomain eq 'N'}">
			<div class="no_data">暂无数据图显示</div>
		</c:if>
	</form>
	
		<input type="hidden" name="msg" id="msg" value="${message}">
		<input type="hidden" name="listUrl" id="listUrl" value="${ctx}/historyReport/list">
		
		<br />
	<br />
	<br />
	</div>
  </body>
</html>
