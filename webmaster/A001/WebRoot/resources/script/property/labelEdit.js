
function saveLabel() {
	//$("#DetailForm").submit();
	
	var labelName = $("#labelName").val();
	if(isEmpty(labelName)) {
		$("#labelName").focus();
		$('body').messagebox("请输入标签名称！", '', 0, 3000); 
	}else {
		$("#btn_save").attr("disabled",true);
		$("#prompt").show();
		var form=$("#DetailForm"),params = form.serializeArray();
		$.post(form.attr("action"),params,function(data){
			if(data){
				if("success"==data.result){
					$("#lcId").val(data.lcId);
	 	     		$('body').messagebox(data.message, '', 1, 2000);
	 	     		setTimeout("labelList()",2000);
				    //window.close();
				}else {
					$("#btn_save").attr("disabled",false);
					$("#prompt").hide();
					if (typeof(data.message) == "undefined") { 
						$('body').messagebox("无法获取响应，有可能账号已在别处登录", '', 0, 3000); 
				    }else {
				    	$('body').messagebox(data.message, '', 0, 3000);
				    }   
				}
			}
			
		});
	}
	
	
}

function back(url) {
	window.location.href = url;
}

function labelList(){
	var id = $("#lcId").val();
	var url = $("#successUrl").val();
	window.location.href = url+"?categoryId="+id;
}


