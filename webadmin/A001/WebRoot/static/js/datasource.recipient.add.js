$(document).ready(function() {
	props_list();
	
	//自定义信息打开
	$('#customMore').click(function() {
		$(this).toggleClass("swtpand");
		$(this).toggleClass("swtclose");
		$("#props").toggle();
	}); 
	
	$('#add').ajaxForm({
		url: 'add',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json',
		beforeSerialize: function($form, options) {
			loading();
		},
		beforeSubmit: function(formData, jqForm, options) {
			var form = jqForm[0]; 
			if (!form.email.value) {
				error_message("请输入邮件地址", 1);
				form.email.focus();
				del_loading();
				return false; 
			}
			if (!form.tagId.value) {
				error_message("请选择所属标签", 1);
				del_loading();
				return false; 
			}
		},
		success: function(data) {
			del_loading();
			if (data.code == 1) {
				error_message(data.message, 0, 0, 1000, "/datasource/recipient/page?tagId=" + $("#tagId").val());
			} else {
				error_message(data.message,1);
			} 
		}   
	}); 
	
	//生成标签列表
	$("#tag").search({
		valueId:"#tagId",
		boxId:"#slideSearch",
		json:catedata,
		itemFn:"pickData(this,'#tag')"
	});
});


var len = 0;   //初始化时就有的长度
var total = 0; //所有属性的长度，porps会减去初始化的长度
// 初始化自定义信息下拉
function props_list() {
	len = $("#props .a_select").size();
	total = len + props.length;
	for (var i = 0; i < len; i++) {
		prop_text = "<ul>";
		for (var j = 0; j < props.length; j++) {
			prop_text += "<li><a hidefocus=\"true\" onclick=\"repeat('#prop_" + i + "','" + props[j][0] + "','#prop_list_" + i + "','" + props[j][1] + "')\" href=\"javascript:void(0)\">" + props[j][1] + "</a></li>";
		}
		prop_text += "</ul>";
		$("#prop_list_" + i).UIselect({
			width: 120,
			html: prop_text
		});
	}
}

// 自定义信息下拉去重
function repeat(id_hide, val_hide, id_click, val_click) {
	var old_val = $(id_hide).val();
	var old_text = $(id_click).text();
	var old_arr = Array(old_val, old_text);
	
	if (old_text != "" && old_text != "请选择") {
		props.push(old_arr);
	}
		
	for (var i = 0; i < props.length; i++) {
		if (props[i][1] == val_click && val_click != "请选择"){
			props.splice(i,1);
		}
	}

	for (var i = 0; i < len; i++) {
		prop_text = "<ul>";
		for (var j = 0; j < props.length; j++) {
			prop_text += "<li><a hidefocus=\"true\" onclick=\"repeat('#prop_" + i + "','" + props[j][0] + "','#prop_list_" + i + "','" + props[j][1] + "')\" href=\"javascript:void(0)\">" + props[j][1] + "</a></li>";
		}
		prop_text += "</ul>";
		$("#prop_list_" + i).data('edit').html = prop_text;
	}
	$(id_click).html(val_click);
	$(id_hide).val(val_hide);
	$(".select_div").attr("id", "");
	$(".select_div").hide();
}

// 添加自定义信息
function add_prop() {
	//最多添加量
	if (len + 1 >= total){
		error_message("自定义信息不能多于" + (total - 1) + "个", 1);
		return;
	}
	
	//添加一个属性
	var text = "";
	text += "<tr><td width='150' align='right'><a id='prop_list_" + len + "' class='a_select' style='width:100px'>请选择</a>";
	text += "<input type='hidden' class='i_select' name='prop_" + len + "' id='prop_" + len + "' value='' /></td>";
	text += "<td width='10' class='red'></td>";
	text += "<td width='370'><input type='text' id='parameter_" + len + "' class='add_text' name='parameter_" + len + "' /></td>";
	text += "<td><span class='del_set' title='删除' onclick='del_prop(" + len + ")'></span></td></tr>";
	$("#props").append(text);
	
	//循环下拉
	prop_text = "<ul>";
	for (var i = 0; i < props.length; i++) {
		prop_text += "<li><a hidefocus=\"true\" onclick=\"repeat('#prop_" + len + "','" + props[i][0] + "','#prop_list_" + len + "','" + props[i][1] + "')\" href=\"javascript:void(0)\">" + props[i][1] + "</a></li>";
	}
	prop_text += "</ul>";
	$("#prop_list_" + len).UIselect({
		width: 120,
		html: prop_text
	});
	
	//已生成的input长度累加
	len++;
}

// 删除自定义信息
function del_prop(id){
	var prop = "#prop_" + String(id);
	var prop_list = "#prop_" + "list_" + String(id);
	var old_val = $(prop).val();
	var old_text = $(prop_list).text();
	var old_arr = Array(old_val, old_text);
	
	//键值回归数组
	if (old_text != "" && old_text != "请选择") {
		props.push(old_arr);
	}
	
	len--;
	
	$(prop).parents("tr").remove();
	
	//重新排序
	$("#props .a_select").each(function(){
		var index = $(this).parents("tr").index();
		$(this).attr("id","prop_list_" + index);
	})
	$("#props .i_select").each(function(){
		var index = $(this).parents("tr").index();
		$(this).attr("id","prop_" + index);
		$(this).attr("name","prop_" + index);
	})
	$("#props .add_text").each(function(){
		var index = $(this).parents("tr").index();
		$(this).attr("id","parameter_" + index);
		$(this).attr("name","parameter_" + index);
	})
	$("#props .del_set").each(function(){
		var index = $(this).parents("tr").index();
		$(this).attr("onclick","del_prop(" + index + ")");
	})
	
	//重新生成下拉
	for (var i = 0; i < len; i++) {
		prop_text = "<ul>";
		for (var j = 0; j < props.length; j++) {
			prop_text += "<li><a hidefocus=\"true\" onclick=\"repeat('#prop_" + i + "','" + props[j][0] + "','#prop_list_" + i + "','" + props[j][1] + "')\" href=\"javascript:void(0)\">" + props[j][1] + "</a></li>";
		}
		prop_text += "</ul>";
		$("#prop_list_" + i).data('edit').html = prop_text;
	}
}



