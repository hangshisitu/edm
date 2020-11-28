$(document).ready(function(){
	$("#order_list").UIselect({
		width: 120,
		id:"#order_list",
		id_id: "#order",
		ho_fun: "order()",
		arrdata: new Array(["deliveryTime", "按投递时间排序"], ["sentCount", "按发送总数排序"], ["reachCount", "按成功率排序"], ["readUserCount", "按打开率排序"], ["clickUserCount", "按点击率排序"])
	}); 
});

function order() {
	var url = $("#listUrl").val()+"&orderBy="+$("#order").val();
	location.assign(url);
}

//查询
function search() {
	$("#currentPage").val(1)
	$("#ListForm").attr("action",$("#listUrl").val());
	$("#ListForm").submit();
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
function exportCsv(url) {
	$("#ListForm").attr("action",url+"&orderBy="+$("#order").val());
	$("#ListForm").submit();
	$("#ListForm").attr("action",$("#listUrl").val());
}

//导出
function exportOneTaskStat(url) {
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
		$("#ListForm").attr("action",url);
		$("#ListForm").submit();
	}
	
}



