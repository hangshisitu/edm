<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><html>
  <head>
    <title>活动统计-用户端</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/js/highcharts/highcharts.js"></script>
	<script type="text/javascript" src="${ctx}/resources/script/campaign/campaign.stat.client.js?${version}"></script>
 	<script type="text/javascript">
 	var browser_names = [
				[0, "${browser['0'].name}", ${browser['0'].openCount}, ${browser['0'].openRate}],
				[1, "${browser['1'].name}", ${browser['1'].openCount}, ${browser['1'].openRate}],
				[2, "${browser['2'].name}", ${browser['2'].openCount}, ${browser['2'].openRate}],
				[3, "${browser['3'].name}", ${browser['3'].openCount}, ${browser['3'].openRate}],
				[4, "${browser['4'].name}", ${browser['4'].openCount}, ${browser['4'].openRate}],
				[5, "${browser['5'].name}", ${browser['5'].openCount}, ${browser['5'].openRate}],
				[6, "${browser['6'].name}", ${browser['6'].openCount}, ${browser['6'].openRate}],
				[7, "${browser['7'].name}", ${browser['7'].openCount}, ${browser['7'].openRate}],
				[8, "${browser['8'].name}", ${browser['8'].openCount}, ${browser['8'].openRate}]
			];
	var browser_datas = [
			[0,${browser['0'].openRate}],
			[1,${browser['1'].openRate}],
			[2,${browser['2'].openRate}],
			[3,${browser['3'].openRate}],
			[4,${browser['4'].openRate}],
			[5,${browser['5'].openRate}],
			[6,${browser['6'].openRate}],
			[7,${browser['7'].openRate}],
			[8,${browser['8'].openRate}]
		]; 
		
 	var system_names = [
			[0, "${os['0'].name}", ${os['0'].openCount}, ${os['0'].openRate}],
			[1, "${os['1'].name}", ${os['1'].openCount}, ${os['1'].openRate}],
			[2, "${os['2'].name}", ${os['2'].openCount}, ${os['2'].openRate}],
			[3, "${os['3'].name}", ${os['3'].openCount}, ${os['3'].openRate}],
			[4, "${os['4'].name}", ${os['4'].openCount}, ${os['4'].openRate}],
			[5, "${os['5'].name}", ${os['5'].openCount}, ${os['5'].openRate}],
			[6, "${os['6'].name}", ${os['6'].openCount}, ${os['6'].openRate}],
			[7, "${os['7'].name}", ${os['7'].openCount}, ${os['7'].openRate}],
			[8, "${os['8'].name}", ${os['8'].openCount}, ${os['8'].openRate}]
			];
	var system_datas = [
			[0,${os['0'].openRate}],
			[1,${os['1'].openRate}],
			[2,${os['2'].openRate}],
			[3,${os['3'].openRate}],
			[4,${os['4'].openRate}],
			[5,${os['5'].openRate}],
			[6,${os['6'].openRate}],
			[7,${os['7'].openRate}],
			[8,${os['8'].openRate}]
		];
		 
 	var language_names = [
			[0, "${lang['0'].name}", ${lang['0'].openCount}, ${lang['0'].openRate}],
			[1, "${lang['1'].name}", ${lang['1'].openCount}, ${lang['1'].openRate}],
			[2, "${lang['2'].name}", ${lang['2'].openCount}, ${lang['2'].openRate}],
			[3, "${lang['3'].name}", ${lang['3'].openCount}, ${lang['3'].openRate}],
			[4, "${lang['4'].name}", ${lang['4'].openCount}, ${lang['4'].openRate}],
			[5, "${lang['5'].name}", ${lang['5'].openCount}, ${lang['5'].openRate}],
			[6, "${lang['6'].name}", ${lang['6'].openCount}, ${lang['6'].openRate}],
			[7, "${lang['7'].name}", ${lang['7'].openCount}, ${lang['7'].openRate}],
			[8, "${lang['8'].name}", ${lang['8'].openCount}, ${lang['8'].openRate}],
			[9, "${lang['9'].name}", ${lang['9'].openCount}, ${lang['9'].openRate}]
			];
	var language_datas = [
			[0,${lang['0'].openRate}],
			[1,${lang['1'].openRate}],
			[2,${lang['2'].openRate}],
			[3,${lang['3'].openRate}],
			[4,${lang['4'].openRate}],
			[5,${lang['5'].openRate}],
			[6,${lang['6'].openRate}],
			[7,${lang['7'].openRate}],
			[8,${lang['8'].openRate}],
			[9,${lang['9'].openRate}]
		]; 
	
 	</script>
 
  </head>
 
  <body>
    <div id="main" class="clearfix" style="position:relative">
		<h1 class="title">终端&nbsp;&nbsp;&nbsp;<span>按打开次数统计用户终端环境，数据隔日更新</span></h1>
		<form method="post" id="ListForm">
		<ul class="tab clearfix">
			<li><a href="${ctx}/campaign/overview?campaignId=${campaign.campaignId}">概览</a></li>
			<li><a href="${ctx}/campaign/send?campaignId=${campaign.campaignId}">发送</a></li>
			<li><a href="${ctx}/campaign/open?campaignId=${campaign.campaignId}">打开</a></li>
			<li><a href="${ctx}/campaign/click?campaignId=${campaign.campaignId}">点击</a></li>
			<li><a href="${ctx}/campaign/domain?campaignId=${campaign.campaignId}">域名</a></li>
			<li><a href="${ctx}/campaign/region?campaignId=${campaign.campaignId}">地域</a></li>
			<%-- <li class="now"><a href="${ctx}/campaign/client?campaignId=${campaign.campaignId}">终端</a></li> --%>
			<li><a href="${ctx}/campaign/delivery?campaignId=${campaign.campaignId}">历史</a></li>
			<li><a href="${ctx}/campaign/compare?campaignId=${campaign.campaignId}">对比</a></li>
			<c:if test="${campaign.touchCount gt 0}">
				<li><a href="${ctx}/campaign/touch?campaignId=${campaign.campaignId}">触发</a></li>
			</c:if>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p>活动邮件打开次数<font color="blue">${total}</font>次</p>
			</div>
			<ul class="btn">
				<li><a href="javascript:exportCsv('${ctx}/campaign/exportCsv?campaignId=${campaign.campaignId}&flag=client');" class="btn_icon btn_icon_down">导出</a></li>
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
		<div class='charts_list m_t20'>
				<div class="task_title">
					<strong>浏览器</strong>
				</div>
				<div class="ct_tank f_l">
					<table border="0" cellspacing="0" cellpadding="0">
						<tr>
							<th>浏览器</th>
							<th>打开次数</th>
							<th>占比</th>
						</tr>
					   <tr>
							<td>Opera</td>
							<td>${browser['1'].openCount}</td>
							<td>${browser['1'].openRate}%</td>
						</tr>
						<tr>
							<td>QQ</td>
							<td>${browser['2'].openCount}</td>
							<td>${browser['2'].openRate}%</td>
						</tr>	
						<tr>
							<td>Chrome</td>
							<td>${browser['3'].openCount}</td>
							<td>${browser['3'].openRate}%</td>
						</tr>	
						<tr>
							<td>UC</td>
							<td>${browser['4'].openCount}</td>
							<td>${browser['4'].openRate}%</td>
						</tr>	
						<tr>
							<td>Safari</td>
							<td>${browser['5'].openCount}</td>
							<td>${browser['5'].openRate}%</td>
						</tr>	
						<tr>
							<td>Firefox</td>
							<td>${browser['6'].openCount}</td>
							<td>${browser['6'].openRate}%</td>
						</tr>	
						<tr>
							<td>360</td>
							<td>${browser['7'].openCount}</td>
							<td>${browser['7'].openRate}%</td>
						</tr>	
						<tr>
							<td>IE</td>
							<td>${browser['8'].openCount}</td>
							<td>${browser['8'].openRate}%</td>
						</tr>	
						<tr>
							<td>其他</td>
							<td>${browser['0'].openCount}</td>
							<td>${browser['0'].openRate}%</td>
						</tr>	
					</table>
				</div>
				<div class="charts f_l">
					<c:if test="${browser['0'].total>0}">
						<div id="open_pie" style="height: 300px; margin: 0 auto;"></div>
					</c:if>
					
					<c:if test="${browser['0'].total==0}">
						<div class="no_data">暂无数据图显示！</div>
					</c:if>
				</div>
			</div>
			<div class='charts_list'>
				<div class="task_title">
					<strong>操作系统</strong>
				</div>
				<div class="ct_tank f_l">
					<table border="0" cellspacing="0" cellpadding="0">
						<tr>
							<th>操作系统</th>
							<th>打开次数</th>
							<th>占比</th>
						</tr>
						<tr>
							<td>Android</td>
							<td>${os['1'].openCount}</td>
							<td>${os['1'].openRate}%</td>
						</tr>
						<tr>
							<td>iPhone</td>
							<td>${os['2'].openCount}</td>
							<td>${os['2'].openRate}%</td>
						</tr>	
						<tr>
							<td>MacOS</td>
							<td>${os['3'].openCount}</td>
							<td>${os['3'].openRate}%</td>
						</tr>	
						<tr>
							<td>Symbian</td>
							<td>${os['4'].openCount}</td>
							<td>${os['4'].openRate}%</td>
						</tr>	
						<tr>
							<td>iPad</td>
							<td>${os['5'].openCount}</td>
							<td>${os['5'].openRate}%</td>
						</tr>	
						<tr>
							<td>Windows</td>
							<td>${os['6'].openCount}</td>
							<td>${os['6'].openRate}%</td>
						</tr>	
						<tr>
							<td>Linux</td>
							<td>${os['7'].openCount}</td>
							<td>${os['7'].openRate}%</td>
						</tr>	
						<tr>
							<td>Windows Phone</td>
							<td>${os['8'].openCount}</td>
							<td>${os['8'].openRate}%</td>
						</tr>	
						<tr>
							<td>其他</td>
							<td>${os['0'].openCount}</td>
							<td>${os['0'].openRate}%</td>
						</tr>	
					</table>
				</div>
				<div class="charts f_l">
					<c:if test="${os['0'].total>0}">
						<div id="system_pie" style="height: 300px; margin: 0 auto;"></div>
					</c:if>
					<c:if test="${os['0'].total==0}">
						<div class="no_data">暂无数据图显示！</div>
					</c:if>
				</div>
			</div>
			
			<div class='charts_list'>
				<div class="task_title">
					<strong>语言环境</strong>
				</div>
				<div class="ct_tank f_l">
					<table border="0" cellspacing="0" cellpadding="0">
						<tr>
							<th>语言环境</th>
							<th>打开次数</th>
							<th>占比</th>
						</tr>
						<tr>
							<td>简体中文</td>
							<td>${lang['1'].openCount}</td>
							<td>${lang['1'].openRate}%</td>
						</tr>
						<tr>
							<td>繁体中文</td>
							<td>${lang['2'].openCount}</td>
							<td>${lang['2'].openRate}%</td>
						</tr>	
						<tr>
							<td>英语</td>
							<td>${lang['3'].openCount}</td>
							<td>${lang['3'].openRate}%</td>
						</tr>	
						<tr>
							<td>法语</td>
							<td>${lang['4'].openCount}</td>
							<td>${lang['4'].openRate}%</td>
						</tr>	
						<tr>
							<td>德语</td>
							<td>${lang['5'].openCount}</td>
							<td>${lang['5'].openRate}%</td>
						</tr>	
						<tr>
							<td>日语</td>
							<td>${lang['6'].openCount}</td>
							<td>${lang['6'].openRate}%</td>
						</tr>	
						<tr>
							<td>韩语</td>
							<td>${lang['7'].openCount}</td>
							<td>${lang['7'].openRate}%</td>
						</tr>	
						<tr>
							<td>西班牙语</td>
							<td>${lang['8'].openCount}</td>
							<td>${lang['8'].openRate}%</td>
						</tr>	
						<tr>
							<td>瑞典语</td>
							<td>${lang['9'].openCount}</td>
							<td>${lang['9'].openRate}%</td>
						</tr>	
						<tr>
							<td>其他</td>
							<td>${lang['0'].openCount}</td>
							<td>${lang['0'].openRate}%</td>
						</tr>	
					</table>
				</div>
				<div class="charts f_l">
					<c:if test="${lang['0'].total>0}">
						<div id="language_pie" style="height: 300px; margin: 0 auto;"></div>
					</c:if>
					<c:if test="${lang['0'].total==0}">
						<div class="no_data">暂无数据图显示！</div>
					</c:if>
				</div>
			</div>
		
	<br>
	<br>
	</form>
	<br>
	<br>
	<br>
	</div>
  </body>
</html>
