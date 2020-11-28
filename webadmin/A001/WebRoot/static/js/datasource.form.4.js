$(document).ready(function(){
	
	$("#open_paramBox").UIselect({
		width: 170,
		height: 120,
		html: function(){
			var prop_text = "";
			prop_text += "<ul>";
			for(var i = 0; i < prop_list.length; i++) {
				prop_text += "<li><a hidefocus=\"true\" onclick=\"insert(\'" + prop_list[i][0] + "\')\" href=\"javascript:void(0)\" title=\"" + prop_list[i][1] + "\">" + prop_list[i][1] + "</a></li>";
			}
			prop_text += "</ul>";
			return prop_text;
		}
	}); 
	
	template($("#recommend"));
	
	//保存提交
	$('#add').ajaxForm({    
		url: '4',
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
			if (!$.trim(form.promptSubject.value)) {
				error_message("请输入邮件主题", 1);
				form.promptSubject.focus();
				del_loading();
				return false; 
			}
		},
		success: function(data) {
			del_loading();
			if (data.code == 1) {
				location.assign("/datasource/form/5?formId=" + data.formId);
			}else if(data.code == 2){
				//弹窗提示错误信息
				popTips('#remindFilter', data.message);
			}else {
				error_message(data.message, 1);
			}	 
		},
		error:function(){
			del_loading();
		}  
	}); 
	
	
	//判断提交
	$("#tempSubmit").click(function(){
		var cusType = $('input[name=personal]:checked').val();
		if(hasPrama() == true && cusType == 'false'){
			popTips('#remindParam','链接含有变量，可能导致链接失常，设置链接个性化？');
			return false;
		}else if(hasPrama() == false && cusType == 'true'){
			popTips('#remindParam','链接没有可替换的变量，链接个性化会失效，取消链接个性化？');
			return false;
		}
		$('#add').submit();
	});
	
	//生成表单链接列表
	$("#form_list").UIselect({
		width: 170,
		html: function(){
			var form_text = "";
			form_text += "<ul>";
			for(var i = 0; i < form_list.length; i++) {
				form_text += "<li><a hidefocus='true' onclick='insertFormUrl(\"" + form_list[i][0] + "\",this)' href='javascript:void(0)' title='" + form_list[i][1] + "'>" + form_list[i][1] + "</a></li>";
			}
			form_text += "</ul>";
			return form_text;
		}
	});
});

function preview(){
	if(eModel=="WYSIWYG"){
		post('/datasource/form/preview?token=' + headers['token'], { content: K.html() });
	}else{
		post('/datasource/form/preview?token=' + headers['token'], { content: $("#templateContent").val() });
	}
}