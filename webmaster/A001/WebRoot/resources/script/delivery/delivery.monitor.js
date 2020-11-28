$(document).ready(function(){
	$(".delevery_table .slide_info li").live("click", function(){
		var text=$(this).text();
		$(this).parents('td').find(".list_select").text(text);
		if($(this).attr('data-state')=='正常'){
			$(this).parents('td').next('td').html("<span class='green'>正常</span>");
		}else{
			$(this).parents('td').next('td').html("<span class='red'>超限停止</span>");
		}
	});
	
	
	
	
	
	
	
	
	
	
	setInterval("search()",1000*60*5);
	
});




















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
	document.getElementById("search_fromDomain").value = "";
	document.getElementById("search_status").value = "";
}






