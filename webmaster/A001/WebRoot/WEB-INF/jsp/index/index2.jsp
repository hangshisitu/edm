<%@ page language="java" pageEncoding="UTF-8"%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
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
	_height: 100%;
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
	function SetWinHeight(obj)
	{
		var win = obj;
		if (document.getElementById) {
			if (win && !window.opera)
			{
				if (win.contentDocument
						&& win.contentDocument.body.offsetHeight)
					win.height = win.contentDocument.body.offsetHeight;
				else if (win.Document && win.Document.body.scrollHeight)
					win.height = win.Document.body.scrollHeight;
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
			
		</div>
		<div id="main">
			<iframe height="1000px" width="100%" align="middle"  id="win" name="win" onload="Javascript:SetWinHeight(this)" frameborder="0" scrolling="auto" src="http://www.cnblogs.com"></iframe>
		</div>
	</div>
	<div id="ft">
		<address class="copyright">
			Copyright &copy;
		</address>
	</div>
</body>
</html>



