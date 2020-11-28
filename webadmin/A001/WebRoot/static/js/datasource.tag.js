$(document).ready(function() {
	$("#recipient_list").UIselect({
		width: 90,
		id:"#recipient_list",
		id_id: "#recipient",
		ho_fun: "recipient()",
		arrdata: new Array(["/selection/import", "导入"], ["/recipient/add?m=tag", "新建"])
	});
});

function recipient() {
	location.assign("/datasource" + $("#recipient").val());
}