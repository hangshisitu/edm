<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>任务发送统计信息</title>
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
		<h1>任务发送统计信息-API触发</h1>
		<table width="100%" border="0" cellspacing="0">
				<tr>
					<th class="thl" width="30"><div>编号</div></th>
					<th><div>任务名称</div></th>
					<th><div>发送总数</div></th>
					<th><div>成功总数</div></th>
					<th><div>弹回总数</div></th>
					<th><div>打开人数</div></th>
					<th><div>打开次数</div></th>
					<th><div>点击人数</div></th>
					<th><div>点击次数</div></th>
					<th><div>退订总数</div></th>
					<th><div>转发总数</div></th>
					<th style="width:20%">时间</th>
				</tr>
				<c:forEach items="${resultList}" var="r" varStatus="i">
				<tr>
					<td><span class="num">${i.index+1}</span></td>
					<td><span class="num">${r.taskName}</span></td>
					<td><span class="num">${r.sentCount}</span></td>
					<td><span class="num">${r.reachCount}</span></td>
					<td><span class="num">${r.softBounceCount+r.hardBounceCount}</span></td>
					<td><span class="num">${r.readUserCount}</span></td>
					<td><span class="num">${r.readCount}</span></td>
					<td><span class="num">${r.clickUserCount}</span></td>
					<td><span class="num">${r.clickCount}</span></td>
					<td><span class="num">${r.unsubscribeCount}</td>
					<td><span class="num">${r.forwardCount}</td>
					
					<td>
						<c:if test="${r.createTime ne null}">
							<fmt:formatDate value="${r.createTime}" pattern="yyyy-MM-dd HH:mm"/>[触发时间]<br>
						</c:if>
					</td>
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
