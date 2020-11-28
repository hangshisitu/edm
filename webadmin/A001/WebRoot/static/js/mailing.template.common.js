$(document).ready(function(){
	if(eModel=="WYSIWYG"){
		createEditor();
	}
	if(eModelPhone=="WYSIWYG"){		
		createEditor_phone();
	}	
		
	$("input[name=pageHeader]").click(function() {	
		var header = ""; 
		header += "<div id=\"page_header\" style=\"font:12px/1.5 Consolas, Monaco, Bitstream Vera Sans Mono, Courier New, Courier, monospace;text-align:center;\">";
		header += "如果您想分享此邮件，请点击&nbsp;<a href=\"" + appUrl + "/forward\" target=\"_blank\">转发给好友</a>";
		header += "<br />";
		header += "如果您不想再接收此类邮件，请点击&nbsp;<a href=\"" + appUrl + "/unsubscribe\" target=\"_blank\">退订</a>";
		header += "</div>";
		
		if(eModel == "WYSIWYG"){
			if($("input[name=pageHeader]").is(":checked")){
				var k = $($('.tabLi').eq(0).find('iframe')[0].contentWindow.document.body);
				k.prepend(header);
			}else{
				var k = $($('.tabLi').eq(0).find('iframe')[0].contentWindow.document.body);
				k.find("#page_header").remove();
			}
		}else{
			if($("input[name=pageHeader]").is(":checked")){
				    addHeader(header);
			}else{
				    delHeader(header);
			}
		}
		
	});
		
	$("input[name=pageFooter]").click(function() {
		var footer = "";
		footer += "<div id=\"page_footer\" style=\"font:12px/1.5 Consolas, Monaco, Bitstream Vera Sans Mono, Courier New, Courier, monospace;text-align:center;\">";
		footer += "如果您觉得此封邮件给您带来了不便，您还可以 <a href=\"" + appUrl + "/complain\" target=\"_blank\">投诉</a>";
		footer += "</div>";	
		if(eModel == "WYSIWYG"){
			if($("input[name=pageFooter]").is(":checked")){
				var k = $($('.tabLi').eq(0).find('iframe')[0].contentWindow.document.body);
				k.append(footer);
			}else{
				var k = $($('.tabLi').eq(0).find('iframe')[0].contentWindow.document.body);
				k.find("#page_footer").remove();
			}
		}else{
			if($("input[name=pageFooter]").is(":checked")){
				    addFooter(footer);
			}else{
				    delFooter(footer);
			}
		}		
	});
	
	$("input[name=pagePhoneHeader]").click(function() {  //点击phone页头
		var header = ""; 
		header += "<div id=\"phone_page_header\" style=\"font:12px/1.5 Consolas, Monaco, Bitstream Vera Sans Mono, Courier New, Courier, monospace;text-align:center;\">";
		header += "如果您想分享此邮件，请点击&nbsp;<a href=\"" + appUrl + "/forward\" target=\"_blank\">转发给好友</a>";
		header += "<br />";
		header += "如果您不想再接收此类邮件，请点击&nbsp;<a href=\"" + appUrl + "/unsubscribe\" target=\"_blank\">退订</a>";
		header += "</div>";
		
		if(eModelPhone == "WYSIWYG"){
			if($("input[name=pagePhoneHeader]").is(":checked")){
				var k1 = $($('.tabLi').eq(1).find('iframe')[0].contentWindow.document.body);
				k1.prepend(header);
			}else{
				var k1 = $($('.tabLi').eq(1).find('iframe')[0].contentWindow.document.body);
				k1.find("#phone_page_header").remove();
			}
		}else{
			if($("input[name=pagePhoneHeader]").is(":checked")){
				    addHeaderPhone(header);
			}else{
				    delHeaderPhone(header);
			}
		}
	});
		
	$("input[name=pagePhoneFooter]").click(function() {  //点击phone页脚
		var footer = "";
		footer += "<div id=\"phone_page_footer\" style=\"font:12px/1.5 Consolas, Monaco, Bitstream Vera Sans Mono, Courier New, Courier, monospace;text-align:center;\">";
		footer += "如果您觉得此封邮件给您带来了不便，您还可以 <a href=\"" + appUrl + "/complain\" target=\"_blank\">投诉</a>";
		footer += "</div>";	
		
		if(eModelPhone == "WYSIWYG"){
			if($("input[name=pagePhoneFooter]").is(":checked")){
				var k1 = $($('.tabLi').eq(1).find('iframe')[0].contentWindow.document.body);
				k1.append(footer);
			}else{
				var k1 = $($('.tabLi').eq(1).find('iframe')[0].contentWindow.document.body);
				k1.find("#phone_page_footer").remove();
			}
		}else{
			if($("input[name=pagePhoneFooter]").is(":checked")){
				addFooterPhone(footer);
			}else{
				delFooterPhone(footer);
			}
		}		
	});	
	
	$("#workMode").UIselect({
		width:102,
		height:200,
		html:"<ul><li><a href='javascript:changeModel(\"WYSIWYG\")'>可视编辑</a></li><li><a href='javascript:changeModel(\"TEXT\")'>文本输入</a></li></ul>"
	}); 
	$("#phone_workMode").UIselect({
		width:102,
		height:200,
		html:"<ul><li><a href='javascript:ph_changeModel(\"WYSIWYG\")'>可视编辑</a></li><li><a href='javascript:ph_changeModel(\"TEXT\")'>文本输入</a></li></ul>"
	});
	
	$("#open_paramBox").UIselect({
		width: 170,
		height: 120,
		html: function(){
			var prop_text = "";
			prop_text += "<ul>";
			for(var i = 0; i < prop_list.length; i++) {
				prop_text += "<li><a hidefocus=\"true\" onclick=\"insert(\'" + prop_list[i][0] + "\')\" href=\"javascript:void(0)\" title=\"" + prop_list[i][1] + "\">" + prop_list[i][1] + "</a></li>";
			}
			prop_text += "</ul>";
			return prop_text;
		}
	}); 
	$("#open_manyBox").UIselect({
		width: 170,
		height: 120,
		html: function(){
			var prop_text = "";
			prop_text += "<ul>";
			for(var i = 0; i < many_list.length; i++) {
				prop_text += "<li><a hidefocus=\"true\" onclick=\"insert(\'" + many_list[i][0] + "\')\" href=\"javascript:void(0)\" title=\"" + many_list[i][1] + "\">" + many_list[i][1] + "</a></li>";
			}
			prop_text += "</ul>";
			return prop_text;
		}
	}); 
	
	$("#phone_open_paramBox").UIselect({
		width: 170,
		height: 120,
		html: function(){
			var prop_text = "";
			prop_text += "<ul>";
			for(var i = 0; i < prop_list.length; i++) {
				prop_text += "<li><a hidefocus=\"true\" onclick=\"insertPhone(\'" + prop_list[i][0] + "\')\" href=\"javascript:void(0)\" title=\"" + prop_list[i][1] + "\">" + prop_list[i][1] + "</a></li>";
			}
			prop_text += "</ul>";
			return prop_text;
		}
	});
	$("#phone_open_manyBox").UIselect({
		width: 170,
		height: 120,
		html: function(){
			var prop_text = "";
			prop_text += "<ul>";
			for(var i = 0; i < many_list.length; i++) {
				prop_text += "<li><a hidefocus=\"true\" onclick=\"insertPhone(\'" + many_list[i][0] + "\')\" href=\"javascript:void(0)\" title=\"" + many_list[i][1] + "\">" + many_list[i][1] + "</a></li>";
			}
			prop_text += "</ul>";
			return prop_text;
		}
	});
	
	//点击分类
	$('#cateWrap').on("click","a",function(){
		var id = $(this).attr("itemid");
		var html = "";
		for(var key in rcmTempData){
			if(key == id){
				var obj = rcmTempData[key];
				for(var i=0;i<obj.length;i++){
					if(i>0){
						html += "<li><a href='javascript:showTempImg(\""+obj[i].id+"\")'>";
					}else{
						html += "<li class='selected'><a href='javascript:showTempImg(\""+obj[i].id+"\")'>";
						showTempImg(obj[i].id);
					}	
					html += obj[i].title;
					html += "</a></li>";
				}
			}
		}	
		$("#tempWrap ul").html('').append(html);
	});
	
	//划过样式
	$(".temp_item").on("mouseover",'li',function(){
		$(this).addClass("hover");
	})
	$(".temp_item").on("mouseout",'li',function(){
		$(this).removeClass("hover");
	})
	
	//选中样式
	$(".temp_item").on("click",'li',function(){
		$(this).parents(".temp_item").find("li").removeClass("selected");
		$(this).toggleClass("selected");
	})
	
	$(".temp_img img").bind("error",function(){
		$(this).attr("src","/static/img/template/nopic.jpg");
	});
	
	$(".temp_img img").bind("load",function(){
		var ht=$(this).height();
		var wt=$(this).width();
		if(ht>wt){
			$(this).css({"height":"480px","width":"auto"});
		}else{
			$(this).css({"height":"auto","width":"480px"});
		}
	});
    
    $("#templateContent").on("focus",function(){	
    	$("#codeTips").hide();
    });
    
    $("#templateContent").on("change",function(){	
    	$("#codeTips").hide();
    });
	
	//自定义信息打开
	$('#senderMore').on('click',function() {
		var $this = $(this);
		if($this.hasClass("swtpand")){
			$this.removeClass("swtpand").addClass("swtclose");
			$("#advancedOption").hide();
		}else{
			$this.removeClass("swtclose").addClass("swtpand");
			$("#advancedOption").show();
		}
	});
	$('#phone_senderMore').on('click',function() {
		var $this = $(this);
		if($this.hasClass("swtpand")){
			$this.removeClass("swtpand").addClass("swtclose");
			$("#phone_advancedOption").hide();
		}else{
			$this.removeClass("swtclose").addClass("swtpand");
			$("#phone_advancedOption").show();
		}
	});
});

function template(form, m) {
	form.ajaxForm({
		url: m,
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json', 
		success: function(data) {
			closeDialog();
			if (data.code == 1) {
				if (eModel == "WYSIWYG") {
					
					if($("#pc").hasClass("now") || $('#pc').length < 1){
						K.html(data.content);
					}
					
					if($("#phone").hasClass("now")){
						K1.html(data.content);
					}
					
				} else {
					$("#templateContent").val(data.content);
				}
				
				if($("#pc").hasClass("now") || $('#pc').length < 1){
					if (data.page_header == "on") {
						$("#pageHeader").attr("checked", true);
					} else {
						$("#pageHeader").attr("checked", false);
					}
					
					if (data.page_footer == "on") {
						$("#pageFooter").attr("checked", true);
					} else {
						$("#pageFooter").attr("checked", false);
					}
				}
				
				if($("#phone").hasClass("now")){
					if (data.page_header == "on") {
						$("#pagePhoneHeader").attr("checked", true);
					} else {
						$("#pagePhoneHeader").attr("checked", false);
					}
					
					if (data.page_footer == "on") {
						$("#pagePhoneFooter").attr("checked", true);
					} else {
						$("#pagePhoneFooter").attr("checked", false);
					}
				}
				
				error_message(data.message);
			} else {
				error_message(data.message, 1);
			} 
		}   
	}); 
}


//生成富文本编辑器
function createEditor(){
	K = KindEditor.create('textarea[id="templateContent"]', {
		uploadJson: "/upload",
		fileManagerJson: "/file_manager",
		allowFileManager: true,
		resizeType: 0,
		height:460,
		filterMode: false,
		wellFormatMode: false,
		afterFocus : function(){
			$("#codeTips").hide();
		},
		items: [
			'source', '|', 'undo', 'redo', '|', 'preview', 'print', 'cut', 'copy', 'paste','plainpaste', 'wordpaste', '|',
		    'justifyleft', 'justifycenter', 'justifyright','justifyfull',  'insertorderedlist', 'insertunorderedlist',
		    'indent', 'outdent', 'subscript','superscript', 'clearhtml', 'quickformat', 'selectall', '|', 'fullscreen', '/',
		    'formatblock', 'fontname', 'fontsize', '|', 'forecolor', 'hilitecolor', 'bold', 'italic', 'underline', 'strikethrough',
		    'lineheight', 'removeformat', '|', 'image', 'table', 'hr', 'link', 'unlink'
		]
	});
}

//生成富文本编辑器
function createEditor_phone(){
	K1 = KindEditor.create('textarea[id="phoneContent"]', {
		uploadJson: "/upload",
		fileManagerJson: "/file_manager",
		allowFileManager: true,
		resizeType: 0,
		height:460,
		filterMode: false,
		wellFormatMode: false,
		afterFocus : function(){
			$("#codeTips").hide();
		},
		items: [
			'source', '|', 'undo', 'redo', '|', 'preview', 'print', 'cut', 'copy', 'paste','plainpaste', 'wordpaste', '|',
		    'justifyleft', 'justifycenter', 'justifyright','justifyfull',  'insertorderedlist', 'insertunorderedlist',
		    'indent', 'outdent', 'subscript','superscript', 'clearhtml', 'quickformat', 'selectall', '|', 'fullscreen', '/',
		    'formatblock', 'fontname', 'fontsize', '|', 'forecolor', 'hilitecolor', 'bold', 'italic', 'underline', 'strikethrough',
		    'lineheight', 'removeformat', '|', 'image', 'table', 'hr', 'link', 'unlink'
		]
	});
}

//文本域添加删除页眉
function addHeader(header){
	var prop=$("#templateContent").val();
	if(/<body[^>]*>/ig.test(prop)){
		prop=prop.replace(/(<body[^>]*>)/ig,'$1'+header);
		$("#templateContent").val(prop);
	}else{
		$("#templateContent").val(header+prop);
	}
}

function delHeader(header){
	var prop=$("#templateContent").val();
	prop=prop.replace(/<div((?!<\/div>).)*page_header(((?!<\/div>).)*\n*){0,}<\/div>/ig,'');
	$("#templateContent").val(prop);
}

//文本域添加删除页脚
function addFooter(footer){
	var prop=$("#templateContent").val();
	if(/<\/body[^>]*>/ig.test(prop)){
		prop=prop.replace(/(<\/body[^>]*>)/ig,footer+'$1');
		$("#templateContent").val(prop);
	}else{
		$("#templateContent").val(prop+footer);
	}	
}

function delFooter(footer){
	var prop=$("#templateContent").val();
	prop=prop.replace(/<div((?!<\/div>).)*page_footer(((?!<\/div>).)*\n*){0,}<\/div>/ig,'');
	$("#templateContent").val(prop);
}


//文本域添加删除页眉Phone
function addHeaderPhone(header){
	var prop=$("#phoneContent").val();
	if(/<body[^>]*>/ig.test(prop)){
		prop=prop.replace(/(<body[^>]*>)/ig,'$1'+header);
		$("#phoneContent").val(prop);
	}else{
		$("#phoneContent").val(header+prop);
	}
}

function delHeaderPhone(header){
	var prop=$("#phoneContent").val();
	prop=prop.replace(/<div((?!<\/div>).)*phone_page_header(((?!<\/div>).)*\n*){0,}<\/div>/ig,'');
	$("#phoneContent").val(prop);
}

//文本域添加删除页脚Phone
function addFooterPhone(footer){
	var prop=$("#phoneContent").val();
	if(/<\/body[^>]*>/ig.test(prop)){
		prop=prop.replace(/(<\/body[^>]*>)/ig,footer+'$1');
		$("#phoneContent").val(prop);
	}else{
		$("#phoneContent").val(prop+footer);
	}	
}

function delFooterPhone(footer){
	var prop=$("#phoneContent").val();
	prop=prop.replace(/<div((?!<\/div>).)*phone_page_footer(((?!<\/div>).)*\n*){0,}<\/div>/ig,'');
	$("#phoneContent").val(prop);
}

//改变编辑模式
function changeModel(type){
	if(eModel==type){
		return;
	}else{
		eModel=type;
		$("input[name=editor]").val(eModel);
		if(eModel=="WYSIWYG"){
			var prop=$("#templateContent").val();
			createEditor();
			K.html('').insertHtml(prop);		
		}else{
			$("#templateContent").val(K.html());
			K.remove();		
		}
	}
	closeSelectDiv();
}

function ph_changeModel(type){
	if(eModelPhone==type){
		return;
	}else{
		eModelPhone=type;
		$("input[name=editorPhone]").val(eModelPhone);
		if(eModelPhone=="WYSIWYG"){
			var prop=$("#phoneContent").val();
			createEditor_phone();
			K1.html('').insertHtml(prop);
			
		}else{
			$("#phoneContent").val(K1.html());
			K1.remove();
		}
	}
	closeSelectDiv();
}

function insert(prop){
	if(eModel=="WYSIWYG"){
		K.insertHtml(prop);
	}else{
		$("#templateContent").insertContent(prop);
	}
	
}

function insertPhone(prop){
	if(eModelPhone=="WYSIWYG"){
		K1.insertHtml(prop);
	}else{
		$("#phoneContent").insertContent(prop);
	}
	
}

function post(URL, PARAMS) {      
    var f = document.createElement("form");      
    f.action = URL;      
    f.method = "post";
	f.target = "_blank";
	      
    f.style.display = "none";      
    for (var x in PARAMS) {      
        var opt = document.createElement("textarea");      
        opt.name = x;      
        opt.value = PARAMS[x];      
        f.appendChild(opt);      
    }      
    document.body.appendChild(f);      
    f.submit();      
    return f;      
}      

//显示模板图片
function showTempImg(id){
	$("#recommend input[name=templateId]").val(id);
	$(".temp_img img").attr('src',"/static/img/template/"+id+".jpg");
}

//打开模板推荐
function openSysTemp(){
	openPop(0,0,'#templateRemind');
	createCate(rcmCateData);
	if($("#cateWrap ul li").size()>0){
		$("#cateWrap ul li").eq(0).find('a').trigger('click');
	}
}

//生成分类
function createCate(arr){
	var html="";
	for(i=0;i<arr.length;i++){
		html += "<li><a href='javascript:void(0)' itemid='"+arr[i][0]+"'>";
		html += arr[i][1];
		html += "</a></li>";
	}
	$("#cateWrap ul").html('').append(html);
}

//打开历史模板
function openHisTemp(){
	importHisTemp(hisTempData,'appendHisId(this)','#historyTemplate');
	openPop(0,0,"#historyTemplate");
}

function openPhHisTemp(){
	importHisTemp(hisPhoneData,'appendPhHisId(this)','#ph_historyTemplate');
	openPop(0,0,"#ph_historyTemplate");
}

function importHisTemp(json,fn,boxId){
	var $boxId=$(boxId);
	$(".search_result",$boxId).html("");
	$.fn.search.createSearch(json,fn,boxId);
	 //解除绑定        
   $(".search_input",$boxId).unbind('keyup');  
   $(".search_input",$boxId).bind('keyup',function(){
		var val_txt = $(this).val().replace(/\s/g,"");
		if(val_txt==="") {
			$.fn.search.createSearch(json,fn,boxId);
			return false;
		}		
		$.fn.search.keySearch(json,fn,boxId,val_txt);
   })
}

function appendHisId(obj){
	$("#history input[name=templateId]").val($(obj).attr("data-id"));
}

function appendPhHisId(obj){
	$("#ph_history input[name=templateId]").val($(obj).attr("data-id"));
}

//改变编辑模式
function changeTempCode(){
	if(codeResource==null){return}
	codeModel=(codeModel=="UTF8"?"GB2312":"UTF8");
	if(eModel=="WYSIWYG"){
		
		if($("#pc").hasClass("now") || $('#pc').length < 1){
			if(codeModel=="UTF8"){
				K.html(codeResource.UTF8);
			}else{
				K.html(codeResource.GB2312);
			}
		}
		
		if($("#phone").hasClass("now")){
			if(codeModel=="UTF8"){
				K1.html(codeResource.UTF8);
			}else{
				K1.html(codeResource.GB2312);
			}
		}
		
	}else{
		if(codeModel=="UTF8"){
			$("#templateContent").val(codeResource.UTF8);
		}else{
			$("#templateContent").val(codeResource.GB2312);
		}	
	}
}

function changeTempCodePhone(){
	if(codeResource==null){return}
	codeModelPhone=(codeModel=="UTF8"?"GB2312":"UTF8");
	if(eModelPhone=="WYSIWYG"){
		if(codeModel=="UTF8"){
			K1.html(codeResource.UTF8);
		}else{
			K1.html(codeResource.GB2312);
		}
	}else{
		if(codeMode=="UTF8"){
			$("#phoneContent").val(codeResource.UTF8);
		}else{
			$("#phoneContent").val(codeResource.GB2312);
		}	
	}
}

function hasPrama(){
	var k = (eModel == "WYSIWYG") ? K.html() : $("#templateContent").val();	
	for(i=0; i<prop_list.length; i++){
		var str = prop_list[i][0].replace(/\$/g,'');
		var re = new RegExp("href=\".*\\$"+ str +"\\$.*\"|href=\'.*\\$"+ str +"\\$.*\'|src=\".*\\$"+ str +"\\$.*\"|src=\'.*\\$"+ str +"\\$.*\'","i");
		if(re.test(k)){
			return true;
		};
	}
	return false;
}

function hasPramaPhone(){
	var k1 = (eModelPhone == "WYSIWYG") ? K1.html() : $("#phoneContent").val();	
	for(i=0; i<prop_list.length; i++){
		var str = prop_list[i][0].replace(/\$/g,'');
		var re = new RegExp("href=\".*\\$"+ str +"\\$.*\"|href=\'.*\\$"+ str +"\\$.*\'|src=\".*\\$"+ str +"\\$.*\"|src=\'.*\\$"+ str +"\\$.*\'","i");
		if(re.test(k1)){
			return true;
		};
	}
	return false;
}

function switchParam(){
	$('#advancedOption').show();
	var type = $('input[name=personal]:checked').val();
	if(type == 'true'){
		$('input[name=personal][value=false]').prop('checked',true);
	}else{
		$('input[name=personal][value=true]').prop('checked',true);
	}
}

//插入表单链接
function insertFormUrl(url,obj){

	if(eModel=="WYSIWYG"){	
		$("#tabMain li:nth-child(1)").find('.ke-icon-link:nth-child(1)').eq(0).trigger('click');

		setTimeout(function(){
				$('#keUrl').val(url);
			},500);	

		closeSelectDiv();
	}
	else{
		$("#templateContent").insertContent('<a href="' + url + '">' + url + '</a>');
	}
}


//插入表单链接
function insertPhoneFormUrl(url,obj){

	if(eModelPhone=="WYSIWYG"){		
		$("#tabMain li:nth-child(2)").find('.ke-icon-link:nth-child(1)').trigger('click');
		setTimeout(function(){
				$('#keUrl').val(url);
			},500);	

		closeSelectDiv();
	}
	else{
		$("#phoneContent").insertContent('<a href="' + url + '">' + url + '</a>');
	}
}

//生成表单模板列表
function openSysForm(){
	openPop(0,0,'#templateRemind');
	var html = '';
	for(i=0;i<rcmTempData.length;i++){
		if(i>0){
			html += "<li><a href='javascript:showTempImg(\""+rcmTempData[i].id+"\")'>";
		}else{
			html += "<li class='selected'><a href='javascript:showTempImg(\""+rcmTempData[i].id+"\")'>";
			showTempImg(rcmTempData[i].id);
		}	
		html += rcmTempData[i].title;
		html += "</a></li>";
	}
	$("#tempWrap ul").html('').append(html);
}

//即时触发条件
function openTrigPad(){
	//获取模板里面的链接地址
	getHrefList();
	if(!trig_list.length){
		error_message('模板中没有可用链接！',1);
		return;
	}
	
	compareHref('open');
}

//提交时比较触发url
function compareHref(){
	if($('#category_list').html() == '触发计划'){
		return true;
	}
	
	var initHrefs = [];
	$('.trig_list_id').each(function(){
		if($(this).val() != ''){
			initHrefs = initHrefs.concat($(this).val().split(','));	
		}
	});
	
	var tempArr = getHrefList();
	$('.J_trig_list').each(function(){
		$(this).data('initInput')();
	});
	
	var errorHref = [];
	for(var i=0; i<initHrefs.length; i++){
		if(jQuery.inArray(initHrefs[i], tempArr) == -1){
			errorHref.push(initHrefs[i]);
		}
	}

	var html = '<input class="input_yes" type="button" value="继续提交" onclick="closeDialog();$(\'#add\').submit();" />';
	var	html2 =	'<input class="input_no" type="button" value="触发设置" onclick="closeDialog();openPop(0,0,\'#tempSetting\');" />';
	var err_html = "<div style='text-align:left;padding:0 5px;'>以下设置的触点链接在模板中已被删除：<div class='err_box'>"+errorHref.join('<p></p>')+"</div></div>";
	
	//区分是打开还是提交验证
	if(arguments[0] == 'open'){
		return refer_open();
	}else{
		return refer_submit();
	}
	
	function refer_open(){
		//存在无效地址
		if(errorHref.length){
			popTips("#remindTrig",err_html);
			$("#remindTrig .cmt_wrap").html(html2);
			del_loading();
			return false;
		}else{
			openPop(0,0,'#tempSetting');
			return true;
		}
	}
	
	function refer_submit(){
		//新建模板
		if(init_trig_num == 0){
			//存在无效地址
			if(errorHref.length){
				popTips("#remindTrig",err_html);
				$("#remindTrig .cmt_wrap").html(html+ '&nbsp;' +html2);
				del_loading();
				return false;
			}else{
				return true;
			}
			
		//如果是触发模板
		}else{
			//存在无效地址
			if(errorHref.length){
				popTips("#remindTrig",err_html);	
				//验证通过可以直接提交
				if(testTrig() == true){
					$("#remindTrig .cmt_wrap").html(html+ '&nbsp;' +html2);
					del_loading();
					return false;
				//验证不通过
				}else{
					$("#remindTrig .cmt_wrap").html(html2);
					del_loading();
					return false;
				}
				
			//不存在无效地址
			}else{
				if(testTrig() == false){
					popTips("#remindTrig","修改触发模板必须包含至少一个触发链接！");
					$("#remindTrig .cmt_wrap").html(html2);	
					del_loading();
					return false;
				}else{
					return true;
				}	
			}
		}
	}
}

//验证触发选项
function testTrig(){
	var $boxId = $("#tempSetting"); 
	var $err = $boxId.find(".err")
	$err.html("");
	var flag = true;	
	$boxId.find('.trig_limiter table').each(function(){
		var $this = $(this);
		var obj1 = $this.find('.trig_list_id')[0];
		if(  obj1.value == '' ){
			$err.html("请选择触点链接！");
			$(obj1).prev().focus();
			flag = false;
			return false;
		}
		var obj2 = $this.find('.J_subtemp_list')[0];
		if(  obj2.value == '' ){
			$err.html("请选择子模板！");
			obj2.focus();
			flag = false;
			return false;
		}
		var obj3 = $this.find('.J_trig_time')[0];
		if( !/\d{1,3}/.test(obj3.value) || obj3.value < 0 || obj3.value >120|| obj3.value.indexOf('.')>-1 ){
			$err.html("触发投递时间请输入0-120的整数");
			obj3.focus();
			flag = false;
			return false;
		}
		var obj4 = $this.find('.J_trig_sub')[0];
		if( obj4.value == '' ){
			$err.html("请填写邮件主题");
			obj4.focus();
			flag = false;
			return false;
		}
		
		if(/['!…"#$%&\^*]/.test(obj4.value)){
			$err.html("邮件主题有特殊字符");
            flag = false;
            return false;
		}	 
	});
	
	if(flag){
		if($boxId.is(':visible')){
			closeDialog();
		}
		$err.html("");
	}
	
	return flag;
}


function getHrefList(){
	trig_list.length = 0;
	var tempArr = [];
	if(eModel == "WYSIWYG"){
		$('<div>' + K.html() + '</div>').find('a').each(function(){
			//如果是页眉页脚
			if( $(this).parents('#page_header').length > 0 || $(this).parents('#page_footer').length > 0 ){
				return true;
			}
			
			// This is a problem! Because it's cann't get the correct value that it run on below version of IE8.
			var href = $(this).attr('href') || '';
			//不重复添加
			if(jQuery.inArray(href, tempArr) != -1){
				return true;
			}
			
			if(href.indexOf('http') > -1){
				trig_list.push( {"id":href,"title":href} );
				tempArr.push(href);
			}	
		});
		
	}else{
		//借用模板编辑器
		var prop = $("#templateContent").val();
		createEditor();
		K.html('').insertHtml(prop);
		$('<div>' + K.html() + '</div>').find('a').each(function(){
			//如果是页眉页脚
			if( $(this).parents('#page_header').length > 0 || $(this).parents('#page_footer').length > 0 ){
				return true;
			}
			
			var href = $(this).attr('href') || '';
			if(jQuery.inArray(href, tempArr) != -1){
				return true;
			}
			
			if(href.indexOf('http') > -1){
				trig_list.push( {"id":href,"title":href} );
				tempArr.push(href);
			}	

		});
		
		K.remove();
	}
	
	//返回一个一维数组
	return tempArr;
}

function switchTrig(){
	if($('#category_list').html() == '触发计划'){
		$('.J_notrig_area').hide();
	}else{
		$('.J_notrig_area').show();
	}
}


function initMselect($this){
	//存放子数组
	$this.data('subArr',[]);
	$this.Mselect({
		valueId:"#"+$this.next().attr('id'),
		boxId:"#slideLinks",
		json:trig_list,
		subArr:$this.data('subArr'),
		itemFn:"getTrig(this,'#" + $this.attr('id') + "')"
	});
}

function initSubTemp($this){
	//生成搜索
	$this.search({
		valueId:"#"+$this.next().attr('id'),
		boxId:"#slideTrig",
		json:subtemp_list,
		itemFn:"getSubject(this,'#" + $this.attr('id') + "')",
		sortFn:outSubTemp
	});
}

//赋值主题
function getSubject(obj,inputId){
	pickData(obj,inputId);
	var id = $(inputId).next().val();
	for(var i in subtemp_list){
		if(subtemp_list[i].id == id){
			
			$(inputId).parents('table').find('.J_trig_sub').val(subtemp_list[i].subject);
		}
	}	
}

function getTrig(obj,textId){
	var $textId = $(textId);
	var $obj = $(obj);
	var $span = $obj.next('span');
	var $valueId = $textId.next();
	var id = $span.data('id');
	var valArr = $valueId.val().split(',');
	if(obj.checked){
		$obj.parents('li').addClass('selected');
		valArr.push(id);
	}else if(!obj.checked){
		$obj.parents('li').removeClass('selected');
		for(i=0;i<valArr.length;i++){
			if(valArr[i] == id){
				valArr.splice(i,1);
			}
		}
	}
	$valueId.val(valArr.join(','));
	$textId.data('initInput')();
}

function add_trig(obj){
	var html = [],i = 0;	
	html[i++] = '<table width="100%" cellspacing="0" cellpadding="0" border="0">';
	html[i++] = '	<tr>';
	html[i++] = '		<td colspan="2" style="height: 10px;">';
	html[i++] = '			<p style="border-bottom:1px dashed #A0A0A0;"></p>';
	html[i++] = '		</td>';
	html[i++] = '	</tr>';
	html[i++] = '	<tr>';
	html[i++] = '		<td width="27%">触点链接(可多选)</td>';
	html[i++] = '		<td>';
	html[i++] = '			<textarea name="trig_list_' + trig_num + '" style="width:240px;" class="add_area rad_area J_trig_list" id="trig_list_' + trig_num + '" readonly></textarea>';
	html[i++] = '			<input type="hidden" name="touchUrls_' + trig_num + '" value="" class="trig_list_id" id="trig_list_id_' + trig_num + '">';
	html[i++] = '		</td>';
	html[i++] = '	</tr>';
	html[i++] = '	<tr>';
	html[i++] = '		<td width="27%">触发计划的子模板</td>';
	html[i++] = '		<td>';
	html[i++] = '			<textarea readonly="readonly" id="subtemp_list_' + trig_num + '" class="add_area rad_area J_subtemp_list" style="width:240px;" name="subtemp_list_' + trig_num + '"></textarea>';
	html[i++] = '			<input type="hidden" name="templateIds_' + trig_num + '" value="" id="subtemp_id_' + trig_num + '">';
	html[i++] = '		</td>';
	html[i++] = '	</tr>';
	html[i++] = '	<tr>';
	html[i++] = '		<td width="27%">触发投递时间</td>';
	html[i++] = '		<td>';
	html[i++] = '			<input type="text" id="trig_time_' + trig_num + '" name="afterMinutes_' + trig_num + '" class="input_txt J_trig_time">&nbsp;分钟';
	html[i++] = '		</td>';
	html[i++] = '	</tr>';
	html[i++] = '	<tr>';
	html[i++] = '		<td width="27%">邮件主题</td>';
	html[i++] = '		<td>'
	html[i++] = '			<input type="text" name="subjects_' + trig_num + '" class="input_txt J_trig_sub">';
	html[i++] = '			<span title="删除" onclick="del_trig(this)" class="del_set"></span>';
	html[i++] = '		</td>';
	html[i++] = '	</tr>';
	html[i++] = '</table>';
	html = html.join('\n');
	
	//添加触发表单
	$(obj).parents('.trig_limiter').append(html);
	
	//初始化下拉选择
	initMselect($('#trig_list_' + trig_num ));
	initSubTemp($('#subtemp_list_' + trig_num ));
	$('#trig_time_'+trig_num).gtips({
		eType: "focus",
		content: function(){
			return '请输入0-120的数字，单位是分钟。';
		}
	});
	
	trig_num++;
}

function del_trig(obj){
	$(obj).parents('table').remove();
}

//排除已选子模板
function outSubTemp(){
	var $this = this;
	var textId = arguments[0].attr('id');
	
	var	outCate = [];
	$('.J_subtemp_list').each(function(){
		if(textId != $(this).attr('id') && $(this).val() != ''){
			outCate.push($(this).val());
		}	
	});
	
	$this.find('li').each(function(){
		var	$a = $(this).find('a');
		if(jQuery.inArray($a.attr('title'),outCate) != -1){
			$(this).hide();
		}
	});
	 
}

function cancleTrig(){
	$('.trig_list_id').val('');
	//重新设值选区
	$('.J_trig_list').each(function(){
		$(this).data('initInput')();
	});
	
	$('#tempSetting .trig_limiter table').filter(function(){
		return $(this).index() != 0;
	}).remove();
	
	//有的话，清除提示语
	$('#tempSetting .err').html('');
	
	setTimeout(function(){
		closeDialog();
	},300);
}

//插入多属性表格
function insertMul(){
	var tdSum = $('#tdSum').val();
	var tbWdith = $('#tbWidth').val();
	var wdRule = $('#wdRule').val();
	if(isNaN(tbWdith) || tbWdith < 1){
		$('.Jmul_err').html('表格宽度请输入大于1的数字！');
		return false;
	}
	if(isNaN(tdSum) || tdSum > 100 || tdSum < 1){
		$('.Jmul_err').html('表格列数请输入1-100以内的数字！');
		return false;
	}
	tdSum = parseInt(tdSum);
	tbWdith = wdRule == 'px' ? tbWdith : tbWdith+'%';
	var style = 'text-align:center;';
	var table = '<table border="1" bordercolor="#000000" cellpadding="2" cellspacing="0" style="'+ style +'" width="'+tbWdith+'"><tbody>';
	table += '<tr>';
	for(var k=0;k<tdSum;k++){
		table += '<td>标题'+ (k+1) +'</td>';
	}
	table += '</tr>';
	table += '<tr class="__many__">';
	for(var k=0;k<tdSum;k++){
		table += '<td>变量'+ (k+1) +'</td>';
	}
	table += '</tr>';
	table += '</tbody></table>';
	
	try {
　　     		if($('#phone').length && $('#phone').hasClass('now')){
			if(eModelPhone=="WYSIWYG"){
				K1.insertHtml(table);
			}else{
				$("#phoneContent").insertContent(table);
			}	
		}else{
			if(eModel=="WYSIWYG"){
				K.insertHtml(table);
			}else{
				$("#templateContent").insertContent(table);
			}
		}
　　    } catch(error) {
　　   // 此处是负责例外处理的语句
　　    }
	
	closeDialog();
}
