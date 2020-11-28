<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
    <%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>${pageTitle} - 忘记密码</title>
    <link rel="stylesheet" type="text/css" href="${ctx}/resources/css/global.css?t=${version}" />
	<link rel="stylesheet" type="text/css" href="${ctx}/resources/css/password.css?t=${version}" />
    <script type="text/javascript" src="${ctx}/resources/script/common/jquery-1.9.1.min.js?t=${version}"></script>
	
</head>
	
<body class="inx_body">
	<div class="inx_top">
		<div class="inx_maun"></div><a href="javascript:void(0)" class="logo_mi"></a>
	</div>
    
	<div class="pass_main">
		<div class="pass_maincen">
			<h1 class="pass_h1">${status}</h1>
		</div>
	</div>
    
    <div class="pass_maincen">
		<div class="pass_cut">
			<h1>${title}</h1>
			<ul>
				<li class="f_site12">${message}</li>
				<li class="f_l" style="padding-top:50px;"></li>
				 <li style="padding-top:50px;"><input id="submit" type="button" value="关闭" onclick="window.location.assign('${ctx}/index')" /></li>
			</ul>
			<div class="clear"></div>
		</div>
	</div>
	
 	<div class="inx_bot">
		mtarget.asia&nbsp;&nbsp;<a href="${website}/anti_spam.php?id=26">隐私与服务协议</a>&nbsp;&nbsp;<a href="${website}/anti_spam.php">反垃圾邮件政策</a>&nbsp;&nbsp;京ICP备11035441号
	</div>
</body>
</html>