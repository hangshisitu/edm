$(document).ready(function() {
	$("#DetailForm").validate({
		errorPlacement : function(error, element) { 
			//element.val($(error).text())
			//element.focus(function(){$(this).val("")})
			
			var e = element.attr("id")+"_span";
			var content = "<font color='red'>"+$(error).text()+"</font>";
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
			trueName: {
				required: true,
				minlength: 2
			},
			password: {
				required: true,
				minlength: 8
			},
			confirmPassword: {
				required: true,
				minlength: 8,
				equalTo: "#password"
			}
		},
		messages: {
			userId: {
				required:"请输入用户名",
				minlength: "用户名长度至少4个字符"
			},
			trueName: {
				required:"请输入姓名",
				minlength: "姓名长度至少2个字符"
			},
			password: {
				required:"请输入密码",
				minlength: "密码长度至少8个字符"
			},
			confirmPassword: {
				required:"请输入密码",
				minlength: "密码长度至少8个字符",
				equalTo: "两次输入的密码不相等"
			}
		},
		submitHandler: function() {
			var form=$("#DetailForm"),params = form.serializeArray();
			$("#btn_save").attr("disabled",true);
			$("#prompt").show();
			$.post(form.attr("action"),params,function(data){
				if(data){
					if("success"==data.result){
						//$("#msg",window.dialogArguments.document).val(data.message);
						$('body').messagebox(data.message, '', 1, 2000);
						//window.dialogArguments.location.reload();
		 	     		setTimeout("closeWin()",2000);
					}else {
						$("#prompt").hide();
						$("#btn_save").attr("disabled",false);
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



function saveSubUser(url) {
	$("#DetailForm").attr("action",url);
	$("#DetailForm").submit();
}