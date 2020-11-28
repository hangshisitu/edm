<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>ip投递日志</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/delivery/delivery.log.js?${version}"></script>
  </head>
  
  <body>
	<div id="main" class="clearfix">
		<h1 class="title">
			ip投递日志
			<span></span>
		</h1>
		<form method="post" action="${ctx}/delivery/ipDeliveryLog" id="ListForm">
			<ul class="search clearfix m_b10">
				<li>ip</li>
				<li style="width:12.5%">
					<input maxlength="15" type="text" id="search_ip" name="search_ip" value="${search_ip}" />
				</li>
				<li>&nbsp;&nbsp; 时间</li>
				<li>
					<input type="text"   readonly="readonly" onfocus="WdatePicker()" value="${queryTime}" name="search_queryTime" id="d4311" class="date" maxlength="10" />
				</li>
				<li>
					<input type="button" class="groovybutton" value="查 询" title="查询" onclick="search()" />
				</li>
			</ul>
			<div class="ribbon clearfix">
				<div class="note">
					<p>共<font color="blue">${pagination.recordCount}</font>条记录</p>
				</div>
				<ul class="btn">
					<li>
						<input class="groovybutton" type="button" onclick="exportLog('${ctx}/delivery/exportLog')" title="导出" value="导出"/>
					</li>
				</ul>
			</div>
			<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
				<thead>
					<tr>
						<th width="25">
							<input id="all_checkbox" type="checkbox" />
						</th>
						<th>ip</th>
						<th>发件域</th>
						<th>收件域</th>
						<th>成功数</th>
						<th>失败数</th>
						<th>时间</th>	
					</tr>
				</thead>
				<tbody>
					
					<c:forEach items="${dataMap}" var="entry">
						<tr>
							<%--<td width="25" valign="top">
								<input type="checkbox" value="${entry.key}" name="checkedIds" style="margin-top:7px">
							</td>
							<td valign="top">${entry.key}</td>
							--%><td colspan="7" style="padding:0">
								<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table log_table">
									<c:forEach items="${entry.value.logList}" var="log" varStatus="status">
										<tr>
											<c:if test="${status.first==true}">
												<td width="25" valign="top">
													<input type="checkbox" value="${log.ip}" name="checkedIds" />
												</td>
												<td valign="top">${log.ip}</td>
											</c:if>
											
											<c:if test="${status.first==false}">
												<td width="25" valign="top">
													&nbsp;
												</td>
												<!-- <td valign="top">&nbsp;</td> -->
												<td valign="top">${log.ip}</td>
											</c:if>
											<td>
												<div>${log.fromDomain}</div>
											</td>
											<td>
												<div>${log.rcptDomain}</div>
											</td>	
											<td>
												<div>${log.success}</div>
											</td>	
											<td>
												<div>${log.failure}</div>
											</td>	
											<td>
												<div>${queryTime}</div>
											</td>	
										</tr>
									</c:forEach>
								</table>
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
		<input type="hidden" name="listUrl" id="listUrl" value="${ctx}/delivery/ipDeliveryLog">
		<div style="height:20px;"></div>
	</div>
  </body>
</html>
