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
	<script type="text/javascript" src="${ctx}/resources/script/report/customerStatApiTriggerList.js?${version}"></script>
 	
  </head>
  
  <body>
    <div id="main" class="clearfix" style="position:relative">
	<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->客户发送统计&nbsp;<span></span></h1>
		<form id="ListForm" action="${ctx}/customerReport/apiTriggerList" method="get">
			<%--<ul class="search clearfix m_b00 no_display">--%>
			<ul class="search clearfix m_b10">
				<li>客户账号</li>
				<li>
					<input maxlength="20" type="text" id="search_userIdLike" name="search_userIdLike" value="${search_userIdLike}">
				</li>
<!-- 				<li>触发类型</li> -->
<!-- 				<li style="width:10%"> -->
<!-- 					<select id="search_triggerName" -->
<!-- 							name="search_triggerName" style="width:100%;background-color: #F5F5F5"> -->
<!-- 								<<option value="">全部</option> -->
<%-- 								<option <c:if test="${search_triggerName eq '注册验证'}">selected="selected"</c:if> value="注册验证">注册验证</option> --%>
<%-- 								<option <c:if test="${search_triggerName eq '注册成功'}">selected="selected"</c:if> value="注册成功">注册成功</option> --%>
<%-- 								<option <c:if test="${search_triggerName eq '订单确认'}">selected="selected"</c:if> value="订单确认">订单确认</option> --%>
<%-- 								<option <c:if test="${search_triggerName eq '成功下单'}">selected="selected"</c:if> value="成功下单">成功下单</option> --%>
<%-- 								<c:forEach items="${triggerNameList}" var="triggerName"> --%>
<%-- 									<option <c:if test="${search_triggerName eq triggerName}">selected="selected"</c:if> value="${triggerName}">${triggerName}</option> --%>
<%-- 								</c:forEach> --%>
<!-- 					</select> -->
<!-- 				</li> -->
				<li>创建时间</li>
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
					<input type="button" class="groovybutton" value="查 询" title="查询" onclick="search()">
				</li>
			</ul>
			
		
		<ul class="tab clearfix">
			<li><a href="${ctx}/customerReport/list">投递发送类</a></li>
			<li class="now"><a href="${ctx}/customerReport/apiTriggerList">API触发类</a></li>
			<li><a href="${ctx}/customerReport/apiGroupsStatList">API群发类</a></li>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p>共<span><font color="blue">${pagination.recordCount}</font></span>个</p>
			</div>
			<ul class="btn">
				<li>
					<input type="button" class="groovybutton" value="导 出" title="导出" onclick="exportStatApiNew('${ctx}/customerReport/exportUserStatApiNew')">
				</li>
				<%--<li>
					<input type="button" class="groovybutton" value="打 印" title="打印" onclick="printStatApi('${ctx}/customerReport/printStatApi')">
				</li>
			--%></ul>
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
				<c:forEach items="${pagination.recordList}" var="r" varStatus="i">
				<tr>
					<td><input type="checkbox" name="checkedIds" value="${r.corpId}" /></td>
					<td title="${r.companyName}">${r.companyName}</td>
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
					<td class="edit">
						<ul class="f_ul">
							<li><a href="javascript:customerTaskDetail('${ctx}/customerReport/taskApiList?corpId=${r.corpId}')">查看</a></li>
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
	</form>
		<input type="hidden" name="msg" id="msg" value="${message}">
		<input type="hidden" name="listUrl" id="listUrl" value="${ctx}/customerReport/apiTriggerList">
		<br />
	<br />
	<br />
	</div>
  </body>
</html>
