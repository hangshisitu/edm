<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>新建标签</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/property/labelImport.js?${version}"></script>
	<script type="text/javascript">

	var ass_prop = {
		"行业兴趣": [
			<c:forEach items="${industryList}" var="label" varStatus="status">
				{"id":${label.lableId},"title":'${label.labelName}',"info":"行业兴趣"}<c:if test="${status.last==false}">,</c:if>
			</c:forEach>
		],
		"社会身份": [
			<c:forEach items="${societyList}" var="label" varStatus="status">
				{"id":${label.lableId},"title":'${label.labelName}',"info":"社会身份"}<c:if test="${status.last==false}">,</c:if>
			</c:forEach>
		],
		"年龄段": [
			<c:forEach items="${ageList}" var="label" varStatus="status">
				{"id":${label.lableId},"title":'${label.labelName}',"info":"年龄段"}<c:if test="${status.last==false}">,</c:if>
			</c:forEach>
		],
		"性别": [
			<c:forEach items="${genderList}" var="label" varStatus="status">
				{"id":${label.lableId},"title":'${label.labelName}',"info":"性别"}<c:if test="${status.last==false}">,</c:if>
			</c:forEach>
		]
	}; 
	
	var propVal=[];
	</script>
  </head>
  
  <body>
	<div id="main">
	
		<h1 class="title"><span class="f_r"><a  href="${ctx}/property/propertyIndex"><font color="blue">返回>></font>&nbsp;&nbsp;</a></span>属性数据导入&nbsp;<span></h1>
   		<form id="DetailForm" action="${ctx}/label/importPropLib" method="post">
   		<table width="100%"  border="0" cellspacing="0" cellpadding="0" class="user_table  m_t20">
			<tr>
				<td width="100" align="right">任务ID：</td>
				<td width="10" class="red">*</td>
				<td width="400">
					<input id="task" class="task_text" type="text" value="${taskId}" name="task" maxlength="25">
					<input type="hidden" id="taskId" name="taskID" value="" />
				</td>
				<td>&nbsp;</td>
			</tr>
			<tr class="mul_cate">
				<td width="100" align="right">关联属性：</td>
				<td width="10" class="red">*</td>
				<td width="400">
					<textarea id="assProp" name="assProp" class="add_text" style="height:28px; width:380px; margin-bottom:5px;"></textarea>
					<input type="hidden" id="assPropId" name="label_ids" value="${labelId}" />     		         		
				</td>
				<td>
					<span id="propSet" class="input_set">设置</span>
				</td>
			</tr>
		</table>
		<table width="100%" border="0" cellspacing="0" cellpadding="0" class="user_table m_t10">
			<tr>
				<td width="100" align="right"></td>
				<td width="10" class="red"></td>
				<td>
					<ul class="f_ul">
						<li><input type="button" id="btn_save" class="input_yes" value="确定" onclick="importPropLib()" /></li>
					</ul>
				</td>
			</tr>
		</table>
		<input type="hidden" name="token" value="${token}" />
		<input type="hidden" name="successUrl" id="successUrl" value="${ctx}/property/propertyIndex">
		</form>
	</div>
	<div class="searchBox searchHoriz" id="slideSearchProp" style="display:none">
		<div class="limiter" style="width:390px">
			<div class="pop_content">
				<div class="search_result"></div>
			</div>
		</div>
	</div>
	<br/>
	<br/>
	<br/>
  </body>
</html>
