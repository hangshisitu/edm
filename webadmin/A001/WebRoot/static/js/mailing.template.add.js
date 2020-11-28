$(document).ready(function(){
	
	var myUrl = window.location.href;
	if(myUrl.indexOf("add?templateId=")>-1){
		
		if($("#joinPhone").attr("value")==1){
			$("#phone").addClass("show");
		}
		if($("#joinSms").attr("value")==1){
			$("#dx").addClass("show");
		}
		$("#pc").addClass("show");
		$("#tabMain li:first").show();
		$("#tab-main-wrap").show();
		$("#tb-wrap").hide();
	}
	
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
	template($("#ph_history"));
	
	//保存提交
	$('#add').ajaxForm({    
		url: 'add',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json', 
		beforeSerialize: function($form, options) {
			if(eModel=="WYSIWYG"){
				K.sync();
			}	
			if(eModelPhone=="WYSIWYG"){
				K1.sync();
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
		},
		success: function(data) {
			del_loading();
			if (data.code == 1) {
				error_message(data.message, 0, 0, 1000, "/mailing/template/page");
			}else if(data.code == 2){
				//弹窗提示错误信息
				popTips('#remindFilter', data.message);
			}else {
				error_message(data.message, 1);
			}	 
		}   
	}); 

	$('#tempSubmit').click(function(){
		//pc判断是否个性化
		var cusType = $('input[name=personal]:checked').val();
		var flag = hasPrama();
		if( (flag == true && cusType == 'false') || (flag == true && cusType != 'true')){
			popTips('#remindParam','PC模板的链接含有变量，可能导致链接失常，设置链接个性化？');
			return false;
		}else if(flag == false && cusType == 'true'){
			popTips('#remindParam','PC模板的链接没有可替换的变量，链接个性化会失效，取消链接个性化？');
			return false;
		}
		
		//phone判断是否个性化
		var joinPhone = $("#joinPhone").val();
		if(joinPhone == 1){		//代表有phone选项
			var phoneCusType = $('input[name=phonePersonal]:checked').val();	
			var phoneFlag = hasPramaPhone();
			if( (phoneFlag == true && phoneCusType == 'false') || (phoneFlag == true && phoneCusType != 'true')){
				popTips('#remindParam','PHONE模板的链接含有变量，可能导致链接失常，设置链接个性化？');
				return false;
			}else if(phoneFlag == false && phoneCusType == 'true'){
				popTips('#remindParam','PHONE模板的链接没有可替换的变量，链接个性化会失效，取消链接个性化？');
				return false;
			}
		}	
					
		//重新设值触发选区
		getHrefList();
		//比较触点链接
		return compareHref();
		return true;
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
	
	$("#phone_form_list").UIselect({
		width: 170,
		html: function(){
			var form_text = "";
			form_text += "<ul>";
			for(var i = 0; i < form_list.length; i++) {
				form_text += "<li><a hidefocus='true' onclick='insertPhoneFormUrl(\"" + form_list[i][0] + "\",this)' href='javascript:void(0)' title='" + form_list[i][1] + "'>" + form_list[i][1] + "</a></li>";
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



if($("input[name=templateId]").val()!=""){

		if($("#joinPhone").attr("value")==1){
			$("#phone").addClass("show");
		}
		if($("#joinSms").attr("value")==1){
			$("#dx").addClass("show");
		}
		$("#pc").addClass("show");
		$("#tabMain li:first").show();
		$("#tab-main-wrap").show();
		$("#tb-wrap").hide();

}	



$("#jbj").click(function(){
	$("#ph_cb").attr("disabled","true");
	$("#dx_cb").attr("disabled","true");
	if($("#ph_cb").is(':checked')){
		$("#phone").addClass("show");
	}
	else{
		$("#phone").removeClass("show");
	}
	if($("#dx_cb").is(':checked')){
		$("#dx").addClass("show");
	}
	else{
		$("#dx").removeClass("show");
	}
	$("#pc").addClass("show");
	$("#tabMain .tabLi:first").show();
	$("#tab-main-wrap").show();
	$("#tb-wrap").hide();
  });
var cur = 0;
$("#cq_tab li").click(function(){
	$("#tabMain .tabLi").eq(cur).hide();
	$("#cq_tab li").removeClass("now");
	$(this).addClass("now");
	cur = $("#cq_tab li").index(this);
	$("#tabMain .tabLi").eq(cur).show();
	if($('#codeTips')) $('#codeTips').hide();
	if($('#codeTipsPhone')) $('#codeTipsPhone').hide();

});
/**
$("#ph_checkbox").click(function(){
   if($("#ph_checkbox").is(":checked")){
	   $("#joinPhone").attr("value","1");
   }
   else{

	   $("#joinPhone").attr("value","0");
   }
});
*/
$("#ph_cb").click(function(){
	   if($("#ph_cb").is(":checked")){
		   $("#joinPhone").attr("value","1");	  
	   }
	   else{

		   $("#joinPhone").attr("value","0");
	   }

	});
$("#dx_cb").click(function(){
	   if($("#dx_cb").is(":checked")){
		   $("#joinSms").attr("value","1");
	   }
	   else{

		   $("#joinSms").attr("value","0");
	   }
	});
});

function preview(){
	if($("#pc").hasClass("now")){
		if(eModel=="WYSIWYG"){
			post('/mailing/template/preview?token=' + headers['token'], { content: K.html() });
		}else{
			post('/mailing/template/preview?token=' + headers['token'], { content: $("#templateContent").val() });
		}
	}
	if($("#phone").hasClass("now")){
		if(eModel=="WYSIWYG"){
			post('/mailing/template/preview?token=' + headers['token'], { content: K1.html() });
		}else{
			post('/mailing/template/preview?token=' + headers['token'], { content: $("#phoneContent").val() });
		}	
	}
	if($("#dx").hasClass("now")){
		post('/mailing/template/preview?token=' + headers['token'], { content: $("#smsContent").val() });
	}
};


/**
 *   以下由zc 新增方法 1107
 * ***/
var zcUtil={
    personSwitch:function(obj){   //点击个性化进行切换
    	var that=$(obj);
	    if(that.val()=='false'){
	    	that.val('true');
	    }else if(that.val()=='true'){
	    	that.val('false');
	    }
    }		
};


