<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
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
	var b = datas[0] == 0 ? 0 : (datas[0] - datas[5]) / datas[0] * 100;
	var c = b == 0 ? 0 : datas[2] / b * 100;
	var d = b == 0 ? 0 : datas[3] / b * 100;
	
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
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->数据报告&nbsp;<span>历史发送总览</span></h1>
		
		<form id="ListForm1" action="${ctx}/historyReport/list" method="post">
			<%--<ul class="search clearfix m_b00 no_display">--%>
				<ul class="search clearfix m_b10">
				<li>域名</li>
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
				
				<li>投递时间</li>
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
				<li><input onmouseover="this.style.cursor='pointer';this.style.cursor='hand';this.style.color='blue'" onmouseout="this.style.color=''"  type="button" value="查 询" onclick="search1()"></li>
				<li><input onmouseover="this.style.cursor='pointer';this.style.cursor='hand';this.style.color='blue'" onmouseout="this.style.color=''"  type="button" value="重 置" onclick="resetParam1()"></li>
			</ul>
			
		<ul class="tab clearfix">
			<li class="now"><a href="javascript:void(0)">发送总览</a></li>
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
					<th>打开人数</th>
					<th>打开次数</th>
					<th>点击人数</th>
					<th>点击次数</th>
					<th>退订总数</th>
					<th>转发总数</th>
				</tr>
				<tr onMouseOver="this.style.backgroundColor='#C7E2E0'" onMouseOut="this.style.backgroundColor=''">
					<td>${domain.sentCount}</td>
					<td>${domain.reachCount}</td>
					<td>${domain.softBounceCount+domain.hardBounceCount}</td>
					<td>${domain.readUserCount}</td>
					<td>${domain.readCount}</td>
					<td>${domain.clickUserCount}</td>
					<td>${domain.clickCount}</td>
					<td>${domain.unsubscribeCount}</td>
					<td>${domain.forwardCount}</td>
				</tr>
			</tbody>
		</table>
		<c:if test="${domain!=null}">
			<div class="charts">
				<div id="stat_funnel" style="width: 480px; height: 240px; margin: 0 auto;"></div>
			</div>
		</c:if>
		<c:if test="${domain==null}">
			<div class="no_data">没有匹配的数据</div>
		</c:if>
	</form>
	<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->数据报告&nbsp;<span>客户发送统计信息</span></h1>
		<form id="ListForm" action="${ctx}/historyReport/list" method="post">
			<%--<ul class="search clearfix m_b00 no_display">--%>
				<ul class="search clearfix m_b10">
				<li>客户账号</li>
				<li>
					<input maxlength="10" type="text" id="search_userId" name="search_userId" value="${search_userId}">
				</li>
				<li>投递时间</li>
				<li>
					<input maxlength="10" class="date" id="d5311"  name="search_beginTimeStr" value="${search_beginTimeStr}" class="Wdate" type="text" onFocus="WdatePicker({maxDate:'#F{$dp.$D(\'d5312\')}'})"/>
				</li>
				<li style="width:3%">到</li>
				<li>
					 <input maxlength="10"  class="date" id="d5312" name="search_endTimeStr" value="${search_endTimeStr}" class="Wdate" type="text" onFocus="WdatePicker({minDate:'#F{$dp.$D(\'d5311\')}'})"/>
				</li>
				<li style="width:10%">
					<select id="search_cycle2" onchange="getDateRange(this.value,'d5311','d5312')"
							name="search_cycle2" style="width:100%;background-color: #F5F5F5">
								<option value="">选择周期</option>
								<option <c:if test="${search_cycle2==6}">selected="selected"</c:if> value="6">近一周</option>
								<option <c:if test="${search_cycle2==30}">selected="selected"</c:if> value="30">近一月</option>
								<option <c:if test="${search_cycle2==91}">selected="selected"</c:if> value="91">近一季度</option>
								<option <c:if test="${search_cycle2==365}">selected="selected"</c:if> value="365">近一年</option>
					</select>
				</li>
				<li><input onmouseover="this.style.cursor='pointer';this.style.cursor='hand';this.style.color='blue'" onmouseout="this.style.color=''"  type="button" value="查 询" onclick="search2()"></li>
				<li><input onmouseover="this.style.cursor='pointer';this.style.cursor='hand';this.style.color='blue'" onmouseout="this.style.color=''"  type="button" value="重 置" onclick="resetParam2()"></li>
			</ul>
			
		
		<ul class="tab clearfix">
			<li class="now"><a href="javascript:void(0)">客户发送统计信息</a></li>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p>共<span><font color="blue">${pagination.recordCount}</font></span>个</p>
			</div>
			<ul class="btn">
				<li>
					<input onmouseover="this.style.cursor='pointer';this.style.cursor='hand';this.style.color='blue'" onmouseout="this.style.color=''" type="button" value="导 出" onclick="exportStat('${ctx}/historyReport/exportUserStat')">
				</li>
				<li>
					<input onmouseover="this.style.cursor='pointer';this.style.cursor='hand';this.style.color='blue'" onmouseout="this.style.color=''" type="button" value="打 印" onclick="printStat('${ctx}/historyReport/printUserStat')">
				</li>
				<li>
					<input onmouseover="this.style.cursor='pointer';this.style.cursor='hand';this.style.color='blue'" onmouseout="this.style.color=''" type="button" value="查看全部任务" onclick="viewAllTask('${ctx}/taskReport/list')">
				</li>
				
			</ul>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th width="25"><input type="checkbox" id="all_checkbox"/></th>
					<th>客户账号</th>
					<th>公司名称</th>
					<th>发送总数</th>
					<th>成功总数</th>
					<th>弹回总数</th>
					<th>打开人数</th>
					<th>打开次数</th>
					<th>点击人数</th>
					<th>点击次数</th>
					<th>退订总数</th>
					<th>转发总数</th>
					<th>操作</th>
				</tr>
				<c:forEach items="${pagination.recordList}" var="d" varStatus="i">
				<tr onMouseOver="this.style.backgroundColor='#C7E2E0'" onMouseOut="this.style.backgroundColor=''">
					<td><input type="checkbox" name="checkedIds" value="${d.corpId}" /></td>
					<td title="${d.userId}">${d.userId}</td>
					<td title="${d.companyName}">${d.companyName}</td>
					<td>${d.sentCount}</td>
					<td>${d.reachCount}</td>
					<td>${d.softBounceCount+domain.hardBounceCount}</td>
					<td>${d.readUserCount}</td>
					<td>${d.readCount}</td>
					<td>${d.clickUserCount}</td>
					<td>${d.clickCount}</td>
					<td>${d.unsubscribeCount}</td>
					<td>${d.forwardCount}</td>
					<td class="edit">
						<ul class="f_ul">
							<li><a href="javascript:customerTaskDetail('${ctx}/customerReport/list?corpId=${d.corpId}&userId=${d.userId}')">详细</a></li>
						</ul>
					</td>
				</tr>
			</c:forEach>
					
			</tbody>
		</table>
		
			<c:if test="${pagination.recordList==null||fn:length(pagination.recordList)<1}">
				<div class="no_data">没有匹配的账号</div>
			</c:if>
	
		
		<div class="ribbon clearfix">
			<%@ include file="/WEB-INF/jsp/common/page.jsp"%> 
		</div>
		
		<%--当前页--%>
		<input type="hidden" id="currentPage" name="currentPage" value="${pagination.currentPage}">
		<%--总页数--%>
		<input type="hidden" id="totalPageCount" value="${pagination.pageCount}">
	</form>
		<input type="hidden" name="msg" id="msg" value="${message}">
		<br>
		<br>
	</div>
  </body>
</html>
