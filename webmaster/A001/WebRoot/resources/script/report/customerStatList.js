$(function(){
	$('input.groovybutton:eq(0),.tab a,.page a,select[name=pageSize] option,input[value=GO]').live('click',function(){
		window.loading();
	});
});

function search() {
	$("#currentPage").val(1);
	$("#ListForm").attr("method", "get");
	$("#ListForm").attr("action",$("#listUrl").val());
	$("#ListForm").submit();
}

function resetParam() {
	$("#search_userIdLike").val("");
	$("#d5311").val("");
	$("#d5312").val("");
	$("#search_cycle2").val("");
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

function exportStatNew(url) {
	$("#ListForm").attr("action", url);
	$("#ListForm").submit();
	$("#ListForm").attr("action", $("#listUrl").val());
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
		alert("请选择需要打印的记录");
	}else {
		var beginTimeStr = $("#d5311").val(); // 开始时间
		var endTimeStr = $("#d5312").val(); // 结束时间
		url = url + "?checkedIds=" + checkedIds + "&search_beginTimeStr=" + beginTimeStr + "&search_endTimeStr=" + endTimeStr;
		window.open(url, "newwindow", 'height=800,width=1024,top=0,left=0,toolbar=no,menubar=no,scrollbars=yes, resizable=yes,location=no, status=no');
	}
	
}


//查看客户任务列表
function customerTaskDetail(url) {
	location.assign(url);
}
