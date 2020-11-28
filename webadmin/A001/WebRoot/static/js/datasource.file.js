$(document).ready(function(){
	//显示提示
	$(".list_select").each(function(){
		var id = $(this).attr("data-id");
		$(this).gtips({
			eType:'click',
			content:function(){
				var flag = 0;
				for(var key in selectionData){
					if(key == id){
						flag = 1;
					}
				}
				if(!flag){return '<p>暂无数据！</p>';}
				
				var	txt = '<h4>包含标签：</h4>';
					txt += '<ul>';
					txt += '<li>' + selectionData[id].includes + '</li>';
					txt += '<li>选取标签下的收件人：' + selectionData[id].activeCnd + '</li>';
					txt += '<li>取标签之间的收件人：' + selectionData[id].inCnd + '</li>';
					txt += '</ul>';
					txt += '<h4>排除标签：</h4>'
					txt += '<ul><li>' + selectionData[id].excludes + '</li></ul>';
					txt += '<h4>过滤器：</h4>';
					txt += '<ul><li>' + selectionData[id].filter + '</li></ul>';
					txt += '<h4>当前收件人总数：<span class="red">' + selectionData[id].emailCount + '</span><span class="f666">（' + selectionData[id].repeat + '）</span></h4>';
				return txt;
			}
		});
	});
});

function del() {
	del_tab("请选择需要删除的文件", 'del', 0);
}
