<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>任务模板查看</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">
	<link rel="shortcut icon" href="${ctx}/resources/img/favicon.ico" type="image/x-icon" />    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/task/templateDetail.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		<h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->任务&nbsp;<span>查看任务模板信息</span></h1>
		
		<form id="DetailForm" method="post">
		<ul class="tab clearfix">
			<li class="now"><a href="javascript:void(0)">任务模板</a></li>
		</ul>	
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="edit_table">
			<tbody>
				<tr>
					<th width="20%">模板类别:</th>
					<td>
						<c:if test="${template.categoryName ne null}">${template.categoryName}</c:if>
						<c:if test="${template.categoryName eq null}"> － </c:if>
					</td>
				</tr>
				<tr>
					<th>创建时间:</th>
					<td>
						<c:if test="${template.createTime ne null}"><fmt:formatDate value="${template.createTime}" pattern="yyyy-MM-dd HH:mm"/></c:if>
						<c:if test="${template.createTime eq null}"> － </c:if>
					</td>
				</tr>
				<tr>
					<th>修改时间:</th>
					<td>
						<c:if test="${template.modifyTime ne null}"><fmt:formatDate value="${template.modifyTime}" pattern="yyyy-MM-dd HH:mm"/></c:if>
						<c:if test="${template.modifyTime eq null}"> － </c:if>
					</td>
				</tr>
				<tr>
					<th>发件人:</th>
					<td>
						<c:if test="${task.senderName ne null}">${task.senderName}</c:if>
						<c:if test="${task.senderName eq null}">${senderName}</c:if>
					</td>
				</tr>
				<tr>
					<th>主   题:</th>
					<td>
						${template.subject}
					</td>
				</tr>
				<tr>
					<th>发件人邮箱地址:</th>
					<td>
						<c:if test="${task.senderEmail ne null}">${task.senderEmail}</c:if>
						<c:if test="${task.senderEmail eq null}">${senderEmail}</c:if>
					</td>
				</tr>
				<tr>
					<th>网络发件人地址:</th>
					<td>
						${robot}
					</td>
				</tr>
				<tr>
					<th>加入随机数:</th>
					<td>
						<c:if test="${rand eq true}">是</c:if>
						<c:if test="${rand eq false}">否</c:if>
					</td>
				</tr>
				<c:if test="${tSettingList ne null && fn:length(tSettingList) gt 0 }">
					<tr>
						<th>模 板:</th>
						<td>
							<select id="selTemp" onchange="javascript:sel();" style="width: 280px;">
								<option value="${templateId}" data-url='/task/view?taskId=${taskId}'>（主模板）${templateName}</option>
								 <c:choose>
								 	<c:when test="${action!=null && action=='touch'}">
								 		<c:forEach items="${tSettingList}" var="s">
								 			<option value="${s.templateId}" data-url='/task/view?action=touch&touchId=${s.taskId}&corpId=${s.corpId}&templateId=${s.templateId}&taskId=${taskId}&parentTemplateId=${templateId}'>（子模板）${s.templateName}</option>
								 		</c:forEach>
								 	</c:when>
								 </c:choose>
							</select>
							<script>
								$(function() {
									$("#selTemp").val("${template.templateId}");
								});
								function sel() {
									var url = $("#selTemp option:selected").attr("data-url");
									location.href = url;
								}
							</script>
						</td>
					</tr>
				</c:if>
				<%--<tr>
					<th>回复邮箱:</th>
					<td>
						${template.isRand}
					</td>
				</tr>
			--%></tbody>
		</table>
		<br>
		<div align="center">
			${template.content}
		</div>
		<br>
		<table align="center">
			<tr>
				<td>
					<%--<input onmouseover="this.style.cursor='pointer';this.style.cursor='hand';this.style.color='blue'" onmouseout="this.style.color=''" type="button" value="关 闭" onclick="closeWin()">
					--%><input type="button" class="input_no" value="关 闭" title="关闭" onclick="closeWin()">
				</td>
			</tr>
		</table>
		<input type="hidden" name="msg" id="msg" value="${message}">
	</form>
	</div>
	<br/>
	<br/>
  </body>
</html>
