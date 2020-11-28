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
	<script type="text/javascript" src="${ctx}/resources/script/account/accountReadonlyEdit.js?${version}"></script>
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
						<th width="20%">公司名称<font color="red">*</font></th>
						<td>
							<input id="company" name="company" type="text" style="width:220px; height: 25px;border: #DCDCDC 1px solid;" maxlength="50" value="${corp.company}">
							<span id="company_span"></span>
						</td>
					</tr>
					<tr>
						<th width="20%">公司地址</th>
						<td>
							<input id="address" name="address" type="text" style="width:220px; height: 25px;border: #DCDCDC 1px solid;" maxlength="50" value="${corp.address}">
							<span id="address_span"></span>
						</td>
					</tr>
					<tr>
						<th width="20%">公司网址</th>
						<td>
							<input id="website" name="website" type="text" style="width:220px; height: 25px;border: #DCDCDC 1px solid;" maxlength="50" value="${corp.website}">
							<span id="website_span"></span>
						</td>
					</tr>
					<tr>
						<th width="20%">联系人姓名</th>
						<td>
							<input id="contact" name="contact" type="text" style="width:220px; height: 25px;border: #DCDCDC 1px solid;" maxlength="20" value="${corp.contact}">
							<span id="contact_span"></span>
						</td>
					</tr>
					<tr>
						<th width="20%">联系人职位</th>
						<td>
							<input id="job" name="job" type="text" style="width:220px; height: 25px;border: #DCDCDC 1px solid;" maxlength="20" value="${user.job}">
							<span id="job_span"></span>
						</td>
					</tr>
					<tr>
						<th width="20%">联系电话</th>
						<td>
							<input id="telephone" name="telephone" type="text" style="width:220px; height: 25px;border: #DCDCDC 1px solid;" maxlength="20" value="${corp.telephone}">
							<span id="telephone_span"></span>
						</td>
					</tr>
					<tr>
						<th width="20%">行业</th>
						<td>
							<select id="industry" name="industry">
							<option value="">请选择</option>
							<option value="电子商务" <c:if test="${corp.industry=='电子商务'}">selected="selected"</c:if>>电子商务</option>
							<option value="零售/批发" <c:if test="${corp.industry=='零售/批发'}">selected="selected"</c:if>>零售/批发</option>
							<option value="餐饮业" <c:if test="${corp.industry=='餐饮业'}">selected="selected"</c:if>>餐饮业</option>
							<option value="贸易" <c:if test="${corp.industry=='贸易'}">selected="selected"</c:if>>贸易</option>
							<option value="通讯" <c:if test="${corp.industry=='通讯'}">selected="selected"</c:if>>通讯</option>
							<option value="广告公关" <c:if test="${corp.industry=='广告公关'}">selected="selected"</c:if>>广告公关</option>
							<option value="艺术/设计" <c:if test="${corp.industry=='艺术/设计'}">selected="selected"</c:if>>艺术/设计</option>
							<option value="娱乐/体育/休闲" <c:if test="${corp.industry=='娱乐/体育/休闲'}">selected="selected"</c:if>>娱乐/体育/休闲</option>
							<option value="教育/培训" <c:if test="${corp.industry=='教育/培训'}">selected="selected"</c:if>>教育/培训</option>
							<option value="旅游/度假" <c:if test="${corp.industry=='旅游/度假'}">selected="selected"</c:if>>旅游/度假</option>
							<option value="协会/俱乐部" <c:if test="${corp.industry=='协会/俱乐部'}">selected="selected"</c:if>>协会/俱乐部</option>
							<option value="政府/公共事业" <c:if test="${corp.industry=='政府/公共事业'}">selected="selected"</c:if>>政府/公共事业</option>
							<option value="非盈利机构" <c:if test="${corp.industry=='非盈利机构'}">selected="selected"</c:if>>非盈利机构</option>
							<option value="财务/金融" <c:if test="${corp.industry=='财务/金融'}">selected="selected"</c:if>>财务/金融</option>
							<option value="保险" <c:if test="${corp.industry=='保险'}">selected="selected"</c:if>>保险</option>
							<option value="咨询" <c:if test="${corp.industry=='咨询'}">selected="selected"</c:if>>咨询</option>
							<option value="地产" <c:if test="${corp.industry=='地产'}">selected="selected"</c:if>>地产</option>
							<option value="装饰装修" <c:if test="${corp.industry=='装饰装修'}">selected="selected"</c:if>>装饰装修</option>
							<option value="医药/卫生" <c:if test="${corp.industry=='医药/卫生'}">selected="selected"</c:if>>医药/卫生</option>
							<option value="美容美体" <c:if test="${corp.industry=='美容美体'}">selected="selected"</c:if>>美容美体</option>
							<option value="展会" <c:if test="${corp.industry=='展会'}">selected="selected"</c:if>>展会</option>
							<option value="加工制造" <c:if test="${corp.industry=='加工制造'}">selected="selected"</c:if>>加工制造</option>
							<option value="其他" <c:if test="${corp.industry=='其他'}">selected="selected"</c:if>>其他</option>
						</select>
							<span id="industry_span"></span>
						</td>
					</tr>
					<tr>
						<th width="20%">邮编</th>
						<td>
							<input id="zip" name="zip" type="text" style="width:220px; height: 25px;border: #DCDCDC 1px solid;" maxlength="6" value="${corp.zip}">
							<span id="zip_span"></span>
						</td>
					</tr>
					</tbody>
			</table>
		<br>
		<table>
			<tr>
				<td>
					<%--<input onmouseover="this.style.cursor='pointer';this.style.cursor='hand';this.style.color='blue'" onmouseout="this.style.color=''" type="button" value="保 存" id="btn_submit" onclick="saveAccount('${ctx}/account/save','back')">
					<input onmouseover="this.style.cursor='pointer';this.style.cursor='hand';this.style.color='blue'" onmouseout="this.style.color=''" type="button" value="返 回" onclick="back()">
					--%>
					<input id="btn_submit" type="button" class="input_yes" value="保 存" title="保存" onclick="saveAccount('${ctx}/account/saveReadonlyAccount','back')">
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
