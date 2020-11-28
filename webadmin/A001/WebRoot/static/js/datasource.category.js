$(document).ready(function() {
	// 先设为非选中状态
	$(".sort_table input[name=checkedIds]").prop("checked", false);
	// 分类展开效果
	$(document).on("click",".sort_table .J_hook_label",function() {
		var _this = $(this);
		var $sortBox = _this.parents(".sort_box");
		var itemId = $sortBox.find("input").val();
		var $tagBox = $sortBox.next(".tag_box");
		if ( !$tagBox.is(":hidden") ) {		
			$tagBox.slideUp("fast", function() {
				$sortBox.removeClass("sort_select");
			});
		} else {
			$sortBox.addClass("sort_select");
			$tagBox.slideDown("fast");
			var flag = $tagBox.find(".sort_table").size();
			// 先清空
			if (!flag) {
				$tagBox.html("<p class='center'>正在加载标签列表...</p>");
				$.ajax( {
					url: "/datasource/tag/array?categoryId=" + itemId + "&token=" + headers['token'],
					type: "post",
					headers: headers,
					dataType: "json",
					success: function(data) {
						if (data.code == 1) {
							var text = "<table width='100%' cellspacing='0' cellpadding='0' border='0' class='sort_table'>";
							$.each(data.tagMaps, function(i, tag) {
								text += "<tr>";
								text += "<td width='25'></td>";
								if(tag.categoryName == "临时文件"){
									text += "<td>" + tag.tagName + "</td>";
								}else{
									text += "<td><a href='/datasource/recipient/page?action=category&tagId=" + tag.tagId + "'>" + tag.tagName + "</a></td>";
								}
								text += "<td width='20%'>" + tag.tagDesc + "</td>";
								text += "<td width='10%'>" + tag.emailCount + "</td>";
								text += "<td width='12%'>" + tag.createTime + "</td>";
								text += "<td width='12%'>" + tag.modifyTime + "</td>";
								text += "<td class='edit' width='16%'><ul class='f_ul'></ul></td>";
								text += "</tr>";
							})
							text += "</table>";
							$tagBox.html("").append(text);
						} else {
							$tagBox.html("<p class='center'>没有匹配的标签列表</p>");
						}
					},
					error: function() {
						$tagBox.html("<p class='center'>没有匹配的标签列表</p>");
						return false;
					}
				});
			}
		}
	});

	// 分类表格鼠标经过效果
	$(".sort_table .sort_box").on('hover',function() {
		$(this).toggleClass("sort_hover");
	});
});

function del() {
	del_tab("请选择需要删除的类别", 'del', 0);
}
