<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>客户发送统计信息-API触发</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<link rel="stylesheet" type="text/css" href="${ctx}/resources/css/print.css" />
 	<style media="print">
	#print{
		display:none;
	}
	</style>
  </head>
  
  <body>
    <div class="a4" align="center">
		<h1>客户发送统计-API触发</h1>
		<table width="100%" border="0" cellspacing="0">
			<tr>
				<th>客户账号</th>
				<th>触发类型</th>
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
			<c:forEach items="${domainList}" var="d" varStatus="i">
				<tr>
					<td class="td1"><span class="a_link">${d.userId}</span></td>
					<td><span class="a_link">${d.triggerName}</span></td>
					<td><span class="num">${d.sentCount}</span></td>
					<td><span class="num">${d.reachCount}</span></td>
					<td><span class="num">${d.softBounceCount+d.hardBounceCount}</span></td>
					<td><span class="num">${d.readUserCount}</span></td>
					<td><span class="num">${d.readCount}</span></td>
					<td><span class="num">${d.clickUserCount}</span></td>
					<td><span class="num">${d.clickCount}</span></td>
					<td><span class="num">${d.unsubscribeCount}</td>
					<td><span class="num">${d.forwardCount}</td>
				</tr>
			</c:forEach>
					
		</table>
		<div id="print">
			<a href="javascript:void(0)" onclick="window.print();">确定打印</a>
			<a href="javascript:void(0)" onclick="window.close();">关闭</a>
		</div>
		<br />
	<br />
	<br />
	</div>
  </body>
</html>
