$(document).ready(function(){
	showType();
	$('#add').ajaxForm({    
		url: 'add',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json', 
		
		beforeSubmit: function(formData, jqForm, options) {   //验证
			loading();
			var form = jqForm[0]; 
		},
		success: function(data) {
			del_loading();
			if (data.code == 1) {
				location.assign("/datasource/form/1?formId=" + data.formId);
			} else {
				error_message(data.message, 1);
			}	 
		},
		error:function(){
			del_loading();
		}  
	}); 

	//表单类型
	$('#form_list').UIselect({
		width:390,
		id_id:"#formType",
		ho_fun:'showType()',
		arrdata:new Array(["0", "注册"], ["1", "订阅"])
	})
	 
	//数据储存
	$("#dataStore").search({
		valueId:"#dataStoreId",
		boxId:"#slideSearch",
		json:catedata,
		itemFn:"pickData(this,'#dataStore')"
	}); 
	
	//设置高级选项
	$('#formMore').click(function() {
		$(this).toggleClass("swtpand");
		$(this).toggleClass("swtclose");
		$('#moreTank').toggle();
		showType();
	}); 
	
	$('#smt').click(function(){
		if($.trim($('#formName').val()) == ''){
			error_message("请输入表单名称", 1);
			$('#formName').focus();
			return false;
		}
		
		if($.trim($('#formDesc').val()) == ''){
			error_message("请输入表单用途", 1);
			$('#formDesc').focus();
			return false;
		}
		
		if($.trim($('#formType').val()) == ''){
			error_message("请选择表单类型", 1);
			$('#formType').focus();
			return false;
		}
		
		if($.trim($('#dataStoreId').val()) == ''){
			error_message("请选择数据储存", 1);
			$('#dataStoreId').focus();
			return false;
		}
		
		openPop(0,0,'#confirmCancle');
	})
})

function showType() {
	var type = $('#formType').val();
	if (type == '0') {
		$("#confirm").show();
		$("#confirm_txt").text("邮件二次确认成功后再收集到标签");
		$("#prompt_txt").text("成功注册后发出成功提示邮件");
	} else if (type == '1') {
		$("#confirm").show();
		$("#confirm_txt").text("对填写的邮箱发出验证邮件，验证成功再收集到标签");
		$("#prompt_txt").text("订阅完成后发出成功订阅的提示或感谢邮件");
	} else if (type == "2") {
		$("#confirm").hide();
		$("#prompt_txt").text("退订提交后发出成功退订的提示邮件");
	} else {
		$("#confirm").hide();
		$("#confirm_txt").text("");
		$("#prompt_txt").text("");
	}
}
