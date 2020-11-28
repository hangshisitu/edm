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
	<script type="text/javascript" src="${ctx}/resources/script/property/property.pie.js?${version}"></script>
	<script type="text/javascript" src="${ctx}/resources/script/property/propertyIndex.js?${version}"></script>
 	<script type="text/javascript">
 	
	var industry_datas = [
	             		 <c:forEach items="${industryStat.propertyVoList}" var="p" varStatus="status">   
	             		 	{name:'${p.labelName}',y:${p.percentage},num:${p.num}}<c:if test="${status.last==false}">,</c:if>
	             		 </c:forEach>   
	];
	
	var society_datas = [
		             		 <c:forEach items="${societyStat.propertyVoList}" var="p" varStatus="status">   
		             		 	{name:'${p.labelName}',y:${p.percentage},num:${p.num}}<c:if test="${status.last==false}">,</c:if>
		             		 </c:forEach>   
		];
	
	var age_datas = [
		             		 <c:forEach items="${ageStat.propertyVoList}" var="p" varStatus="status">   
		             		 	{name:'${p.labelName}',y:${p.percentage},num:${p.num}}<c:if test="${status.last==false}">,</c:if>
		             		 </c:forEach>   
		];
	
	var gender_datas = [
		             		 <c:forEach items="${genderStat.propertyVoList}" var="p" varStatus="status">   
		             		 	{name:'${p.labelName}',y:${p.percentage},num:${p.num}}<c:if test="${status.last==false}">,</c:if>
		             		 </c:forEach>   
		];

 	</script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->属性库&nbsp;<span></span></h1>
		
		<form id="ListForm" action="${ctx}/task/auditedList" method="post">
		
		<div class="ribbon clearfix">
			<div class="note">
				<p>共<span><font color="blue">${totalEmailNum}</font></span>个收件人</p>
			</div>
			<ul class="btn">
				<input type="button" class="groovybutton" value="导 出" title="导出" onclick="importLabel('${ctx}/label/export')">&nbsp;
				<input type="button" class="groovybutton" value="导入" title="重置" onclick="exportLabel('${ctx}/label/import')">&nbsp;
				<input type="button" class="groovybutton" value="新建标签" title="新建标签" onclick="createLabel('${ctx}/label/add')">
			</ul>
		</div>
		<div class="prop_lib clearfix">
			<ul class="prop_ul">
				<li>
					<div class="prop_lib_title">
						<a class="aicon" href="${ctx}/label/list?categoryId=${industryStat.labelCategory.categoryId}">行业兴趣</a>共<span><c:if test="${industryStat eq null }">0</c:if><c:if test="${industryStat ne null }">${industryStat.labelNum}</c:if></span>个标签，<span><c:if test="${industryStat eq null }">0</c:if><c:if test="${industryStat ne null }">${industryStat.emailNum}</c:if></span>个收件人
					</div>
					<div class="prop_lib_pie">
							<c:if test="${industryStat.emailNum eq 0}">
								<div style="padding: 60px 0 0 100px;">标签下无数据</div>
							</c:if>
							<c:if test="${industryStat.emailNum ne 0}">
								<div id="stat_industry_pie" style="width: 500px; height: 200px; margin: 0 auto;"></div>
							</c:if>
					</div>
				</li>
				<li>
					<div class="prop_lib_title">
						<a class="aicon" href="${ctx}/label/list?categoryId=${societyStat.labelCategory.categoryId}">社会身份</a>共<span><c:if test="${societyStat eq null }">0</c:if><c:if test="${societyStat ne null }">${societyStat.labelNum}</c:if></span>个标签，<span><c:if test="${societyStat eq null }">0</c:if><c:if test="${societyStat ne null }">${societyStat.emailNum}</c:if></span>个收件人
					</div>
					<div class="prop_lib_pie">
						<c:if test="${societyStat.emailNum eq 0}">
							<div style="padding: 60px 0 0 100px;">标签下无数据</div>
						</c:if>
						<c:if test="${societyStat.emailNum ne 0}">
							<div id="stat_society_pie" style="width: 500px; height: 200px; margin: 0 auto;"></div>
						</c:if>
					</div>
				</li>
				<li>
					<div class="prop_lib_title">
						<a class="aicon" href="${ctx}/label/list?categoryId=${ageStat.labelCategory.categoryId}">年龄段</a>共<span><c:if test="${ageStat eq null }">0</c:if><c:if test="${ageStat ne null }">${ageStat.labelNum}</c:if></span>个标签，<span><c:if test="${ageStat eq null }">0</c:if><c:if test="${ageStat ne null }">${ageStat.emailNum}</c:if></span>个收件人
					</div>
					<div class="prop_lib_pie">
						<c:if test="${ageStat.emailNum eq 0}">
							<div style="padding: 60px 0 0 100px;">标签下无数据</div>
						</c:if>
						<c:if test="${ageStat.emailNum ne 0}">
							<div id="stat_age_pie" style="width: 500px; height: 200px; margin: 0 auto;"></div>
						</c:if>
					</div>
				</li>
				<li>
					<div class="prop_lib_title">
						<a class="aicon" href="${ctx}/label/list?categoryId=${genderStat.labelCategory.categoryId}">性别</a>共<span><c:if test="${genderStat eq null }">0</c:if><c:if test="${genderStat ne null }">${genderStat.labelNum}</c:if></span>个标签，<span><c:if test="${genderStat eq null }">0</c:if><c:if test="${genderStat ne null }">${genderStat.emailNum}</c:if></span>个收件人
					</div>
					<div class="prop_lib_pie">
						<c:if test="${genderStat.emailNum eq 0}">
							<div style="padding: 60px 0 0 100px;">标签下无数据</div>
						</c:if>
						<c:if test="${genderStat.emailNum ne 0}">
							<div id="stat_gender_pie" style="width: 500px; height: 200px; margin: 0 auto;"></div>
						</c:if>
					</div>
				</li>
			<ul>
		</div>
		
	</form>
	<br>
	<br>
	<br>
	</div>
  </body>
</html>
