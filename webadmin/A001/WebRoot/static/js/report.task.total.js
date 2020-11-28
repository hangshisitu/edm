function exports(checkedIds) {
	if(checkedIds == "") {
		error_message("没有需要导出的汇总", 1);
		return false;
	}
	
	location.assign("/report/export/task/total?checkedIds=" + checkedIds);
}