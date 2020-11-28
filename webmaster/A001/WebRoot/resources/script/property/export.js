$(document).ready(function(){
	prompt();
});

/**
 * 提示
 */
function prompt() {
	var msg = $("#msg").val();
	if(msg != null&&msg!="") {
		$('body').messagebox(msg, '', 0, 2000);
	}
}

function statNum() {
	//判断是否选择了标签
	var ids = $("#enTagId").val();
	if(ids=="") {
		$('body').messagebox("请至少选择一个包含标签！", '', 0, 3000);
	}else {
		
		var emails = $("#personSum").html();
		var num = parseInt(emails);
		if(num==0) {
			$('body').messagebox("收件人总数为0", '', 0, 3000);
		}else {
			popTips("#remindCustom","导出数据","正在筛选 <img src='/resources/img/loading.gif'/>");
			$("#export_btn").attr("disabled",true);
			var form=$("#export"),params = form.serializeArray();
			$.post(form.attr("action"),params,function(data){
				$("#export_btn").attr("disabled",false);
				if(data){	
					if("success"==data.result){
						$("#lcId").val(data.lcId);
						var tips="<p><strong>处理成功！</strong></p><p>筛选成功：当前共筛选<span class='red'>" + data.counter + "</span>个收件人</p>";
						$("#exportPath").val(data.fileName);
						$("#counter").val(data.counter);
						popTips("#remindCustom","导出数据",tips,1);
					}else {
						closeDialog();
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

function downloadFile() {
	var counter = $("#counter").val();
	if(counter==0) {
		closeDialog();
		$('body').messagebox("筛选结果为0，无数据可导出！", '', 0, 3000);
	}else {
		closeDialog();
		$("#addPath").submit();
	}
	
	
}