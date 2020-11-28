$(document).ready(function(){

	$("#propSet").search({
		textId:"#assProp",
		valueId:"#assPropId",
		boxId:"#slideSearchProp",
		json:ass_prop,
		itemFn:"addAssProp(this,'#assProp',propVal)",
		subArr:propVal,
		isHoriz:true,
		showSelectFn:addPropSelect
	});	

	$("#assProp").live('keydown',function(e){
		if(e.keyCode==8){
			var item=getDeleteItem($(this)[0]);
			removeProp(propVal,item,"#assProp");
		}
		e.preventDefault();
	});
	
});


function importPropLib() {
	//$("#DetailForm").submit();
	var taskId = $("#task").val();
	if(isEmpty(taskId)) {
		$("#task").focus();
		$('body').messagebox("请输入正确的任务ID！", '', 0, 2000); 
	}else {
		var array = taskId.split(".");
		if(array.length!=3) {
			$('body').messagebox("请输入正确的任务ID！", '', 0, 2000); 
			$("#task").focus();
		}else {
			if(isNaN(array[0])||isNaN(array[1])||isNaN(array[2])) {
				$('body').messagebox("请输入正确的任务ID！", '', 0, 2000); 
				$("#task").focus();
			}else {
				if($("#assProp").size() && findArrItem(propVal,2,'行业兴趣') == -1) {
					$('body').messagebox("请至少设置一项行业兴趣关联属性！", '', 0, 2000); 
				}else {
					$("#btn_save").attr("disabled",true);
					$("#prompt").show();
					var form=$("#DetailForm"),params = form.serializeArray();
					$.post(form.attr("action"),params,function(data){
						if(data){
							if("success"==data.result){
				 	     		$('body').messagebox(data.message, '', 1, 2000);
				 	     		setTimeout("toPropertyIndex()",2000);
							}else {
								$("#btn_save").attr("disabled",false);
								$("#prompt").hide();
								if (typeof(data.message) == "undefined") { 
									$('body').messagebox("无法获取响应，有可能账号已在别处登录", '', 0, 3000); 
							    }else {
							    	$('body').messagebox(data.message, '', 0, 2000);
							    }   
							}
						}
						
					});
				}
			}
		}
	}
}

function toPropertyIndex(){
	var url = $("#successUrl").val();
	window.location.href = url;
}