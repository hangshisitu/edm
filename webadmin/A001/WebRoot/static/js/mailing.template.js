function del() {
	del_tab("请选择需要删除的模板", 'del', 0);
}

$(document).ready(function() {	
	$("#corp_list").UIselect({
		width: 120,
		id_id: "#corpId",
		arrdata: corp_list
	});
});
