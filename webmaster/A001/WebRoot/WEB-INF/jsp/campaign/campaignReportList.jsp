<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>活动统计</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/campaign/campaignReportList.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="position:relative">
	<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->活动发送统计&nbsp;<span></span></h1>
		<form id="ListForm" action="${ctx}/campaign/list" method="get">
			<%--<ul class="search clearfix m_b00 no_display">--%>
				<ul class="search clearfix m_b10">
				<li>活动名称</li>
				<li>
					<input maxlength="20" type="text" id="search_campaignName" name="search_campaignName" value="${search_campaignName}">
				</li>
				
				<li>
					<input type="button" class="groovybutton" value="查 询" title="查询" onclick="search()">
				</li>
			</ul>
			
		<ul class="tab clearfix">
			
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p>共<span><font color="blue">${pagination.recordCount}</font></span>个活动</p>
			</div>
			<ul class="btn">
				
			</ul>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th>活动名称</th>
					<th>任务数</th>
					<th>发送总数</th>
					<th>成功数</th>
					<th>打开人数</th>
					<th>点击人数</th>
					<th>退订人数</th>
					<th>无效人数</th>
					<th>成功率</th>
					<th>打开率</th>
					<th>点击率</th>
					<th style="width:13%">操作</th>
				</tr>
				<c:forEach items="${pagination.recordList}" var="r" varStatus="i">
				<tr>
					<td title="${r.campaignName}">
						<c:if test="${r.campaignName eq null}">0</c:if>
						<c:if test="${r.campaignName ne null}"><font color="blue">${r.campaignName}</font></c:if>
					</td>
					<td>
						<c:if test="${r.taskNum eq null}">0</c:if>
						<c:if test="${r.taskNum ne null}">${r.taskNum}</c:if>
					</td>
					<td>
						<c:if test="${r.sentCount eq null}">0</c:if>
						<c:if test="${r.sentCount ne null}">${r.sentCount}</c:if>
					</td>
					<td>
						<c:if test="${r.reachCount eq null}">0</c:if>
						<c:if test="${r.reachCount ne null}">${r.reachCount}</c:if>
					</td>
					<td>
						<c:if test="${r.readUserCount eq null}">0</c:if>
						<c:if test="${r.readUserCount ne null}">${r.readUserCount}</c:if>
					</td>
					<td>
						<c:if test="${r.clickUserCount eq null}">0</c:if>
						<c:if test="${r.clickUserCount ne null}">${r.clickUserCount}</c:if>
					</td>
					<td>
						<c:if test="${r.unsubscribeCount eq null}">0</c:if>
						<c:if test="${r.unsubscribeCount ne null}">${r.unsubscribeCount}</c:if>
					</td>
					<td>
						<c:if test="${r.afterNoneCount eq null}">0</c:if>
						<c:if test="${r.afterNoneCount ne null}">${r.afterNoneCount}</c:if>
					</td>
					<td>
						<c:if test="${r.sentCount-r.afterNoneCount==0}">0.00%</c:if>
						<c:if test="${r.sentCount-r.afterNoneCount!=0}">
							<fmt:formatNumber value="${r.reachCount/(r.sentCount-r.afterNoneCount)*100}" pattern="#.##"/>%
						</c:if>
					</td>
					<td>
						<c:if test="${r.reachCount==0}">0.00%</c:if>
						<c:if test="${r.reachCount!=0}">
							<fmt:formatNumber value="${r.readUserCount/r.reachCount*100}" pattern="#.##"/>%
						</c:if>
					</td>
					<td>
						<c:if test="${r.readUserCount==0}">0.00%</c:if>
						<c:if test="${r.readUserCount!=0}">
							<fmt:formatNumber value="${r.clickUserCount/r.readUserCount*100}" pattern="#.##"/>%
						</c:if>
					</td>
					<td class="edit">
						<ul class="f_ul">
							<li><a href="javascript:detail('${ctx}/campaign/overview?campaignId=${r.campaignId}')">报告</a></li>
							
							<li><a href="javascript:exportCsv('${ctx}/campaign/exportCsv?flag=all&campaignId=${r.campaignId}')">导出</a></li>
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
		<input type="hidden" name="listUrl" id="listUrl" value="${ctx}/campaign/list">
		
		<br />
	<br />
	<br />
	</div>
  </body>
</html>
