$(document).ready(function() {
	$(".trig_links ul").each(function(){
		var $this = $(this);
		if($this.height() > 27){
			$this.height('27px');
			$this.parents('.trig_links').find("span").show();
		}
	});
	
	$(".trig_links span").toggle(function(){
		var $this = $(this);
		$this.parents('.trig_links').find('ul').css({"height":"auto"});
		$this.html('收起&lt;&lt;');
	},function(){
		var $this = $(this);
		$this.parents('.trig_links').find('ul').animate({"height":"27px"},500);
		$this.html('展开所有链接&gt;&gt;');
	});
	
});

/**
 * 导出触发报告
 */
function exportTouch(url) {
	location.assign(url);
}
/**
 * 导出收件人详情
 * @param url
 */
function exportRecipients(url) {
	location.assign(url);
}
/**
 * 预览
 * @param url
 */
function view(url) {
	window.open(url, "newwindow", 'height=600, width=800, top=0, left=0, toolbar=no, menubar=no, scrollbars=no, resizable=no,location=no, status=no');
}