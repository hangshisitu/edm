$(document).ready(function(){
	prompt();
});

/**
 * 提示
 */
function prompt() {
	var msg = $("#msg").val();
	if(msg != null&&msg!="") {
		$('body').messagebox(msg, '', 1, 2000)
	}
}


//更改通道
function changeRes(url)
{
	var value = $("#resId").val();
	if(value=="") {
		alert("请选择通道！");
		$("#resId").focus();
	}else {
		//$("#channelId").val(value);
		var len = $("input[name='checkedIds']:checked").length;
		if(len==0) {
			alert("请选择要更改通道的账号！");
		}else {
			if(window.confirm("确定要为所选账号更改通道吗？")) {
				var form = $("#ListForm");
				$("#ListForm").attr("method", "get");
				form.attr("action",url);
				form.submit();
			}
		}
		
	}
	
	
}


/**
 * 搜索
 */
function search() {
	$("#currentPage").val(1);
	$("#ListForm").submit();
}

/**
 * 重置查询参数
 */
function resetParam() {
	$("#search_userId").val("");
	$("#search_companyName").val("");
	$("#d4311").val("");
	$("#d4312").val("");
	
	document.getElementById("search_formalId").value = "";
	document.getElementById("search_cosId").value = "";
	document.getElementById("search_way").value = "";
}