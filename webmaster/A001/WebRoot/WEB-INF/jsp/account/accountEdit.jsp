<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>账号管理</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/account/accountEdit.js?${version}"></script>
	<script>
		var domain_list = {
			<c:forEach items="${resourceList}" var="e" varStatus="a">
			"${e.resourceId}":[
				<c:forEach items="${fn:split(e.heloList, ',')}" var="o" varStatus="b">"@${o}"<c:if test="${!b.last}">,</c:if></c:forEach>
			]<c:if test="${!a.last}">,</c:if>
			</c:forEach>
		};
		var $globalInput = null;
	</script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		<form id="DetailForm" method="post">
			<ul class="tab clearfix">
				<li class="now"><a href="javascript:void(0);" >${title}账号</a></li>
			</ul>
			<br/>
				<h3 class="title"><span>基本信息</span></h3>
				<table width="100%" cellspacing="0" cellpadding="0" border="0" class="edit_table">
					<tbody>
						<tr>
							<th width="20%">用户名<font color="red">*</font></th>
							<td>
								<input type="text" id="userId" name="userId" style="width:220px; height: 25px;border: #DCDCDC 1px solid;" 
								   maxlength="20" value="${user.userId}" <c:if test="${user!=null}">readonly="readonly"</c:if>>
								<span id="userId_span"></span>
							</td>
							
						</tr>
					<c:if test="${user!=null}">
						<tr>
							<th width="20%">密码</th>
							<td>
								<input id="password" name="password" type="password" style="width:220px; height: 25px;border: #DCDCDC 1px solid;" 
								   maxlength="16">
								<span id="password_span"></span>
							</td>
						</tr>
						<tr>
							<th width="20%">确认密码</th>
							<td>
								<input id="confirmPassword" name="confirmPassword" type="password"
								   style="width:220px; height: 25px;border: #DCDCDC 1px solid;" maxlength="16">
								<span id="confirmPassword_span"></span>
							</td>
						</tr>
					</c:if>
						<tr>
							<th width="20%">邮箱<font color="red">*</font></th>
							<td>
								<input id="email" name="email" type="text" style="width:220px; height: 25px;border: #DCDCDC 1px solid;"
								   maxlength="64" value="${user.email}">
								<span id="email_span"></span>
							</td>
						</tr>
						<tr>
							<th width="20%">机构<font color="red">*</font></th>
							<td>
								<select id="corpId" name="corpId" <c:if test="${user!=null}">disabled="true" title="不可编辑角色"</c:if> >
								  <option value="">--请选择--</option>
								  <c:forEach items="${corpList}" var="corp" varStatus="status" >
									 <option <c:if test="${user.corpId == corp.corpId}">selected</c:if> value="${corp.corpId}">${corp.company}</option>
								  </c:forEach>
							    </select>
								<span id="corpId_span"></span>
							</td>
						</tr>
						<tr>
							<th width="20%">角色<font color="red">*</font></th>
							<td>
								<select id="roleId" name="roleId" <c:if test="${user!=null}">disabled="true" title="不可编辑角色"</c:if> >
								  <option value="">--请选择--</option>
								  <c:forEach items="${roleList}" var="role" varStatus="status" >
									 <option <c:if test="${user.roleId == role.id}">selected</c:if> value="${role.id}">${role.roleName}</option>
								  </c:forEach>
							    </select>
								<span id="roleId_span"></span>
							</td>
						</tr>

						<tr>
							<th width="20%">联系人姓名</th>
							<td>
								<input id="trueName" name="trueName" type="text" style="width:220px; height: 25px;border: #DCDCDC 1px solid;" 
								    maxlength="20" value="${user.trueName}">
								<span id="trueName_span"></span>
							</td>
						</tr>
						<tr>
							<th width="20%">联系电话</th>
							<td>
								<input id="phone" name="phone" type="text" style="width:220px; height: 25px;border: #DCDCDC 1px solid;"
								    maxlength="20" value="${user.phone}">
								<span id="phone_span"></span>
							</td>
						</tr>
						</tbody>
				</table>
				
			<br>
			<table align="center">
				<tr>
					<td>						
						<input id="btn_submit" type="button" class="input_yes" value="保 存" title="保存" onclick="saveAccount('${ctx}/account/save','back')">
						<input type="button" class="input_no" value="返 回" title="返回" onclick="back()">
					</td>
				</tr>
				<tr>
					<td align="center">
						<img style="display:none" id="prompt" src="${ctx}/resources/img/loading3.gif">
					</td>
				</tr>
			</table>
			<input type="hidden" name="token" value="${token}" />
			<input type="hidden" name="exist" value="${user.userId}">
			<input type="hidden" name="msg" id="msg" value="${message}">
			<input type="hidden" name="ctx" id="ctx" value="${ctx}">
			<input type="hidden" name="backUrl" id="backUrl" value="${backUrl}">
			<input type="hidden" name="successUrl" id="successUrl" value="${successUrl}">
			<input type="hidden" name="customerType" id="customerType" value="1">
		</form>
		<br />
		<br />
		<br />
	</div>
	
  </body>
</html>
