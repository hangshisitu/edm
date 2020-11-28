$(document).ready(function(){
	prompt();
});

/**
 * 提示
 */
function prompt() {
	var msg = $("#msg").val();
	if(msg != null&&msg!="") {
		$('body').messagebox(msg, '', 0, 2000);
	}
}
function search() {
	$("#currentPage").val(1);
	$("#ListForm").submit();
}

function resetParam() {
	$("#search_senderEmail").val("");
	$("#d5311").val("");
	$("#d5312").val("");
	$("#search_cycle").val("");
}



//详细
function detail(url) {
	location.assign(url);
}

