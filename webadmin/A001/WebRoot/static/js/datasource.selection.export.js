$(document).ready(function(){
	$("#msg_cfm").live('click',function(){
		closeDialog();
	})	
	
	//生成新标签
	$("#create_btn").click(function(){
		if (!$("input[name=includeTagIds]").val()) {
			error_message("请选择包含标签", 1);
			return false; 
		}
		$('input[name=type]').val(0);
		$("#tagId").val('');
		openPop('/datasource/tag/add?action=selection.export',0,'#newTag');	
	});
	
	//导入到标签
	$("#import_btn").click(function(){
		if (!$("input[name=includeTagIds]").val()) {
			error_message("请选择包含标签", 1);
			return false; 
		}	
		$('input[name=type]').val(1);
		$("#tagId").val('');	
		importTag(catedata,'appendTagId(this)','#popSearch');
		openPop(0,0,'#popSearch');	
	});
	
	$("#enSet").search({
		textId:"#en_cate",
		valueId:"#en_cate_val",
		boxId:"#slideSearch",
		json:catedata,
		itemFn:"pickArr(this,'#en_cate',callReloadSum)",
		deleteFn:deleteCate
	});
	
		
	$("#outSet").search({
		textId:"#out_cate",
		valueId:"#out_cate_val",
		boxId:"#slideSearch",
		json:catedata,
		itemFn:"pickArr(this,'#out_cate',callReloadSum)",
		deleteFn:deleteCate
	});
	
	
	$("#filterSet").search({
		textId:"#filter",
		valueId:"#filter_val",
		boxId:"#slideSearch",
		json:filterdata ,
		itemFn:"pickData(this,'#filter')"
	});
	
	$('#export').ajaxForm({
		url: 'export',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json',
		beforeSerialize: function($form, options) {
			loading();	
		},
		success: function(data) {
			del_loading();
			if (data.code == 1) {
				popTips("#remindCustom");
			} else {
				error_message(data.message, 1);
			} 
		}   
	});
	
	 $("input[name=activeCnd]").change(function() {
		if($("input[name=activeCnd][data-info=1]").prop("checked") == true) {
			$("#period_list").show();
		} else {
			$("#period_list").val("").hide();
		}
	}); 
	
	$("#period_list").UIselect({
		width: 128,
		id_id: "#activePeriod",
		ho_fun:"activeCnd()",
		arrdata: new Array(["1week", "一周以前活跃"], ["1month", "一个月以前活跃"], ["2month", "两个月以前活跃"], ["3month", "三个月以前活跃"])
	});
	
	$("#remindCustom .input_no").click(function(){
		window.location.reload();
	});
});

//删除项
function deleteCate(e){
	var subArr = $(this).data('subArr');
	if(e.keyCode==8){
		var item=getDeleteItem(this);
		removeArrItem(subArr,item);
		addItemInput(subArr,$(this));
		callReloadSum();
	}
	e.preventDefault();
}

function activeCnd() {
	$("#active").val($("#activePeriod").val());
}

function showNewCate(){
	closeSelectDiv();
	$("#category_list").text('');
	$("#categoryId").val('');
	$(".new_cate_box").show();
}

function hideNewCate(){
	$(".new_cate_box").hide();
	$("#newCate").val('');
}

function exportCounter() {
	$.ajax({
		url: "/datasource/selection/counter?token=" + headers['token'],
		type: "post",
		headers: headers,
		dataType: "json",
		success: function(data){
			if (data.code == 1) {
				popTips("#remindCustom");
			}
		}					
	});
}

function appendTagId(obj){
	var val=$(obj).attr("data-id");
	$("#tagId").val(val);
}

function exports(type) {
	if (!$("input[name=includeTagIds]").val()) {
		error_message("请选择包含标签", 1);
		return false; 
	}
		
	if(type == "1") {
		if (!$("input[name=tagId]").val()) {
			error_message("请选择标签", 1);
			return false; 
		}
		
		//防止搜索后没有选中
		if(!$("#popSearch").is(":hidden")){
			if($("#popSearch").find("li.selected").size() < 1){
				error_message("请选择标签", 1);
				return false; 
			}
		}
		
	}
	
	closeDialog();
	$("#type").val(type);
	$('#export').submit();
}

//刷新收件人总数
function callReloadSum(){	
	var en_cate_arr = $("#en_cate").data('subArr');
	var out_cate_arr = $("#out_cate").data('subArr');
	reloadPersonSum(catedata,en_cate_arr,out_cate_arr,"#personSum");
	$("#emailCount").val($("#personSum").text());
}

//导入到标签
function importTag(json,fn,boxId){
	$(boxId + " .search_result").html("");
	$.fn.search.createSearch(json,fn,boxId);
	 //解除绑定        
   $(boxId + " .search_input").unbind('keyup');  
   //定义搜索
   $(boxId + " .search_input").bind('keyup',function(){
		var val_txt = $(this).val().replace(/\s/g,"");
		if(val_txt==="") {
			//判断类型
			$.fn.search.createSearch(json,fn,boxId);
			return false;
		}		
		$.fn.search.keySearch(json,fn,boxId,val_txt);
   })
};

