<%@ page language="java" pageEncoding="UTF-8"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>M.target后台管理</title>
</head>

<frameset cols="*" rows="80, *" id="frame_main"  border="0">
    <frame src="${pageContext.request.contextPath}/header" noresize="noresize" name="header">
    
    <frameset cols="170, *">
    	<frame src="${pageContext.request.contextPath}/menu" name="menu" />
    	<frame src="${pageContext.request.contextPath}/account/waitForActivateList" name="main">
    </frameset>
</frameset>

<noframes><body>
</body>
</noframes></html>
