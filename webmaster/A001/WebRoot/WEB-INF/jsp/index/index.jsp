<%@ page language="java" pageEncoding="UTF-8"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta http-equiv="Content-Language" content="zh-cn" />
<meta http-equiv="Cache-Control" content="no-store" />
<meta http-equiv="Pragma" content="no-cache" />
<meta http-equiv="Expires" content="0" />

<style type="text/css">
html,body {
	overflow: hidden;
	height: 100%;
	margin: 0;
	padding: 0;
	font: 14px/1.8 Georgia, Arial, Simsun;
}

html {
	padding: 110px 0;
}

#hd {
	position: absolute;
	top: 0;
	left: 0;
	width: 100%;
	height: 100px;
	background: #F2F2F2;
}

#bd {
	position: absolute;
	top: 105px;
	right: 0;
	bottom: 55px;
	left: 0;
	overflow: hidden;
	width: 100%;
	_height: 100%;
}

#side {
	position: absolute;
	top: 0;
	left: 0;
	bottom: 0;
	overflow: auto;
	width: 200px;
	_height: 100%;
	background: #F2F2F2;
}

#main {
	position: absolute;
	_position: static;
	top: 0;
	right: 0;
	bottom: 0;
	left: 205px;
	overflow: auto;
	_overflow: hidden;
	height:100%;
	_margin-left: 210px;
	
}

#content {
	_overflow: auto;
	_width: 100%;
	_height: 100%;
}

#ft {
	position: absolute;
	bottom: 0;
	left: 0;
	width: 100%;
	height: 50px;
	background: #999;
}
/* 与布局无关，一些说明性内容样式 */
.tit-layout {
	margin: 30px 0 0;
	font-size: 18px;
	text-align: center;
}

.copyright {
	font-weight: bold;
	text-align: center;
}

#feature {
	width: 200%;
	line-height: 4;
}

#feature .hd {
	padding: 20px 15px;
}

#feature .hd h2 {
	margin: 0;
	font-size: 16px;
}

#feature .bd ol {
	margin-top: 0;
}

#feature .bd h3 {
	margin: 0;
	padding: 0 15px;
	font-size: 14px;
}

#feature .ft {
	padding: 10px 15px 30px;
}
</style>

<script type="text/javascript">
function SetCwinHeight() { 
	var iframeid = document.getElementById("maincontent1"); //iframe id 
	iframeid.height = "10px";//先给一个够小的初值,然后再长高. 
	if (document.getElementById) { 
	if (iframeid && !window.opera) { 
	if (iframeid.contentDocument && iframeid.contentDocument.body.offsetHeight) { 
	iframeid.height = iframeid.contentDocument.body.offsetHeight; 
	} else if (iframeid.Document && iframeid.Document.body.scrollHeight) { 
	iframeid.height = iframeid.Document.body.scrollHeight; 
	} 
	} 
	} 
	} 
</script>

</head>
<body>
	<div id="hd">
		<h1 class="tit-layout">M.Target后台管理</h1>
		<a href="${pageContext.request.contextPath}/j_spring_security_logout">注销</a>
	</div>
	<div id="bd">
		<div id="side">
			<ul>
				<li><a href="${pageContext.request.contextPath}/account/waitForActivateList" target="win">账号管理</a></li>
				<li><a href="${pageContext.request.contextPath}/account/waitForActivateList" target="iframepage">待激活账号</a></li>
				<li><a href="${pageContext.request.contextPath}/account/hadBeenActivatedList" target="iframepage">已激活账号</a></li>
				<li><a href="${pageContext.request.contextPath}/account/cosManage" target="iframepage">套餐管理</a></li>
				<li><a href="http://www.csdn.net" target="win">CSDN</a></li>
				<li><a href="http://www.iteye.com" target="win">ITEYE</a></li>
			</ul>
		</div>
		<div id="main">
			<%--<iframe height="1000px" width="100%" align="middle"  id="win" name="win" onload="Javascript:SetWinHeight(this)" frameborder="0" scrolling="auto" src="http://www.cnblogs.com"></iframe>
			--%>
			<iframe style="z-index: 1; visibility: inherit; width: 100%;" name="maincontent1" 
onload="Javascript:SetCwinHeight()" height="1" id="maincontent1" src="http://cnblogs.com" 
marginwidth="0" marginheight="0" frameborder="0" scrolling="no"></iframe> 
		</div>
	</div>
	<div id="ft">
		<address class="copyright">
			Copyright &copy;
		</address>
	</div>
</body>
</html>



