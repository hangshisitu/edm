$(document).ready(function() {
	$("#unsubscribe").search({
		valueId:"#unsubscribeId",
		boxId:"#slideSearch2",
		json:catedata['退订'],
		itemFn:"pickData(this,'#unsubscribe')",
		cookieArr:cookieArr
	});
	
	$('#add').ajaxForm({
		url: '3',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json', 
		beforeSubmit: function(formData, jqForm, options) {
			loading();
			var form = jqForm[0]; 
			if (!form.taskName.value) { 
				error_message("请输入任务名称", 1);
				form.taskName.focus();
				del_loading();
				return false; 
			}
			if ($("#bindCorpId").size() && !form.bindCorpId.value) { 
				error_message("请选择所属企业", 1);
				del_loading();
				return false; 
			}
			if (!form.senderEmail.value) { 
				error_message("请输入发件人邮箱", 1);
				//form.senderEmail.focus();
				del_loading();
				return false; 
			}
			if ($("#sms_list").size() && !form.sms.value) { 
				error_message("请选择短信通知", 1);
				del_loading();
				return false; 
			}
			if ($("#assProp").size() && $("#assProp").val()!='' && findArrItem($("#assProp").data('subArr'),2,'行业兴趣') == -1) { 
				error_message("请选择关联属性的行业兴趣", 1);
				del_loading();
				return false; 
			}
		},
		success: function(data) {
			del_loading();
			if (data.code == 1) {
				 error_message(data.message, 0, 0, 1000, "/mailing/task/page");
			} else {
				error_message(data.message, 1);
			} 
		}
	});
});

function gotoDrafted(){
	var form = $("#add")[0]; 
		if (!form.taskName.value) { 
			error_message("请输入任务名称", 1);
			form.taskName.focus();
			del_loading();
			return false; 
		}
		if ($("#bindCorpId").size() && !form.bindCorpId.value) { 
			error_message("请选择所属企业", 1);
			del_loading();
			return false; 
		}
		if (!form.senderEmail.value) { 
			error_message("请输入发件人邮箱", 1);
			//form.senderEmail.focus();
			del_loading();
			return false; 
		}
		if ($("#sms_list").size() && !form.sms.value) { 
			error_message("请选择短信通知", 1);
			del_loading();
			return false; 
		}
		if ($("#assProp").size() && $("#assProp").val()!='' && findArrItem($("#assProp").data('subArr'),2,'行业兴趣') == -1) { 
			error_message("请选择关联属性的行业兴趣", 1);
			del_loading();
			return false; 
		}
	popTips('#draftedRemind');
}
function drafted(){
	$('#add').ajaxSubmit({
		url: '3?t=drafted',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json',
		beforeSubmit: function(formData, jqForm, options) {
			loading();
			var form = jqForm[0];
		},
		success: function(data) { 
			 if (data.code == 1) {
				$("input[name=taskId]").val(data.taskId);
				$("input[name=planId]").val(data.planId);
				$("input[name=orgTaskName]").val(data.taskName);
				error_message(data.message);
			 } else {
				error_message(data.message,1);
			 }
			 del_loading();
			 closeDialog();
		},
		error:function(){
			del_loading();
			closeDialog();
		}  
	}); 
}

function _2(){
	$('#add').ajaxSubmit({
		url: '3?t=2',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json',
		beforeSubmit: function(formData, jqForm, options) {
			var form = jqForm[0]; 
		},
		success: function(data) { 
			 if (data.code == 1) {
				 location.assign("/step/2?id=" + $("input[name=id]").val());
			 } else {
				error_message(data.message, 1);
			 }
		}   
	}); 
}
