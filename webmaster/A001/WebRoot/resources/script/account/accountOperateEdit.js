$(document).ready(function() {
	$("#DetailForm").validate({
		errorPlacement : function(error, element) { 
			var e = element.attr("id")+"_span";
			var content = "<font color='red'>"+$(error).text()+"</font>"
			$("#"+e).html(content);
			element.change(function(){
				$("#"+e).html("");
	        });
				
		},
		rules: {
			userId:{
				required: true,
				minlength: 4
			},
			roleId: "required",
			password: {
				required: false,
				minlength: 8
			},
			confirmPassword: {
				required: false,
				minlength: 8,
				equalTo: "#password"
			},
			email: {
				required: true,
				email: true
			},
			trueName: {
				required: false
			},
			phone: {
				required: false
			},
			robots: {
				required: false
			}
		},
		messages: {
			userId: {
				required:"请输入用户名",
				minlength: "用户名长度至少4个字符"
			},
			roleId: "请选择角色",
			password: {
				minlength: "密码长度至少8个字符"
			},
			confirmPassword: {
				minlength: "密码长度至少8个字符",
				equalTo: "两次输入的密码不相等"
			},
			email: {
				required:"请输入邮箱",
				email: "请输入正确的邮箱格式"
			}
		},
		submitHandler: function() {
			if(checkRobots()) {
				if(pass()) {
					var form=$("#DetailForm"),params = form.serializeArray();
					$("#btn_submit").attr("disabled",true);
					$("#prompt").show();
					$.post(form.attr("action"),params,function(data){
						if(data){
							if("success"==data.result){
								//$("#msg",window.dialogArguments.document).val(data.message);
								$('body').messagebox(data.message, '', 1, 2000);
								//window.dialogArguments.location.reload();
				 	     		setTimeout("success()",2000);
							}else {
								$("#prompt").hide();
								$("#btn_submit").attr("disabled",false);
								if (typeof(data.message) == "undefined") { 
									$('body').messagebox("无法获取响应，有可能账号已在别处登录", '', 0, 3000); 
							    }else {
							    	$('body').messagebox(data.message, '', 0, 3000);
							    }   
							}
						}
						
					});
				}
			}
		}
	});
});



function saveAccount(url,flag) {
	//$("#DetailForm").validate();
	//$("#DetailForm").valid();
	$("#DetailForm").attr("action",url);
	$("#DetailForm").submit();
}



//返回
function success() {
	var url = $("#successUrl").val();
	window.location.href = url;
}

function back(){
//	var url = $("#backUrl").val();
	var url = "/account/accountList";
	window.location.href = url;
}

function pass() {
	var p = $("#password").val();
	if(p!=null && p!="") {
		if(p.replace(/\s/g,"") == "") {
			$('body').messagebox("密码不能为空格", '', 0, 3000);
			$("#password").focus();
			return false;
		}else {
			return true;
		}
	}
	return true;
}

function changeType(url) {
	var accountType = $("#roleId  option:selected").val();
	window.location.href = url + accountType;
}


function checkRobots() {
	var flag = false;
	var result = true;
	$("input[name='robots']").each(function(i){
		if($(this).val()!="") {
			flag = true;
		}
	});
	
	if(!flag) {
		$('body').messagebox("请输入网络发件人！", '', 0, 2000);
		$("#robot_0").focus();
		result = false;
	}else {
		$("input[name='robots']").each(function(i) {
			var emailValue = $(this).val();
			if(emailValue!="") {
				if(!isEmail(emailValue)) {
					$('body').messagebox("网络发件人格式不正确！", '', 0, 2000);
					$(this).focus();
					result = false;
					return false;
				}
			}
		});
	}
	
	return result;
}


