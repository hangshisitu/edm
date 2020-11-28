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
 * 创建子账号
 * @param id  
 */
function createSubUser(url) {
	openWin(url,'','380px','450px');
	$("#ListForm").submit();
}

/**
 * 修改子账号
 * @param id  
 */
function editSubUser(url) {
	openWin(url,'','380px','450px');
	$("#ListForm").submit();
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
	$("#search_trueName").val("");
}

/**
 * 激活账号
 * @param url	 
 * @param userId
 */
function activate(url,userId) {
	if(window.confirm("确定要激活 "+userId+" 这个账号吗？")) {
		location.assign(url);
	}
}

/**
 * 删除账号
 * @param url
 * @param userId
 */
function deleteSubUser(url,userId) {
	if(window.confirm("确定要删除 "+userId+" 这个账号吗？")) {
		location.assign(url);
	}
}

/**
 * 冻结账号
 * @param url
 * @param userId
 */
function freeze(url,userId) {
	if(window.confirm("确定要冻结 "+userId+" 这个账号吗？")) {
		location.assign(url);
	}
}

/**
 * 解冻
 * @param url
 * @param userId
 */
function unfreeze(url,userId) {
	if(window.confirm("确定要解冻 "+userId+" 这个账号吗？")) {
		location.assign(url);
	}
}



