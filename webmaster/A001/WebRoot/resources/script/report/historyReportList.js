function search1() {
	$("#currentPage").val(1);
	$("#ListForm1").attr("action", $("#listUrl").val());
	$("#ListForm1").submit();
}

function resetParam1() {
	//$("#search_emailDomain").val("");
	$("#d4311").val("");
	$("#d4312").val("");
	$("#search_cycle1").val("");
}



function search2() {
	$("#currentPage").val(1);
	$("#ListForm").submit();
}

function resetParam2() {
	$("#search_userId").val("");
	$("#d5311").val("");
	$("#d5312").val("");
	$("#search_cycle2").val("");
}


function resetParam3() {
	$("#search_triggerName").val("");
	$("#d4311").val("");
	$("#d4312").val("");
	$("#search_cycle1").val("");
}

//导出
function exportStat(url) {
	var checkedIds = "";
	$("input[name=checkedIds]").each(function() {
		if($(this).prop("checked") == true) {
			checkedIds += $(this).val() + ",";
		}
	});
	if(checkedIds == "") {
		if(window.confirm("您未选择特定记录，确定导出全部记录吗？")) {
			location.assign(url);
		}
	}else {
		location.assign(url+"?checkedIds="+checkedIds);
	}
	
	
}

//打印
function printStat(url){
	var checkedIds = "";
	$("input[name=checkedIds]").each(function() {
		if($(this).prop("checked") == true) {
			checkedIds += $(this).val() + ",";
		}
	});
	if(checkedIds == "") {
		alert("请选择需要打印记录");
	}else {
		window.open(url+"?checkedIds="+checkedIds, "newwindow", 'height=600,width=800,top=0,left=0,toolbar=no,menubar=no,scrollbars=no, resizable=no,location=no, status=no');
	}
	
}

//查看全部任务
function viewAllTask(url) {
	location.assign(url);
}

//查看客户任务列表
function customerTaskDetail(url) {
	location.assign(url);
}

/**
 * 導出
 * @param url
 */
function exportHis(url) {
	$("#ListForm1").attr("action", url);
	$("#ListForm1").submit();
}
/**
 * 导出
 * @param url
 */
function exportApiDelivery(url) {
	$("#ListForm1").attr("action", url);
	$("#ListForm1").submit();
}
