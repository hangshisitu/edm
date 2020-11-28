$(document).ready(function(){
	prompt();
	//显示提示
	$(".list_select").each(function(){
		var id = $(this).attr("data-id");
		$(this).gtips({
			eType:'click',
			content:function(){
				var flag = 0;
				for(var key in taskData){
					if(key == id){
						flag = 1;
					}
				}
				if(!flag){return '<p>暂无数据！</p>';}
				var	txt = '<h4>邮件设置</h4>';
					txt += '<ul>';
					txt += '<li>任务名称：' + taskData[id].task + '</li>';
					txt += '<li>所属活动：' + taskData[id].campaign + '</li>';
					txt += '<li>邮件模板：' + taskData[id].template + '</li>';
					txt += '<li>邮件主题：' + taskData[id].subject + '</li>';
					txt += '</ul>';
					txt += '<h4>发件人设置</h4>';
					txt += '<ul>';
					txt += '<li>发件人邮箱：' + taskData[id].senderEmail + '</li>';
					txt += '<li>发件人昵称：' + taskData[id].senderName + '</li>';
					txt += '<li>网络发件人：' + taskData[id].robot + '</li>';
					txt += '<li>回复邮箱：' + taskData[id].replier + '</li>';
					txt += '</ul>';
					txt += '<h4>收件人设置</h4>';
					txt += '<ul>';
					txt += '<li>所属标签：' + taskData[id].includes + '</li>';
					txt += '<li>排除退订：' + taskData[id].excludes + '</li>';
					txt += '<li>收件人昵称：' + taskData[id].receiverName + '</li>';
					txt += '<li>默认退订：' + taskData[id].unsubscribe + '</li>';
					txt += '</ul>';
					txt += '<h4>发送设置</h4>';
					txt += '<ul><li>发送方式：' + taskData[id].sendCode + '</li></ul>';		
				return txt;
			}
		});
	});
	
	 //显示审核不通过提示
	$(".J_hook_more").each(function(){
		var txt = $(this).find("p").text();
		$(this).gtips({
			eType:'click',
			content:"<div style='padding:0 10px;max-width:200px'>"+txt+"</div>"
		});
	}); 
	
});

/**
 * 提示
 */
function prompt() {
	var msg = $("#msg").val();
	if(msg != null&&msg!="") {
		$('body').messagebox(msg, '', 0, 2000);
	}
}

/**
 * 搜索
 */
function search() {
	$("#currentPage").val(1);
	$("#ListForm").submit();
}

/**
 * 重置查询参数
 */
function resetParam() {
	$("#search_taskName").val("");
	$("#search_userId").val("");
	$("#d4311").val("");
	$("#d4312").val("");
}

/**
 * 查看
 * @param url
 */
function view(url) {
	//windows.(url,'','590px','800px');
	//$("#ListForm").submit();
	window.open(url, "newwindow", 'height=800px,width=1024px,top=0,left=0,toolbar=no,menubar=no,scrollbars=no, resizable=no,location=no, status=no');
}

/**
 * 审核
 * @param url
 */
function auditPage(url) {
	openWin(url,'','300px','500px');
	$("#ListForm").submit();
}

/**
 * 报告
 * @param url
 */
function detail(url) {
	location.assign(url);
}