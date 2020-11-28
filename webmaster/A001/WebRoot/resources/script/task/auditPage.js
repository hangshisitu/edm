/**
 * 审核
 * @param flag
 */
function audit(flag) {
	$("#flag").val(flag);
	var suggestion = $("#suggestion").val();
	if(flag=='no') {
		if(suggestion.replace(/\s/g,"") == "") {
			$('body').messagebox("请填写审核意见", '', 0, 2000);
			$("#suggestion").focus();
		}else {
			if(suggestion.length > 60){
				$('body').messagebox("不能超过60个字符", '', 0, 2000);
				$("#suggestion").focus();
			}else{
				if(window.confirm("确定审核不通过吗？")) {
					$("#prompt").show();
					$("#btn_yes").attr("disabled",true);
					$("#btn_no").attr("disabled",true);
					var form=$("#DetailForm"),params = form.serializeArray();
					$.post(form.attr("action"),params,function(data){
						if(data){
							if("success"==data.result){
								$('body').messagebox(data.message, '', 1, 2000);
								setTimeout("timingClose()",2000);
							}else {
								$("#prompt").hide();
								$("#btn_yes").attr("disabled",false);
								$("#btn_no").attr("disabled",false);
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
		}
	}else {
		if(suggestion.length > 60){
			$('body').messagebox("不能超过60个字符", '', 0, 2000);
			$("#suggestion").focus();
		}else{
			if(window.confirm("确定审核通过吗？")) {
				$("#prompt").show();
				$("#btn_yes").attr("disabled",true);
				$("#btn_no").attr("disabled",true);
				var form=$("#DetailForm"),params = form.serializeArray();
				$.post(form.attr("action"),params,function(data){
					if(data){
						if("success"==data.result){
							$('body').messagebox(data.message, '', 1, 2000);
							setTimeout("timingClose()",2000);
						}else {
							$("#prompt").hide();
							$("#btn_yes").attr("disabled",false);
							$("#btn_no").attr("disabled",false);
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
	}
}

/**
 * 定时关闭
 */
function timingClose() {
	window.close();
}