<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>任务发送统计</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/report/getDateRange.js?${version}"></script>
	<script type="text/javascript" src="${ctx}/resources/script/report/taskReportList.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="position:relative">
	<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->任务发送统计&nbsp;<span></span></h1>
		<form id="ListForm" action="${ctx}/taskReport/list" method="get">
			<%--<ul class="search clearfix m_b00 no_display">--%>
			<ul class="search clearfix m_b00">
				<li>任务名称</li>
				<li>
					<input maxlength="20" type="text" id="search_taskName" name="search_taskName" value="${search_taskName}">
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
					<select id="search_cycle" onchange="getDateRange(this.value,'d5311','d5312')"
							name="search_cycle" style="width:100%;background-color: #F5F5F5">
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
			<ul class="search clearfix m_b10">
				<%-- <li>任务ID&nbsp;&nbsp;&nbsp;</li>
				<li>
					<input maxlength="25" type="text" id="search_comTaskId" name="search_comTaskId" value="${search_comTaskId}">
				</li> --%>
				<li>机构名称</li>
				<li>
					<input maxlength="25" type="text" id="search_corpPath" name="search_corpPath" value="${search_corpPath}">
				</li>
				<c:if test="${hrCatalog == true}">
				<li>数据状态</li>				
				<li style="width:10%">
					<select id="search_labelStatus" name="search_labelStatus" style="width:100%;background-color: #F5F5F5">
								<option value="">全部</option>
								<option <c:if test="${search_labelStatus==0}">selected="selected"</c:if> value="0">未导入</option>
								<option <c:if test="${search_labelStatus==2}">selected="selected"</c:if> value="2">处理中</option>
								<option <c:if test="${search_labelStatus==1}">selected="selected"</c:if> value="1">已导入</option>
								<option <c:if test="${search_labelStatus==3}">selected="selected"</c:if> value="3">导入失败</option>
					</select>
				</li>
				</c:if>
			</ul>
			
		
		<ul class="tab clearfix">
			<li class="now"><a href="${ctx}/taskReport/list?type=0">单项任务</a></li>
			<li><a href="${ctx}/taskReport/list?type=1">测试任务</a></li>
<%-- 			<li><a href="${ctx}/taskReport/apiList">API触发</a></li> --%>
			<li><a href="${ctx}/plan/planReport/reportList">周期任务</a></li>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p>共<span><font color="blue">${pagination.recordCount}</font></span>个</p>
			</div>
			<ul class="btn">
				<li>
					<input type="button" class="groovybutton" value="对 比" title="对比" onclick="compareTask('${ctx}/taskDetailStat/compareTask')">
				</li>
				<li>
					<input type="button" class="groovybutton" value="导 出" title="导出" onclick="exportTaskStatNew('${ctx}/taskReport/exportTaskStatNew?type=0')">
				</li>
				<li>
					<input type="button" class="groovybutton" value="打 印" title="打印" onclick="printTaskStat('${ctx}/taskReport/printTaskStat')">
				</li>
			</ul>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th width="25"><input type="checkbox" id="all_checkbox"/></th>
					<th>任务名称</th>
					<th>机构名称</th>
					<th>发送总数</th>
					<th>成功数</th>
					<th>弹回数</th>
					<th>打开人数</th>
					<th>打开次数</th>
					<th>点击人数</th>
					<th>点击次数</th>
					<th>转发总数</th>
					<th>无效人数</th>
					<th>触发计划</th>
					<th>任务状态</th>
					<c:if test="${hrCatalog == true}">
					<th>数据状态</th>
					</c:if>
					<th style="width:13%">操作</th>
				</tr>
				<c:forEach items="${pagination.recordList}" var="r" varStatus="i">
					<tr>
						<td><input type="checkbox" name="checkedIds" value="${r.taskId}" /></td>
						<td title="${r.taskName}">${r.taskName}</td>
						<td title="${r.corpPath}">${r.corpPath}</td>
						<td>${r.sentCount}</td>
						<td>${r.reachCount}</td>
						<td>${r.softBounceCount+r.hardBounceCount}</td>
						<td>${r.readUserCount}</td>
						<td>${r.readCount}</td>
						<td>${r.clickUserCount}</td>
						<td>${r.clickCount}</td>
						<%--<td>${r.unsubscribeCount}</td>
						--%><td>${r.forwardCount}</td>
						<td>${r.afterNoneCount}</td>
						<td>
							<c:if test="${r.touchCount gt 0 }">有</c:if>
							<c:if test="${r.touchCount eq 0 }">无</c:if>
						</td>
						<td>
							<c:if test="${r.status eq 0 }">草稿</c:if>
							<c:if test="${r.status eq 11 }">待批示</c:if>
							<c:if test="${r.status eq 12 }">批示不通过</c:if>
							<c:if test="${r.status eq 13 }">待审核</c:if>
							<c:if test="${r.status eq 14 }">审核不通过</c:if>
							<c:if test="${r.status eq 21 }">测试发送</c:if>
							<c:if test="${r.status eq 22 }">待发送</c:if>
							<c:if test="${r.status eq 23 }">队列中</c:if>
							<c:if test="${r.status eq 24 }">发送中</c:if>
							<c:if test="${r.status eq 25 }">已暂停</c:if>
							<c:if test="${r.status eq 26 }">已取消</c:if>
							<c:if test="${r.status eq 27 }">已完成</c:if>
						</td>
						<c:if test="${hrCatalog == true}">
						<td>
							<c:if test="${r.labelStatus eq 0 || r.labelStatus eq null}">未导入</c:if>
							<c:if test="${r.labelStatus eq 1 }">已导入</c:if>
							<c:if test="${r.labelStatus eq 2 }">处理中</c:if>
							<c:if test="${r.labelStatus eq 3 }">导入失败</c:if>
							
						</td>
						</c:if>
						<td class="edit">
							<ul class="f_ul">
								<li><a href="javascript:view('${ctx}/task/view?taskId=${r.taskId}')">查看</a></li>
								<li><a href="javascript:detail('${ctx}/taskDetailStat/overview?taskId=${r.taskId}')">报告</a></li>
								<li><a href="javascript:exportOneTaskStat('${ctx}/taskReport/exportTaskStat?checkedIds=${r.taskId}')">导出</a></li>
								
								<c:if test="${(r.labelStatus eq 0 || r.labelStatus eq null|| r.labelStatus eq 3)&&(r.isCanImport eq 'Y')&&(hrCatalog == true)}">
									<li><a href="javascript:importPropLib('${ctx}/label/import?taskId=${r.taskId}')">导入</a></li>
								</c:if>
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
		<input type="hidden" name="type" id="type" value="${type}">
		<input type="hidden" name="fromFlag" id="fromFlag" value="task">
	</form>
		<input type="hidden" name="msg" id="msg" value="${message}">
		<input type="hidden" name="listUrl" id="listUrl" value="${ctx}/taskReport/list">
		<br />
	<br />
	<br />
	</div>
  </body>
</html>
