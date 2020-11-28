$(document).ready(function(){
	
	$(".main_table tr").die("mouseover");
	$(".main_table tr").die("mouseout");
	//主表格经过变色
	$('.log_table tr').live('mouseover', function() {
	  	$(this).addClass("tr_hover");
	});
	$('.log_table tr').live('mouseout', function() {
		$(this).removeClass("tr_hover");
	});
	
});


/**
 * 搜索
 */
function search() {
	var ip = $("#search_ip").val();
	if(ip!="") {
		if(checkIp(ip)) {
			$("#currentPage").val(1);
			$("#ListForm").attr("action",$("#listUrl").val());
			$("#ListForm").submit();
		}else {
			$('body').messagebox("ip格式不正确", '', 0, 2000);
			$("#search_ip").focus();
		}
	}else {
		$("#currentPage").val(1);
		$("#ListForm").attr("action",$("#listUrl").val());
		$("#ListForm").submit();
	}
	
	
	
}

/**
 * 导出
 * @param url
 */
function exportLog(url) {
	$("#ListForm").attr("action",url);
	$("#ListForm").submit();
	$("#ListForm").attr("action",$("#listUrl").val());
}


function checkIp(ip){
	var reg=/^(([01]?[\d]{1,2})|(2[0-4][\d])|(25[0-5]))(\.(([01]?[\d]{1,2})|(2[0-4][\d])|(25[0-5]))){3}$/
	if(reg.test(ip)){
	   return true;
	}else{
	   return false;
	}
}