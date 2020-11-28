<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<link rel="shortcut icon" href="${ctx}/resources/img/favicon.ico" type="image/x-icon" />
<title>M.target后台系统-用户登录</title>
<link href="${ctx}/resources/login/images/login.css" rel="stylesheet"
	type="text/css" />
<%@ include file="/WEB-INF/jsp/common/script.jsp"%>

<script type="text/javascript">
	if (window.top != window.self)
		top.location.href = window.location.href;
	
	
	var errorCount = "${errorCount}";
	$(document).ready(function() {
		if ("${message}" != "") {
			$("#promt").html("<font color='red'>${message}</font>");
			shakeLogin();
		}
		
		if(errorCount!=null&&parseInt(errorCount)>=5) {
			$("#btn").show();
			$("#kapchainput").show();
		}
		
	});
	
	$(document).keyup(function(event){
		  if(event.keyCode ==13){
			  doSubmit();
		  }
	});

	function doSubmit() {
		var username = $("#j_username").val();
		var password = $("#j_password").val();
		if (isEmpty(username)) {
			$("#promt").html("<font color='red'>请输入用户名</font>");
			$("#j_username").focus();
			shakeLogin();
		} else if (isEmpty(password)) {
			$("#promt").html("<font color='red'>请输入密码</font>");
			$("#j_password").focus();
			shakeLogin();
		} else {
			if(errorCount!=null&&parseInt(errorCount)>=5) {
				var verifyCodeValue = $("#verifyCode").val();
				if(isEmpty(verifyCodeValue)) {
					$("#promt").html("<font color='red'>请输入验证码</font>");
					$("#verifyCode").focus();
				}else {
						var verifyUrl = "${ctx}/verifyKaptchaCode?verifyCode="+verifyCodeValue;
						$.ajax({
			                type:"GET",
			                url:verifyUrl,
			                success:function(data){
			                    if(data.result=="failure") {
			                    	$("#promt").html("<font color='red'>请输入正确的验证码</font>");
			        				$("#verifyCode").focus();
			        				shakeLogin();
			                    }else if(data.result=="expired"){
			                    	document.getElementById("kaptchaimg").src ="${ctx}/Kaptcha.jpg?"+Math.floor(Math.random()*100);   
			                    	$("#promt").html("<font color='red'>请输入正确的验证码</font>");
			        				$("#verifyCode").focus();
			        				shakeLogin();
			                    }else {
			                    	$("#loadImg").show();
			            			$("#LoginForm").submit();
			                    }
			                },
			                error:function(e){
			                    alert(e);
			                }
			                
			            });
				}
			}else {
				$("#btn_login").attr("disabled",true);
				$("#loadImg").show();
    			$("#LoginForm").submit();
			}
			
			
		}
	}
	
	//点击切换验证码
    function changeVerifyCode(img){
        img.src ="${ctx}/Kaptcha.jpg?"+Math.floor(Math.random()*100);    
    }

	//抖动登陆
	function shakeLogin(){
		$("#login").addClass("shake");
		setTimeout(function(){
			$("#login").removeClass("shake");
		},600);
	}
</script>
</head>
<body>
	<div class="limiter">
		<div id="login" class="wt_shake">
			<div class="lg_top">			
			<c:choose>
                <c:when test="${logoUrl == null || logoUrl =='' }">
                      <img height='45' src="${ctx}/resources/img/logo_blue7.png"/>后台系统
                </c:when>
                <c:otherwise>
                      <img height='45' src="${nginxUrl}/${logoUrl}"/>后台系统
                </c:otherwise>
            </c:choose>						
			</div>
			<div class="lg_content">
				<div id="darkbanner" class="banner320">
					<h2>Login</h2>
				</div>
				<div id="darkbannerwrap"> </div>
				<form id="LoginForm" action="${ctx}/j_spring_security_check" method="post">
					<input type="hidden" name="token" value="${token}" />
					<div id="user" class="item">
						<label for="j_username">用户名：</label>
						<input type="text" id="j_username" name="j_username" value="${mid}" maxlength="20"/>
					</div>
					<div id="password" class="item">
						<label for="j_password">密　码：</label>
						<input type="password" id="j_password" name="j_password" maxlength="16"/>
					</div>
					<div id="remenber">
						<input id="remember_me" name="remember_me" class="inx_check" type="checkbox" value="true" <c:if test="${remember_me eq true}">checked="checked"</c:if>/>
						<label for="remember_me">记住用户</label>
					</div>
					<div id="kapchainput" class="item" style="display:none;">
						<label for="verifyCode">验证码：</label>
						<input type="text" name="verifyCode" id="verifyCode" style="width:85px" maxlength="4" />
						<img id="kaptchaimg" src="${ctx}/Kaptcha.jpg" onclick="changeVerifyCode(this)" height='30' title="看不清，换一张" />
					</div>
					<div class="item" id="promt" style=""></div>
					<div id="loginbtn" class="item">
						<input type="button" id="btn_login" onclick="doSubmit()" value="登　录"><img id="loadImg" src="${ctx}/resources/img/loading.gif" /> 
					</div>
				</form>
			</div>
			<div class="lg_bottom"></div>
		</div>
	</div>
</body>
</html>
