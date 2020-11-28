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
			if (!form.userId.value) { 
				error_message("请输入用户名", 1);
				form.userId.focus();
				del_loading();
				return false; 
			}
			if (!form.orgUserId.value && !form.password.value) { 
				error_message("请输入密码", 1);
				form.password.focus();
				del_loading();
				return false; 
			}
			if (!form.email.value) { 
				error_message("请输入邮箱地址", 1);
				form.email.focus();
				del_loading();
				return false; 
			}
		},
		success: function(data) {
			if (data.code == 1) {
				error_message(data.message, 0, 0, 1000, "/account/user/page");
			} else {
				error_message(data.message,1);
			}
			del_loading();
		}   
	}); 
	
	$("#corp_list").search({
	 	valueId:"#corpId",
		boxId:"#slideSearch5",
		json:corp_list,
		itemFn:"postHook(this,'#corp_list')"
	});	
	
	$("#role").search({
	 	valueId:"#roleId",
		boxId:"#slideSearch5",
		json:modelList,
		itemFn:"pickData(this,'#role')",
		sortFn:function(){
			var isChild = $('#corpId').attr('data-level');
			this.find('a').each(function(i){
				if(isChild != $(this).attr('data-level')){
					$(this).parents('li').hide();
				}
			});
		}
	});	
	
	$("#role2").search({
	 	valueId:"#roleId2",
		boxId:"#slideSearch5",
		json:modelList,
		itemFn:"sendFunc(this)",
		deleteFn:function(){
			return false;
		}
	});	
	
	$("#permission_list").search({
	 	valueId:"#permissionId",
		boxId:"#slideSearch5",
		json:permission_list,
		itemFn:"pickData(this,'#permission_list')"
	});	
	
});

function postHook(obj,id){
	pickData(obj,id);
	var level = $(obj).attr('data-level');
	$('#corpId').attr('data-level',level);
	if($(obj).attr('data-level') == '1'){
		$('.Jhook').hide();
	}else if($(obj).attr('data-level') == '0'){
		$('.Jhook').show();
	}
}

//增加发送量
function addSum(){
	var raiseSum=$("input[name=raiseSum]").val();
	var totalSend=$("input[name=totalSend]").val();
	var surplus=$("#surplus").text();
	var totalSum=$("#totalSum").text();
	
	if(totalSend == ''){
		error_message("最大发送总量不能为空",1);
		$("input[name=totalSend]").focus();
		return;
	}
	if(isNaN(totalSend)){
		error_message("最大发送总量只能为数字",1);
		$("input[name=totalSend]").focus();
		return;
	}
	if(raiseSum == ''){
		error_message("增加的投递总量不能为空",1);
		$("input[name=raiseSum]").focus();
		return;
	}
	if(isNaN(raiseSum)){
		error_message("增加的投递总量只能为数字",1);
		$("input[name=raiseSum]").focus();
		return;
	}
	
	var raise = parseInt(raiseSum), 
		assign =parseInt(totalSend),
		sur = parseInt(surplus), 
		total = parseInt(totalSum);
		
	if((total-raise) <= 0){
		error_message("不足以分配",1);
		return;
	}
	
	$("input[name=totalSend]").val('' + (assign+raise));
	$("#surplus").text('' + (sur+raise));
	$("#totalSum").text('' + (total-raise));
}


function sendFunc(obj){
	pickData(obj,'#role2');
	var id = $("#roleId2").val();
	var html = '';
	for(var key in modelList){
		if(modelList[key]["id"] == id){
			for(i in modelList[key]['info']){
				html += (modelList[key]['info'][i]+'<br>');
			}
		}
	}
	$("#funcDesc").html(html);
	$("#slideSearch5").hide();
}

function triggerHelp(){
	openPop(0,0,'#remindRole');
	$("#role2").trigger('click');
	$('#slideSearch5').find("a:first").trigger('click');
}
	