$(document).ready(function(){
	$("#page_btm").UIselect({
			  width:53,
			  id_id:"#pageSize",
			  ho_fun:"pageSize_fun()",
			  arrdata:new Array([20,"20"],[40,"40"],[80,"80"],[100,"100"])
	});
	
	//两个显示同步更新
	$("input[name=pageNo]").keyup(function(){
		$('input[name=pageNo]').val($(this).val());
	});
	
	$('.go a').click(function(){
		var num=$(this).parents(".page").find("input[name=pageNo]").val();
		if(!/^[1-9][0-9]*$/.test(num)){
			error_message("输入的页码有误",1);
			$(this).parents(".page").find("input[name=pageNo]").val('').focus();
			return false;
		}
		location.assign(pageNo_fun());
	});
});


function pageSize_fun(){
	document.cookie="PSC="+$("#pageSize").val()+";path=/";
	location.reload();
}

function pageNo_fun() {
	var url = "http://" + window.location.host + location.pathname + "?";
	var query = location.search.substring(1);
	var params = query.split("&");
	var hasPageNo = false;
	for(var i = 0; i < params.length; i++) {
		var pos = "";
		var param = params[i];
		pos = param.indexOf("=");
		if(pos == -1) {
			continue;
		}
		var key = param.substring(0, pos);
		var value = param.substring(pos + 1);

		if(key == "pageNo") {
			hasPageNo = true;
			url = url + key + "=" + $("input[name=pageNo]").val() + "&";
		} else {
			if(!unescape(value) == "")  {
				url = url + key + "=" + value + "&";
			}
		}
	}
	if(!hasPageNo) {
		url = url + "pageNo=" + $("input[name=pageNo]").val();
	}
	
	if(url.substring(url.length) == "&") {
		url = url.substring(0, url.length - 1);
	}
	return url;
}
