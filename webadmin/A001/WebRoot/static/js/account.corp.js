$(document).ready(function(){
	$(".Jaddcorp").on('click',function(){
		openPop("add");
	});
	
	$("#corp_list").UIselect({
		width: 120,
		id_id: "#corpId",
		arrdata: corp_list
	});
});
