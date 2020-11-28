<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><html>
  <head>
    <title>任务统计-投递历史</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/js/highcharts/highcharts.js"></script>
	<script type="text/javascript" src="${ctx}/resources/script/report/plan/plan.report.history.js?${version}"></script>
 	<script type="text/javascript">
 	
		var datas = [
			${result.sentCount},
		    ${result.reachCount},
		    ${result.softBounceCount+result.hardBounceCount}
		];
		
		var backDatas = [
	   		${domain.softBounceCount}, 
	   	    ${domain.hardBounceCount},
	   	    ${domain.afterNoneCount},
	   	    ${domain.dnsFailCount},
	   	    ${domain.spanGarbageCount}
		];
		
		var total = backDatas[0]+backDatas[1];
		var address = total==0?0:backDatas[2]*100/total;
		var dns = total==0?0:backDatas[3]*100/total;
		var spam = total==0?0:backDatas[4]*100/total;
		var otherNum = total - backDatas[2]-backDatas[3]-backDatas[4];
		var other = total==0?0:otherNum*100/total;
	
	
		var response_chart=null;
		var response_datas=[{name:"邮箱无效",y:address,num:backDatas[2]},{name:"域名无效",y:dns,num:backDatas[3]},{name:"判定垃圾",y:spam,num:backDatas[4]},{name:"其他",y:other,num:otherNum}];
 	</script>
 
  </head>
  
  <body>
    <div id="main" class="clearfix" style="position:relative">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->发送</h1>
		
		<form id="ListForm" method="post">
			<ul class="tab clearfix">
				<li><a href="${ctx}/plan/planDetailStat/planReportView?planId=${task.planId}">概览</a></li>
				<li class="now"><a href="${ctx}/plan/planDetailStat/deliveryHistory?planId=${task.planId}">投递历史</a></li>
				<c:if test="${plan.touchCount > 0}">
					<li><a href="${ctx}/plan/planDetailStat/planReportTouch?planId=${task.planId}&templateId=${task.templateId}">触发</a></li>
				</c:if>
			</ul>
			<div class="ribbon clearfix">
				<div class="note">
					<p>共<span>${taskCount}</span>次投递</p>
				</div>
				<ul class="btn">
					<li><a class="btn_icon btn_icon_down" href="javascript:exportDeliveryHistory('${ctx}/plan/planDetailStat/exportDeliveryHistory?planId=${task.planId}');">导出</a></li>
				</ul>
			</div>
			<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
				<tbody>
					<tr>
						<th width="120">投递时间</th>
						<th>发送状态</th>
						<th>发送总数</th>
						<th>成功数</th>
						<th>弹回数</th>
						<th>打开人数</th>
						<th>打开次数</th>
						<th>点击人数</th>
						<th>点击次数</th>
						<th>成功率</th>
						<th>打开率</th>
						<th>点击率</th>
					</tr>
					<c:forEach items="${pagination.recordList}" var="r">
						<tr>
							<td><fmt:formatDate value="${r.deliveryTime}" pattern="yyyy-MM-dd HH:mm:ss" /></td>
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
							<td>${r.sentCount}</td>
							<td>${r.reachCount}</td>
							<td>${r.softBounceCount + r.hardBounceCount}</td>
							<td>${r.readUserCount}</td>
							<td>${r.readCount}</td>							
							<td>${r.clickUserCount}</td>
							<td>${r.clickCount}</td>
							<td>${r.resultReach}&nbsp;%</td>
							<td>${r.resultRead}&nbsp;%</td>
							<td>${r.resultClick}&nbsp;%</td>
						</tr>
					</c:forEach>
				</tbody>
			</table>
			<br />
			<br />
			<br />
		</form>
		<input type="hidden" name="msg" id="msg" value="${message}" />
		<br />
		<br />
		<br />
	</div>
  </body>
</html>
