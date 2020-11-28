<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><html>
  <head>
    <title>活动统计-发送</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/js/highcharts/highcharts.js"></script>
	<script type="text/javascript" src="${ctx}/resources/script/campaign/campaign.stat.send.js?${version}"></script>
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
   	    ${domain.spamGarbageCount}
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
		<h1 class="title">发送&nbsp;&nbsp;&nbsp;<span>针对发送成功率和弹回原因提供图表分析</span></h1>
		
		<form id="ListForm" method="post">
		<ul class="tab clearfix">
			<li><a href="${ctx}/campaign/overview?campaignId=${campaign.campaignId}">概览</a></li>
			<li class="now"><a href="${ctx}/campaign/send?campaignId=${campaign.campaignId}">发送</a></li>
			<li><a href="${ctx}/campaign/open?campaignId=${campaign.campaignId}">打开</a></li>
			<li><a href="${ctx}/campaign/click?campaignId=${campaign.campaignId}">点击</a></li>
			<li><a href="${ctx}/campaign/domain?campaignId=${campaign.campaignId}">域名</a></li>
			<li><a href="${ctx}/campaign/region?campaignId=${campaign.campaignId}">地域</a></li>
			<%-- <li><a href="${ctx}/campaign/client?campaignId=${campaign.campaignId}">终端</a></li> --%>
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
				<li><a class="btn_icon btn_icon_down" href="javascript:exportCsv('${ctx}/campaign/exportCsv?campaignId=${campaign.campaignId}&flag=send');">导出</a></li>
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
		<div style="width:800px;margin:0 auto;">
		
			<div class="charts f_l" style="width:320px">
				<c:if test="${result.sentCount!=0 && result!=null}">
					<div id="stat_column" style="width: 290px; height: 300px; margin: 0 auto;"></div>
				</c:if>
				<c:if test="${result.sentCount==0||result==null}">
					<div style="width: 290px; height: 300px; margin: 0 auto;">暂无数据图显示</div>
				</c:if>
			</div>
			<div class="charts f_l" style="width:480px">
				<c:if test="${(domain.softBounceCount+domain.hardBounceCount)!=0}">
					<div id="stat_response_pie" style="width: 480px; height: 300px; margin: 0 auto;"></div>
				</c:if>
				<c:if test="${(domain.softBounceCount+domain.hardBounceCount)==0}">
					<div style="width: 480px; height: 300px; margin: 0 auto;">暂无数据图显示</div>
				</c:if>
			</div>
		</div>
		
		
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table" style="position:relative">
			<tbody>
				<tr>
					<th>弹回详情</th>
					<th>弹回数</th>
					<th>占比</th>
				</tr>
				<tr>
					<td>
						<div class="tips_trg" >
                   			邮箱无效
							<div class="global_tips">
								<p>域名存在，账户地址不存在</p>
								<div class="triangle-border top-triangle-border">
							        <div class="triangle top-triangle"></div>
							    </div>								
							</div>
               			</div>
					</td>
					<td> 
						<c:if test="${domain.afterNoneCount==null}">0</c:if>
						<c:if test="${domain.afterNoneCount!=null}">
							${domain.afterNoneCount}
						</c:if>
					</td>
					<td>
						<c:if test="${(domain.softBounceCount+domain.hardBounceCount)==0}">0.00%</c:if>
						<c:if test="${(domain.softBounceCount+domain.hardBounceCount)!=0}">
							<fmt:formatNumber value="${domain.afterNoneCount/(domain.softBounceCount+domain.hardBounceCount)*100}" pattern="#.##"/>%
						</c:if>
						
					</td>
				</tr>
				<tr>
					<td>
						<div class="tips_trg" >
                   			域名无效
							<div class="global_tips">
								<p>DNS解析错误，域名不存在</p>
								<div class="triangle-border top-triangle-border">
							        <div class="triangle top-triangle"></div>
							    </div>								
							</div>
               			</div>
					</td>
					<td>
						<c:if test="${domain.dnsFailCount==null}">0</c:if>
						<c:if test="${domain.dnsFailCount!=null}">
							${domain.dnsFailCount}
						</c:if>
					</td>
					<td>
						<c:if test="${(domain.softBounceCount+domain.hardBounceCount)==0}">0.00%</c:if>
						<c:if test="${(domain.softBounceCount+domain.hardBounceCount)!=0}">
							<fmt:formatNumber value="${domain.dnsFailCount/(domain.softBounceCount+domain.hardBounceCount)*100}" pattern="#.##"/>%
						</c:if>
						
					</td>
				</tr>
				<tr>
					<td>
						<div class="tips_trg" >
                   			判定垃圾
                   			<div class="global_tips">
								<p>被ISP判定为垃圾弹回</p>
								<div class="triangle-border top-triangle-border">
							        <div class="triangle top-triangle"></div>
							    </div>								
							</div>
               			</div>
					</td>
					<td> 
						<c:if test="${domain.spamGarbageCount==null}">0</c:if>
						<c:if test="${domain.spamGarbageCount!=null}">
							${domain.spamGarbageCount}
						</c:if>
					</td>
					<td>
						<c:if test="${(domain.softBounceCount+domain.hardBounceCount)==0}">0.00%</c:if>
						<c:if test="${(domain.softBounceCount+domain.hardBounceCount)!=0}">
							<fmt:formatNumber value="${domain.spamGarbageCount/(domain.softBounceCount+domain.hardBounceCount)*100}" pattern="#.##"/>%
						</c:if>
						
					</td>
				</tr>
				<tr>
					<td>
						<div class="tips_trg" >
                   			其他
                   			<div class="global_tips">
								<p>其他原因统一归为此类</p>
								<div class="triangle-border top-triangle-border">
							        <div class="triangle top-triangle"></div>
							    </div>								
							</div>
               			</div>
					</td>
					<td> ${domain.softBounceCount+domain.hardBounceCount-domain.afterNoneCount-domain.dnsFailCount-domain.spamGarbageCount}</td>
					<td>
						<c:if test="${(domain.softBounceCount+domain.hardBounceCount)==0}">0.00%</c:if>
						<c:if test="${(domain.softBounceCount+domain.hardBounceCount)!=0}">
							<fmt:formatNumber value="${(domain.softBounceCount+domain.hardBounceCount-domain.afterNoneCount-domain.dnsFailCount-domain.spamGarbageCount)/(domain.softBounceCount+domain.hardBounceCount)*100}" pattern="#.##"/>%
						</c:if>
						
					</td>
				</tr>
			</tbody>
		</table>
		<br />
	<br />
	<br />
	</form>
		<input type="hidden" name="msg" id="msg" value="${message}">
	<br />
	<br />
	<br />
	</div>
  </body>
</html>
