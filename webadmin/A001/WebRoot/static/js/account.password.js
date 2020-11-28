$(document).ready(function() {
	$('#add').ajaxForm({    //表单名
		url: 'add',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json', 
		beforeSubmit: function(formData, jqForm, options) {   //验证
			loading();
			var form = jqForm[0]; 
			if (!form.oldPassword.value) { 
				error_message("请输入原密码", 1);
				form.oldPassword.focus();
				del_loading();
				return false; 
			}
			if (!form.password.value) { 
				error_message("请输入新密码", 1);
				form.password.focus();
				del_loading();
				return false; 
			}
			if (!form.passwordConfirm.value) { 
				error_message("请输入确认密码", 1);
				form.passwordConfirm.focus();
				del_loading();
				return false; 
			}
		},
		success: function(data) { 
			$("input[name=oldPassword]").val("");
			$("input[name=password]").val("");
			$("input[name=passwordConfirm]").val("");
			if (data.code == 1) {
				error_message(data.message);
			} else {
				error_message(data.message,1);
			}
			del_loading();
		}   
	}); 
});
