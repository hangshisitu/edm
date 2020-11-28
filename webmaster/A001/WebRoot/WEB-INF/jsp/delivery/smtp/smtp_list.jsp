<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<title>SMTP列表</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
		<meta http-equiv="Content-Language" content="zh-cn" />
		<meta http-equiv="pragma" content="no-cache" />
		<meta http-equiv="cache-control" content="no-cache" />
		<meta http-equiv="expires" content="0" />
		<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
		<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
		<script type="text/javascript" src="${ctx}/resources/script/common/jquery.form-min.js?${version}"></script>
		<script type="text/javascript" src="${ctx}/resources/script/delivery/delivery.smtp.js?${version}"></script>
	</head>
	
	<body>
		<div id="main" class="clearfix">
			<h1 class="title">SMTP设置&nbsp;<span></span></h1>
			<form id="ListForm" action="${ctx}/delivery/smtp/smtpList" method="GET">
				<ul class="search clearfix m_b10">
					<li>IP</li>
					<li>
						<input maxlength="30" type="text" id="search_ip" name="search_ip" value="${search_ip}">
					</li>
					<li>
						<input type="button" class="groovybutton" value="查 询" title="查询" onclick="search()">
					</li>
				</ul>
				
				<div class="ribbon clearfix">
					<sec:authorize ifAnyGranted="ROLE_ADM">
						<ul class="btn">
							<input class="groovybutton" type="button" onclick="location.href='/delivery/smtp/add'" value="新建配置">
						</ul>
					</sec:authorize>
				</div>
				<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table nosis_table">
					<tbody>
						<tr>
							<th width="25%">ip</th>
							<th>15分钟频率</th>
							<th>1小时频率</th>
							<th>1天频率</th>
							<th>邮件大小</th>
							<th>操作</th>
						</tr>
						<c:forEach items="${pagination.recordList}" var="smtp" varStatus="i">
							<tr class="">
								<td>${smtp.ip}</td>
								<td>${smtp.quarter}</td>
								<td>${smtp.hour}</td>
								<td>${smtp.day}</td>
								<td>${smtp.size}</td>
								<td class="edit">
									<sec:authorize ifAnyGranted="ROLE_ADM">
										<ul class="f_ul">
											<li><a href="javascript:updateSmtp('/delivery/smtp/add?id=${smtp.id}');">修改</a></li>
											<li><a href="javascript:deleteSmtp('/delivery/smtp/delete?id=${smtp.id}');">删除</a></li>
										</ul>
									</sec:authorize>
								</td>
							</tr>
						</c:forEach>
					</tbody>
				</table>
				
				<c:if test="${pagination.recordList==null||fn:length(pagination.recordList)<1}">
					<div class="no_data">没有匹配的ip</div>
				</c:if>
			
				<div class="ribbon clearfix">
					<%@ include file="/WEB-INF/jsp/common/page.jsp"%> 
				</div>
				<%--当前页--%>
				<input type="hidden" id="currentPage" name="currentPage" value="${pagination.currentPage}">
				<%--总页数--%>
				<input type="hidden" id="totalPageCount" value="${pagination.pageCount}">
				<input type="hidden" name="msg" id="msg" value="${message}">
				<input type="hidden" name="token" value="${token}" />
			</form>
		<br>
		<br>
		<br>
	</div>
	</body>
</html>
