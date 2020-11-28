$(document).ready(function() {	
	$("#status_list").UIselect({
		width: 95,
		id_id: "#status",
		arrdata: new Array(["", "全部"], ["1", "有效"], ["2", "冻结"])
	});
	
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
	
	$("#role_list").UIselect({
		width: 120,
		id_id: "#roleId",
		arrdata: role_list
	});
	
	$("#corp_list").UIselect({
		width: 120,
		id_id: "#corpId",
		arrdata: corp_list
	});
	
});
