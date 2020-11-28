$(document).ready(function(){
	$('#settings').ajaxForm({   	
		url: '/settings',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: "json",
		timeout: 15000,
		error: function() {
			error_message("修改密码超时", 1);
		}, 
		beforeSubmit: function(formData, jqForm, options) {
			var form = jqForm[0]; 
			if (!form.password.value) {
				error_message("请输入密码", 1);
				form.password.focus();
				return false; 
			}
			if (!form.passwordConfirm.value) {
				error_message("请输入重复密码", 1);
				form.passwordConfirm.focus();
				return false; 
			}
		},
		success: function(data) {
			if (data.code == "1") {
				error_message(data.message, 0, 0, 1000, "/login");
			} else {
				error_message(data.message, 1);
			}
		}
	});
});

