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

/**
 * 查看账号信息
 * @param id  
 */
function view(url) {
	//openWin(url,'','500px','600px');
	//$("#ListForm").submit();
//	$("#ListForm").attr("action", url);
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
 * 删除账号
 * @param url
 * @param userId
 */
function del(url,userId) {
	if(window.confirm("删除此账号将会对该账号下创建的全部信息一并进行删除，包括应用于邮件中的模板、收件人数据及旗下子账户等。数据删除后不可恢复，请做好数据备份工作。确认删除吗？")) {
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

/**
 * 编辑账号
 */
function edit(url,userId){
	window.location = url;	
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

