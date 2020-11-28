<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>属性库</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/js/highcharts/highcharts.js"></script>
	<%--<script type="text/javascript" src="${ctx}/resources/script/property/property.pie.js?${version}"></script>
	--%><script type="text/javascript" src="${ctx}/resources/script/property/property.pie2.js?${version}"></script>
	<script type="text/javascript" src="${ctx}/resources/script/property/propertyIndex.js?${version}"></script>
 	
  </head>
  
  <body>
    <div id="main" class="clearfix" style="position:relative">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->属性库&nbsp;<span></span></h1>
		
		<form id="ListForm" action="${ctx}/task/auditedList" method="post">
		
		<div class="ribbon clearfix">
			<div class="note">
				<p>共<span id="totalEmails"><font color="blue">${totalEmailNum}</font></span>个收件人</p>
			</div>
			<ul class="btn">
				<sec:authorize ifAnyGranted="ROLE_ADM"><input type="button" class="groovybutton" value="导 出" title="导出" onclick="importLabel('${ctx}/label/export')"/>&nbsp;</sec:authorize>
				<input type="button" class="groovybutton" value="导入" title="重置" onclick="exportLabel('${ctx}/label/import')">&nbsp;
				<sec:authorize ifAnyGranted="ROLE_ADM"><input type="button" class="groovybutton" value="新建标签" title="新建标签" onclick="createLabel('${ctx}/label/add')"/></sec:authorize>
			</ul>
		</div>
		<div class="prop_lib clearfix">
			<ul class="prop_ul">
				<li>
					<div class="prop_lib_title">
						<a class="aicon" href="${ctx}/label/list?categoryId=${industryCategory.categoryId}">行业兴趣</a>共<span id="industryLabelsNum"><c:if test="${industryStat eq null }">0</c:if><c:if test="${industryStat ne null }">${industryStat.labelNum}</c:if></span>个标签，<span id="industryEmailsNum"><c:if test="${industryStat eq null }">0</c:if><c:if test="${industryStat ne null }">${industryStat.emailNum}</c:if></span>个收件人
					</div>
					<div class="prop_lib_pie">
							<div id="industryDiv" class="pie_tips">正在加载 <img src="${ctx}/resources/img/loading.gif"/></div>
							<div id="stat_industry_pie" class="pie_img" style="width:100%; height:370px;"></div>
					</div>
				</li>
				<li>
					<div class="prop_lib_title">
						<a class="aicon" href="${ctx}/label/list?categoryId=${societyCategory.categoryId}">社会身份</a>共<span id="societyLabelsNum"><c:if test="${societyStat eq null }">0</c:if><c:if test="${societyStat ne null }">${societyStat.labelNum}</c:if></span>个标签，<span id="societyEmailsNum"><c:if test="${societyStat eq null }">0</c:if><c:if test="${societyStat ne null }">${societyStat.emailNum}</c:if></span>个收件人
					</div>
					<div class="prop_lib_pie">
							<div id="societyDiv" class="pie_tips">正在加载 <img src="${ctx}/resources/img/loading.gif"/></div>
							<div id="stat_society_pie" class="pie_img" style="width:100%; height:370px;"></div>
					</div>
				</li>
				<li>
					<div class="prop_lib_title">
						<a class="aicon" href="${ctx}/label/list?categoryId=${ageCategory.categoryId}">年龄段</a>共<span id="ageLabelsNum"><c:if test="${ageStat eq null }">0</c:if><c:if test="${ageStat ne null }">${ageStat.labelNum}</c:if></span>个标签，<span id="ageEmailsNum"><c:if test="${ageStat eq null }">0</c:if><c:if test="${ageStat ne null }">${ageStat.emailNum}</c:if></span>个收件人
					</div>
					<div class="prop_lib_pie">
							<div id="ageDiv" class="pie_tips">正在加载 <img src="${ctx}/resources/img/loading.gif"/></div>
							<div id="stat_age_pie" class="pie_img" style="width:100%; height:370px;"></div>
					</div>
				</li>
				<li>
					<div class="prop_lib_title">
						<a class="aicon" href="${ctx}/label/list?categoryId=${genderCategory.categoryId}">性别</a>共<span id="genderLabelsNum"><c:if test="${genderStat eq null }">0</c:if><c:if test="${genderStat ne null }">${genderStat.labelNum}</c:if></span>个标签，<span id="genderEmailsNum"><c:if test="${genderStat eq null }">0</c:if><c:if test="${genderStat ne null }">${genderStat.emailNum}</c:if></span>个收件人
					</div>
					<div class="prop_lib_pie">
							<div id="genderDiv" class="pie_tips">正在加载 <img src="${ctx}/resources/img/loading.gif"/></div>
							<div id="stat_gender_pie" class="pie_img" style="width:100%; height:370px;"></div>
					</div>
				</li>
			<ul>
		</div>
		<input type="hidden" id="ajaxUrl" value="${ctx}/property/ajaxStat"/>
	</form>
	<br/>
	<br/>
	<br/>
	</div>
  </body>
</html>
