$(document).ready(function(){
	
	//切换选择收件人方式
	$("input[name=action]").change(function(){
		if($("input[name=action]:checked").val()=="file"){
			$(".tab_content2").hide();
			$(".tab_content1").show();	
		}else{
			$(".tab_content1").hide();
			$(".tab_content2").show();	
		}
	});
	
	//是否导入到标签
	$("#isImport").change(function(){
		if(this.checked){
			$(this).parents("tr").next().show();
			$("input[name=opt]").val("input");
		}else{
			$(this).parents("tr").next().hide();
			$("input[name=opt]").val("export");
		}
	});
	
	//判断提交
	$("#recipientSubmit").click(function(){
		if($("input[name=action]:checked").val()=="file"){
			importFile();
			$('#import').submit();
		}else{
			exportFile();
			$('#export').submit();
		}
	});
	
	
	$("#igType").click(function(){
		$("#igType").attr("flag","go");
		closeDialog();
		$('#import').submit();
		$("#igType").attr("flag","stop");
	});
	
	$("#modType").click(function(){
		closeDialog();
	});

	$("#exclude").search({
		valueId:"#excludeId",
		boxId:"#slideSearch",
		json:catedata['退订'],
		itemFn:"pickData(this,'#exclude')"
	});

});

//检测模板变量与所选属性
function testProps(){
	if($(".data_prop a")[0] && parameters !=''){
		if($("#igType").attr("flag") == "go"){
			return true;
		}
		//提取文件字段数组
		var paramArr=parameters.split(',');
		var propsArr=[];
		$(".data_prop a").each(function(){
			propsArr.push($(this).text());
		});

		//检测prosArr是否包含paramArr所有项
		for(i=0;i<paramArr.length;i++){
			if(jQuery.inArray(paramArr[i],propsArr) == -1){
				popTips('#confirmProps',"您设置的收件人属性与邮件模板的变量不符");
				return false;		
			}
		}
		return true;
	}
	
	return true;
}

function assignStep3(){
	location.assign("/step/3?id=" + $("input[name=id]").val());
}
