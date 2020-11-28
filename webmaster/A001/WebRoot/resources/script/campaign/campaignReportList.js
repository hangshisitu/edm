//查询
function search() {
	$("#currentPage").val(1);
	$("#ListForm").attr("action",$("#listUrl").val());
	$("#ListForm").submit();
}


//详细
function detail(url) {
	location.assign(url);
}



function exportCsv(url) {
//	$("#ListForm").attr("action", url);
//	$("#ListForm").submit();
	window.location = url;
	
} 




