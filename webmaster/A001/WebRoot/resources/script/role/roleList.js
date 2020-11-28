$(function(){

	$(".J_hook_more").each(function(){
		var txt = $(this).find("ul").html();
		var boxEle = $(this).find("ul");
		var num = $(this).find("li").length;
		if(num > 1){
			$(this).gtips({
				eType:'mouseover',
				content:txt,
				boxEle:boxEle,
				dirct:'top',
				spac:-29
			});
		}
	}); 
});

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

function createAccount(url) {
	window.location = url;
}

function createRole(url) {
	window.location = url;
}

function delRole(url){
	if(window.confirm("确定删除？")){
		window.location = url;
	}	
}

