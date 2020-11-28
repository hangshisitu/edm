<%@ page language="java" pageEncoding="UTF-8"%>
<div id="header" class="clearfix">
	<a href="#" id="logo"></a>
	<div id="logo_title">
		后台管理系统
	</div>
	<div class="f_r">
		<ul id="topnav" class="clearfix">
			<li><a href="javascript:logout()" id="logout">退出系统&nbsp;</a>
			</li>
		</ul>
		<div id="welcome">${currentUser.username}，M.target欢迎您！</div>
	</div>
</div>
<script>
	function logout() {
		if(window.confirm("确定要退出系统吗？")) {
			var url = "${ctx}/j_spring_security_logout";
			window.location.href = url;
		}
	}
	
</script>