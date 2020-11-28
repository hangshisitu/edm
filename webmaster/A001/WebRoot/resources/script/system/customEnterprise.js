/**
 * 删除logo
 * @param url
 * @param id
 */
function deleteCustomLogo(url,id) {
	if(window.confirm("确定要删除 "+id+" 这个Logo吗？")) {
		location.assign(url);
	}
}

/**
 * 冻结logo
 * @param url
 * @param id
 */
function freeze(url,id) {
	if(window.confirm("确定要禁用 "+id+" 这个Logo吗？")) {
		location.assign(url);
	}
}

/**
 * 解冻logo
 * @param url
 * @param id
 */
function unfreeze(url,id) {
	if(window.confirm("确定要启用 "+id+" 这个Logo吗？")) {
		location.assign(url);
	}
}

/**
 * 创建logo
 * @param id  
 */
function createCustomLogo(url) {
	openWin(url,'','380px','450px');
	$("#customForm").submit();
}


/**
 * 搜索
 */
function search() {
	$("#currentPage").val(1);
	$("#customForm").submit();
}



