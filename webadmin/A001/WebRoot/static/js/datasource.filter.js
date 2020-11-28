$(document).ready(function() {
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
			if (!form.filterName.value) {
				error_message("请输入过滤器名称", 1);
				form.filterName.focus();
				del_loading();
				return false; 
			}
		},
		success: function(data) {
			del_loading();
			if (data.code == 1) {
				error_message(data.message, 0, 0, 1000, "/datasource/filter/page");
			} else {
				error_message(data.message, 1);
			} 
		}   
	});
	
	$(".list_select").each(function(i) {
		var id = $(this).attr("data-id");
		$(this).gtips({
			eType:'click',
			content:function(){
				var flag = 0;
				for(var key in filter_list){
					if(filter_list[key][0] == id){
						flag = 1;
						var arr = filter_list[key][1].split("；");
					}
				}
				if(!flag){return '<p>暂无数据！</p>';}
				
				var txt = "<ul>";
				for (var j = 0; j < arr.length; j++) {
					txt += "<li>" + (arr[j]).toString() + "</li>";			
				}
				txt += "</ul>";
				return txt;
			}
		});
	});
});

// 增加过滤机制
function add_filter() {
	index++;
	var num = $("#filter input[readonly]").length;
	if(num >=8 ){
		error_message("过滤机制不能超过8个", 1);
		return;
	}
	var text = "";
	text += "<tr><td align=\"right\">&nbsp;</td><td class=\"red\">&nbsp;</td>";
	text += "<td>";
	text += "<input type=\"text\" id=\"key_" + index + "\" class=\"add_text\" value=\"\" readonly=\"readonly\" />";
	text += "<input type=\"hidden\" id=\"val_" + index + "\" class=\"add_text\" name=\"prop_" + index + "\" value=\"\" />";
	text += "</td>";
	text += "<td><span class=\"input_set\" onclick=\"$('#pos').val('" + index + "');openPop('setting?prop=' + $('#val_" + index + "').val(), this)\">设置</span>";
	text += "<span class=\"del_set\" title=\"删除\" onclick=\"del_filter(" + index + ")\"></span></td></tr>";
	$("#filter").append(text);
}

// 删除过滤机制
function del_filter(id){
	var tab = "#key_" + String(id);
	$(tab).parents("tr").remove();
	
}

$(".edit_tab .input_txt").live('focus', function() {
	$(this).parents("tr").find("input.df_input").prop("checked",true);
})

function del() {
	del_tab("请选择需要删除的过滤器", 'del', 0);
}

