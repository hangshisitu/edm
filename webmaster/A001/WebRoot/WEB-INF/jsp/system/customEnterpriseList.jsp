<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<%@ page import="cn.edm.modules.utils.Props" %>
<%@ page import="cn.edm.constant.Config" %>
<%
  String logourl = Props.getStr(Config.NGINX_SERVER);
%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>登录信息</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache"/>
	<meta http-equiv="cache-control" content="no-cache"/>
	<meta http-equiv="expires" content="0"/>    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/system/customEnterprise.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->企业定制管理&nbsp;<span></span></h1>
		
		<form id="customForm" action="${ctx}/custom/page" method="get">
       
        <ul class="search clearfix m_b10">
			<li>状态</li>
			<li>
				<select id="search_roleId" name="search_status" style="width:100%;background-color: #F5F5F5">
				<option value="">--请选择--</option>
			    <option value="1" 
			              <c:if test="${search_status} == 1">selected="selected"</c:if> >正常</option>	
			    <option value="0" 
			              <c:if test="${search_status} == 0">selected="selected"</c:if> >冻结</option>	
				</select>
			</li>
			<li>
				<input type="button" class="groovybutton" value="查 询" title="查询" onclick="search()" />
			</li>
		</ul>

		<ul class="tab clearfix">
			<li><a href="${ctx}/history/myHistory">登录信息</a></li>
			<li><a href="${ctx}/password/edit">修改密码</a></li>
			<sec:authorize ifAnyGranted="ROLE_ADM">
				<li><a href="${ctx}/subUser/subUserList">子账号管理</a></li>
				<li><a href="${ctx}/history/subUserHistory">子账号日志</a></li>
			</sec:authorize>
			<li class="now"><a href="${ctx}/custom/page">企业定制</a></li>
		</ul>
		<div class="ribbon clearfix">
		</div>
		<ul class="tab clearfix">			
			<li class="now"><a href="#">LOGO设置</a></li>
		</ul>
        <div class="ribbon clearfix">
			<div class="note">
				<p>共<span><font color="blue">${pagination.recordCount}</font></span>个logo</p>
			</div>
			<ul class="btn">
				<li>
					<input type="button" class="groovybutton" value="创建LOGO" title="创建LOGO" onclick="createCustomLogo('${ctx}/custom/add')" />
				</li>				
			</ul>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th>序号</th>
					<th>logo</th>
					<th>创建时间</th>
					<th>状态</th>
					<th>操作</th>
				</tr>
				<c:forEach items="${pagination.recordList}" var="logo" varStatus="i">
				<tr>
					<td title="${logo.id}">${logo.id}</td>
					<td><img src="<%=logourl%>/${logo.logoUrl}" width="50" height="40"   /></td>					
					<td>
						<fmt:formatDate value="${logo.createTime}" pattern="yyyy-MM-dd HH:mm"/>
					</td>
					<td>
						<c:if test="${logo.status==1}">正常</c:if>
						<c:if test="${logo.status==0}">冻结</c:if>
					</td>
					<td class="edit">
						<ul class="f_ul">
							<li><a href="javascript:deleteCustomLogo('${ctx}/custom/del?id=${logo.id}','${logo.id}')">删除</a></li>
							<c:if test="${logo.status==1}"><li><a href="javascript:freeze('${ctx}/custom/freeze?id=${logo.id}','${logo.id}')">禁用</a></li></c:if>
							<c:if test="${logo.status==0}"><li><a href="javascript:unfreeze('${ctx}/custom/unfreeze?id=${logo.id}','${logo.id}')">启用</a></li></c:if>
						</ul>
					</td>
				</tr>
			</c:forEach>
					
			</tbody>
		</table>
		
			<c:if test="${pagination.recordList==null||fn:length(pagination.recordList)<1}">
				<div class="no_data">没有匹配的logo</div>
			</c:if>
	
		
		<div class="ribbon clearfix">
			<%@ include file="/WEB-INF/jsp/common/page.jsp"%> 
		</div>
		
		<%--当前页--%>
		<input type="hidden" id="currentPage" name="currentPage" value="${pagination.currentPage}">
		<%--总页数--%>
		<input type="hidden" id="totalPageCount" value="${pagination.pageCount}">
		<input type="hidden" name="msg" id="msg" value="${message}">
        
	</form>
	<br />
	<br />
	<br />
	</div>
  </body>
</html>
