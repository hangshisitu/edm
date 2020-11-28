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
	<script type="text/javascript" src="${ctx}/resources/script/report/getDateRange.js?${version}"></script>
	<script type="text/javascript" src="${ctx}/resources/script/report/customerStatList.js?${version}"></script>
 	
  </head>
  
  <body>
    <div id="main" class="clearfix" style="position:relative">
	<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->客户发送统计&nbsp;<span></span></h1>
		<form id="ListForm" action="${ctx}/customerReport/list" method="get">
			<%--<ul class="search clearfix m_b00 no_display">--%>
				<ul class="search clearfix m_b10">
				<li>机构名称</li>
				<li>
					<input maxlength="20" type="text" id="search_company" name="search_company" value="${search_company}">
				</li>
				<li>创建时间</li>
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
				<li>
					<input type="button" class="groovybutton" value="查 询" title="查询" onclick="search()">
				</li>
			</ul>
					
		<ul class="tab clearfix">
			<li class="now"><a href="${ctx}/customerReport/list">投递发送类</a></li>
			<li><a href="${ctx}/customerReport/apiTriggerList">API触发类</a></li>
			<li><a href="${ctx}/customerReport/apiGroupsStatList">API群发类</a></li>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p>共<span><font color="blue">${pagination.recordCount}</font></span>个</p>
			</div>
			<ul class="btn">
				<li>
					<input type="button" class="groovybutton" value="导 出" title="导出" onclick="exportStatNew('${ctx}/customerReport/exportUserStatNew')">
				</li>
				<li>
					<input type="button" class="groovybutton" value="打 印" title="打印" onclick="printStat('${ctx}/customerReport/printUserStat')">
				</li>
			</ul>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th width="25"><input type="checkbox" id="all_checkbox"/></th>
					<th>机构名称</th>
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
					<th>操作</th>
				</tr>
				<c:forEach items="${pagination.recordList}" var="d" varStatus="i">
				<tr>
					<td><input type="checkbox" name="checkedIds" value="${d.corpId}" /></td>
					<td title="${d.companyName}">${d.companyName}</td>
					<td>${d.sentCount}</td>
					<td>${d.reachCount}</td>
					<td>${d.softBounceCount+d.hardBounceCount}</td>
					<td>${d.readUserCount}</td>
					<td>${d.readCount}</td>
					<td>${d.clickUserCount}</td>
					<td>${d.clickCount}</td>
					<td>${d.unsubscribeCount}</td>
					<td>${d.forwardCount}</td>
					<td>${d.afterNoneCount}</td>
					<td class="edit">
						<ul class="f_ul">
							<li><a href="javascript:customerTaskDetail('${ctx}/customerReport/taskList?corpId=${d.corpId}')">查看</a></li>
						</ul>
					</td>
				</tr>
			</c:forEach>
					
			</tbody>
		</table>
		
			<c:if test="${pagination.recordList==null||fn:length(pagination.recordList)<1}">
				<div class="no_data">没有匹配的数据</div>
			</c:if>
	
		
		<div class="ribbon clearfix">
			<%@ include file="/WEB-INF/jsp/common/page.jsp"%> 
		</div>
		
		<%--当前页--%>
		<input type="hidden" id="currentPage" name="currentPage" value="${pagination.currentPage}">
		<%--总页数--%>
		<input type="hidden" id="totalPageCount" value="${pagination.pageCount}">
		<!-- 0，1 单项与测试任务 -->
		<input type="hidden" name="typeStr" id="typeStr" value="0,1,3">
	</form>
		<input type="hidden" name="msg" id="msg" value="${message}">
		<input type="hidden" name="listUrl" id="listUrl" value="${ctx}/customerReport/list">
		<br />
	<br />
	<br />
	</div>
  </body>
</html>
