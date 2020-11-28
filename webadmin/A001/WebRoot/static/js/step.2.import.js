$(document).ready(function(){
	$("#msg_cfm").live('click',function(){
		closeDialog();
	});	
	
	//生成标签列表
	$("#tag").search({
		valueId:"#tagId",
		boxId:"#slideSearch",
		json:catedata,
		itemFn:"pickData(this,'#tag')",
		outCate:'表单收集'
	});
	
	//模板下载
	$("#downloadTemp").UIselect({
		width: 150,
		height: 200,
		align:"right",
		html: "<ul class='center'>" + 
			  "<li style='border-bottom:1px solid #99A9BA'><a hidefocus=\"true\" href=\"/download?t=txt\">txt</a></li>" +
			  "<li style='border-bottom:1px solid #99A9BA'><a hidefocus=\"true\" href=\"/download?t=csv\">csv</a></li>" +
			  "<li><a hidefocus=\"true\" href=\"/download?t=xlsx\">xls/xlsx</a></li>" + 
			  "</ul>"
	});
});

function importFile(){
	$('#import').ajaxForm({
		url: '/step/2?action=file',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json',
		beforeSerialize: function($form, options) {
			loading();
		},
		beforeSubmit: function(formData, jqForm, options) {
			var form = jqForm[0]; 
			if(!$("#isUpload").is(":visible")){
				error_message("请先上传文件", 1);
				del_loading();
				return false; 
			}
			
			if(!testProps()){
				del_loading();
				return false; 
			}
			
			if ($("#isImport").prop("checked") && !form.tagId.value) {
				error_message("请选择所属标签", 1);
				form.tagId.focus();
				del_loading();
				return false; 
			}
		},
		success: function(data) {
			closeDialog();
			del_loading();
			if (data.code == 1) {
				popTips("#remindCustom2","<p>筛选成功：当前共选中了<span class='red'>" + data.emailCount + "</span>个收件人用于投递<span class=\"f666\">（已去重过滤）</span></p>");	
			} else {
				error_message(data.message, 1);
			}
		}   
	});
}

// 生成所有下拉
function import_props_list(){
	len = $("#data_wrap .a_select").size();
	for (var i = 0; i < len; i++) {
		prop_text = "<ul>";
		for (var j = 0; j < props.length; j++) {
			prop_text += "<li><a hidefocus=\"true\" onclick=\"im_repeat(\'#prop_" + i + "\',\'" + props[j][0] + "\','#prop_list_" + i + "',\'" + props[j][1] + "\')\" href=\"javascript:void(0)\">" + props[j][1] + "</a></li>";
		}
		prop_text += "</ul>";
		$("#prop_list_" + i).UIselect({
			width: 130,
			html: prop_text
		});
	}
}

// 去重
function im_repeat(id_hide, val_hide, id_click, val_click){
	var old_val = $(id_hide).val();
	var old_text = $(id_click).text();
	var old_arr = Array(old_val, old_text);
	// 先取出占位数组
	if (old_text !== "" && old_text !== "请选择") {
		props.push(old_arr);
	}
		
	for (var i = 0; i < props.length; i++ ) {
		// 传入的值对应在数组中删除
		if(props[i][1] == val_click && val_click !== "请选择"){
			props.splice(i,1);
		}
	}
	
	for (var i = 0; i < len; i++) {
		prop_text = "<ul>";
		for (var j = 0; j < props.length; j++) {
			prop_text += "<li><a hidefocus=\"true\" onclick=\"im_repeat(\'#prop_" + i + "\',\'" + props[j][0] + "\','#prop_list_" + i + "',\'" + props[j][1] + "\')\" href=\"javascript:void(0)\">" + props[j][1] + "</a></li>";
		}
		prop_text += "</ul>";
		$("#prop_list_" + i).data('edit').html = prop_text;
	}

	// 传入的值放入input
	$(id_click).html(val_click);
	$(id_hide).val(val_hide);
	$(".select_div").attr("id","");
	$(".select_div").hide();
}

function showChar() {
	if(fileData==null){return};
	//编码后的信息
	var obj=($("input[name=charset]").val() == "GB2312"?fileData.GB2312:fileData.UTF8);
	//先清空
	$("#data_wrap ul").html('');
	var text = '<li style="width:100px;">';
		text += '<div class="data_prop">属　　性</div>';
		text += '<div class="data_title">数据标题</div>';
		text += '<div class="data_preview">数据预览</div>';
		text += '</li>';
	//属性列数	
	var len = obj[0].split(",").length;
	//拼接属性下拉
	for(var i = 0; i < len; i++) {
		text += '<li class="item_'+ i +'">';
	    text += '<div class="data_prop">';
	    text += '<a style="width:110px" class="a_select" id="prop_list_'+ i +'">请选择</a>';
	    text += '<input type="hidden" value="" name="prop_'+ i +'" id="prop_'+ i +'" />';
	    text += '<input type="hidden" value="'+ obj[0].split(",")[i] +'" name="parameter_'+ i +'">';
	    text += '</div>'; 
	    text += '<div class="data_title" title="'+ obj[0].split(",")[i] +'">' + obj[0].split(",")[i] + '</div>';
		text += '<div class="data_preview" title="'+ obj[1].split(",")[i] +'">' + obj[1].split(",")[i] + '</div>'; 				
	    text += '</li>';
	}
	//植入html
	$("#data_wrap ul").append(text);
	//生成下拉列表
	import_props_list();
	// 自适应宽度
	var auto_width = $("#data_wrap ul li").size() - 1;
	$("#data_wrap ul").width(auto_width * 172 + 100);
	$(".preview_box").width(auto_width * 172 + 220);
}

function changeCode(){
	var charset = ($("input[name=charset]").val() == "GB2312" ? "UTF-8" : "GB2312");
	$("input[name=charset]").val(charset);
	//这里重新赋值给下拉数组，以免因为已选择数据缺少
	props = orig_props.slice(0);
	showChar();
}

function previewCode(){
	$('#import').ajaxForm({
		url: '/datasource/selection/upload',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json', 
		beforeSubmit: function(formData, jqForm, options) {
			loading(0,".upload_box .load_img_open");
			var form = jqForm[0]; 
			if (!form.file.value) {
				error_message("请选择需要上传的文件",1);
				form.file.focus();
				del_loading(".upload_box .load_img_open");
				return false; 
			}
			if (!suff(form.file.value, ",txt,csv,xls,xlsx,")) {
				error_message("您选择的文件格式不正确", 1);
				form.file.focus();
				del_loading(".upload_box .load_img_open");
				return false; 
			}
		},
		success: function(data) {
			if (data.code == 1) {
				fileData=data;
				//多选模式
				if(data.GB2312[0].split(",").length > 1){
					props=orig_props.slice(0);
					showChar();
					$(".preview_box").show();
				}else{
					$(".preview_box").hide();
				}
				$("#isUpload,#isImportTag,#isExclude").show();
				$("#emailCount1").text(data.emailCount);
				$("#importTips").text('（未去重）');
		
			} else {
				error_message(data.message, 1);
			}
			del_loading(".upload_box .load_img_open");
		},
		error: function(){
			error_message("上传失败", 1);
			del_loading(".upload_box .load_img_open");
		}
	});
}

function importUpload(obj){
	getFilePath(obj);
	previewCode();
	$("#import").submit();
}