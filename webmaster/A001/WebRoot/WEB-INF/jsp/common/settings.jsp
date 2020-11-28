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
	<script>
		$(document).ready(function() { 
			$('#submit').hover(function(){
				$(this).css("background-image","url(${ctx}/resources/images/pass_but1_2.png)"); 
			},function(){
				$(this).css("background-image","url(${ctx}/resources/images/pass_but1.png)"); 
			});
			$('#reset').hover(function(){
				$(this).css("background-image","url(${ctx}/resources/images/pass_but2_2.png)"); 
			},function(){
				$(this).css("background-image","url(${ctx}/resources/images/pass_but2.png)"); 
			});
		});

		function chkfrm(obj){	
			if(obj.password.value.replace(/\s/g,"") == "") {
				$(".pass1_tit").html("* 请输入密码");
				obj.password.focus();
				$("input[name=password]").addClass("input_err"); 
		        return false; 
			}
			
			if (!obj.password.value) { 
				$(".pass1_tit").html("* 请输入密码");
				obj.password.focus();
				$("input[name=password]").addClass("input_err"); 
		        return false; 
		    } else {
				$(".pass1_tit").html("*");
				$("input[name=password]").removeClass("input_err"); 
			}
			
			if (!obj.passwordConfirm.value) { 
				$(".pass2_tit").html("* 请输入密码确认");
				obj.passwordConfirm.focus();
				$("input[name=passwordConfirm]").addClass("input_err"); 
		        return false; 
		    } else {
				if(obj.passwordConfirm.value == obj.password.value) {
					$(".pass2_tit").html("*");
					$("input[name=passwordConfirm]").removeClass("input_err"); 
				} else {
					$(".pass2_tit").html("* 二次输入的密码不相同");
					$("input[name=passwordConfirm]").removeClass("input_err");
					return false; 
				}
			}
			
			if(obj.password.value.length<6) {
				$(".pass1_tit").html("* 密码的长度在6到16位之间");
				obj.password.focus();
				$("input[name=password]").addClass("input_err");
				return false; 
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
			<h1 class="pass_h1">重置密码</h1>
		</div>
	</div>

	<div class="pass_maincen">
		<div class="pass_cut">
			<h1>修改帐户密码<span>（请输入您的密码，我们会将重置密码。）</span></h1>
			<form id="settings" name="settings" action="settings" method="post" onsubmit="return chkfrm(this);">
				<input type="hidden" name="SID" value="${SID}" />
				<input type="hidden" name="R" value="${R}" />
				<ul>
					<li class="f_l">用户名：</li>
					<li>${userId}</li>
					<li class="f_l">电子邮件：</li>
					<li>${email}</li>
					<li class="f_l">密码：</li>
					<li><input name="password" type="password" class="pass" /><span class="redfont pass1_tit">*</span></li>
					<li class="f_l">密码确认：</li>
					<li><input name="passwordConfirm" type="password" class="pass" /><span class="redfont pass2_tit">*</span></li>
					<li class="f_l" style="padding-top: 50px;"></li>
					<li style="padding-top: 50px;"><input name="submit" type="submit" id="submit" value="提交" /><input name="reset" type="reset" id="reset" value="取消" /></li>
				</ul>
			</form>
			<div class="clear"></div>
		</div>
	</div>

 	<div class="inx_bot">
		mtarget.asia&nbsp;&nbsp;<a href="${website}/anti_spam.php?id=26">隐私与服务协议</a>&nbsp;&nbsp;<a href="${website}/anti_spam.php">反垃圾邮件政策</a>&nbsp;&nbsp;京ICP备11035441号
	</div>
</html>