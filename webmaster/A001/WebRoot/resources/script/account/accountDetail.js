

/**
 * 激活账号
 * @param url	 
 * @param userId
 */
function activate(url,userId) {
	if(window.confirm("确定要激活 "+userId+" 这个账号吗？")) {
		$("#btn_submit").attr("disabled",true);
		$("#prompt").show();
		var form=$("#DetailForm");
		var params = form.serializeArray();
		form.attr("action",url);
		$.post(form.attr("action"),params,function(data){
			if(data){
				if("success"==data.result){
					//$("#msg",window.dialogArguments.document).val(data.message);
					$('body').messagebox(data.message, '', 1, 2000)
					//window.dialogArguments.location.reload();
	 	     		setTimeout("success()",2000);
				}else {
					$("#prompt").hide();
					$("#btn_submit").attr("disabled",false);
					$('body').messagebox(data.message, '', 0, 3000)
				}
			}
			
		});
	}
}

//返回
function success() {
	var url = $("#backUrl").val();	
	window.location.href = url;
}

function back(){
//	var url = $("#backUrl").val();
	var url = "/account/accountList";
	window.location.href = url;
}