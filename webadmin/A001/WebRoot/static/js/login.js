$(document).ready(function() {
	var headers = {};
	headers['token'] = $("input[name=token]").val();
	
	// 输入框holdplace
	if (!$("input[name=j_password]").val() == "") {
		$("input[name=j_password]").removeClass("bg");
	}
	if (!$("input[name=j_username]").val() == "") {
		$("input[name=j_username]").removeClass("bg");
	}
	if (!$("input[name=kaptcha]").val() == "") {
		$("input[name=kaptcha]").removeClass("bg");
	}
	$("input[name=j_username],input[name=j_password],input[name=kaptcha]").focus(function() {
	  	$(this).removeClass("bg");
	});
	$("input[name=j_username],input[name=j_password],input[name=kaptcha]").blur(function() {
		if($(this).val() == ""){
	  		$(this).addClass("bg");
		} else {
			$(this).removeClass("bg");
		}
	});
	
	// 登陆处理
	$('#login_form').ajaxForm({   	
		url: '/j_spring_security_check',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: "json",
		timeout: 15000,
		beforeSerialize: function($form, options) {
			loginStart();
		},
		error: function(){
			loginEnd();
			$("#login_form .err").html('登录超时'); 
		}, 
		beforeSubmit: function(formData, jqForm, options) {
			$("#login_form .err").html("");
			var form = jqForm[0]; 
			if (!form.j_username.value) { 
				$("#login_form .err").html('请输入用户名'); 
				form.j_username.focus();
				loginEnd(); 
				return false; 
			}
			if (!form.j_password.value) { 
				$("#login_form .err").html('请输入密码'); 
				form.j_password.focus();
				loginEnd(); 
				return false; 
			}
			if ($("input[name=kaptcha]").is(":hidden") == false && !form.kaptcha.value){
				$("#login_form .err").html('请输入验证码'); 
				form.kaptcha.focus();
				loginEnd(); 
				return false; 
			}
		},
		success: function(data) {
			if (data.code == "1") {
				if(data.assign=="AROUND" || data.assign=="ALREADY" || data.assign=="NOTNOW"){
					alert(data.message);
				}
				location.assign("/");
			} else {
				loginEnd();
				if(data.message == "账号输入有误" || data.message == "账号已失效") {
					$("input[name=j_username]").val("");
					$("input[name=j_username]").focus();
					$("input[name=j_password]").val("");
					$("input[name=kaptcha]").val("");
				} else if(data.message == "密码输入有误") {
					$("input[name=j_password]").val("");
					$("input[name=j_password]").focus();
				} else if(data.message == "验证码输入有误" || data.message == "验证码超时" || data.message == "登录超时") {
					$("input[name=kaptcha]").val("");
					$("input[name=kaptcha]").focus();
				} else {
					$("input[name=j_username]").focus();
				}
				refresh();
				if(data.count > 4) {
					$("#code_li").show();
				}
				$("#login_form .err").html(data.message); 
			}
		}
	});
	
	$('#forget_form').ajaxForm({   	
		url: '/forget',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: "json",
		timeout: 15000,
		error: function(){
			$("#forget_form .err").html('邮件发送超时'); 
		}, 
		beforeSubmit: function(formData, jqForm, options) {
			loading(".msg_content",".load_img");
			$("#forget_form .err").html("");
			
			var form = jqForm[0]; 
			if (!form.userId.value) {
				del_loading(".load_img");
				$("#forget_form .err").html('请输入用户名'); 
				form.userId.focus();
				return false; 
			}
		},
		success: function(data) {
			del_loading(".load_img");
			if (data.code == "1") {
				$("#forget_form .err").html(data.message); 
			} else {
				$("#forget_form .err").html(data.message); 
			}
		}
	});
});

function loginStart(){
	$("input[name=submit]").addClass("submit_load");
}

function loginEnd(){
	$("input[name=submit]").removeClass("submit_load");
}

function refresh() {
	$("input[name=kaptcha]").val("");
	$("input[name=kaptcha]").focus();
	var version = new Date().getTime();
	$("#kaptcha").attr("src", "/kaptcha.jpg?" + version);
}

function resetPassBox(){
	$("#forget_form")[0].reset();
	$("#forget_form").find('.err').html('');
	openSlideBox('.locat','#passBox');
	$("#forget_form")[0].userId.focus();
}
