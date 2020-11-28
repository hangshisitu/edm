$(document).ready(function() {
	$("#order_list").UIselect({
		width: 120,
		id:"#order_list",
		id_id: "#order",
		ho_fun: "order()",
		arrdata: new Array(["deliveryTime", "按投递时间排序"], ["sentCount", "按发送总数排序"], ["reachCount", "按成功率排序"], ["readUserCount", "按打开率排序"], ["clickUserCount", "按点击率排序"])
	});
});

function order() {
	location.assign("history?campaignId=" + cid + "&orderBy=" + $("#order").val());
}

function campaign_exports(cid) {
	if(cid == "") {
		error_message("没有需要导出的历史报告", 1);
		return false;
	}
	
	location.assign("/report/export/campaign/history?campaignId=" + cid + "&orderBy=" + $("#order").val());
}

function plan_exports(pid) {
	if(pid == "") {
		error_message("没有需要导出的周期报告", 1);
		return false;
	}
	
	location.assign("/report/export/plan?planId=" + pid);
}