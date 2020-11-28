<%@ page language="java" pageEncoding="UTF-8"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />



<script src="${pageContext.request.contextPath}/resources/menu/sdmenu.js" type="text/javascript"></script>
<link href="${pageContext.request.contextPath}/resources/menu/css.css" rel="stylesheet" type="text/css" />
</head>

<body>
<div align="center">
<div id="my_menu" class="sdmenu">
      <div>
        <span>账号管理</span>
        <a href="${pageContext.request.contextPath}/account/accountTab" target="main">待激活账号</a>
        <a href="${pageContext.request.contextPath}/account/hadBeenActivatedList" target="main">已激活账号</a>
      </div>
	  <br>
      <div>
        <span>任务管理</span>
        <a href="http://51xuediannao.com/show/qiye/">待审核任务</a>
        <a href="http://51xuediannao.com/show/blog/">已审核任务</a>
        <a href="http://51xuediannao.com/show/shop/">不需要审核任务</a>
      </div>
      
    </div>
</div>






</body>
</html>
<script type="text/javascript">
	// <![CDATA[
	var myMenu;
	window.onload = function() {
		myMenu = new SDMenu("my_menu");
		myMenu.init();
		var firstSubmenu = myMenu.submenus[0];
myMenu.expandMenu(firstSubmenu);
	};
	// ]]>
	</script>