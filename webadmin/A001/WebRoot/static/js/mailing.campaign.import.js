$(document).ready(function(){
	$('#import').ajaxForm({
		url: '/mailing/campaign/import',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json',
		beforeSubmit: function(formData, jqForm, options) {
			var form = jqForm[0]; 
			if (!form.campaignId.value) {
				error_message("请选择活动", 1);
				form.campaignId.focus();
				del_loading();
				return false; 
			}
			if (!form.checkedIds.value) {
				error_message("请选择任务", 1);
				form.checkedIds.focus();
				del_loading();
				return false; 
			}
		},
		success: function(data) {
			del_loading();
			if (data.code == 1) {
				 error_message(data.message, 0, 0, 1000, "/mailing/campaign/page");
			} else {
				error_message(data.message, 1);
			} 
		}   
	});
	
	//选择活动
	 $("#camp").search({
	 	valueId:"#campId",
		boxId:"#slideSearch5",
		json:campaign_list,
		itemFn:"pickData(this,'#camp')"
	}); 
	
	$("#trgTask").search({
		valueId:"#taskId",
		textId:"#taskList",
		boxId:"#slideSearch",
		json:taskData,
		createFn:taskSearch,
		keyFn:taskKeySearch,
		itemFn:"taskHandle(this)"
	});
	
	//删除任务
	$('#taskList em.close').live('click',function(){
		var id=$(this).parents('.item').attr('data-id');
		ctrlTaskItem(id,'del');
	});
});

//增加任务
function taskHandle(obj){
	var $this = $(obj);
	var id = $this.attr('data-id');
	if($('#taskId').val().split(',').length>=10){
		error_message('任务数不能超过10个',1);
		return;
	}
	ctrlTaskItem(id,'add');
	var text1 = $this.find('span').text();
	var text2 = $this.find('em').text();
	var html = "<a class='item' data-id='" +id+ "'> ";
		html += "<span class='m_r5'>" +text1+ " (" +text2+ ")</span>";
		html += "<em class='close'></em>";				
        html += "</a>"; 
        
    var $input = $('#taskList');
    $input.append(html);
    $.fn.search.posfix($input,$input.data('boxId'));
}


//控制加减
function ctrlTaskItem(id,type){
	$('#taskList .item[data-id='+id+']').remove();
	var values = $('#taskId').val() || '';
	var arr = values?($('#taskId').val().split(',')):[];
	//去重
	for(i=0;i<arr.length;i++){
		if(arr[i] == id){
			arr.splice(i,1);
		}
	}
	//增加
	if(type=='add'){
		arr.push(id);
	}
	
	$('#taskId').val(arr.join(','));
}

//下拉搜索
function taskSearch(json,fn,boxId){
	var $boxId=$(boxId);
	var $resultId=$boxId.find(".search_result");
	var html="";
	var obj=json;	
	html+="<ul class='ul_result task_result'>";
	for(i=0;i<obj.length;i++){
		html+="<li>";
		html+="<a href=\"javascript:void(0);\" onclick=\""+ fn +";\" title=\""+ obj[i].title +"\"";
		for(var key in obj[i]){
			if(key!="title"){
				html+=" data-"+ key +"=\""+ obj[i][key] +"\"";
			}	
		}
		html+="><span class='f_l'>" + obj[i].title + "</span><em class='f_r'>" +obj[i].date+ "</em></a>";
		html+="</li>";
	}
	html+="</ul>";
	
	//先清空内容
	$resultId.html("").append(html).scrollTop(0);	
}

function taskKeySearch(json,fn,boxId,val_txt){
		var $boxId=$(boxId);
		var $resultId=$boxId.find(".search_result");
		var html="";
		var counter=0;
		html+="<ul class='ul_result task_result'>";
		var obj=json;	
		for(i=0;i<obj.length;i++){
			var title=obj[i].title;
			var index1 = title.indexOf(val_txt);
			//搜索
			if(index1!=-1){ 
				title = title.replace(val_txt,'<b class="red_cate">'+val_txt+'</b>');
				html+="<li>";
				html+="<a href=\"javascript:void(0);\" onclick=\""+ fn +";\" title=\""+ obj[i].title +"\"";
				for(var key in obj[i]){
					if(key!="title"){
						html+=" data-"+ key +"=\""+ obj[i][key] +"\"";
					}	
				}
				html+="><span class='f_l'>" + title + "</span><em class='f_r'>" +obj[i].date+ "</em></a>";
				html+="</li>";
				counter++;
			}
		}	
		html+="</ul>";
		//先清空内容
		$resultId.html("");
		if(counter){
			$resultId.append(html);
		}else{
			$resultId.append("<p class='flg' style='padding-left:10px'>没有匹配的结果！</p>");
		}
		$resultId.scrollTop(0);
	}
