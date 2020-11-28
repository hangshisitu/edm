$(document).ready(function(){
	prompt();
});

/**
 * 提示
 */
function prompt() {
	var msg = $("#msg").val();
	if(msg != null&&msg!="") {
		$('body').messagebox(msg, '', 1, 2000);
	}
}

/**
 * 查看账号信息
 * @param id  
 */
function view(url) {
//	$("#ListForm").attr("action",url);
//	$("#ListForm").submit();
	window.location = url;
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
	
	document.getElementById("search_singleStatus").value = "";
	document.getElementById("search_cosId").value = "";
	document.getElementById("search_way").value = "";
}

/**
 * 激活账号
 * @param url	 
 * @param userId
 */
function activate(url,userId) {
	if(window.confirm("确定要激活 "+userId+" 这个账号吗？")) {
		$("#ListForm").attr("method", "POST");
		$("#ListForm").attr("action",url);
		$("#ListForm").submit();
	}
}

/**
 * 删除账号
 * @param url
 * @param userId
 */
function del(url,userId) {
	if(window.confirm("确定要删除 "+userId+" 这个账号吗？")) {
		$("#ListForm").attr("method", "POST");
		$("#ListForm").attr("action",url);
		$("#ListForm").submit();
	}
}

/**
 * 冻结账号
 * @param url
 * @param userId
 */
function freeze(url,userId) {
	if(window.confirm("确定要冻结 "+userId+" 这个账号吗？")) {
		$("#ListForm").attr("method", "POST");
		$("#ListForm").attr("action",url);
		$("#ListForm").submit();
	}
}

/**
 * 解冻
 * @param url
 * @param userId
 */
function unfreeze(url,userId) {
	if(window.confirm("确定要解冻 "+userId+" 这个账号吗？")) {
		$("#ListForm").attr("method", "POST");
		$("#ListForm").attr("action",url);
		$("#ListForm").submit();
	}
}

/**
 * 创建新账号
 */
function createAccount(url) {
//	$("#ListForm").attr("action",url);
//	$("#ListForm").submit();
	window.location = url;
}

function showSubUser(url,userId) {
	$.ajax({
        type: "POST",
        url: url,
        success: function(data) {
            if("success"==data.result) {
            	var title = "";
            	$.each(data.userList, function(i,user){      
            	    title += user.userId+" "; 
            	}); 
            	if(title!="") {
            		$("#"+userId).attr("title",title);
            	}
            	
            }
        },
        error: function(e) {
           
        }
    });
}
	





