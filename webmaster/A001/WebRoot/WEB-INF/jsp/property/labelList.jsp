<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>标签列表</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/property/labelList.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		<h1 class="title"><span class="f_r"><a  href="${ctx}/property/propertyIndex"><font color="blue">返回>></font>&nbsp;&nbsp;</a></span>属性库&nbsp;<span>当前类别：<font color="blue">${labelCategory.categoryName}</font></span></h1>
		
		<form id="ListForm" action="" method="post">
		<div class="ribbon clearfix">
			<div class="note">
				<p>共<span><font color="blue">${propertyVoStat.labelNum}</font></span>个标签，<span><font color="blue">${propertyVoStat.emailNum}</font></span>个收件人</p>
			</div>
			<ul class="btn" style="width:8%;">
					<select id="categoryId" onchange="selectCategory('${ctx}/label/list')"
							name="categoryId" style="width:100%;background-color: #F5F5F5">
								<c:forEach items="${labelCategoryList}" var="lc">
										<option
											<c:if test="${labelCategory.categoryId==lc.categoryId}">selected="selected"</c:if>
											value="${lc.categoryId}">${lc.categoryName}</option>
								</c:forEach>
					</select>
			</ul>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th>标签名称</th>
					<th>收件人数</th>
					<th>占比</th>
					<th>创建时间</th>
					<th>更新时间</th>
					<th>操作</th>
				</tr>
				<c:forEach items="${propertyVoStat.propertyVoList}" var="pv" varStatus="i">
				<tr>
					<td title="${pv.labelDesc}">${pv.labelName}</td>
					<td>${pv.num}</td>
					<td>
						<fmt:formatNumber value="${pv.percentage}" pattern="#.##"/>%
					</td>
					<td>${pv.createTime}</td>
					<td>
						${pv.updateTime}
					</td>
					<td class="edit">
						<ul class="f_ul">
							<li><a href="javascript:importLabel('${ctx}/label/import?labelId=${pv.labelId}')">导入</a></li>
							<sec:authorize ifAnyGranted="ROLE_ADM"><li><a href="javascript:importLabel('${ctx}/label/export?labelId=${pv.labelId}')">导出</a></li></sec:authorize>
						</ul>
					</td>
					
				</tr>
			</c:forEach>
					
			</tbody>
		</table>
		
			<c:if test="${propertyVoStat==null||fn:length(propertyVoStat.propertyVoList)<1}">
				<div class="no_data">没有匹配的数据</div>
			</c:if>
		
	</form>
	<br />
	<br />
	<br />
	</div>
  </body>
</html>
