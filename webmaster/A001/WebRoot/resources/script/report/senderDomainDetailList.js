function search() {
	$("#currentPage").val(1);
	$("#ListForm").submit();
}

function resetParam() {
	$("#search_emailDomain").val("");
	$("#d5311").val("");
	$("#d5312").val("");
	$("#search_cycle").val("");
}




//返回
function back(url) {
	location.assign(url);
}


