<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>客户发送统计</title>
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
	<script type="text/javascript" src="${ctx}/resources/script/report/customerTaskStatApiTriggerList.js?${version}"></script>
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
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->客户发送统计&nbsp;<span>当前客户：<font color="blue">${userId}</font></span></h1>
		
		<form id="ListForm1" action="${ctx}/customerReport/taskApiList" method="post">
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
				
				<li>触发时间</li>
				<li>
					<input maxlength="10" class="date" id="d4311"  name="search_beginTimeStr" value="${search_beginTimeStr}" class="Wdate" type="text" onFocus="WdatePicker({maxDate:'#F{$dp.$D(\'d4312\')}'})"/>
				</li>
				<li style="width:3%">到</li>
				<li>
					 <input maxlength="10"  class="date" id="d4312" name="search_endTimeStr" value="${search_endTimeStr}" class="Wdate" type="text" onFocus="WdatePicker({minDate:'#F{$dp.$D(\'d4311\')}'})"/>
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
					<%--<input type="button" class="groovybutton" value="重 置" title="重置" onclick="resetParam1()">
				--%></li>
			</ul>
			
		<ul class="tab clearfix">
			<li class="now"><a href="javascript:void(0)">发送总览</a></li>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p></p>
			</div>
			<ul class="btn">
				<li>
					<input type="button" class="groovybutton" value="返 回" title="返回" onclick="back('${ctx}/customerReport/apiTriggerList')">
				</li>
				
			</ul>
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
					<th>无效人数</th>
				</tr>
				<tr>
					<td><c:if test="${domain.sentCount!=null}">${domain.sentCount}</c:if><c:if test="${domain.sentCount==null}">0</c:if></td>
					<td><c:if test="${domain.reachCount!=null}">${domain.reachCount}</c:if><c:if test="${domain.reachCount==null}">0</c:if></td>
					<td>${domain.softBounceCount+domain.hardBounceCount}</td>
					<td><c:if test="${domain.readUserCount!=null}">${domain.readUserCount}</c:if><c:if test="${domain.readUserCount==null}">0</c:if></td>
					<td><c:if test="${domain.readCount!=null}">${domain.readCount}</c:if><c:if test="${domain.readCount==null}">0</c:if></td>
					<td><c:if test="${domain.clickUserCount!=null}">${domain.clickUserCount}</c:if><c:if test="${domain.clickUserCount==null}">0</c:if></td>
					<td><c:if test="${domain.clickCount!=null}">${domain.clickCount}</c:if><c:if test="${domain.clickCount==null}">0</c:if></td>
					<td><c:if test="${domain.unsubscribeCount!=null}">${domain.unsubscribeCount}</c:if><c:if test="${domain.unsubscribeCount==null}">0</c:if></td>
					<td><c:if test="${domain.forwardCount!=null}">${domain.forwardCount}</c:if><c:if test="${domain.forwardCount==null}">0</c:if></td>
					<td><c:if test="${domain.afterNoneCount!=null}">${domain.afterNoneCount}</c:if><c:if test="${domain.afterNoneCount==null}">0</c:if></td>
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
		<input type="hidden" name="userId" id="userId" value="${userId}">
		<input type="hidden" name="corpId" id="corpId" value="${corpId}">
	</form>
	<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->&nbsp;<span></span></h1>
		<form id="ListForm" action="${ctx}/customerReport/taskApiList" method="post">
			<%--<ul class="search clearfix m_b00 no_display">--%>
			<ul class="search clearfix m_b10">
				<li>任务名称</li>
				<li>
					<input maxlength="20" type="text" id="search_taskName" name="search_taskName" value="${search_taskName}">
				</li>
				<li>账号</li>
				<li>
					<input maxlength="10" type="text" id="search_userIdLike" name="search_userIdLike" value="${search_userIdLike}" >
				</li>
				<li>触发时间</li>
				<li>
					<input maxlength="10" class="date" id="d5311"  name="search_beginTimeStr1" value="${search_beginTimeStr1}" class="Wdate" type="text" onFocus="WdatePicker({maxDate:'#F{$dp.$D(\'d5312\')}'})"/>
				</li>
				<li style="width:3%">到</li>
				<li>
					 <input maxlength="10"  class="date" id="d5312" name="search_endTimeStr1" value="${search_endTimeStr1}" class="Wdate" type="text" onFocus="WdatePicker({minDate:'#F{$dp.$D(\'d5311\')}'})"/>
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
				<li>
					<input type="button" class="groovybutton" value="查 询" title="查询" onclick="search2()">
				</li>
			</ul>
			
		
		<ul class="tab clearfix">
			<li class="now"><a href="javascript:void(0)">任务统计</a></li>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p>共<span><font color="blue">${pagination.recordCount}</font></span>个</p>
			</div>
			<ul class="btn">
				<%--<li>
					<input type="button" class="groovybutton" value="对 比" title="对比" onclick="compare('${ctx}/taskDetailStat/compareApi')">
				</li>
				--%><li>
					<input type="button" class="groovybutton" value="导 出" title="导出" onclick="exportTaskStatNew('${ctx}/taskReport/exportTaskStatApiNew','${corpId}')">
				</li>
				<%--<li>
					<input type="button" class="groovybutton" value="打 印" title="打印" onclick="printTaskStat('${ctx}/taskReport/printTaskStatApi')">
				</li>
			--%></ul>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th width="25"><input type="checkbox" id="all_checkbox"/></th>
					<th>任务名称</th>
					<th>账号</th>
					<th>发送总数</th>
					<th>成功总数</th>
					<th>弹回总数</th>
					<th>打开人数</th>
					<th>打开次数</th>
					<th>点击人数</th>
					<th>点击次数</th>
					<th>退订总数</th>
					<th>转发总数</th>
					<th>无效人数</th>
					<th>触发时间</th>
					<th>操作</th>
				</tr>
				<c:forEach items="${pagination.recordList}" var="r" varStatus="i">
				<tr>
					<td><input type="checkbox" name="checkedIds" value="${r.taskId}" /></td>
					<td title="${r.taskName}">${r.taskName}</td>
					<td>${r.userId}</td>
					<td>${r.sentCount}</td>
					<td>${r.reachCount}</td>
					<td>${r.softBounceCount+r.hardBounceCount}</td>
					<td>${r.readUserCount}</td>
					<td>${r.readCount}</td>
					<td>${r.clickUserCount}</td>
					<td>${r.clickCount}</td>
					<td>${r.unsubscribeCount}</td>
					<td>${r.forwardCount}</td>
					<td>${r.afterNoneCount}</td>
					<td title="<fmt:formatDate value="${r.createTime}" pattern="yyyy-MM-dd HH:mm"/>">
						<fmt:formatDate value="${r.createTime}" pattern="yyyy-MM-dd"/>
					</td>
					<td class="edit">
						<ul class="f_ul">
<%-- 							<li><a href="javascript:view('${ctx}/task/viewApi?taskId=${r.taskId}&corpId=${r.corpId}&templateId=${r.templateId}')">查看</a></li> --%>
						</ul>
					</td>
				</tr>
			</c:forEach>
					
			</tbody>
		</table>
		
			<c:if test="${pagination.recordList==null||pagination.recordCount<1}">
				<div class="no_data">没有匹配的数据</div>
			</c:if>
	
		
		<div class="ribbon clearfix">
			<%@ include file="/WEB-INF/jsp/common/page.jsp"%> 
		</div>
		
		<%--当前页--%>
		<input type="hidden" id="currentPage" name="currentPage" value="${pagination.currentPage}">
		<%--总页数--%>
		<input type="hidden" id="totalPageCount" value="${pagination.pageCount}">
		<input type="hidden" name="userId" id="userId" value="${userId}">
		<input type="hidden" name="corpId" id="corpId" value="${corpId}">
	</form>
		<input type="hidden" name="msg" id="msg" value="${message}">
		<input type="hidden" name="listUrl" id="listUrl" value="${ctx}/customerReport/taskApiList">
		<br />
	<br />
	<br />
	</div>
  </body>
</html>
