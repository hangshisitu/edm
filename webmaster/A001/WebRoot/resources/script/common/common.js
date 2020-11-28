
var ctxPath = $(".ctx");

var gtimer=0;
$(document).ready(function(){
	//保持满屏最小高度
	$("#main").height($(window).height()-$("#header").height()-40);
	$("#main").append("<div style=\"height:20px;\"></div>");
	
	//侧边栏伸缩效果
	$("#nav li").click(function(event){
		var $this = $(this);
		//li包含ul时
		if($this.hasClass("subwrap")){	
			$this.siblings(".subwrap").find("a:first").removeClass("sublinkDown");
			$this.siblings(".subwrap").find("ul").slideUp('slow');
			$this.find("a:first").toggleClass("sublinkDown");
			$this.find("ul").slideToggle('slow');	
		}else{
			if(!$this.parents(".subwrap").length){
				$(".subwrap").find("a:first").removeClass("sublinkDown");
				$(".subwrap").find("ul").slideUp('slow');
			}
			$("#nav li").removeClass("now");
			$this.addClass("now");
		}
		
		event.stopPropagation();
	});
	
	//下拉框背景色
	$(".select_div li").live("mouseover", function(){
		$(this).css("background-color","#FFE7A2");
	});
	
	$(".select_div li").live("mouseout", function(){
		$(this).css("background-color","#ffffff");
	});
		
	//查找按钮
	$('.search_top a').click(function() {
	  	$(this).toggleClass("search_hide");
		$(this).toggleClass("search_show");
		$(".search").toggle(500);
	});
	
	//主表格经过变色
	$('.main_table tr').live('mouseover', function() {
	  	$(this).addClass("tr_hover");
	});
	$('.main_table tr').live('mouseout', function() {
		if($(this).find("input[name=checkedIds]").attr("checked")!=="checked")
		{
			$(this).removeClass("tr_hover");
		}
	});
	
	//全选
	$( "#all_checkbox" ).click(function(){
		if(this.checked){ 
			$("input[name='checkedIds']").each(function(){
				this.checked=true;
				$(this).parent().parent("tr").addClass("tr_hover");
			}); 
		}else{ 
			$("input[name='checkedIds']").each(function(){
				this.checked=false;
				$(this).parent().parent("tr").removeClass("tr_hover");
			}); 
		} 
	});
	
	//主表格选中一行
	$('input[name=checkedIds]').click(function(){
		if($(this).attr("checked")=="checked"){
			$(this).parent().parent("tr").addClass("tr_hover");
		}
		else
		{
			$(this).parent().parent("tr").removeClass("tr_hover");
		}
	});
	
	//关闭小弹窗
	$(".pop_close").live("click",function(){
		closeDialog();
	});
	
	//树形展开
	$(".search_sort_title .icon_expand").live("click",function(){
		$(this).toggleClass("icon_close");
		$(this).parent().next("ul").slideToggle("fast");
	});
	
	//搜索列表划过样式
	$(".search_result ul li").live("mouseover",function(){
		$(this).addClass("hover");
	});
	
	$(".search_result ul li").live("mouseout",function(){
		$(this).removeClass("hover");
	});
	
	//高亮提示
	$('.tips_trg').gtips({
		eType: "mouseover",
		content: function(){
			return this.find(".global_tips").html();
		}
	});
	
	//下拉提示
	$('.slide_trg').hover(function() {
		openTips($(this),$('.slide_info',$(this)));
		},function(){
			clearTimeout(gtimer);
			$('.slide_info',$(this)).hide();
		}
	); 
	
	//enter键提交表单
	if($('#ListForm').length){
		$(document).on('keyup',function(e){
			if(e.keyCode == 13){
				$("#currentPage").val(1);
				$('#ListForm').submit();
			}
		});
	}
	
});

$(window).resize(function(){
	//保持满屏最小高度
	$("#main").height($(window).height()-$("#header").height()-40);
});

//设置最大层级
;(function($) {
    $.fn.extend({
        setMaxIndex:function(){  
        	__maxIndex = typeof(__maxIndex) != "undefined"? __maxIndex++ : 10000000;
        	return this.each(function(){
        		var _this = $(this);
			    _this.css("zIndex",__maxIndex++);      
	        });	  
        } 
    })
})(jQuery);


/**
 * 打开模态窗口
 * @param url		url
 * @param obj		向对话框传递的参数
 * @param height	高度
 * @param width		宽度
 * @returns
 */
function openWin(url,obj,height,width) {
	if(navigator.userAgent.indexOf("Chrome") >0 ){
		 var top = document.documentElement.clientHeight/2 - parseInt(height)/2;
		 var left = document.documentElement.clientWidth/2 - parseInt(width)/2;
	     return  window.open(url,window,"height="+height+",width="+width+",top="+top+",left="+left+",toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,fullscreen=0");
	}
	else{
	    //return window.showModalDialog(url, window, 'dialogHeight: ' + height + 'px; dialogWidth: ' + width + 'px;edge: Raised; center: Yes; help: Yes;scroll:no; resizable: no; status: no;resizable:yes');
		return window.showModalDialog(url,obj,"dialogWidth="+width+";dialogHeight="+height+";status=no;resizable=no");
	}
}


/**
 * 关闭窗口
 */
function closeWin() {
	window.close();
}

/**
 * 限制输入字数
 * @param field
 * @param maxlimit
 */
function textCounter(field, maxlimit) { 
	var temp;
	var len = maxlimit; 
	for (i=0;i<field.value.length;i++) { 
		//如果是两位字符
		if (field.value.charCodeAt(i)>50) {
			len-=2; 
		} else {
			len--; 
		}
		
		//减完小于0，则说明只能这么长
		if (len < 0) {
			temp = i;
			field.value = field.value.substring(0,temp);
			alert("输入字符超长,不可再输入");
		}
  	}
	if(len >= 0){
		//document.form1.strLen.value = len; 		
	} 		
}

//判断值是否为空
function isEmpty(value){
	var flag = false;
	if(value.replace(/\s/g,"") == "") {
		flag = true;
	}
	return flag;
}

//打开遮罩层
function openCover(){
	if($("#msg_bg2").size()<1)
	{
		$(document.body).append("<div id=\"msg_bg2\" ></div>");
	}
	
	$("#msg_bg2").width($(window).width());
	$("#msg_bg2").height($(window).height());

	$("#msg_bg2").show();
}


//小弹窗打开
function openPop(url,obj,boxEle) {
	var boxId=boxEle?boxEle:".popBox";
	openCover();
	//跟踪点击对应的input对象
	if(obj){
		$inputObj=$(obj).parents("tr").find(".add_text");
	}
	var winT = $(window).scrollTop();
	var winH = $(window).height();
	var winW = $(window).width();
	if(url){
		if(url.indexOf("?")>0){
			url=url+"&rnd="+new Date().getTime();
		}else{
			url=url+"?rnd="+new Date().getTime();
		}
		url=getEncode(url);
		
		$(boxId).load(url, function(){	
			var boxW = $(boxId).width();
			var boxH = $(boxId).height();
			$(boxId).css({'left':(winW-boxW)/2,'top':winT+(winH-boxH)/2});
			$(boxId).show();	
		});	
	}else{
		var boxW = $(boxId).width();
		var boxH = $(boxId).height();
		$(boxId).css({'left':(winW-boxW)/2,'top':winT+(winH-boxH)/2});
		$(boxId).show();
	}
	
}

function closeDialog(){
	$(".popBox").hide();
	$("#msg_bg2").remove();
}

//数据提示
function popTips(boxId,title,tips,opShow){
	$(boxId+" .pop_title").html(title);
	$(boxId+" .al_center").html('').append(tips);
	if(opShow){
		$(boxId+" .al_submit").show();
	}else{
		$(boxId+" .al_submit").hide();
	}
	openPop(0,0,boxId);			
}

//验证子元素
function isParent(obj,parentObj){
    while (obj != undefined && obj != null && obj.tagName!= 'BODY'){
        $(window).error(function(){
		  return false;
		});
		if (obj == parentObj){
            return true;
        }
        obj = obj.parentNode;
    }
    return false;
}

//jquery验证子元素
function isShow(target, box, source){
	//点击元素，包含元素，事件源
    return target.closest(box).length || target.get(0) === source.get(0);  
}

var t_time;
/*
 * 功能：操作信息提示
 * mess 信息
 * bg 类型背景
 * top_num 距顶位置
 * time 隐藏提示延后时间
 * url 重定位地址
 */
function error_message(mess,bg,top_num,time,url){
	clearTimeout(t_time);
	if($('#err_mess').size()>0){
		$('#err_mess').remove();
	}
	//分类显示
	if(bg==1)
	{
		//错误
		$(document.body).append("<div id=\'err_mess\'><div class=\'err2\'></div></div>");
	}
	else if(bg==10)
	{
		//加载
		$(document.body).append("<div id=\'err_mess\'><div class=\'err10\'></div></div>");
	}
	else
	{
		//默认成功
		$(document.body).append("<div id=\'err_mess\'><div class=\'err1\'></div></div>");
	}
	$("#err_mess div").html(mess);
	
	//设置距离屏幕顶端位置，不传参默认显示居中
	if(top_num>0){
		$("#err_mess").css("top",top_num+ "px");   
	}
	else
	{
		$("#err_mess").css("top", ( $(window).height() - $("#err_mess").height() ) / 2+$(window).scrollTop() + "px");   
	}
	
	//设置距离屏幕左端位置显示居中
	$("#err_mess").css("left", ( $(window).width() - $("#err_mess").width() ) / 2+$(window).scrollLeft() + "px"); 
	$("#err_mess").fadeIn();
	
	//延后刷新
	if(time>0)
	{
		t_time=setTimeout("location.assign('"+url+"')",time);
	}
	else if(bg==10)
	{
	}
	else
	{
		t_time=setTimeout("err_mess_div('#err_mess',800)",2500);
	}
	
}

//隐藏操作信息提示框
function err_mess_div(id,hidetime){
	$(id).fadeOut(hidetime);
}

//下拉框单选，设置分页等
function categoryId_fun(id1,tit1,id2,tit2,ho_fun){
	$(id1).val(tit1);
	$(id2).html(tit2);
	$(".select_div").attr("id","");
	$(".select_div").hide();
	eval(ho_fun);
}

//插入到json
function insertJsonItem(json,item,keyString){
	for(var key in json){
		if(key==keyString){
			catedata[key].unshift(item);
		}
	}
}

//通过title删除数组项
function removeArrItem(arr,title){
	for (var i=0;i<arr.length;i++) {
		if(arr[i][1]==title){
			arr.splice(i,1);
		}
	}
}

//获取删除项
function getDeleteItem(textarea){
	var position=getCursorPosition(textarea).start;
	var leftStr=textarea.value.substr(0,position);
	var rightStr=textarea.value.substr(position);
	var itemStr="";
	if(!/,/i.test(leftStr)){
		if(!/,/i.test(rightStr)){
			itemStr=leftStr+rightStr;
		}else{
			itemStr=leftStr+rightStr.substr(0,rightStr.indexOf(','));
		}
	}else{
		if(!/,/i.test(rightStr)){
			itemStr=leftStr.substr(leftStr.lastIndexOf(','))+rightStr;
			itemStr=itemStr.replace(', ','');
		}else{
			itemStr=leftStr.substr(leftStr.lastIndexOf(','))+rightStr.substr(0,rightStr.indexOf(','));
			itemStr=itemStr.replace(', ','');
		}
	}
	return itemStr;
}

//获取光标
function getCursorPosition(textarea) {
    var rangeData = {text: "", start: 0, end: 0 };
        textarea.focus();
    if (textarea.setSelectionRange) { // W3C
        rangeData.start= textarea.selectionStart;
        rangeData.end = textarea.selectionEnd;
        rangeData.text = (rangeData.start != rangeData.end) ? textarea.value.substring(rangeData.start, rangeData.end): "";
    } else if (document.selection) { // IE
        var i,
        oS = document.selection.createRange(),
        // Don't: oR = textarea.createTextRange()
        oR = document.body.createTextRange();
        oR.moveToElementText(textarea);

        rangeData.text = oS.text;
        rangeData.bookmark = oS.getBookmark();

        // object.moveStart(sUnit [, iCount])
        // Return Value: Integer that returns the number of units moved.
        for (i = 0; oR.compareEndPoints('StartToStart', oS) < 0 && oS.moveStart("character", -1) !== 0; i ++) {
            // Why? You can alert(textarea.value.length)
            if (textarea.value.charAt(i) == '\n') {
                i ++;
            }
        }
        rangeData.start = i;
        rangeData.end = rangeData.text.length + rangeData.start;
    }

    return rangeData;  
}

//二维数组去重
function filterArr(parentArr,new_arr){
	for (var i = 0; i < parentArr.length; i++) {
		if (parentArr[i][0] ==new_arr[0]){
			parentArr.splice(i,1);
			parentArr.push(new_arr);
			return;
		}
	}
	parentArr.push(new_arr);
}

//添加值input
function addItemInput(parentArr,$input){
	var valArr=[];
	var txtArr=[];
	for (var key in parentArr) {
		valArr.push(parentArr[key][0]);
		txtArr.push(parentArr[key][1]);
	}
	$input.val(txtArr.join(", "));
	
	if($input[0].scrollHeight > parseInt($input[0].style.height)){
		$input[0].style.height = $input[0].scrollHeight + 'px';
		$.fn.search.posfix($input,$input.data('boxId'));
	}
	if(!$input.val()){
		$input[0].style.height="28px";
		$.fn.search.posfix($input,$input.data('boxId'));
	}
	$input.next().val(valArr.join(","));
}

//验证email
function isEmail(strEmail) { 
	if (strEmail.search(/^\w+((-\w+)|(\.\w+))*\@[A-Za-z0-9]+((\.|-)[A-Za-z0-9]+)*\.[A-Za-z0-9]+$/) != -1) 
		return true; 
	else 
		return false; 
} 


function openUrl(url) {
	window.location.href=url;
}

//刷新总数
function reloadPersonSum(json,enArr,outArr,targetObj){
	var personSum=0;
	for(var key in json){
		var obj=json[key];
		for(i=0;i<obj.length;i++){
			//包含标签
			for(k=0;k<enArr.length;k++){
				if(enArr[k][0]==obj[i].id){
					personSum+=parseInt(obj[i].count);
				}
			}
			//排除标签
			for(j=0;j<outArr.length;j++){
				for(k=0;k<enArr.length;k++){
					if(outArr[j][0]==enArr[k][0]){
						if(outArr[j][0]==obj[i].id){
							personSum-=parseInt(obj[i].count);
						}
					}
				}
			}
		}
	}
	$(targetObj).html(personSum);
}

//处理属性所选
function addAssProp(obj,inputId,itemArr,fn){
	var tagLimit = 10;
	var cate_info = $(obj).attr("data-info");
	var val = $(obj).attr("data-id");
	var txt = $(obj).text();
	//已选的话就删除，没有选中就添加
	if($(obj).parents('li').hasClass("selected")){ 
		removeArrItem(itemArr,txt);
	}else{
		if(itemArr.length >= tagLimit){
			error_message('选择标签数不能大于' + tagLimit + '个！',1);
			return;
		}
		
		if(cate_info == "行业兴趣"){
			var new_arr = Array(val,txt,cate_info);
			filterArr(itemArr,new_arr);	
		}else{
			var new_arr = Array(val,txt);
			for(i=0;i<itemArr.length;i++){
				var id = itemArr[i][0];
				if($("a[data-id="+id+"]").attr("data-info") == cate_info){
					itemArr.splice(i,1);
				}
			}
			filterArr(itemArr,new_arr);	
		}
	}		
	addItemInput(itemArr,$(inputId));
	addPropSelect(inputId);
	if(typeof fn != 'undefined'){
		fn();
	}
}


//添加已选样式
function addPropSelect(inputId){
	$("#slideSearchProp ul li").removeClass("selected");
	var ids = $(inputId).next().val();
	if(ids == ""){return;}
	var arr = ids.split(',');
	if(arr.length < 1){return}
	for(i=0;i<arr.length;i++){	
		$("#slideSearchProp ul li a[data-id='" + arr[i] + "']").parents('li').addClass('selected');
	}	
}

//回调函数
function callReloadSum(){
	reloadPersonSum(ass_prop,enTagVal,outTagVal,"#personSum");
}

//删除属性
function removeProp(itemArr,item,inputId,fn){
	removeArrItem(itemArr,item);
	addItemInput(itemArr,$(inputId));
	addPropSelect(inputId);
	if(typeof fn != 'undefined'){
		fn();
	}
}

//指定数组下标查找某项
function findArrItem(arr,subindex,txt){
	for(i=0;i<arr.length;i++){
		if(arr[i][subindex] == txt){
			return i;
		}
	}
	return -1;
}

//打开新建弹出框
function open_msg(cut,type){
	if($("#msg_bg").size()<1)
	{
		$(document.body).append("<div id=\"msg_bg\"><div id=\"newPad\" class=\"popBox\"></div></div>");
	}
	
	if ($.browser.msie && ($.browser.version == "6.0") && !$.support.style) {
		$("#msg_bg").width($(window).width());
		$("#msg_bg").height($(window).height());
	}
	if(type==1)
	{
		$("#newPad").html(cut);
		$("#msg_bg").show();  
		$("#newPad").css("top", ( $(window).height() - $("#newPad").height() ) / 2+ "px");	
		$("#newPad").css("left", ( $(window).width() - $("#newPad").width() ) / 2+ "px"); 
	}
	else
	{
		if(cut.indexOf("?")>0){
			cut=cut+"&rnd="+new Date().getTime();
		}else{
			cut=cut+"?rnd="+new Date().getTime();
		}

		$("#newPad").load(cut, function(){
		  	$("#msg_bg").show(); 
			$("#newPad").css("top", ( $(window).height() - $("#newPad").height() ) / 2+ "px"); 
			$("#newPad").css("left", ( $(window).width() - $("#newPad").width() ) / 2+ "px"); 
		 });
	}
}

//新建类弹出窗销毁
function close_msg(){
	if ($.browser.msie && ($.browser.version == "6.0") && !$.support.style) {
		$("#msg_bg").width(0);
		$("#msg_bg").height(0);
	}
	$("#msg_bg").remove();
	$("#newPad").remove();
}

//选中删除
function del_tab(title, url, type) {
	var checkedIds = "";
	$("input[name=checkedIds]").each(function() {
		if($(this).prop("checked") == true) {
			checkedIds += $(this).val() + ",";
		}
	});
	if(checkedIds == "") {
		error_message(title, 1);
	} else {
		var location = "";
		if(url.indexOf('?') != -1) {
			location = url + "&checkedIds=" + checkedIds;
		} else {
			location = url + "?checkedIds=" + checkedIds;
		}
		open_msg(location, type);
	}
	return false;
}


//默认网络发件人
var robot_index = 0;
// 增加网络发件人
function addRobot() {
	if($(".count_spoke").size()>=10){
		 $('body').messagebox("网络发件人不能超过10个", '', 0, 2000);
		return;
	}
	
	robot_index = $(".count_spoke").size() || 0;
	//robot_index += 1;
	var html = "";
	html += "<div class='robot'>";
	html += "<input type=\"text\" id=\"robot_" + robot_index + "\" name=\"robots\" class=\"input_text count_spoke\" value=\"\" maxlength='64' />";
	html += " <span class=\"input_set del_set\" onclick=\"delRobot(" + robot_index + ")\">-</span></div>";
	$(".count_spoke:last").parents(".robot").after(html);
}

// 删除网络发件人
function delRobot(id){
	var tab = "#robot_" + String(id);
	$(tab).parents(".robot").remove();
}

//加载开始，第二个参数img容器
function loading(){
	var append_id = arguments[0] ? arguments[0] : document.body;
	if(arguments[1]){
		var show_id = arguments[1];
		//节点方式显示
		$(show_id).show();
	}else{
		//背景方式显示加载图，loading_bg带背景图
		$(append_id).append("<div class='loading_bg'><div class='loading_img'></div></div>");
	}	
}

//加载结束
function del_loading(){
	if(arguments[0]){
		var hide_id = arguments[0];
		$(hide_id).hide();
	}else{
		$(".loading_bg").remove();
	}	
}