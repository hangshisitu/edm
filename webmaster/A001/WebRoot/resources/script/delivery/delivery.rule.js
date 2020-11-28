$(document).ready(function(){
	props_list();
	
})

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
		alert("设置不能多于" + (total - 1) + "个", 1);
		return;
	}
	
	//添加一个属性
	var text = "";
	text += "<tr><td width='60' align='center'>&nbsp;</td>";
	text += "<td width='150'><a id='prop_list_" + len + "' class='a_select b_select' style='width:100px'>请选择</a>";
	text += "<input type='hidden' class='i_select' name='prop_" + len + "' id='prop_" + len + "'/></td>";
	text += "<td width='460'>";
	text += "每15分钟<input type='text' id='mins_" + len + "' class='add_text' name='mins_" + len + "' /><em>封</em>";
	text += "每小时<input type='text' id='hours_" + len + "' class='add_text' name='hours_" + len + "' /><em>封</em>";
	text += "每天<input type='text' id='days_" + len + "' class='add_text' name='days_" + len + "' /><em>封</em></td>";
	text += "<td><span class='input_set del_set' onclick='del_prop(" + len + ")'>-</span></td></tr>";
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
		var descId=$(this).attr("id").split("_")[0];
		var descName=$(this).attr("name").split("_")[0];
		$(this).attr("id",descId+"_" + index);
		$(this).attr("name",descName+"_" + index);
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
