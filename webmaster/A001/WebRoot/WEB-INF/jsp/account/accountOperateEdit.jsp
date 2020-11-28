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
	<script type="text/javascript" src="${ctx}/resources/script/account/accountOperateEdit.js?${version}"></script>
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
							<input type="text" id="userId" name="userId" style="width:220px; height: 25px;border: #DCDCDC 1px solid;" maxlength="20"
							   value="${user.userId}" <c:if test="${user!=null}">readonly="readonly"</c:if>>
							<span id="userId_span"></span>
						</td>
						
					</tr>
				<c:if test="${user!=null}">
					<tr>
						<th width="20%">密码</th>
						<td>
							<input id="password" name="password" type="password" style="width:220px; height: 25px;border: #DCDCDC 1px solid;" maxlength="16">
							<span id="password_span"></span>
						</td>
					</tr>
					<tr>
						<th width="20%">确认密码</th>
						<td>
							<input id="confirmPassword" name="confirmPassword" type="password" style="width:220px; height: 25px;border: #DCDCDC 1px solid;" maxlength="16">
							<span id="confirmPassword_span"></span>
						</td>
					</tr>
				</c:if>
					<tr>
						<th width="20%">邮箱<font color="red">*</font></th>
						<td>
							<input id="email" name="email" type="text" style="width:220px; height: 25px;border: #DCDCDC 1px solid;" maxlength="64" value="${user.email}">
							<span id="email_span"></span>
						</td>
					</tr>
					<tr>
							<th width="20%">角色<font color="red">*</font></th>
							<td>
								<select id="roleId" name="roleId" onchange="changeType('${ctx}/account/selectAccountTypePage?accountType=')" 
								  <c:if test="${user!=null}">disabled="true" title="不可编辑角色"</c:if> >
								  <option value="">--请选择--</option>
								  <!-- 把运营人员和企业只读角色放置在下拉列表最后 -->
								  <c:forEach items="${roleList}" var="role" varStatus="status" >
								     <c:if test="${role.id != 4 && role.id != 6}">
									 <option <c:if test="${accountType == role.id}">selected</c:if> value="${role.id}">${role.roleName}</option>
									 </c:if>
								  </c:forEach>
								  <c:forEach items="${roleList}" var="role" varStatus="status" >
								     <c:if test="${role.id == 4 || role.id == 6}">
									 <option <c:if test="${accountType == role.id}">selected</c:if> value="${role.id}">${role.roleName}</option>
									 </c:if>
								  </c:forEach> 
								  <!--  -->  
							    </select>
								<span id="roleId_span"></span>
							</td>
						</tr>
					<tr>
						<th width="20%">联系人</th>
						<td>
							<input id="trueName" name="trueName" type="text" style="width:220px; height: 25px;border: #DCDCDC 1px solid;" maxlength="20" value="${user.trueName}">
							<span id="trueName_span"></span>
						</td>
					</tr>
					<tr>
						<th width="20%">联系电话</th>
						<td>
							<input id="phone" name="phone" type="text" style="width:220px; height: 25px;border: #DCDCDC 1px solid;" maxlength="20" value="${user.phone}">
							<span id="phone_span"></span>
						</td>
					</tr>
					<tr>
						<th width="20%">网络发件人<font color="red">*</font></th>
						<td>
							<c:if test="${robotsList==null||fn:length(robotsList)<1}">
								<div class="robot">
									<input id="robot_0" class="input_text count_spoke" type="text" maxlength="64" value="" name="robots">
									<span class="input_set add_set" onclick="addRobot()">+</span>
								</div>
							</c:if>
							<c:if test="${robotsList!=null}">
								
								<c:forEach items="${robotsList}" var="r" varStatus="status">
									<c:if test="${status.index==0}">
										<div class="robot">
											<input id="robot_0" class="input_text count_spoke" type="text" maxlength="64" value="${r.email}" name="robots">
											<span class="input_set add_set" onclick="addRobot()">+</span>
										</div>
									</c:if>
									<c:if test="${status.index!=0}">
										<div class="robot">
											<input id="robot_${status.index}" class="input_text count_spoke" type="text" maxlength="64" value="${r.email}" name="robots">
											<span class="input_set del_set" onclick="delRobot(${status.index})">-</span>
										</div>
									</c:if>
								</c:forEach>
							</c:if>
						</td>
					</tr>
					
					<tr>
						<th width="20%">短信设置<font color="red">*</font></th>
						<td>
							<input name="openSms" value="1" type="radio" <c:if test="${user.openSms == 1}">checked="checked"</c:if>>需要 &nbsp;&nbsp;
							<input name="openSms" value="0" type="radio" <c:if test="${user.openSms != 1}">checked="checked"</c:if>>不需要
							<span id="openSms_span"></span>
						</td>
					</tr>
					
					<tr>
						<th width="20%">发件人验证<font color="red">*</font></th>
						<td>
							<input name="senderValidate" value="1" type="radio" checked="checked" <c:if test="${corp.senderValidate == 1}">checked="checked"</c:if>>需要&nbsp;&nbsp; 
							<input name="senderValidate" value="0" type="radio" <c:if test="${corp.senderValidate == 0}">checked="checked"</c:if>>不需要
							<span id="senderValidate_span"></span>
						</td>
					</tr>
					<tr>
						<th width="20%">任务审核<font color="red">*</font></th>
						<td>
							<input type="radio" name="moderate" value="1" checked="checked" <c:if test="${user.moderate eq 1}">checked="checked"</c:if>/>需要&nbsp;&nbsp;
							<input type="radio" name="moderate" value="0" <c:if test="${user.moderate ne 1}">checked="checked"</c:if>/>不需要
						</td>
					</tr>
					</tbody>
			</table>
		<br>
		<table align="center">
			<tr>
				<td>
					<input id="btn_submit" type="button" class="input_yes" value="保 存" title="保存" onclick="saveAccount('${ctx}/account/saveOperateAccount','back')">
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
		<input type="hidden" name="backUrl" id="backUrl" value="${ctx}/account/accountList">
		<input type="hidden" name="successUrl" id="successUrl" value="${ctx}/account/accountList">
	</form>
	<br />
	<br />
	<br />
	</div>
	
  </body>
</html>
