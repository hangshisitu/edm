$(document).ready(function(){
	$("#msg_cfm").live('click',function(){
		closeDialog();
	})	

	$("#enSet").search({
		textId:"#en_cate",
		valueId:"#en_cateId",
		boxId:"#slideSearch",
		json:catedata,
		itemFn:"pickArr(this,'#en_cate',callReloadSum)",
		deleteFn:deleteCate
	});
		
	$("#outSet").search({
		textId:"#out_cate",
		valueId:"#out_cateId",
		boxId:"#slideSearch",
		json:catedata,
		itemFn:"pickArr(this,'#out_cate',callReloadSum)",
		deleteFn:deleteCate
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
});

function activeCnd() {
	$("#active").val($("#activePeriod").val());
}

function exportFile(){
	$('#export').ajaxForm({
		url: '/step/2?action=tag',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json',
		beforeSerialize: function($form, options) {
			loading();
		},
		beforeSubmit: function(formData, jqForm, options) {
			var form = jqForm[0]; 
			if (!$("input[name=includeTagIds]").val()) {
				error_message("请选择包含标签", 1);
				del_loading();
				return false; 
			}	
		},
		success: function(data) {
			closeDialog();
			del_loading();
			if (data.code == 1) {
				popTips("#remindCustom2","<p>筛选成功：当前共选中了<span class='red'>" + data.emailCount + "</span>个收件人用于投递<span class=\"f666\">（已去重过滤）</span></p>");	
			} else {
				error_message(data.message, 1);
			} 
		}   
	});
}

function exports(opt) {
	if (!$("input[name=includeTagIds]").val()) {
		error_message("请选择包含标签", 1);
		return false; 
	}
		
	if(opt == "input") {
		if (!$("input[name=tagId]").val()) {
			error_message("请选择标签", 1);
			return false; 
		}
	}
	closeDialog();
	$("#opt").val(opt);
	$('#export').submit();
}

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


//刷新收件人总数
function callReloadSum(){
	var en_cate_arr = $("#en_cate").data('subArr');
	var out_cate_arr = $("#out_cate").data('subArr');
	reloadPersonSum(catedata,en_cate_arr,out_cate_arr,"#emailCount2");
	$("#exportTips").text('（未过滤去重）');
}