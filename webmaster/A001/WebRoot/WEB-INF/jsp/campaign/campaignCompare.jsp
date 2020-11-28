<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><html>
  <head>
    <title>活动统计-对比分析</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/js/highcharts/highcharts.js"></script>
	<script type="text/javascript" src="${ctx}/resources/script/campaign/campaign.stat.compare.js?${version}"></script>
 	<script type="text/javascript">
 	
	var cid = "2";
	var names = ["发送总数", "成功总数", "打开人数", "点击人数", "退订人数"];
	var temp = [
	       <c:forEach items="${resultList}" var="r" varStatus="status">
	       		{name:'${r.type}',data:[${r.sentCount},${r.reachCount},${r.readUserCount},${r.clickUserCount},${r.unsubscribeCount}]}<c:if test="${status.last==false}">,</c:if>      
	       </c:forEach>
	];
	
	var datas = [];
	for(var i=0;i<temp.length;i++) {
		datas[i] = temp[temp.length-i-1];
	}
	var dateArr = [['','请选择']],seasonArr = [['','请选择']];
	//生成月季数组
	createDateArr(); 
 	</script>
 
  </head>
 
  <body>
    <div id="main" class="clearfix" style="position:relative">
		<h1 class="title">对比&nbsp;&nbsp;&nbsp;<span>提供活动下周度、月度、季度投递数据的对比分析</span></h1>
		
		<form id="ListForm" method="post">
		<ul class="tab clearfix">
			<li><a href="${ctx}/campaign/overview?campaignId=${campaign.campaignId}">概览</a></li>
			<li><a href="${ctx}/campaign/send?campaignId=${campaign.campaignId}">发送</a></li>
			<li><a href="${ctx}/campaign/open?campaignId=${campaign.campaignId}">打开</a></li>
			<li><a href="${ctx}/campaign/click?campaignId=${campaign.campaignId}">点击</a></li>
			<li><a href="${ctx}/campaign/domain?campaignId=${campaign.campaignId}">域名</a></li>
			<li><a href="${ctx}/campaign/region?campaignId=${campaign.campaignId}">地域</a></li>
			<%-- <li><a href="${ctx}/campaign/client?campaignId=${campaign.campaignId}">终端</a></li> --%>
			<li><a href="${ctx}/campaign/delivery?campaignId=${campaign.campaignId}">历史</a></li>
			<li class="now"><a href="${ctx}/campaign/compare?campaignId=${campaign.campaignId}">对比</a></li>
			<c:if test="${campaign.touchCount gt 0}">
				<li><a href="${ctx}/campaign/touch?campaignId=${campaign.campaignId}">触发</a></li>
			</c:if>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p>活动历时<font color="blue">${last}</font></p>
			</div>
			<ul class="btn">
					<li>
						<a id="period_list" class="but_select" style="width:80px;" href="javascript:void(0)"></a>
						<input type="hidden" id="periodId" value="${mode}" />
					</li>
					<li id='weekBox' class='prd_box none'>
						<a id="week_list" class="but_select" style="width:80px;" href="javascript:void(0)"></a>
						<input type="hidden" id="weekId" value="${weekId}" />
					</li>
					<li id='monthBox' class='prd_box none'>
						<a id="month_list" class="but_select" style="width:80px;" href="javascript:void(0)"></a>
						<input type="hidden" id="monthId" value="${monthId}" />
					</li>
					<li id='seasonBox' class='prd_box none'>
						<a id="season_list" class="but_select" style="width:80px;" href="javascript:void(0)"></a>
						<input type="hidden" id="seasonId" value="${seasonId}" />
					</li>
				<li><a class="btn_icon btn_icon_down" href="javascript:exportCsv('${ctx}/campaign/exportCsv?campaignId=${campaign.campaignId}&flag=compare');">导出</a></li>
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
		
		<c:set var="showFlag" value="false"/>
		<c:forEach items="${resultList}" var="r">
			<c:if test="${r.sentCount>0}">
				<c:set var="showFlag" value="true"/>
			</c:if>
		</c:forEach>
		
		
		<c:if test="${showFlag eq true}">
		<div style="width:850px;margin:0 auto;">
			<div class="charts">
				<div id="bar" style="width: 850px; height: 380px; margin: 0 auto;"></div>
			</div>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th>对比项</th>
					<th>发送总数</th>
					<th>成功数</th>
					<th>打开人数</th>
					<th>点击人数</th>
					<th>退订人数</th>
					<th>无效人数</th>
					<th>成功率</th>
					<th>打开率</th>
					<th>点击率</th>
				</tr>
				<c:forEach items="${resultList}" var="result" varStatus="i">
					<tr>
					<td title="${result.type}"><span class="color_grid"></span>${result.type}</td>
					<td>
						<c:if test="${result.sentCount eq null}">0</c:if>
						<c:if test="${result.sentCount ne null}">${result.sentCount}</c:if>
					</td>
					<td>
						<c:if test="${result.reachCount eq null}">0</c:if>
						<c:if test="${result.reachCount ne null}">${result.reachCount}</c:if>
					</td>
					<td>
						<c:if test="${result.readUserCount eq null}">0</c:if>
						<c:if test="${result.readUserCount ne null}">${result.readUserCount}</c:if>
					</td>
					<td>
						<c:if test="${result.clickUserCount eq null}">0</c:if>
						<c:if test="${result.clickUserCount ne null}">${result.clickUserCount}</c:if>
					</td>
					<td>
						<c:if test="${result.unsubscribeCount eq null}">0</c:if>
						<c:if test="${result.unsubscribeCount ne null}">${result.unsubscribeCount}</c:if>
					</td>
					<td>
						<c:if test="${result.afterNoneCount eq null}">0</c:if>
						<c:if test="${result.afterNoneCount ne null}">${result.afterNoneCount}</c:if>
					</td>
					<td>
						<c:if test="${result.sentCount-result.afterNoneCount==0}">0%</c:if>
						<c:if test="${result.sentCount-result.afterNoneCount!=0}">
							<fmt:formatNumber value="${result.reachCount/(result.sentCount-result.afterNoneCount)*100}" pattern="#.##"/>%
						</c:if>
					</td>
					<td>
						<c:if test="${result.reachCount==0}">0%</c:if>
						<c:if test="${result.reachCount!=0}">
							<fmt:formatNumber value="${result.readUserCount/result.reachCount*100}" pattern="#.##"/>%
						</c:if>
						
					</td>
					<td>
						<c:if test="${result.readUserCount==0}">0%</c:if>
						<c:if test="${result.readUserCount!=0}">
							<fmt:formatNumber value="${result.clickUserCount/result.readUserCount*100}" pattern="#.##"/>%
						</c:if>
					</td>
				</tr>
				</c:forEach>
				
			</tbody>
		</table>
		</c:if>
		<c:if test="${showFlag eq false}">
			<div class="no_data">暂无可对比的数据图显示！</div>
		</c:if>
			
	<br />
	<br />
	<br />
	</form>
	
	<br />
	<br />
	<br />
	</div>
	<div id="monthPad" class="popBox" style="display:none">
	<div class="pop_close">
		X
	</div>
	<div style="width:400px" class="limiter">
		<div class="pop_title">
			其他月份数据对比
		</div>
		<div class="pop_content">
			<form method="get" action="setting" class="setting" autocomplete="off">
				<table width="100%" cellspacing="0" cellpadding="0" border="0" style="" class="edit_tab">
					<tbody>
						<tr>
							<td width="60">
								对比项1
							</td>
							<td width='10' class='red'>*</td>
							<td>
								<a style="width:240px" id='m_list_0' class="a_select b_select">请选择</a>
								<input type='hidden' id='m_0' name='mth' />
							</td>
						</tr>
						<tr>
							<td>
								对比项2
							</td>
							<td width='10' class='red'>*</td>
							<td>
								<a style="width:240px" id='m_list_1' class="a_select b_select">请选择</a>
								<input type='hidden' id='m_1' name='mth' />
							</td>
						</tr>
						<tr>
							<td>
								对比项3
							</td>
							<td width='10' class='red'>&nbsp;</td>
							<td>
								<a style="width:240px" id='m_list_2' class="a_select b_select">请选择</a>
								<input type='hidden' id='m_2' name='mth' />
							</td>
						</tr>
						<tr>
							<td>
								对比项4
							</td>
							<td width='10' class='red'>&nbsp;</td>
							<td>
								<a style="width:240px" id='m_list_3' class="a_select b_select">请选择</a>
								<input type='hidden' id='m_3' name='mth' />
							</td>
						</tr>
					</tbody>
				</table>
				<table width="100%" cellspacing="0" cellpadding="0" border="0">
					<tbody>
						<tr>
							<td width="70">&nbsp;</td>
							<td><span class="err"></span>
							<ul class="f_ul">
								<li>
									<input type="button" value="对比" id="smt" class="input_yes" />
								</li>
								<li>
									<input type="button" value="取消" onclick="closeDialog();" class="input_no" />
								</li>
							</ul></td>
						</tr>
					</tbody>
				</table>
			</form>
		</div>
	</div>
</div>

<div id="seasonPad" class="popBox" style="display:none">
	<div class="pop_close">
		X
	</div>
	<div style="width:400px" class="limiter">
		<div class="pop_title">
			其他季度数据对比
		</div>
		<div class="pop_content">
			<form method="get" action="setting" class="setting" autocomplete="off">
				<table width="100%" cellspacing="0" cellpadding="0" border="0" style="" class="edit_tab">
					<tbody>
						<tr>
							<td width="60">
								对比项1
							</td>
							<td width='10' class='red'>*</td>
							<td>
								<a style="width:240px" id='s_list_0' class="a_select b_select">请选择</a>
								<input type='hidden' id='s_0' name='sth' />
							</td>
						</tr>
						<tr>
							<td>
								对比项2
							</td>
							<td width='10' class='red'>*</td>
							<td>
								<a style="width:240px" id='s_list_1' class="a_select b_select">请选择</a>
								<input type='hidden' id='s_1' name='sth' />
							</td>
						</tr>
						<tr>
							<td>
								对比项3
							</td>
							<td width='10' class='red'>&nbsp;</td>
							<td>
								<a style="width:240px" id='s_list_2' class="a_select b_select">请选择</a>
								<input type='hidden' id='s_2' name='sth' />
							</td>
						</tr>
						<tr>
							<td>
								对比项4
							</td>
							<td width='10' class='red'>&nbsp;</td>
							<td>
								<a style="width:240px" id='s_list_3' class="a_select b_select">请选择</a>
								<input type='hidden' id='s_3' name='sth' />
							</td>
						</tr>
					</tbody>
				</table>
				<table width="100%" cellspacing="0" cellpadding="0" border="0">
					<tbody>
						<tr>
							<td width="70">&nbsp;</td>
							<td><span class="err"></span>
							<ul class="f_ul">
								<li>
									<input type="button" value="对比" id="seasonBtn" class="input_yes" />
								</li>
								<li>
									<input type="button" value="取消" onclick="closeDialog();" class="input_no" />
								</li>
							</ul></td>
						</tr>
					</tbody>
				</table>
			</form>
			<input type="hidden" id="listUrl" value="${ctx}/campaign/compare?campaignId=${campaign.campaignId}">
			<input type="hidden" name="msg" id="msg" value="${message}">
			<input type="hidden" name="checkeds" id="checkeds" value="${checkeds}">
			
		</div>
	</div>
</div>

<div class="select_div"></div>
  </body>
</html>
