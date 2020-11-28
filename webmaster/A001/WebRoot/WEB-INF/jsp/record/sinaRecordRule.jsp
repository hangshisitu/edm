<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>备案信息</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache"/>
	<meta http-equiv="cache-control" content="no-cache"/>
	<meta http-equiv="expires" content="0"/>    
	<link rel="stylesheet" type="text/css" href="${ctx}/resources/css/sina.css?${version}"/>
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/record/sina.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->备案管理&nbsp;<span></span></h1>
		
	<form id="ListForm" action="${ctx}/history/myHistory" method="get">
		<ul class="tab clearfix">
			<li class="now"><a href="${ctx}/sina/list">企业信誉管理</a></li>
			<li><a href="${ctx}/sina/manage">企业信誉管理</a></li>
			<li><a href="${ctx}/sina/rule">信誉等级规则</a></li>
		</ul>
	    <div class="ribbon clearfix">
			<div class="note">
				<p>企业账号:<span><font color="blue">${pagination.recordCount}</font></span>个登录信息</p>
			</div>
			<ul class="btn">
			</ul>
		</div>
		
	<div class="signUp" style="min-height: 0px">
	<h2 class="themeTitle title">信誉等级分配额度</h2>
	<!--规则-->
	<div class="curSend curSendTop02">
		<h2 class="tableSubject">当前发信配额</h2>
		<table width="100%" border="0" cellspacing="0" cellpadding="0" class="tableList">
			<tr class="tableTitle">
				<th align="center" valign="middle" scope="col" width="156">信誉等级</th>
				<th align="center" valign="middle" scope="col">日发送量限制额度（单位：封）</th>
				<th align="center" valign="middle" scope="col">月发送量限制额度（单位：封）</th>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>1级信誉等级</span></td>
				<td align="center" valign="middle"><span>1000封</span></td>
				<td align="center" valign="middle"><span>20000封</span></td>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>2级信誉等级</span></td>
				<td align="center" valign="middle"><span>2000封</span></td>
				<td align="center" valign="middle"><span>40000封</span></td>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>3级信誉等级</span></td>
				<td align="center" valign="middle"><span>3000封</span></td>
				<td align="center" valign="middle"><span>60000封</span></td>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>4级信誉等级</span></td>
				<td align="center" valign="middle"><span>5000封</span></td>
				<td align="center" valign="middle"><span>100000封</span></td>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>5级信誉等级</span></td>
				<td align="center" valign="middle"><span>7000封</span></td>
				<td align="center" valign="middle"><span>140000封</span></td>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>6级信誉等级</span></td>
				<td align="center" valign="middle"><span>9000封</span></td>
				<td align="center" valign="middle"><span>180000封</span></td>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>7级信誉等级</span></td>
				<td align="center" valign="middle"><span>10000封</span></td>
				<td align="center" valign="middle"><span>200000封</span></td>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>8级信誉等级</span></td>
				<td align="center" valign="middle"><span>20000封</span></td>
				<td align="center" valign="middle"><span>400000封</span></td>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>9级信誉等级</span></td>
				<td align="center" valign="middle"><span>30000封</span></td>
				<td align="center" valign="middle"><span>600000封</span></td>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>10级信誉等级</span></td>
				<td align="center" valign="middle"><span>70000封</span></td>
				<td align="center" valign="middle"><span>1400000封</span></td>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>11级信誉等级</span></td>
				<td align="center" valign="middle"><span>90000封</span></td>
				<td align="center" valign="middle"><span>1800000封</span></td>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>12级信誉等级</span></td>
				<td align="center" valign="middle"><span>100000封</span></td>
				<td align="center" valign="middle"><span>2000000封</span></td>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>13级信誉等级</span></td>
				<td align="center" valign="middle"><span>200000封</span></td>
				<td align="center" valign="middle"><span>4000000封</span></td>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>14级信誉等级</span></td>
				<td align="center" valign="middle"><span>300000封</span></td>
				<td align="center" valign="middle"><span>6000000封</span></td>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>15级信誉等级</span></td>
				<td align="center" valign="middle"><span>500000封</span></td>
				<td align="center" valign="middle"><span>10000000封</span></td>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>16级信誉等级</span></td>
				<td align="center" valign="middle"><span>700000封</span></td>
				<td align="center" valign="middle"><span>14000000封</span></td>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>17级信誉等级</span></td>
				<td align="center" valign="middle"><span>900000封</span></td>
				<td align="center" valign="middle"><span>18000000封</span></td>
			</tr>
						<tr>
				<td align="center" valign="middle"><span>18级信誉等级</span></td>
				<td align="center" valign="middle"><span>1200000封</span></td>
				<td align="center" valign="middle"><span>24000000封</span></td>
			</tr>
					</table>
	</div>
	<!--规则 end-->
</div>
<!--信誉等级规则 end-->
<div class="signUp" style="min-height: 0px">
	<h2 class="themeTitle title">信誉等级调整规则</h2>
	<!--规则-->
	<div class="curSend curSendTop02">
		<h2 class="tableSubject">调整规则</h2>
		<table width="100%" border="0" cellspacing="0" cellpadding="0" class="tableList">
			<tr class="tableTitle">
				<th align="center" valign="middle" scope="col" width="156"></th>
				<th align="center" valign="middle" scope="col">进入上升通道</th>
				<th align="center" valign="middle" scope="col">进入下降通道</th>
			</tr>
			<tr>
				<td align="center" valign="middle"><span>每日发信量</span></td>
				<td align="center" valign="middle"><span>&gt;限额的90%</span></td>
				<td align="center" valign="middle"><span>&lt;限额的20%</span></td>
			</tr>
			<tr>
				<td align="center" valign="middle"><span>本月发信量</span></td>
				<td align="center" valign="middle"><span>未超过上限</span></td>
				<td align="center" valign="middle"><span>未超过上限</span></td>
			</tr>
			<tr>
				<td align="center" valign="middle"><span>用户投诉率</span></td>
				<td align="center" valign="middle"><span>&lt;万分之5</span></td>
				<td align="center" valign="middle"><span>&gt;万分之18</span></td>
			</tr>
			<tr>
				<td align="center" valign="middle"><span>前两周邮件用户打开率</span></td>
				<td align="center" valign="middle"><span>&gt;10%</span></td>
				<td align="center" valign="middle"><span>&lt;3%</span></td>
			</tr>
		</table>
	</div>
	<!--规则 end-->
</div>
<!--信誉等级规则 end-->
	</form>
	<br />
	<br />
	<br />
	</div>
  </body>
</html>
