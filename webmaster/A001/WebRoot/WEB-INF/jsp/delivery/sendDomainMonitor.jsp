<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>发件域投递监控</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/delivery/delivery.monitor.js?${version}"></script>
	
  </head>
  
  <body>
    <div id="main" class="clearfix" style="position:relative">
		<h1 class="title">
			发件域投递监控
			<span></span>
		</h1>
		<form method="post" action="${ctx}/delivery/sendDomainMonitor" id="ListForm">
			<ul class="search clearfix m_b10">
				<li>发件域</li>
				<li style="width:12.5%">
					<input maxlength="64" type="text" id="search_fromDomain" name="search_fromDomain" value="${search_fromDomain}">
				</li>
				<li>状态选择</li>
				<li style="width:12.5%">
					<select style="width:100%;background-color: #F5F5F5" name="search_status" id="search_status">
							<option value="">请选择</option>
							<option value="normal" <c:if test="${search_status eq 'normal'}">selected="selected"</c:if>>正常</option>
							<option value="stop" <c:if test="${search_status eq 'stop'}">selected="selected"</c:if>>超限停止</option>
					</select>
				</li>
				<li>
					<input type="button" class="groovybutton" value="查 询" title="查询" onclick="search()" />
				</li>
				<li>	
					<input type="button" class="groovybutton" value="重 置" title="重置" onclick="resetParam()" />
				</li>
			</ul>
			<div class="ribbon clearfix">
				<div class="note">
					<p>共<font color="blue">${pagination.recordCount}</font>个发件域投递中</p>
				</div>
				<ul class="btn">
					<li>
						<input class="groovybutton" type="button" onclick="openUrl('${ctx}/delivery/ruleSet')" title="收件域规则配置" value="收件域规则配置">
					</li>
					<li>
						<input class="groovybutton" type="button" onclick="openUrl('${ctx}/delivery/ruleQuery')" title="发件域配置查询" value="发件域配置查询">
					</li>
				</ul>
			</div>
			<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table delevery_table" style="position:relative">
				<thead>
					<tr>
						<th>发件域</th>
						<th>收件域</th>
						<th>投递规则</th>
						<th>状态</th>
						<th>状态查询时间</th>
					</tr>
				</thead>
				<tbody>
					<c:forEach items="${pagination.recordList}" var="d" varStatus="i">
						<tr>
						<td>${d.fromDomain}</td>
						<td>${d.rcptDomain}</td>
						<td>
							<div class="slide_trg">
								<a class="list_select" href="javascript:void(0)">${d.qvList[0].rule}</a>
								<div class="slide_info">
									<ul>
										<%--<li data-state="${d.quarterStatus}">${d.quarter}</li>
										<li data-state="${d.hourStatus}">${d.hour}</li>
										<li data-state="${d.dayStatus}">${d.day}</li>
										--%>
										<c:forEach items="${d.qvList}" var="qv" varStatus="qvi">
											<li data-state="${qv.status}">${qv.rule}</li>
										</c:forEach>
										
									</ul>
								</div>
							</div>
						</td>
						<td><span  <c:if test="${d.qvList[0].status=='正常'}">class="green"</c:if><c:if test="${d.qvList[0].status =='超限停止'}">class="red"</c:if>>${d.qvList[0].status}<span></td>
						<td>
							<fmt:formatDate value="${d.queryTime}" pattern="yyyy-MM-dd HH:mm"/>
						</td>
					</tr>
					</c:forEach>
					
					
				</tbody>		
			</table>
			<c:if test="${pagination.recordCount==0}">
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
		<br>
		<br>
		<br>
		<div style="height:20px;"></div>
	</div>
  </body>
</html>
