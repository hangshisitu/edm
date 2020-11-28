var triger_list;

$(document).ready(function() {
	$("#domain_list").UIselect({
		width: 118,
		id_id: "#group",
		arrdata: new Array(["","不限"],["139","139邮箱"],["163","网易邮箱"],["gmail","Gmail邮箱"],["qq","QQ邮箱"],["sina","新浪邮箱"],["sohu","搜狐邮箱"])
	});
	
	$("#triger_list").UIselect({
		width: 118,
		id_id: "#triger",
		arrdata: triger_list
	});
	
	$("#corp_list").UIselect({
		width: 120,
		id_id: "#corpId",
		arrdata: corp_list
	});
});

function action(action, message) {
	var checkedIds = [];
	$("input[name=checkedIds]").each(function() {
		if($(this).prop("checked") == true) {
			checkedIds.push($(this).val());
		}
	});
	if (checkedIds.length < 2) {
		error_message("请至少选择2个以上需要" + message + "的任务", 1);
	} else if (action == "compare" && checkedIds.length > 5) {
		error_message("请至多选择5个以下需要" + message + "的任务", 1);
	} else {
		if ($("#m").length > 0) {
			location.assign(action + "?m=" + $("#m").val() + "&checkedIds=" + checkedIds.join(','));			
		} else {
			location.assign(action + "?checkedIds=" + checkedIds.join(','));
		}
	}
}
