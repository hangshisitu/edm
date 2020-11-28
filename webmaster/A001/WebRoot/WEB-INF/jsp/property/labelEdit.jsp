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
	<script type="text/javascript" src="${ctx}/resources/script/property/labelEdit.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
    	<h1 class="title">新建标签</h1>
		<form id="DetailForm" method="post" action="${ctx}/label/saveLabel">
			<table width="100%" cellspacing="0" cellpadding="0" border="0" class="edit_table">
				<tbody>
					<tr>
						<th width="20%">标签类别<font color="red">*</font></th>
						<td>
							<select name="categoryId" id="categoryId" style="width:222px; height: 25px;border: #DCDCDC 1px solid;">
								<c:forEach items="${labelCategoryList}" var="lc">
									<option value="${lc.categoryId }">${lc.categoryName}</option>
								</c:forEach>
							</select>
							<span id="categoryId_span"></span>
						</td>
						
					</tr>
					<tr>
						<th width="20%">标签名称<font color="red">*</font></th>
						<td>
							<input id="labelName" name="labelName" type="text" style="width:220px; height: 25px;border: #DCDCDC 1px solid;" maxlength="20" >
							<span id="labelName_span"></span>
						</td>
					</tr>
					<tr>
						<th width="20%">标签描述</th>
						<td>
							<textarea name="labelDesc" id="labelDesc" rows="3" cols="" style="width:210px;border: #DCDCDC 1px solid;" onPropertyChange="textCounter(this,60)"></textarea>
						</td>
					</tr>
					</tbody>
			</table>
		<br>
		<table align="center">
			<tr>
				<td>
					<input id="btn_save" type="button" class="input_yes" value="保 存" title="保存" onclick="saveLabel()">
					<input type="button" class="input_no" value="返 回" title="返回" onclick="back('${ctx}/property/propertyIndex')">
				</td>
			</tr>
			<tr>
				<td align="center">
					<img style="display:none" id="prompt" src="${ctx}/resources/img/loading3.gif">
				</td>
			</tr>
		</table>
		<input type="hidden" name="token" value="${token}" />
		<input type="hidden" name="msg" id="msg" value="${message}">
		<input type="hidden" name="lcId" id="lcId">
		<input type="hidden" name="successUrl" id="successUrl" value="${ctx}/label/list">
	</form>
	<br />
	<br />
	<br />
	</div>
	
  </body>
</html>
