$(document).ready(function(){
	
	//placeholder
	$(".placehold").placeholder();
	
	//退出系统
	$("#logout").UIselect({
		width:226,
		border:"2px solid #000000",
		html:"<div class=\"clearfix logout_open\"><h1>您确定退出系统吗？</h1><form name=\"logout_from\" method=\"get\" action=\"/logout\"><input type=\"button\" value=\"取消\" onFocus=\"this.blur()\" class=\"input_no f_r\" onclick=\"closeSelectDiv()\" /><input type=\"submit\" value=\"确定\" onFocus=\"this.blur()\" class=\"input_yes f_r\" /></form><div>"
	});
	
	//侧边栏匹配当前
	var url_title = document.title.split("-");
	$("#nav li").each(function(){
		var $this = $(this);
   		if($this.children('a').html()==$.trim(url_title[1]))
		{
			if($this.parents(".subwrap").size()>0){
				//避免重名处理
				var cate = $this.parents(".subwrap").attr('data-cate');
				if(location.href.indexOf(cate) > 0){
					$this.parents(".subwrap").find("ul").show();
					$this.parents(".subwrap").find("a:first").addClass('sublinkDown');
					$this.addClass("now");
				}
			}else{
				$this.find("a:first").addClass('sublinkDown');
				$this.addClass("now");
			}	
		}
    });
    
	//侧边栏伸缩效果
	$("#nav li").click(function(event){
		var $this = $(this);
		if($this.has("ul")){	
			//下拉图标切换
			$this.siblings(".subwrap").find("a:first").removeClass("sublinkDown");
			$this.siblings(".subwrap").find("ul").slideUp('slow');
			$this.find("a:first").toggleClass("sublinkDown");
			$this.find("ul").slideToggle('slow');
		}
		event.stopPropagation();
	});
	
	//查找按钮
	$('.search_top a').click(function() {
	  	$(this).toggleClass("search_hide");
		$(this).toggleClass("search_show");
		$(".search").toggle(500);
	});
	
	//主表格经过变色
	$('.main_table').on('mouseover', 'tr', function() {
	  	$(this).addClass("tr_hover");
	});
	$('.main_table').on('mouseout', 'tr', function() {
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
		}else{
			$(this).parent().parent("tr").removeClass("tr_hover");
		}
	});
	
	//下拉框背景色
	$(".select_div li").live("mouseover", function(){
		$(this).css("background-color","#FFE7A2");
	});
	
	$(".select_div li").live("mouseout", function(){
		$(this).css("background-color","#ffffff");
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
	
	//选中样式
	$(".popBox .search_result ul li").live("click",function(){
		$(this).parents(".popBox").find("li").removeClass("selected");
		$(this).toggleClass("selected");
	});
	
	//定义拖拽
	$(".popBox").drag();
	
	//textarea
	$(".msg_ul li textarea.msg_txt, .desc_area").live("focus",function(){
		$(this).animate({height:"48px"},200);
	})
	
	$(".msg_ul li textarea.msg_txt, .desc_area").live("blur",function(){
		$(this).animate({height:"18px"},200);
	})
	
	//显示套餐信息
	$(".record").hover(
		function(){
			$(this).find(".rec_tips").show(50);
		},
		function(){
			$(this).find(".rec_tips").hide(50);
		}
	);
	
	$('.tips_trg').gtips({
		eType: "mouseover",
		content: function(){
			return this.find(".global_tips").html();
		}
	});
	
	$("input[name=pageNo]").bind("keydown",function(e){
		if(e.keyCode==13){
			$('.go a').trigger("click");
		}
	})

})

window.onload=window.onresize=function(){
	$("#main").height($(window).height()-$("#header").height()-20);
	$("#asist").remove();
	$("#main").append("<div id='asist' style=\"height:20px;\"></div>");
}

//自定义拖拽插件
;(function($) {
    $.fn.extend({
        drag:function(){  
        	return this.each(function(){
        		var _this=$(this);
	        	 var _move=false;//移动标记 
	        	 var _x,_y;         
	        	 $(this).find(".pop_title").live('mousedown',function(e){   
	        	 	 _move=true;     
	        	 	 _x=e.pageX;    
	        	 	 _y=e.pageY;  
	        	 	 return false;
	        	 });   
	        	 
	        	 $(document).bind('mousemove',function(e){    
	        	 	 var diffx = _x - e.pageX;
	        	 	 var diffy = _y - e.pageY;
	        	 	 if(_move) {     
		        	 	 var offset = _this.offset();      
		        	 	 _this.css({left:offset.left - diffx ,top:offset.top - diffy});       
		        	 	 _x=e.pageX;       
		        	 	 _y=e.pageY;     
	        	 	  }  
	        	 	  //这里造成chrome ie8- 文字拖选不上，所以屏蔽
	        	 	  //return false;
	        	 });
	        	 
	        	 $(document).bind('mouseup',function(e){        
	        	 	_move=false;  
	        	 	return false;           
	        	 });        
	        });	  
        } 
    })
})(jQuery);

//定制日历插件
jQuery(function($){
     $.datepicker.regional['zh-CN'] = {
        clearText: '清除',
        clearStatus: '清除已选日期',
        closeText: '关闭',
        closeStatus: '不改变当前选择',
        prevText: '上一月',
        prevStatus: '显示上月',
        prevBigText: '<<',
        prevBigStatus: '显示上一年',
        nextText: '下一月',
        nextStatus: '显示下月',
        nextBigText: '>>',
        nextBigStatus: '显示下一年',
        currentText: '今天',
        currentStatus: '显示本月',
        monthNames: ['一月','二月','三月','四月','五月','六月', '七月','八月','九月','十月','十一月','十二月'],
        monthNamesShort: ['一','二','三','四','五','六', '七','八','九','十','十一','十二'],
        monthStatus: '选择月份',
        yearStatus: '选择年份',
        weekHeader: '周',
        weekStatus: '年内周次',
        dayNames: ['星期日','星期一','星期二','星期三','星期四','星期五','星期六'],
        dayNamesShort: ['周日','周一','周二','周三','周四','周五','周六'],
        dayNamesMin: ['日','一','二','三','四','五','六'],
        dayStatus: '设置 DD 为一周起始',
        dateStatus: '选择 m月 d日, DD',
        dateFormat: 'yy-m-dd',
        firstDay: 1,
        initStatus: '请选择日期',
        isRTL: false};
        $.datepicker.setDefaults($.datepicker.regional['zh-CN']);
});

//占位符插件
; (function($) {
	    $.fn.extend({
	        placeholder: function() {
	            if ('placeholder' in document.createElement('input')) {
	                return this;
	            } else {
	                return this.each(function() {
	                    var _this = $(this),
	                    orig_color = _this.css('color'),
	                    this_placeholder = _this.attr('placeholder'),
	                    init_val = _this.val() || '';
	                    
	                    if(init_val == ''){
	                    	_this.val(this_placeholder);
	                    	_this.css('color','#999');
	                    }
	                          
	                    _this.focus(function() {
	                        if (_this.val() === this_placeholder) {
	                            _this.val('');
	                            _this.css('color',orig_color);
	                        }
	                    }).blur(function() {
	                        if (_this.val().length === 0) {
	                            _this.val(this_placeholder);
	                            _this.css('color','#999');
	                        }
	                    })
	                })
	            }
	        }
	    })
})(jQuery);

//光标定位插件
(function($) { 
	$.fn.extend({ 
		insertContent: function(myValue, t) { 
			var $t = $(this)[0];
			if (document.selection) { //ie 
				this.focus(); 
				var sel = document.selection.createRange(); 
				sel.text = myValue; 
				this.focus(); 
				sel.moveStart('character', -1); 
				var wee = sel.text.length; 
				if (arguments.length == 2) { 
				var l = $t.value.length; 
				sel.moveEnd("character", wee + t); 
				t <= 0 ? sel.moveStart("character", wee - 2 * t - myValue.length) : sel.moveStart("character", wee - t - myValue.length); 
				sel.select();
				} 
			} else if ($t.selectionStart || $t.selectionStart == '0') { 
				var startPos = $t.selectionStart; 
				var endPos = $t.selectionEnd; 
				var scrollTop = $t.scrollTop; 
				$t.value = $t.value.substring(0, startPos) + myValue + $t.value.substring(endPos, $t.value.length); 
				this.focus(); 
				$t.selectionStart = startPos + myValue.length; 
				$t.selectionEnd = startPos + myValue.length; 
				$t.scrollTop = scrollTop; 
				if (arguments.length == 2) { 
				$t.setSelectionRange(startPos - t, $t.selectionEnd + t); 
				this.focus(); 
				} 
			} 
			else { 
				this.value += myValue; 
				this.focus(); 
			} 
		} 
	}) 
})(jQuery); 

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

//加载
function loading(){
	var append_id = arguments[0] ? arguments[0] : document.body;
  	var show_id = arguments[1] ? arguments[1] : ".load_img";
	
	//背景方式显示加载图，loading_bg带背景图
	$(append_id).append("<div class='loading_bg'></div>");
	
	//IE6
	if ($.browser.msie && ($.browser.version == "6.0") && !$.support.style) {
		$(".loading_bg").width($(window).width());
		$(".loading_bg").height($(window).height());
	}
	
	//节点方式显示
	$(show_id).show();
}

//加载结束
function del_loading(){
	var hide_id = arguments[0] ? arguments[0] : ".load_img";
	
	//IE6
	if ($.browser.msie && ($.browser.version == "6.0") && !$.support.style) {
		$(".loading_bg").width(0);
		$(".loading_bg").height(0);
	}
	
	$(".loading_bg").remove();
	$(hide_id).hide();
}

function chkfrm_find(){
	loading(document.body);
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


//下拉框单选，设置分页等
function categoryId_fun(id1,tit1,id2,tit2,ho_fun){
	$(id1).val(tit1);
	$(id2).html(tit2);
	$(".select_div").attr("id","");
	$(".select_div").hide();
	eval(ho_fun);
}

//关闭下拉框
function closeSelectDiv(){
	$(".select_div").attr("id","");
	$(".select_div").hide();
	$(document).unbind('mousedown.select');
}

//打开新建弹出框
function open_msg(cut,type){
	if($("#msg_bg").size()<1)
	{
		$(document.body).append("<div id=\"msg_bg\"  ><div id=\"msg\" class=\"clearfix\"></div></div>");
	}
	
	if ($.browser.msie && ($.browser.version == "6.0") && !$.support.style) {
		$("#msg_bg").width($(window).width());
		$("#msg_bg").height($(window).height());
	}
	if(type==1)
	{
		$("#msg").html(cut);
		$("#msg_bg").setMaxIndex().show();  
		$("#msg").css("top", ( $(window).height() - $("#msg").height() ) / 2+ "px");	
	}
	else
	{
		if(cut.indexOf("?")>0){
			cut=cut+"&rnd="+new Date().getTime();
		}else{
			cut=cut+"?rnd="+new Date().getTime();
		}

		$("#msg").load(cut, function(){
		  	$("#msg_bg").setMaxIndex().show(); 
			$("#msg").css("top", ( $(window).height() - $("#msg").height() ) / 2+ "px"); 
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
	$("#msg").remove();
}

//重设新建窗位置
function reset_msg(){
	$("#msg").css("top", ( $(window).height() - $("#msg").height() ) / 2+ "px");
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
	$("#err_mess").setMaxIndex().fadeIn();
	
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

//选中删除
function del_tab(title, url, type) {
	var checkedIds = "";
	$("input[name=checkedIds]").each(function() {
		if($(this).prop("checked") == true) {
			checkedIds += $(this).val() + ",";
		}
	});
	if (checkedIds == "") {
		error_message(title, 1);
	} else {
		var location = "";
		if (url.indexOf('?') != -1) {
			location = url + "&checkedIds=" + checkedIds;
		} else {
			location = url + "?checkedIds=" + checkedIds;
		}
		open_msg(location, type);
	}

	return false;
}

// 后缀
function suff(path, suffs) {
	var regex = /(\\+)/g;  
	var name = path.replace(regex, "#"); 
	var one = name.split("#");
	var two = one[one.length - 1];
	var three = two.split(".");
	var last = three[three.length - 1];
	var contains = suffs.indexOf(last);
	if (contains >= 0) {
		return true;
	} else {
		return false;
	}
}

//打开遮罩层
function openCover(){
	if($("#msg_bg2").size()<1){
		$(document.body).append("<div id=\"msg_bg2\" ></div>");
	}
	
	var $obj = $("#msg_bg2");
	
	if($.browser.msie && ($.browser.version == "6.0") && !$.support.style) {
		$obj.width($(window).width());
		$obj.height($(window).height());
	} 
	
	$obj.show();
}

//小弹窗打开
function openPop(url,obj,boxEle) {
	var $boxId = boxEle ? $(boxEle) : $(".popBox");
	openCover();
	//跟踪点击对应的input对象
	if(obj){
		$inputObj = $(obj).parents("tr").find(".add_text");
	}
	var winT = $(window).scrollTop();
	var winH = $(window).height();
	var winW = $(window).width();
	if(url){
		if(url.indexOf("?")>0){
			url = url+"&rnd="+new Date().getTime();
		}else{
			url = url+"?rnd="+new Date().getTime();
		}
		url = getEncode(url);
		
		$boxId.load(url, function(){	
			var boxW = $boxId.width();
			var boxH = $boxId.height();
			$boxId.css({'left':(winW-boxW)/2,'top':winT+(winH-boxH)/2});
			$boxId.show();	
		});	
	}else{
		var boxW = $boxId.width();
		var boxH = $boxId.height();
		$boxId.css({'left':(winW-boxW)/2,'top':winT+(winH-boxH)/2});
		$boxId.show();
	}
	
}

//小弹窗关闭
function closeDialog(){
	$(".popBox").hide();
	$("#msg_bg2").remove();
}

//编码
function getEncode(url){
	var arr=url.split("?");
	var innerArr=arr[1].split("&");
	for(i=0;i<innerArr.length;i++){
		var val=innerArr[i].substr(innerArr[i].indexOf("=")+1);
		innerArr[i]=innerArr[i].replace(val,encodeURIComponent(val));
	}
	arr[1]=innerArr.join('&');
	return arr.join('?');
}

//位置obj,下拉框boxId,间距spac,指定水平对齐xdirct
function openSlideBox(obj,boxId,spac,xdirct){
	spac = spac ? spac : 0;
	//判断类型
	var $input = typeof(obj) == 'object' ? (obj instanceof jQuery ? obj : $(obj)) : $(obj);
	var $boxId = typeof(boxId) == 'object' ? (boxId instanceof jQuery ? boxId : $(boxId)) : $(boxId);
	
	var top = $input.position().top + $input.outerHeight();
	var left = $input.position().left;
	var boxHeight = $boxId.outerHeight();
	var boxWidth = $boxId.outerWidth();
	var inputHeight = $input.outerHeight();
	var inputWidth = $input.outerWidth();
    if($(window).height() - top > boxHeight){
    	if(xdirct == 'right'){
    		$boxId.css('left',left - (boxWidth - inputWidth) + 'px');
    	}else{
    		$boxId.css('left',left + 'px');
    	}
		$boxId.css('bottom','auto');
	    $boxId.css('top',top + spac +'px');
	}else{
		if(xdirct == 'right'){
    		$boxId.css('left',left - (boxWidth - inputWidth) + 'px');
    	}else{
    		$boxId.css('left',left + 'px');
    	}
	    $boxId.css('top','auto');
	    $boxId.css('bottom',$(window).height() - top + spac + inputHeight + 'px');
	}	
	$boxId.show();
}

//下拉搜索框单选
function pickData(obj,inputId){
	var val=$(obj).attr("data-id");
	var txt=$(obj).text();
	var new_arr = Array(val, txt);
	$(inputId).val(txt);
	$(inputId).next().val(val);
}

//刷新下拉列表
function reloadSelect(input){
	$(input).trigger("click").trigger("click");
}

//刷新下拉搜索
function reloadSearch(input){
	$(input).trigger("click");
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
	for(var key in parentArr) {
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


//数据提示
function popTips(boxId,message){
	if(message){
		$(boxId+" .al_center").html(message);
	}
	openPop(0,0,boxId);			
}

//获取上传文件路径
function getFilePath(obj){
	document.getElementById('file_txt').value=obj.value.replace(/.*\\/i,'');
}

//验证数组是否为空，或者子项都为0
function isEmptyArr(arr){
	for(i=0;i<arr.length;i++){
		if(arr[i] != 0){
			return false;
		}
	}
	return true;
}

//多选
function pickArr(obj,inputId,callFn){
	var parentArr = $(inputId).data('subArr');
	var val = $(obj).data("id");
	var txt = $(obj).text();
	var new_arr = Array(val, txt);
	filterArr(parentArr,new_arr);
	addItemInput(parentArr,$(inputId));
	if(callFn){
		callFn();
	}
}

//去除多选
function reduceArr(e,fn){
	var subArr = $(this).data('subArr');
	if(e.keyCode == 37 || e.keyCode == 38 || e.keyCode == 39 || e.keyCode == 40){
		return;
	}
	
	if(e.keyCode==8){
		var item=getDeleteItem(this);
		removeArrItem(subArr,item);
		addItemInput(subArr,$(this));
		if(typeof fn != 'undefined'){
			fn();
		}
	}
	e.preventDefault();
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

//查看数组是否重复
function isRepeat(array) {
	var hash = {};
	for(var i in array) {
		if(hash[array[i]]) {
			return true;
		}
		hash[array[i]] = true;
	}
	return false;
}

