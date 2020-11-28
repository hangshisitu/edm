$(function(){
	$('input.groovybutton:eq(0),.tab a,.page a,select[name=pageSize] option,input[value=GO]').live('click',function(){
		window.loading();
	});
});

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
function exportStatApiGroups(url) {
	$("#ListForm").attr("action",url);
	$("#ListForm").submit();
	$("#ListForm").attr("action",$("#listUrl").val());
}
/**
 * 查看群发任务详细
 * @param url
 */
function customerTaskDetail(url) {
	location.assign(url);
}
