<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
    <%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="shortcut icon" href="${ctx}/resources/img/favicon.ico" type="image/x-icon" />
<title>${pageTitle} - 忘记密码</title>
    <link rel="stylesheet" type="text/css" href="${ctx}/resources/css/global.css?t=${version}" />
	<link rel="stylesheet" type="text/css" href="${ctx}/resources/css/password.css?t=${version}" />
    <script type="text/javascript" src="${ctx}/resources/script/common/jquery-1.9.1.min.js?t=${version}"></script>
	<script>
		$(document).ready(function() { 
			$("#submit").hover(function(){
				$(this).css("background-image","url(${ctx}/resources/images/pass_but1_2.png)"); 
			},function(){
				$(this).css("background-image","url(${ctx}/resources/images/pass_but1.png)"); 
			});
			
			$("#reset").hover(function(){
				$(this).css("background-image","url(${ctx}/resources/images/pass_but2_2.png)"); 
			},function(){
				$(this).css("background-image","url(${ctx}/resources/images/pass_but2.png)"); 
			});
		});

		function chkfrm(obj){	
			if (obj.userId.value.replace(/\s/g,"") == "") { 
				$(".pass_cut ul li span").html("* 请输入用户名");
				obj.userId.focus();
				$("input[name=userId]").addClass("input_err"); 
		        return false; 
		    } else {
				$("input[name=userId]").removeClass("input_err"); 
			}
		}
	</script>
</head>
	
<body class="inx_body">
	<div class="inx_top">
		<div class="inx_maun"></div><a href="javascript:void(0)" class="logo_mi"></a>
	</div>
    
	<div class="pass_main">
		<div class="pass_maincen">
			<h1 class="pass_h1">忘记密码</h1>
		</div>
	</div>
    
    <div class="pass_maincen">
		<div class="pass_cut">
        	<h1>找回我的密码<span>（请输入您的用户名，我们会将重置密码邮件发送到您账号绑定的邮箱中，根据邮件内容重置密码。）</span></h1>
           	<form id="forget" name="forget" action="${ctx}/forget" method="post" onsubmit="return chkfrm(this);">
	            <ul>
		            <li class="f_l">用户名：</li>
		            <li><input name="userId" type="text" /><span class="redfont">*</span></li>
		            <li class="f_l" style="padding-top:50px;"></li>
		            <li style="padding-top:50px;"><input id="submit" name="submit" type="submit" value="提交" /><input id="reset" name="reset" type="reset" value="取消" /></li>
	            </ul>
			</form>
        	<div class="clear"></div>
    	</div>
	</div>

 	<div class="inx_bot">
		mtarget.asia&nbsp;&nbsp;<a href="${website}/anti_spam.php?id=26" target="_blank">隐私与服务协议</a>&nbsp;&nbsp;<a href="${website}/anti_spam.php" target="_blank">反垃圾邮件政策</a>&nbsp;&nbsp;京ICP备11035441号
	</div>
</body>
</html>