$(document).ready(function() {
	$("#DetailForm").validate({
		rules: {
			userId:{
				required: true,
				minlength: 3
			},
			company: "required",
			cosId:"required",
			formalId:"required",
			testedId:"required",
			password: {
				required: false,
				minlength: 8
			},
			confirmPassword: {
				required: false,
				minlength: 8,
				equalTo: "#password"
			},
			email: {
				required: false,
				email: true
			}
		},
		messages: {
			userId: {
				required:"请输入用户名",
				minlength: "用户名长度至少3个字符"
			},
			company: "请输入公司名称",
			cosId: "请选择套餐类型",
			formalId: "请选择正式投递通道",
			testedId: "请选择测试投递通道",
			password: {
				minlength: "密码长度至少8个字符"
			},
			confirmPassword: {
				minlength: "密码长度至少8个字符",
				equalTo: "两次输入的密码不相等"
			},
			email: "请输入正确的邮箱格式"
		},
		submitHandler: function() {
			var form=$("#DetailForm"),params = form.serializeArray();
			$.post(form.attr("action"),params,function(data){
				$("#prompt").css("display","none");
				$("#btn_submit").attr("disabled",false);
				if(data){
					if("success"==data.result){
						//$("#msg",window.dialogArguments.document).val(data.message);
						$('body').messagebox(data.message, '', 1, 2000)
						//window.dialogArguments.location.reload();
		 	     		setTimeout("timeClose()",2000);
					}else {
						$('body').messagebox(data.message, '', 0, 3000)
					}
				}
				
			});
		}
	});
});

/**
 * 获取套餐信息
 * @param obj
 */
function getCosInfo(obj,ctx) {
	if(obj.value=="") {
		$("#subUserCount").html("");
		$("#daySendCount").html("");
		$("#weekSendCount").html("");
		$("#monthSendCount").html("");
		$("#totalSendCount").html("");
	}else {
		$.ajax({
	         type: "POST",
	         url: ctx+"/cos/getByCosId",
	         data: {cosId:obj.value},
	         async: true,
	         success: function(data) {
	             //请求成功后的回调函数
	             //returnedData--由服务器返回，并根据 dataType 参数进行处理后的数据；
	             //根据返回的数据进行业务处理
	        	if(data!=null) {
	        		$("#subUserCount").html(data.userCount);
	 	     		$("#daySendCount").html(data.daySend);
	 	     		$("#weekSendCount").html(data.weekSend);
	 	     		$("#monthSendCount").html(data.monthSend);
	 	     		$("#totalSendCount").html(data.totalSend);
	        	}
	           
	         },
	         error: function(e) {
	             alert(e);
	             //请求失败时调用此函数
	         }
	     });
	 }
	
}


function saveAccount(url,flag) {
	//$("#DetailForm").validate();
	//$("#DetailForm").valid();
	$("#DetailForm").attr("action",url);
	$("#DetailForm").submit();
}


/**
 * 自定义套餐
 */
function customCos(url){
	openWin(url,window,'350px','410px');
	var msg = $("#msg").val();
	if(msg != null&&msg!="") {
		$('body').messagebox(msg, '', 1, 2000)
	}
}

function timeClose() {
	window.close();
}



