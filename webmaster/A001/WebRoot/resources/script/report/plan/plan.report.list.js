//查询
function search() {
	$("#currentPage").val(1);
	$("#ListForm").submit();
}

//导出
function exportOnePlanStat(url) {
	location.assign(url);
}

//返回
function back(url) {
	location.assign(url);
}

// 报告
function report(url) {
	location.assign(url);
}

//汇总
function planCollect(url){
	var checkedIds = "";
	$("input[name=checkedIds]").each(function() {
		if($(this).prop("checked") == true) {
			checkedIds += $(this).val() + ",";
		}
	});
	var len = $("input[name='checkedIds']:checked").length;
	if(len == 0) {
		alert("请选择要汇总的任务");
	}else if(len<2) {
		alert("请至少选择2个以上需要汇总的任务");
	}else {
//		$("#ListForm").attr("method", "get");
//		$("#ListForm").attr("action", url);
//		$("#ListForm").submit();
		window.location = url + "?checkedIds=" + checkedIds.substring(0, checkedIds.length-1);
	}
	
}


