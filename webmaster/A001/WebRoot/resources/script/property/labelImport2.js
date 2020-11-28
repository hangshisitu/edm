$(document).ready(function(){
	$("#propSet").bind('click',function(){
		openSearch(this,ass_prop,"addAssProp(this,propTemp,propVal)","#slideSearchProp",createCateHtml);
		$("#slideSearchProp .search_sort_title span:contains('行业兴趣')").html("行业兴趣<em>（多选 *）</em>");
		addPorpSelect();
	});

	$("#assProp").on('keydown',function(e){
		if(e.keyCode==8){
			var item=getDeleteItem($(this)[0]);
			for(key in propTemp){
				removeArrItem(propTemp[key],item);
			}
			appendToAssProp(propTemp,propVal,$(this));
		}
		e.preventDefault();
	});
	
	//初始化关联属性
	initAssProp("#assProp",propTemp,propVal,ass_prop);	
	
});


function importPropLib() {
	$("#DetailForm").submit();
}


$(document).ready(function() {
	jQuery.validator.addMethod("taskid", function(value, element) {
		var array = value.split(".");
		if(array.length!=3) {
			return false;
		}else {
			return true;
		}
	}, "请输入合法的值");   
	$("#DetailForm").validate({
		errorPlacement : function(error, element) { 
			var e = element.attr("id")+"_span";
			var content = "<font color='red'>"+$(error).text()+"</font>"
			$("#"+e).html(content);
			element.change(function(){
				$("#"+e).html("");
	        });
				
		},
		rules: {
			task: {
				required: true,
				taskid:true
			}
		},
		messages: {
			task: "请输入正确的任务ID"
			
		},
		submitHandler: function() {
			
			if($("#assProp").size() && !propTemp.hobby.length) {
				$('body').messagebox("请至少设置一项行业兴趣关联属性！", '', 0, 3000); 
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
						    	$('body').messagebox(data.message, '', 0, 3000);
						    }   
						}
					}
					
				});
			}
		}
	});
});

function toPropertyIndex(){
	var url = $("#successUrl").val();
	window.location.href = url;
}