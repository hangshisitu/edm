<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>更改通道</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/res/resUserList.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->通道管理&nbsp;<span></span></h1>
		
		<form id="ListForm" action="${ctx}/resource/resUserList" method="get">
			<%--<ul class="search clearfix m_b00 no_display">--%>
				<ul class="search clearfix m_b00">
				<li>账号</li>
				<li>
					<input maxlength="20" type="text" id="search_userId" name="search_userId" value="${search_userId}">
				</li>
				<li>机构名称</li>
				<li>
					<input maxlength="50" type="text" id="search_corpPath" name="search_corpPath" value="${search_corpPath}">
				</li>
				<li>加入通道时间</li>
				<li style="width:13.6%">
					<input maxlength="10" class="date" id="d4311" id="search_beginTimeStr" name="search_beginTimeStr" value="${search_beginTimeStr}" class="Wdate" type="text" onFocus="WdatePicker({maxDate:'#F{$dp.$D(\'d4312\')}'})"/>
				</li>
				<li>到</li>
				<li>
					 <input maxlength="10"  class="date" id="d4312"  id="search_endTimeStr" name="search_endTimeStr" value="${search_endTimeStr}" class="Wdate" type="text" onFocus="WdatePicker({minDate:'#F{$dp.$D(\'d4311\')}'})"/>
				</li>
				<li>
				     <input type="button" class="groovybutton" value="查 询" title="查询" onclick="search()">
				</li>
				
			</ul>
			<%--<ul class="search clearfix m_b10 no_display">--%>
				<ul class="search clearfix m_b10">
				<li>通道</li>
				<li style="width:12.5%">
					<select id="search_formalId"
							name="search_formalId" style="width:100%;background-color: #F5F5F5">
								<option value="">请选择</option>
								<c:forEach items="${resourceList}" var="res">
										<option
											<c:if test="${search_formalId==res.resourceId}">selected="selected"</c:if>
											value="${res.resourceId}">${res.resourceName}</option>
								</c:forEach>
					</select>
				</li>
				<li>套餐类型</li>
				<li style="width:12.5%">
					<select id="search_cosId" name="search_cosId" style="width:100%;background-color: #F5F5F5">
								<option value="">请选择</option>
								<c:if test="${testCosList!=null}">
									<option
										<c:if test="${search_cosId=='10000000'}">selected="selected"</c:if>
										value="10000000">试用套餐</option>
									<c:forEach items="${testCosList}" var="cos">
										<option
											<c:if test="${search_cosId==cos.cosId}">selected="selected"</c:if>
											value="${cos.cosId}">&nbsp;&nbsp;&nbsp;&nbsp;${cos.cosName}</option>
									</c:forEach>
								</c:if>
								<c:if test="${formalCosList!=null}">
									<option
										<c:if test="${search_cosId=='11111111'}">selected="selected"</c:if>
										value="11111111">正式套餐</option>
									<c:forEach items="${formalCosList}" var="cos">
										<option
											<c:if test="${search_cosId==cos.cosId}">selected="selected"</c:if>
											value="${cos.cosId}">&nbsp;&nbsp;&nbsp;&nbsp;${cos.cosName}</option>
									</c:forEach>
								</c:if>
						</select>
				</li>
				<li>创建方式</li>
				<li style="width:12.5%">
					<select id="search_way" name="search_way" style="width:100%;background-color: #F5F5F5">
								<option value="">请选择</option>
								<option value="custom"
									<c:if test="${search_way=='custom'}">selected="selected"</c:if>>自主注册</option>
								<option value="system"
									<c:if test="${search_way=='system'}">selected="selected"</c:if>>系统创建</option>
						</select>
				</li>
			</ul>
		
		<ul class="tab clearfix">
			<li class="now"><a href="javascript:void(0)">通道管理</a></li>
			<%--<li><a href="${ctx}/account/hadBeenActivatedList">已激活账号</a></li>
		--%></ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p>共<span><font color="blue">${pagination.recordCount}</font></span>个账号</p>
			</div>
			<sec:authorize ifAnyGranted="ROLE_ADM">
			<ul class="btn" style="width:30%;">
				<%--<li>更改通道</li>
				<li style="width:45%">
					<select id="resId"
							name="resId" style="width:100%;background-color: #F5F5F5">
								<option value="">请选择</option>
								<c:forEach items="${resourceList}" var="res">
										<option
											<c:if test="${resId==res.resourceId}">selected="selected"</c:if>
											value="${res.resourceId}">${res.resourceName}</option>
								</c:forEach>
					</select>
				</li>
				<li>
						<input type="button" class="groovybutton" value="确 定" title="确 定" onclick="changeRes('${ctx}/resource/changeRes')">
				</li>
				
				--%>
			</ul>
			</sec:authorize>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th width="25"><input type="checkbox" id="all_checkbox"/></th>
					<th>账号</th>
					<th>机构名称</th>
					<th>套餐类型</th>
					<th>状态</th>
					<th>创建方式</th>
					<th>通道</th>
					<th>加入通道时间</th>
				</tr>
				<c:forEach items="${pagination.recordList}" var="user" varStatus="i">
				<tr>
					<td><input type="checkbox" name="checkedIds" value="${user.corpId}" /></td>
					<td title="${user.userId}">${user.userId}</td>
					<td title="${user.corpPath}">${user.corpPath}</td>
					<td>${user.cosName}</td>
					<td>
						<c:if test="${user.status==1}">有效</c:if>
						<c:if test="${user.status==2}">冻结</c:if>
					</td>
					<td>
						<c:if test="${user.way=='system'}">系统创建</c:if>
						<c:if test="${user.way!='system'}">自主注册</c:if>
					</td>
					<td>
						<c:forEach items="${resourceList}" var="res">
							<c:if test="${user.formalId==res.resourceId}">${res.resourceName}</c:if>
						</c:forEach>
					</td>
					<td>
						<fmt:formatDate value="${user.createTime}" pattern="yyyy-MM-dd HH:mm"/>
					</td>
					
				</tr>
			</c:forEach>
					
			</tbody>
		</table>
		
			<c:if test="${pagination.recordList==null||fn:length(pagination.recordList)<1}">
				<div class="no_data">没有匹配的数据</div>
			</c:if>
	
		
		<div class="ribbon clearfix">
			<%@ include file="/WEB-INF/jsp/common/page.jsp"%> 
		</div>
		
		<%--当前页--%>
		<input type="hidden" id="currentPage" name="currentPage" value="${pagination.currentPage}">
		<%--总页数--%>
		<input type="hidden" id="totalPageCount" value="${pagination.pageCount}">
		<input type="hidden" name="msg" id="msg" value="${message}">
		<input type="hidden" name="token" value="${token}" />
	</form>
	   <br />
	<br />
	<br />
	</div>
  </body>
</html>
