
/**
 * 查询
 */
function search() {
	$("#currentPage").val(1);
	$("#ListForm").attr("action",$("#listUrl").val());
	$("#ListForm").submit();
}
/**
 * 导出
 */
function exportApiTrigger(url) {
	$("#ListForm").attr("action",url);
	$("#ListForm").submit();
	$("#ListForm").attr("action",$("#listUrl").val());
}
/**
 * 查看群发任务详细
 * @param url
 */
function apiTriggerTaskDetail(url) {
	window.open(url, "newwindow", 'height=600,width=800,top=0,left=0,toolbar=no,menubar=no,scrollbars=no, resizable=no,location=no, status=no');
}
