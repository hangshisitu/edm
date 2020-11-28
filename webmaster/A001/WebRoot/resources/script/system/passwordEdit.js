$(document).ready(function() {
	$("#DetailForm").validate({
		errorPlacement : function(error, element) { 
			var e = element.attr("id")+"_span";
			var content = "<font color='red'>"+$(error).text()+"</font>";
			$("#"+e).html(content);
			element.change(function(){
				$("#"+e).html("");
	        });
		},
		rules: {
			oldPassword: {
				required: true
			},
			newPassword: {
				required: true,
				rangelength:[8,16]	
			},
			confirmNewPassword: {
				required: true,
				rangelength:[8,16],
				equalTo: "#newPassword"
			}
		},
		messages: {
			oldPassword: {
				required:"请输入原密码"
			},
			newPassword: {
				required:"请输入新密码",
				rangelength:"密码长度为8-16个字符"
			},
			confirmNewPassword: {
				required:"请确认新密码",
				rangelength:"密码长度为8-16个字符",
				equalTo: "两次输入的新密码不相等"
			}
		},
		submitHandler: function() {
			var form=$("#DetailForm"),params = form.serializeArray();
			$("#btn_submit").attr("disabled",true);
			$("#prompt").show();
			$.post(form.attr("action"),params,function(data){
				if(data){
					if("success"==data.result){
						resetPassword();
						$("#prompt").hide();
						$("#btn_submit").attr("disabled",false);
						$('body').messagebox(data.message, '', 1, 2000);
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
		
	});
		
});

//保存密码
function savePassword(url) {
	$("#DetailForm").attr("action",url);
	$("#DetailForm").submit();
}

//取消
function cancle(url){
	window.location.href = url;
	
}

//清空
function resetPassword() {
	$("#oldPassword").val("");
	$("#newPassword").val("");
	$("#confirmNewPassword").val("");
}




