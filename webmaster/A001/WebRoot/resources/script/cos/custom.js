
$(document).ready(function() {
	
	$("#type").change(function(){ 
		var typeStr = $(this).children('option:selected').val(); 
		if(typeStr == '0'){
			$("#test").css("display","block");	
		}else{
			$("#test").css("display","none");
		}
    }); 
   
    jQuery.validator.addMethod("sendCount", function(value, element) {
		   var aint=parseInt(value);	
		    return (aint>=-1)&&(aint+"")==value;   
		  }, "请输入合法的值");   

	
	$("#DetailForm").validate({
		errorPlacement : function(error, element) { 
			var e = element.attr("id")+"_span";
			var content = "<font color='red'>"+$(error).text()+"</font>";
			$("#"+e).html(content);
			element.change(function(){
				$("#"+e).html("");
	        });
				
		},
		rules: {
			type: "required",
			cosName: "required",
			userCount: {
				required: true,
				digits: true,
				min:0
			},
			daySend: {
				required: true,
				sendCount: true,
				min:0
			},
			weekSend: {
				required: true,
				sendCount: true,
				min:0
			},
			monthSend: {
				required: true,
				sendCount: true,
				min:0
			},
			totalSend: {
				required: true,
				sendCount: true,
				min:0
			}
		},
		messages: {
			type: "请选择套餐类型",
			cosName: "请输入套餐名称",
			userCount: "请输入合法的值",
			daySend: "请输入合法的值",
			weekSend: "请输入合法的值",
			monthSend: "请输入合法的值",
			totalSend: "请输入合法的值"
		}
	});
});





function saveCos() {
	var flag = $("#DetailForm").valid();
	if(flag) {
		if($("#type").val()==0){
			 if($("#startTime").val()=="" ||  $("#endTime").val()=="" ){
				 alert("试用时间不能为空"); 
			 }
		} 		
		if($("#type").val()==0 && $("#userCount").val()>1) {
			alert("试用套餐子账号个数不能大于1");
			$("#userCount").focus();
		}else if($("#type").val()==1 && $("#userCount").val()>50) {
			alert("正式套餐子账号个数不能大于50");
			$("#userCount").focus();
		}else {
			$("#btn_save").attr("disabled",true);
			$("#prompt").show();
			var form=$("#DetailForm");
			var params = form.serialize();
			$.post(form.attr("action"),params,function(data){
				if(data){
					if("success"==data.result){
		 	     		//$("#msg",window.dialogArguments.document).val("自定义套餐成功");
		 	     		$('body').messagebox('自定义套餐成功', '', 1, 2000);
		 	     		setTimeout("closeWin()",2000);
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
}

