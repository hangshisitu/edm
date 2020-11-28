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
<script type="text/javascript" src="${ctx}/resources/script/common/jquery.form-min.js?${version}"></script>
<script type="text/javascript" src="${ctx}/resources/script/role/roleEdit.js?${version}"></script>
</head>
<body>
	<div id="main" class="clearfix" style="">
		<!--创建角色开始-->
		<h1 class="title">${title}&nbsp;</h1>
		<form id="addRole" name="addRole" method="post"/>
			<input type="hidden" name="token" value="" />
			<div class="sys_blueline">
				<ul class="role_wrapper clearfix">
					<li>
						<div class="role_desc">
							<span class="role_title">角色名称：</span>
							&nbsp;<span class="red">*</span>
							<input type="text" name="roleName" id="roleName" class="rate_sum" value="${role.roleName}" maxlength="15"/>
							&nbsp;<span class="smtp_tips">(15字以内)</span>
						</div>
					</li>
					<li>
						<div class="role_desc">
							<span class="role_title">角色描述：</span>
							&nbsp;<span class="red">&nbsp;&nbsp;</span>
							<input type="text" name="roleDesc" id="roleDesc" class="rate_sum" value="${role.roleDesc}" maxlength="50"/>
							
							&nbsp;<span class="smtp_tips">(50字以内)</span>
						</div>
					</li>
					<li>
						<div class="role_desc">
							<div class="f_l">
								<span class="role_title">功能权限：</span>
								&nbsp;<span class="red">*</span>
							</div>
							<div class="f_l selection">
								<div class="select_box">
									<div class="tt">数据管理</div>
									<ol>
										<li><label title="收件人管理：收件人导入标签的增删改查，标签与标签分类的增删改查；">
											<input type="checkbox" name="func" value="M_1" id="M_1"/>收件人管理</label></li>
										<li><label title="属性管理：属性字段的增删改查；">
											<input type="checkbox" name="func" value="M_2" id="M_2"/>属性管理</label></li>
										<li><label title="过滤器管理：过滤器的增删改查；">
											<input type="checkbox" name="func" value="M_3" id="M_3"/>过滤器管理</label></li>
										<li><label title="收件人筛选与导出：收件人数据的筛选与导出，筛选记录的查询；">
											<input type="checkbox" name="func" value="M_4" id="M_4"/>收件人筛选与导出</label></li>
										<li><label title="表单管理：表单的增删改查，表单收集数据的关联查阅；">
											<input type="checkbox" name="func" value="M_5" id="M_5"/>表单管理</label></li>
									</ol>
									<div class="tt">邮件管理</div>
									<ol>
										<li><label title="模板设计与管理：模板与模板类别的增删改查；">
											<input type="checkbox" name="func" value="M_6" id="M_6"/>模板设计与管理</label></li>
										<li><label title="模板预览：预览模板效果；">
											<input type="checkbox" name="func" value="M_7" id="M_7"/>模板预览</label></li>
										<li><label title="任务创建：新建投递任务，包括复制任务列表中已有的任务；">
											<input type="checkbox" name="func" value="M_8" id="M_8"/>任务创建</label></li>
										<li><label title="任务审核：审核提交的投递任务；">
											<input type="checkbox" name="func" value="M_9" id="M_9"/>任务审核</label></li>
										<li><label title="任务投递管理：对任务列表中的任务可进行暂停、取消、重启、删除；">
											<input type="checkbox" name="func" value="M_10" id="M_10"/>任务投递监控管理</label></li>
										<li><label title="活动创建与管理：活动的增删改查和任务导入；">
											<input type="checkbox" name="func" value="M_11" id="M_11"/>活动创建与管理</label></li>
									</ol>
									<div class="tt">数据报告</div>
									<ol>
										<li><label title="报告查阅：任务和活动报告查阅，除数据导出外的所有阅览和处理功能"><input type="checkbox" name="func" value="M_12" id="M_12"/>报告查阅</label></li>
										<li><label title="报告导出：任务和活动的整体报告和收件人清单数据导出为文件。"><input type="checkbox" name="func" value="M_13" id="M_13"/>报告导出</label></li>
									</ol>
								</div>
								<div>
									<label><input type="checkbox" name="func" value="M_14" id="M_14"   
									          <c:if test="${role != null}"> onclick="return false" title="不可编辑此功能"</c:if> />
									          此为管理员角色<span class="smtp_tips">(默认具备子账号管理功能)</span>
									</label>
								</div>
								<div>
									<p class="err" style="color:red;padding-top:2px;"></p>
								</div>
							</div>
						</div>
					</li>
				</ul>
			</div>
			<table style="margin-left:100px">
				<tbody>
					<tr>
						<td>
							<input type="submit"  value="提交" class="input_yes" id="btn_submit">
							<input type="button" onclick="back()"  value="返 回" class="input_no">
						</td>
					</tr>
					<tr>
						<td align="center">
							<img src="/resources/img/loading3.gif" id="prompt" style="display:none">
						</td>
					</tr>
				</tbody>
			</table>
			<input type="hidden" name="token" value="${token}" />
			<input type="hidden" name="id" value="${role.id}">
			<input type="hidden" name="msg" id="msg" value="${message}">
			<input type="hidden" name="ctx" id="ctx" value="${ctx}">
			<input type="hidden" name="backUrl" id="backUrl" value="${ctx}/role/roleList">
			<input type="hidden" name="successUrl" id="successUrl" value="${ctx}/role/roleList">
			<input type="hidden" name="funcList" id="funcList" value="${funcList}">
			<input type="hidden" name="oldRoleName" id="oldRoleName" value="${role.roleName}">
		</form>
		<!--创建角色结束-->
		
		<br /> <br /> <br />
	</div>

</body>
</html>
