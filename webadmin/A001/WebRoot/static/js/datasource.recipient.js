$(document).ready(function() {
	$("#_ids").UIselect({
		width: 73,
		id_id: "#_id",
		arrdata: _ids,
		ho_fun: "go()"
	});
	
	$(".tab li").click(function(){
		$(this).siblings().removeClass("now");
		$(this).addClass("now");
		$(".tab_content").hide().eq($(this).index()).show();
	});
	
	//显示提示
	$(".list_select").each(function(){
		var id = $(this).attr("data-id");
		var e = $(this).attr("title");
		$(this).gtips({
			eType:'click',
			ajaxFn:function(){
				var _this = this;
				_this.ajaxGet = $.ajax({
					url: "prop?tagId=" + id + "&email=" + e + "&token=" + headers['token'],
					type: "post",
					headers: headers,
					dataType: "json",
					success: function(data) {
						var newarr=[];
						$.each(data, function(k, v) {
							if(k){
								newarr.push(k + "&nbsp;等于&nbsp;" + v);
							}				
						});
						//放入一个数组循环
						if(newarr.length){
							var filter_text = "<ul>";
							
							for(k=0;k<newarr.length;k++){
								filter_text += "<li>" + newarr[k] + "</li>";
							}
							
							filter_text += "</ul>";
							//更改内容
							_this.setData(filter_text);
							_this.setHtml(filter_text);
						}else{
							_this.setHtml('<p>暂无数据！</p>');
						}
						
					},
					error:function(){
						_this.setHtml('<p>加载失败！</p>');
					}
				});
			}
		});
	});
	
});


function go() {
	location.assign($("#_id").val());
}

function del(action) {
	del_tab("请选择需要删除的收件人", 'del?action=' + action, 0);
}
