$(document).ready(function(){
	template($("#recommend"));
	//保存提交
	$('#add').ajaxForm({    
		url: '2',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json', 
		beforeSerialize: function($form, options) { 
			if(eModel=="WYSIWYG"){
				K.sync();
			}	
			
		},
		beforeSubmit: function(formData, jqForm, options) {   //验证
			loading();
			var form = jqForm[0]; 
		},
		success: function(data) { 
			if (data.code == 1) {
				del_loading();
				var step = "5";
				if (data.emailConfirm == 1) {
					step = "3";
				} else if (data.emailPrompt == 1) {
					step = "4";
				}
				error_message(data.message, 0, 0, 1000, "/datasource/form/" + step + "?formId=" + data.formId);
			} else {
				 error_message(data.message, 1);
				 del_loading();
			}	 
		},
		error:function(){
			del_loading();
		} 
	});
	
	//点击下一步
	$('#tempSubmit').click(function(){
		 $('#add').submit();
	})
});

function preview(){
	if(eModel=="WYSIWYG"){
		post('/datasource/form/preview?token=' + headers['token'], { content: K.html() });
	}else{
		post('/datasource/form/preview?token=' + headers['token'], { content: $("#templateContent").val() });
	}
}