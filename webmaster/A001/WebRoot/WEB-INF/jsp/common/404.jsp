<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link href="/resources/img/favicon.gif" type="image/x-icon" rel="shortcut icon" />
<title>404</title>
<style>
	* {
		margin: 0;
		padding: 0;
		font-family: "微软雅黑";
	}
	#error_404 {
		text-align: center;
		padding: 120px 0 50px 0;
		line-height: 25px;
		background-image: url(/resources/img/logo_blue.png);
		background-repeat: no-repeat;
		background-position: center 40px;
		margin-top: 15%;
		border-top: 2px solid #000;
		border-bottom: 2px solid #000;
		font-size: 14px;
	}
	#error_404 h1 {
		font-size: 18px;
		font-weight: normal;
	}
</style>
</head>

<body>
	<div id="error_404">
		<h1>Sorry，您访问的页面没有找到</h1>
		我们已经将此错误信息记录下来，并尽快处理，为此造成您的不便请多谅解。<br />
		<a href="javascript:void(0);" onclick="javascript:history.go(-1);">返回上页</a>&nbsp;&nbsp;&nbsp;<a href="/">返回首页</a><br />
	</div>
</body>
</html>
