$(document).ready(function(){
	$("#category_list").UIselect({
		height: 150,
		width: 490,
		id_id: "#categoryId",
		arrdata: category_list,
		edit_top: 0,
		ho_fun:"switchTrig()",
		foot_html:"<p class=\"select_foot\"><a class=\"highlight f_l add_reload\" href=\"javascript:open_msg('/mailing/category/add?action=template',0);\">新建模板类别</a><a class=\"highlight add_reload f_r\" href=\"javascript:reloadSelect('#category_list');\">刷新</a></p>"
	});
	//控制默认触发计划
	switchTrig();
	
	template($("#recommend"));
	template($("#history"));
	
	//保存提交
	$('#add').ajaxForm({    
		url: '1?action=create',
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
			if (!form.templateName.value) {
				error_message("请输入模板名称", 1);
				form.templateName.focus();
				del_loading();
				return false; 
			}
			if (!form.categoryId.value) {
				error_message("请选择模板类别", 1);
				del_loading();
				return false; 
			}
			if (!form.subject.value) {
				error_message("请输入邮件主题", 1);
				form.subject.focus();
				del_loading();
				return false; 
			}
		},
		success: function(data) {
			del_loading();
			if (data.code == 1) {
				location.assign("/step/2?id=" + $("input[name=id]").val());
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
	
	$('#user').ajaxForm({    
		url: '1?action=select',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json', 
		beforeSerialize: function($form, options) { 
			
		},
		beforeSubmit: function(formData, jqForm, options) { 	
			loading();
			var form = jqForm[0]; 
			//验证没有模板的情况下
			if(!templateData.length){
				popTips('#remindCustom');
				del_loading();
				return false; 
			}
			
			if (!form.templateId.value) {
				error_message("请选择邮件模板", 1);
				del_loading();
				return false; 
			}
			
			if (!form.subject.value) {
				error_message("请输入邮件主题", 1);
				del_loading();
				return false; 
			}
		},
		success: function(data) {
			del_loading();
			if (data.code == 1) {
				location.assign("/step/2?id=" + $("input[name=id]").val());
			} else {
				error_message(data.message, 1);
			}	 
		},
		error:function(){
			del_loading();
		}  
	}); 
	
	//选择设计模板方式
	$("input[name=action]").change(function(){
		if($("input[name=action]:checked").val() == "create"){
			$(".old_temp").hide();
			$(".new_temp").show();
		}else{
			$(".new_temp").hide();
			$(".old_temp").show();
		}
	}); 
	
	$("#template").search({
		valueId:"#userTempId",
		boxId:"#slideSearch",
		json:templateData,
		itemFn:"postSubject(this,'#template')"
	});
	
	
	//判断提交
	$("#tempSubmit").click(function(){
		if($("input[name=action]:checked").val() == "create"){			
			var cusType = $('input[name=personal]:checked').val();
			var flag = hasPrama();
			if( flag == true && cusType == 'false'){
				popTips('#remindParam','链接含有变量，可能导致链接失常，设置链接个性化？');
				return false;
			}else if(flag == false && cusType == 'true'){
				popTips('#remindParam','链接没有可替换的变量，链接个性化会失效，取消链接个性化？');
				return false;
			}

			//比较触点链接
			if(compareHref()){
				$('#add').submit();
			}else{
				return false;
			}
	
		}else{
			$('#user').submit();
		}
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
	
	//初始化触发选项
	getHrefList();
	trig_num = $(".J_trig_list").length || 0;
	
	$(".J_trig_list").each(function(){
		var $this = $(this);
		initMselect($this);
	});	
	
	$(".J_subtemp_list").each(function(){
		var $this = $(this);
		initSubTemp($this);
	});
	
	$('.add_set').on('click',function(){
		add_trig(this);
	});	
	
	$('.J_trig_time').gtips({
		eType: "focus",
		content: function(){
			return '请输入0-120的数字，单位是分钟。';
		}
	});
});

function preview(){
	//新建模板预览
	if($("input[name=action]:checked").val() == "create"){
		if(eModel=="WYSIWYG"){
			post('/mailing/template/preview?token=' + headers['token'], { content: K.html() });
		}else{
			post('/mailing/template/preview?token=' + headers['token'], { content: $("#templateContent").val() });
		}
	}else{ //已有模板预览
		var id = $("#userTempId").val();
		if(id){
			window.open('/mailing/template/view?templateId=' + id, "_blank"); 
		}else{
			if(!templateData.length){
				popTips('#remindCustom');
				return false; 
			}
			error_message("请选择邮件模板", 1);	
		}
	}
}

function gotoNewTemp(){
	$('input[name=action][value=create]').prop('checked',true);
	$(".old_temp").hide();
	$(".new_temp").show();
	closeDialog();
	$('#main').scrollTop(0);
}

//赋值主题
function postSubject(obj,inputId){
	pickData(obj,inputId);
	var id=$("#userTempId").val();
	for(var i in templateData){
		if(templateData[i].id==id){
			$("#subject2").val(templateData[i].subject);
		}
	}	
}