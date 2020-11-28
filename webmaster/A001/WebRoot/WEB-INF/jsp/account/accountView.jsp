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
	<script type="text/javascript" src="${ctx}/resources/script/account/accountDetail.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		
		<form id="DetailForm" method="post">
		<ul class="tab clearfix">
			<li class="now"><a href="javascript:void(0);" >查看账号</a></li>
		</ul>
		<h3 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> --><span>基本信息</span></h3>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="edit_table">
			<tbody>
				<tr>
					<th width="30%">用户名</th>
					<td>
						${user.userId}
					</td>
					
				</tr>
				<tr>
					<th width="30%">Email</th>
					<td>
						${user.email}
					</td>
					
				</tr>
				<tr>
					<th width="30%">公司名称</th>
					<td>
						${corp.company}
					</td>
				</tr>
				<tr>
					<th width="30%">公司地址</th>
					<td>
						${corp.address}
					</td>
				</tr>
				<tr>
					<th width="30%">邮编</th>
					<td>
						${corp.zip}
					</td>
				</tr>
				<tr>
					<th width="30%">联系人姓名</th>
					<td>
						${user.trueName}
					</td>
				</tr>
				<tr>
					<th width="30%">联系人职位</th>
					<td>
						${user.job}
					</td>
				</tr>
				<tr>
					<th width="30%">联系电话</th>
					<td>
						${user.phone}
					</td>
				</tr>
				<tr>
					<th width="30%">行业</th>
					<td>
						${corp.industry}
					</td>
				</tr>
				<tr>
					<th width="30%">您通过什么途径找到我们的</th>
					<td>
						${corp.way}
					</td>
				</tr>
				<tr>
					<th width="30%">您的邮件地址数量是多少</th>
					<td>
					  <c:choose>
					    <c:when test="${cos.totalSend == -1}">无限制</c:when>
					    <c:otherwise>${cos.totalSend}万</c:otherwise>
					  </c:choose>						
					</td>
				</tr>
				<tr>
					<th width="30%">预计每月发送数量</th>
					<td>
					  <c:choose>
					    <c:when test="${cos.monthSend == -1}">无限制</c:when>
					    <c:otherwise>${cos.monthSend}万</c:otherwise>
					  </c:choose>
					</td>
				</tr>
				<tr>
					<th width="30%">注册时间</th>
					<td>
						<fmt:formatDate value="${user.createTime}" pattern="yyyy-MM-dd HH:mm"/>
					</td>
				</tr>
				</tbody>
		</table>
		
		<br>
		<table align="center">
			<tr>
				<td>
				<!-- 					
					<sec:authorize ifAnyGranted="ROLE_ADM">
					   <input id="btn_submit" type="button" class="input_yes" value="激 活" title="激活" onclick="activate('${ctx}/account/activateAsny?userId=${user.userId}','${user.userId}')"/>
					</sec:authorize>
				 -->
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
		<input type="hidden" name="msg" id="msg" value="${message}">
		<input type="hidden" id="successUrl" name="successUrl" value="${ctx}/account/hadBeenActivatedList" />
		<input type="hidden" id="backUrl" name="backUrl" value="${ctx}/account/waitForActivateList" />
		
	</form>
	<br>
	</div>
  </body>
</html>
