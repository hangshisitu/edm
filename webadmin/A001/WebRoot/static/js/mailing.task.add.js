$(document).ready(function() {
	refreshTag(initTag);
	
	$('#add').ajaxForm({
		url: 'add',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json', 
		beforeSubmit: function(formData, jqForm, options) {
			loading();	
		},
		success: function(data) {
			if (data.code == 1) {
				 error_message(data.message, 0, 0, 1000, "/mailing/task/page");
				 del_loading();
			 } else {
				 error_message(data.message, 1);
				 del_loading();
			 } 
		}
	});
	
	//提交前先取得收件人数
	$("#smt").bind('click',function(){
		getAjaxSum();
		return false;
	});
	
	//显示文件信息
	$("li a[data-type=file]").live('mouseover',function(){
		var id = $(this).data('id');
		var tips = selectionData[id];
		//没有数据的就跳出
		if(typeof tips == 'undefined'){return};
		var	txt = '<h4>包含标签：</h4>';
			txt += '<ul>';
			txt += '<li>' + tips.includes + '</li>';
			txt += '<li>选取标签下的收件人：' + tips.activeCnd + '</li>';
			txt += '<li>取标签之间的收件人：' + tips.inCnd + '</li>';
			txt += '</ul>';
			txt += '<h4>排除标签：</h4>'
			txt += '<ul><li>' + tips.excludes + '</li></ul>';
			txt += '<h4>过滤器：</h4>';
			txt += '<ul><li>' + tips.filter + '</li></ul>';
			txt += '<h4>当前收件人总数：<span class="red">' + tips.emailCount + '</span><span class="f666">（' + tips.repeat + '）</span></h4>';

		$.openTips(this,'#fileTips','left',0,txt);
	});
	
	$("li a[data-type=file]").live('mouseout',function(){
		$('#fileTips').hide();
	});
	
});

function drafted(){
	$('#add').ajaxSubmit({
		url: 'add?t=drafted',
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
			if (!form.templateId.value) { 
				error_message("请选择邮件模板", 1);
				del_loading();
				return false; 
			}
			if (!form.subject.value) { 
				error_message("请输入邮件主题", 1);
				form.subject.focus();
				del_loading();
				return false; 
			}
			if (!form.senderEmail.value) { 
				error_message("请输入发件人邮箱", 1);
				//form.senderEmail.focus();
				del_loading();
				return false; 
			}
			if (!form.includeIds.value) { 
				error_message("请选择所属标签", 1);
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
			 if (data.code == 1) {
				$("input[name=taskId]").val(data.taskId);
				$("input[name=planId]").val(data.planId);
				$("input[name=orgTaskName]").val(data.taskName);
				error_message(data.message);
			 } else {
				error_message(data.message,1);
			 }
			 del_loading();
		}   
	}); 
}

function getAjaxSum(){
	loading();
	var form = $("#add")[0]; 
	if (!form.taskName.value) { 
		error_message("请输入任务名称", 1);
		form.taskName.focus();
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
		form.subject.focus();
		del_loading();
		return false; 
	}
	if ($("#bindCorpId").size() && !form.bindCorpId.value) { 
		error_message("请选择所属企业", 1);
		del_loading();
		return false; 
	}
	if (!form.senderEmail.value) { 
		error_message("请选择发件人邮箱", 1);
		del_loading();
		return false; 
	}
	if (!form.includeIds.value) { 
		error_message("请选择所属标签", 1);
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
	popTips("#remindCustom","<p><img src='/static/img/loading2.gif' style='margin-bottom:-5px;' />&nbsp;&nbsp;正在筛选收件人，请稍候...</p>");
	$('#remindCustom input').prop("disabled",true);
	var includeIds = $('#includeId').val() || '';
	var excludeIds = $('#excludeId').val() || '';
	$.ajax({
		url: "/mailing/task/selection?token=" + headers['token'],
		type: "post",
		headers: headers,
		dataType: "json",
		data: {"includeIds": includeIds, "excludeIds": excludeIds},
		success: function(data) {
			if (data.code == 1) {
				$("input[name=fileId]").val(data.fileId);
				popTips("#remindCustom","<p>筛选成功：当前共筛选<span class='red'>" + data.count + "</span>个收件人<span class=\"f666\">（已去重）</span></p>");	
			} else {
				popTips("#remindCustom","<p>筛选失败：" + data.message + "</p>");					
			}
			$('#remindCustom input').prop("disabled",false);
			del_loading();
		},
		error:function(){
			popTips('#remindCustom',"筛选收件人失败！");
			$('#remindCustom input').prop("disabled",false);
			del_loading();
		}					
	});
}

function refreshTag(callFn) {
	$.ajax({
		url: "/mailing/task/refresh?token=" + headers['token'],
		type: "post",
		headers: headers,
		dataType: "json",
		success: function(data){
			
			catedata = data.categoryMap;
			file_list = data.fileMap;
			selectionData = data.selectionMap;
			include_list = $.extend({},catedata,file_list);
			
			if(callFn){
				callFn();
			}else{
				//重新传入数据
				$("#include").data('json',include_list);
				$("#exclude").data('json',catedata);
				$("#unsubscribe").data('json',catedata['退订']);
				reloadSearch('#include');
			}
		},
		error: function(){
			error_message('数据加载失败！',1);
		}
	});
} 

function initTag(){
	$("#include").search({
		valueId:"#includeId",
		boxId:"#slideSearch3",
		json:include_list,
		itemFn:"pickIncludes(this,'#include',includeLimit)",
		deleteFn:function(e){
			reduceArr.call(this,e);
			//选项为空的话恢复后面禁用项
			if($("#includeId").val() == ''){
				//处理排除标签
				$('#exclude').prop('disabled',false);
				testSendCode();
			}
		},
		sortFn:function(){
			this.find('.search_result').addClass('Jsort');
			this.find('.search_result ul').each(function(i){
				if( $(this).attr('data-cate') == '筛选文件' || $(this).attr('data-cate') == '临时文件' ){
					var $h4 = $(this).prev('h4');
					$(this).prependTo('.Jsort');
					$h4.prependTo('.Jsort');
				};
			});
			this.find('.search_result').removeClass('Jsort');
			//隐藏全部退订
			this.find('a[data-id=-1]').parents('li').hide();
		}
	});
	
	$("#exclude").search({
		valueId:"#excludeId",
		boxId:"#slideSearch",
		json:catedata,
		deleteFn:reduceArr,
		itemFn:"pickExcludes(this,'#exclude',excludeLimit)",
		sortFn:function(){
			//隐藏临时文件
			this.find('.search_result h4').each(function(i){
				if($(this).attr('data-cate') == '临时文件'){
					$(this).hide();
					$(this).next('ul').hide();
				};
			});
		}
	});	
	
	$("#unsubscribe").search({
		valueId:"#unsubscribeId",
		boxId:"#slideSearch2",
		json:catedata['退订'],
		itemFn:"pickData(this,'#unsubscribe')",
		sortFn:function(){
			//隐藏全部退订
			this.find('a[data-id=-1]').parents('li').hide();
		},
		cookieArr:cookieArr
	});
	
	//加载包含标签是文件，排除标签不可用
	if(findArrItem($('#include').data('subArr'),2,'file')>-1){
		$('#exclude').prop('disabled',true);
	}
	
}


//包含标签多选
function pickIncludes(obj,inputId,limit){
	var $obj = $(obj);
	var parentArr = $(inputId).data('subArr');
	if(typeof limit != 'undefined'){
		if(parentArr.length >= limit){
			error_message("不能超过"+ limit +"个",1);
			return;
		}
	}
	var type = $obj.data("type");
	var val = $obj.data("id");
	var txt = $obj.text();
	//指定为文件时
	if(typeof type != 'undefined' && type == 'file'){
		//如果是定时和周期发送
		if($("input[value=schedule]")[0].checked == true || $("input[value=plan]")[0].checked == true){
			error_message("如果选择了【定时发送】和【周期发送】，包含标签禁用临时文件",1);
			return false;	
		}
			
		parentArr.length = 0;
		//如果是文件放多一个字段区别
		var new_arr = Array(val, txt, type);
		
		//处理排除标签
		$('#exclude').prop('disabled',true);
		$('#exclude,#excludeId').val('');
	}else{
		for(var i=0;i<parentArr.length;i++){
			if(parentArr[i][2] == 'file'){
				parentArr.splice(i,1);
			}
		}
		var new_arr = Array(val, txt);
		
		//处理排除标签
		$('#exclude').prop('disabled',false);
	}
	
	filterArr(parentArr,new_arr);
	addItemInput(parentArr,$(inputId));
	//处理定时和周期发送
	testSendCode();
}

//排除标签多选
function pickExcludes(obj,inputId,limit){
	var parentArr = $(inputId).data('subArr');
	if(typeof limit != 'undefined'){
		if(parentArr.length >= limit){
			error_message("不能超过"+ limit +"个",1);
			return;
		}
	}
	pickArr(obj,inputId);
}

