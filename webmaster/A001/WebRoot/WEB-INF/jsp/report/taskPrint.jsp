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
		<h1>任务发送统计信息</h1>
		<table width="100%" border="0" cellspacing="0" style="table-layout:fixed;">
				<tr>
					<th class="thl" width="30"><div>编号</div></th>
					<th style="width:10%"><div>客户账号</div></th>
					<th style="width:15%"><div>公司名称</div></th>
					<th><div>发送总数</div></th>
					<th><div>成功总数</div></th>
					<th><div>弹回总数</div></th>
					<th><div>打开人数</div></th>
					<th><div>打开次数</div></th>
					<th><div>点击人数</div></th>
					<th><div>点击次数</div></th>
					<th><div>退订总数</div></th>
					<th><div>转发总数</div></th>
					<th><div>无效人数</div></th>
					<th><div>状态</div></th>
					<th style="width:20%">时间</th>
				</tr>
				<c:forEach items="${resultList}" var="r" varStatus="i">
					<tr>
						<td><span class="num">${i.index+1}</span></td>
						<td style="word-break: break-all; word-wrap:break-word;"><span class="num">${r.userId}</span></td>
						<td style="word-break: break-all; word-wrap:break-word;"><span class="num">${r.companyName}</span></td>
						<td><span class="num">${r.sentCount}</span></td>
						<td><span class="num">${r.reachCount}</span></td>
						<td><span class="num">${r.softBounceCount+r.hardBounceCount}</span></td>
						<td><span class="num">${r.readUserCount}</span></td>
						<td><span class="num">${r.readCount}</span></td>
						<td><span class="num">${r.clickUserCount}</span></td>
						<td><span class="num">${r.clickCount}</span></td>
						<td><span class="num">${r.unsubscribeCount}</span></td>
						<td><span class="num">${r.forwardCount}</span></td>
						<td><span class="num">${r.afterNoneCount}</span></td>
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
						<td>
							<c:if test="${r.createTime ne null}">
								<fmt:formatDate value="${r.createTime}" pattern="yyyy-MM-dd HH:mm"/>[创建时间]<br>
							</c:if>
							<c:if test="${r.beginSendTime ne null}">
								<fmt:formatDate value="${r.beginSendTime}" pattern="yyyy-MM-dd HH:mm"/>[开始发送]<br>
							</c:if>
							<c:if test="${r.endSendTime ne null}">
								<fmt:formatDate value="${r.endSendTime}" pattern="yyyy-MM-dd HH:mm"/>[结束发送]
							</c:if>
						</td>
					</tr>
				</c:forEach>
					
		</table>
		<div id="print">
			<a href="javascript:void(0)" onclick="window.print();">确定打印</a>
			<a href="javascript:void(0)" onclick="window.close();">关闭</a>
		</div>
		<br/>
	<br/>
	<br/>
	</div>
  </body>
</html>
