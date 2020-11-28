/**
 * 自定义套餐
 */
function customCos(url){
	openWin(url,window,'480px','650px');
	var msg = $("#msg").val();
	if(msg != null&&msg!="") {
		$('body').messagebox(msg, '', 1, 2000);
	};
}


/**
 * 删除套餐
 * @param url
 * @param id
 */
function deleteCos(url,id) {
	if(window.confirm("确定要删除 "+id+" 这个套餐吗？")) {
		location.assign(url);
	}
}

/**
 * 冻结套餐
 * @param url
 * @param id
 */
function freeze(url,id) {
	if(window.confirm("确定要禁用 "+id+" 这个套餐吗？")) {
		location.assign(url);
	}
}

/**
 * 解冻套餐
 * @param url
 * @param id
 */
function unfreeze(url,id) {
	if(window.confirm("确定要启用 "+id+" 这个套餐吗？")) {
		location.assign(url);
	}
}

/**
 * 查看套餐信息
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