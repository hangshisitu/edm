<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>M.target后台管理系统</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="shortcut icon" href="${ctx}/resources/img/favicon.ico" type="image/x-icon" />

<%@ include file="/WEB-INF/jsp/common/css.jsp"%>
<%@ include file="/WEB-INF/jsp/common/script.jsp"%>

<style>
* {
	margin: 0;
	padding: 0
}

body,html {
	height: 100%;
	width: 100%;
	overflow: hidden;
} /*这个高度很重要*/
#frametable .header {
	height: 64px;
	background: #ddd;
	border-bottom: 2px solid #999;
}

#frametable .left {
	width: 180px;
	border-right: 2px solid #999;
	background: #EFEFEF;
	height: 100%;
}
</style>


<script type="text/javascript">

	function openLogout() {
		$("#exit").show();
	}
	
	function logout(){
		var url = "${ctx}/j_spring_security_logout";
		window.location.href = url;
	}
	
	function closeLogout(){
		$("#exit").hide();
	}
</script>
</head>

<body>
	<table id="frametable" cellpadding="0" cellspacing="0" width="100%"
		height="100%" style="width: 100%;">
		<tr>
			<td colspan="2" width="100%" height="64">
				<div class="header">
				<%--<%@ include file="/WEB-INF/jsp/layout/header.jsp"%>
				--%>
					<div id="header" class="clearfix">
					<c:choose>
                       <c:when test="${logoUrl == null || logoUrl =='' }">
                          <a href="#" id="logo" style="background-image:url(resources/img/logo_blue7.png);"></a>
                       </c:when>
                       <c:otherwise>
                          <a href="#" id="logo" style="background-image:url('${nginxUrl}/${logoUrl}');"></a>
                       </c:otherwise>
                    </c:choose>
                    
						<div id="logo_title">
							后台管理系统
						</div>
						<div class="f_r">
							<ul id="topnav" class="clearfix">
								<li><a href="javascript:openLogout()" id="logout">退出系统&nbsp;</a>
								</li>
							</ul>
							<div id="welcome">${currentUser.username}，M.target欢迎您！</div>
						</div>
						<div class="popup" id="exit">
							<div class="clearfix logout_open">
								<h1>您确定退出系统吗？</h1>
								<form method="get" name="logout_from">
									<input type="button" onclick="logout()" class="input_yes" onfocus="this.blur()" value="确定" />
									<input type="button" onclick="closeLogout()" class="input_no" onfocus="this.blur()" value="取消" />			
								</form>
							</div>
						</div>
					</div>
				</div></td>
		</tr>
		<tr>
			<td valign="top" width="182" height="100%">
				<!--这个高度很重要-->
				<div class="left">
					<%@ include file="/WEB-INF/jsp/layout/left.jsp"%>
				</div>
			</td>
			<td valign="top" width="100%" height="100%">
				<!--这个高度很重要--> 
				<iframe id="iframe" name="main"
					src="${ctx}/taskReport/list?type=0" width="100%"
					allowtransparency="true" width="100%" height="100%" frameborder="0" 
					scrolling="yes" style="overflow:visible;"></iframe>
					<br />
					<br />
			</td>
			
		</tr>
	</table>
	<input type="hidden" name="ctx" value="${ctx}" />
</body>
</html>
