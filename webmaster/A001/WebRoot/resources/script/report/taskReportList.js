//查询
function search() {
	$("#currentPage").val(1);
	$("#ListForm").attr("action",$("#listUrl").val());
	$("#ListForm").submit();
}

//重置参数
function resetParam() {
	//$("#search_userId").val("");
	$("#search_taskName").val("");
	$("#d5311").val("");
	$("#d5312").val("");
	$("#search_cycle").val("");
	$("#search_labelStatus").val("");
	$("#search_comTaskId").val("");
}


//导出
function exportTaskStat(url) {
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
		location.assign(url+"&checkedIds="+checkedIds);
	}
}

//导出
function exportTaskStatNew(url) {
	$("#ListForm").attr("action",url);
	$("#ListForm").submit();
	$("#ListForm").attr("action", $("#listUrl").val());
}

//导出
function exportOneTaskStat(url) {
	location.assign(url);
}


//打印
function printTaskStat(url){
	var checkedIds = "";
	$("input[name=checkedIds]").each(function() {
		if($(this).prop("checked") == true) {
			checkedIds += $(this).val() + ",";
		}
	});
	if(checkedIds == "") {
		alert("请选择需要打印的记录");
	}else {
		window.open(url+"?checkedIds="+checkedIds, "newwindow", 'height=800,width=1024,top=0,left=0,toolbar=no,menubar=no,scrollbars=yes, resizable=yes,location=no, status=no');
	}
	
}

//返回
function back(url) {
	location.assign(url);
}

//详细
function detail(url) {
	location.assign(url);
}

//对比
function compare(url){
	var checkedIds = "";
	$("input[name=checkedIds]").each(function() {
		if($(this).prop("checked") == true) {
			checkedIds += $(this).val() + ",";
		}
	});
	var len = $("input[name='checkedIds']:checked").length;
	if(len == 0) {
		alert("请选择需要对比的任务");
	}else if(len>4) {
		alert("对比任务不能超过4个");
	}else {
		window.open(url+"?checkedIds="+checkedIds, "newwindow", 'height=800,width=1024,top=0,left=0,toolbar=no,menubar=no,scrollbars=yes, resizable=yes,location=no, status=no');
	}
	
}


//对比
function compareTask(url){
	var checkedIds = "";
	$("input[name=checkedIds]").each(function() {
		if($(this).prop("checked") == true) {
			checkedIds += $(this).val() + ",";
		}
	});
	var len = $("input[name='checkedIds']:checked").length;
	if(len == 0) {
		alert("请选择要对比的任务");
	}else if(len<2) {
		alert("对比任务不能少于2个");
	}else if(len>5) {
		alert("对比任务不能超过5个");
	}else {
		$("#ListForm").attr("action", url);
		$("#ListForm").submit();
	}
	
}

/**
 * 
 * 查看
 * @param url
 */
function view(url) {
	window.open(url, "newwindow", 'height=600,width=800,top=0,left=0,toolbar=no,menubar=no,scrollbars=no, resizable=no,location=no, status=no');
}

function importPropLib(url) {
	window.location.href=url;
}


