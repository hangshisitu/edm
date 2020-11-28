var modelList = {
	"M_1":["M_2"],
	"M_4":["M_3","M_1","M_2"],
	"M_5":["M_1","M_2"],
	"M_8":["M_7"],
	"M_6":["M_7"],
	"M_9":["M_7"],
	"M_10":["M_7","M_12"],
	"M_11":["M_7","M_12"],
	"M_12":["M_7"],
	"M_13":["M_7","M_12"]
};

$(document).ready(function(){
	$('#addRole input[type=checkbox]').change(function(){
		checkRole(this.id);
	});
	
	$('#addRole').ajaxForm({
		url : '/role/saveRole',
		type : 'post',
		resetForm : false,
		dataType : 'json',
		beforeSubmit : function(formData, jqForm, options) {
			var form = jqForm[0];
			if($.trim(form.roleName.value) == '') {
				$(".err").html("提示：角色名称不能为空！");
				return false;
			}
			
			if(!$("#addRole input[type=checkbox]:checked").length) {
				$(".err").html("提示：请选择功能权限！");
				return false;
			}
			
			return testRelation();
		},
		success : function(data) {
			if(data) {
				if("success" == data.result) {
					error_message(data.message, 0);
					setTimeout("success()", 2000);
				} else {
					if( typeof (data.result) == "undefined") {
						$(".err").html("提示：提交失败！");
					} else {
						$(".err").html(data.message);
					}
				}
			}
		}
	});
	
	//默认勾选已选择功能
	if($('#funcList').val() != ''){
		var func = ($('#funcList').val()).split(",");
		for(var i=0;i<func.length;i++){
			if(func[i] != ''){
				$("#" + func[i])[0].checked = true;
			}	
		}
	}
	
});

function checkRole(id){
	var isChecked = $('#'+id)[0].checked;
	var arr = modelList[id] || [];
	
	if(isChecked == true){
		for(var i=0; i<arr.length; i++){
			$("#" + arr[i])[0].checked = true;
		}
	}else if(isChecked == false){
		/*
		var checkedStr = '';
		$("#addRole input[type=checkbox]:checked").each(function(){
			for(var key in modelList){
				if(this.id == key && this.id != id){
					alert(key);
					checkedStr += (modelList[key].join(',')+',');
				}
			}
		});
		
		for(var i=0; i<arr.length; i++){
			//排除其他主项要用
			if(checkedStr.indexOf(arr[i]) == -1){	
				$("#" + arr[i])[0].checked = false;
			}	
		}
		*/
		
		for(var i=0; i<arr.length; i++){
			$("#" + arr[i])[0].checked = false;
		}
	}
}

function testRelation(){
	for(var key in modelList){
		var isChecked = $("#" + key)[0].checked;
		var arr = modelList[key];
		if(isChecked == true){
			for(var i=0; i<arr.length; i++){
				var flag = $("#" + arr[i])[0].checked;
				if(!flag){
					switch(key){
						case "M_1":
							$(".err").html('提示：选择【收件人管理】，请关联选择【属性管理】');
							return false;
							break;
						case "M_4":
							$(".err").html('提示：选择【收件人筛选与导出】，请关联选择【过滤器管理、收件人管理、属性管理】');
							return false;
							break;
						case "M_5":
							$(".err").html('提示：选择【表单管理】，请关联选择【收件人管理、属性管理】');
							return false;
							break;
						case "M_8":
							$(".err").html('提示：选择【任务创建】，请关联选择【模板预览】');
							return false;
							break;
						case "M_6":
							$(".err").html('提示：选择【模板设计与管理】，请关联选择【模板预览】');
							return false;
							break;
						case "M_9":
							$(".err").html('提示：选择【任务审核】，请关联选择【模板预览】');
							return false;
							break;
						case "M_10":
							$(".err").html('提示：选择【任务投递监控管理】，请关联选择【模板预览、报告查阅】');
							return false;
							break;
						case "M_11":
							$(".err").html('提示：选择【活动创建与管理】，请关联选择【模板预览、报告查阅】');
							return false;
							break;
						case "M_12":
							$(".err").html('提示：选择【报告查阅】，请关联选择【模板预览】');
							return false;
							break;
						case "M_13":
							$(".err").html('提示：选择【报告导出】，请关联选择【报告查阅】');
							return false;
							break;
						default:;
					}
				}
			}
		}
	}
}

//返回
function success() {
	var url = $("#successUrl").val();
	window.location.href = url;
}

function back(){
	var url = $("#backUrl").val();
	window.location.href = url;
}



