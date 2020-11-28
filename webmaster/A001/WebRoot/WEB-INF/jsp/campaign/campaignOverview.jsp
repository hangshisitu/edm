<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>活动统计-概览</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/js/highcharts/highcharts.js"></script>
	<script type="text/javascript" src="${ctx}/resources/script/js/highcharts/modules/funnel.js"></script>
	<script type="text/javascript" src="${ctx}/resources/script/campaign/campaign.stat.overview.js?${version}"></script>
 	<script type="text/javascript">
 	
	var datas = [
		${domain.sentCount}, 
	    ${domain.reachCount},
	    ${domain.softBounceCount+domain.hardBounceCount},
	    ${domain.readUserCount},
	    ${domain.readCount},
	    ${domain.clickUserCount},
	    ${domain.clickCount},
	    ${domain.unsubscribeCount},
	    ${domain.forwardCount},
	    ${domain.afterNoneCount}
	];
	
	
	
	var nones = [
   		${domain.beforeNoneCount},
   		${domain.afterNoneCount}
   	];
   	
   	var a = datas[0] == 0 ? 0 : 100;
   	var b = (datas[0] - nones[1]) == 0 ? 0 : datas[1] * 100 / (datas[0]- nones[1]);
   	var c = datas[1] == 0 ? 0 : datas[3] * 100 / datas[1];
   	var d = datas[3] == 0 ? 0 : datas[5] * 100 / datas[3];
       	
   	var percents = [
   		["发送总数", a.toFixed(2)],
   		["成功总数", b.toFixed(2)],
   		["打开人数", c.toFixed(2)],
   		["点击人数", d.toFixed(2)]
   	];
	
  
	
 	</script>
 
  </head>
  
  <body>
    <div id="main" class="clearfix" style="position:relative">
		<h1 class="title">概览&nbsp;&nbsp;&nbsp;<span>提供活动整体投递数据的概要信息</span></h1>
		
		<form id="ListForm1" method="post">
		<ul class="tab clearfix">
			<li class="now"><a href="${ctx}/campaign/overview?campaignId=${campaign.campaignId}">概览</a></li>
			<li><a href="${ctx}/campaign/send?campaignId=${campaign.campaignId}">发送</a></li>
			<li><a href="${ctx}/campaign/open?campaignId=${campaign.campaignId}">打开</a></li>
			<li><a href="${ctx}/campaign/click?campaignId=${campaign.campaignId}">点击</a></li>
			<li><a href="${ctx}/campaign/domain?campaignId=${campaign.campaignId}">域名</a></li>
			<li><a href="${ctx}/campaign/region?campaignId=${campaign.campaignId}">地域</a></li>
			<%-- <li><a href="${ctx}/campaign/client?campaignId=${campaign.campaignId}">终端</a></li> 代码尚未完善，暂时先屏蔽--%> 
			<li><a href="${ctx}/campaign/delivery?campaignId=${campaign.campaignId}">历史</a></li>
			<li><a href="${ctx}/campaign/compare?campaignId=${campaign.campaignId}">对比</a></li>
			<c:if test="${campaign.touchCount gt 0}">
				<li><a href="${ctx}/campaign/touch?campaignId=${campaign.campaignId}">触发</a></li>
			</c:if>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
			</div>
			<ul class="btn">
				<li>
					<a id="cover_list" class="but_select" style="width:40px;" href="javascript:void(0)"></a>
					<input type="hidden" id="cover" value="column" />
				</li>
				<li><a class="btn_icon btn_icon_down" href="javascript:exportCsv('${ctx}/campaign/exportCsv?campaignId=${campaign.campaignId}&flag=overview');">导出</a></li>
			</ul>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th>活动名称</th>
					<th>活动描述</th>
					<th>投递次数</th>
					<th>更新时间</th>
				</tr>
				<tr>
					<td title="${campaign.campaignName}">${campaign.campaignName}</td>
					<td title="${campaign.campaignDesc}">${campaign.campaignDesc}</td>
					<td>${taskNum}</td>
					<td>${latestTime}</td>
				</tr>
			</tbody>
		</table>
		
		<c:if test="${domain.sentCount!=0 && domain!=null}">
			<div class='description'>
				<div class='success'>成功率：<span id="span_success"></span></div>
				<div class='open'>打开率：<span id="span_open"></span></div>
				<div class='click'>点击率：<span id="span_click"></span></div>
			</div>
			<div class="charts f_l">
				<div id="stat_column" style="width: 770px; height: 300px; margin: 0 auto;"></div>
				<div id="stat_funnel" style="width: 480px; height: 240px; margin: 0 auto; display: none;"></div>
			</div>
		</c:if>
		<c:if test="${domain.sentCount==0||domain==null}">
			<div class="no_data">暂无数据图显示</div>
		</c:if>
		
	<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th>发送总数</th>
					<th>成功数</th>
					<th>弹回数</th>
					<th>打开人数</th>
					<th>打开次数</th>
					<th>点击人数</th>
					<th>点击次数</th>
					<th>退订人数</th>
					<th>转发人数</th>
					<th>无效人数</th>
				</tr>
				<tr>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null}">${domain.sentCount}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null}">${domain.reachCount}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null}">${domain.softBounceCount+domain.hardBounceCount}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null}">${domain.readUserCount}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null}">${domain.readCount}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null}">${domain.clickUserCount}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null}">${domain.clickCount}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null}">${domain.unsubscribeCount}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null}">${domain.forwardCount}</c:if>
					</td>
					<td>
						<c:if test="${domain eq null}">0</c:if>
						<c:if test="${domain ne null}">${domain.afterNoneCount}</c:if>
					</td>
					
				</tr>
			</tbody>
		</table>
	
	</form>
		<input type="hidden" name="msg" id="msg" value="${message}">
		<br />
	<br />
	<br />
	</div>
	<div class="select_div"></div>
  </body>
</html>
