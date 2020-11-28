$(document).ready(function(){
	$('#add').ajaxForm({    //表单名
		url: 'add',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json', 
		beforeSubmit: function(formData, jqForm, options) {   //验证
			loading();
			var form = jqForm[0]; 
			if (!form.email.value) { 
				error_message("请输入邮箱地址", 1);
				form.email.focus();
				del_loading();
				return false; 
			}
			if ($("input[name=company]").size() > 0 && !form.company.value) { 
				error_message("请输入公司地址", 1);
				form.company.focus();
				del_loading();
				return false; 
			}
		},
		success: function(data) { 
			if (data.code == 1) {
				error_message(data.message);
			} else {
				error_message(data.message, 1);
			}
			del_loading();
		}   
	}); 
});
