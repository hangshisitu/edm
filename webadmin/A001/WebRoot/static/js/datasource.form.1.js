$(document).ready(function(){

	template($("#recommend"));

	//保存提交
	$('#add').ajaxForm({    
		url: '1',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json', 
		beforeSerialize: function($form, options) { 
			if(eModel=="WYSIWYG"){
				K.sync();
			}	
			
		},
		beforeSubmit: function(formData, jqForm, options) {   //验证
			loading();
			var form = jqForm[0];
			
			//验证有无插入表单属性
			if(eModel=="WYSIWYG"){
				var $k = $(window.frames[0].document.body);
				if(!$k.find('form#tempForm').length){
					error_message('请插入表单属性', 1);
					del_loading();
					return false;
				}
			}else{
				var text = $("#templateContent").val();
				if(!text.match(/<form[^>]*tempForm[^>]*>([\s\S]*?)<\/form>/ig)){
					error_message('请插入表单属性', 1);
					del_loading();
					return false;
				}
			}

		},
		success: function(data) { 
			if (data.code == 1) {
				del_loading();
				error_message(data.message, 0, 0, 1000, "/datasource/form/2?formId=" + data.formId);
			} else {
				error_message(data.message, 1);
				del_loading();
			}	 
		},
		error:function(){
			del_loading();
		} 
	});
	
	//点击下一步
	$('#tempSubmit').click(function(){
		 $('#add').submit();
	})
	
	//打开插入属性列表
	$('#insertForm').click(function(){
		openSlideBox(this,'#propWrap');
		$(document).bind('mousedown.insertForm', function(e){	   		 
				if(isParent(e.target, $('#propWrap')[0])==false && $(e.target).get(0) !== this){
					$('#propWrap').hide();
					$(document).unbind('mousedown.insertForm');		 	
				}				
		  });	
		 return false;
	})
	
});

function preview(){
	if(eModel=="WYSIWYG"){
		post('/datasource/form/preview?token=' + headers['token'], { content: K.html() });
	}else{
		post('/datasource/form/preview?token=' + headers['token'], { content: $("#templateContent").val() });
	}
	
	
}

function createForm(){
	//生成表单项，限制为10个
	if($('input[name=props]:checked').length > 10){
		error_message('选择属性不能超过10个',1);
		return;
	}
	
	//先删除后添加
	if(eModel=="WYSIWYG"){
		var $k = $(window.frames[0].document.body);
		//查看是否存在form
		if($k.find('form.edm_sys_form').length > 0){
			var html = getInputHtml();
			$k.find('#hook').html('');
			$k.find('#hook').append(html);
		}else{
			var html = getFormHtml();
			$k.find('form').remove();
			K.insertHtml(html);
		}	
	}else{
		
		var text = $("#templateContent").val();
		//查看是否存在form
		if(text.match(/<form[^>]*edm_sys_form[^>]*>([\s\S]*?)<\/form>/i)){
			var html = getInputHtml();
			text = text.replace(/(<ul[^>]*hook[^>]*>)([\s\S]*?)(<\/ul>)/i,'$1'+html+'$3');
			$("#templateContent").val(text);
		}else{
			text = text.replace(/<form[^>]*>([\s\S]*?)<\/form>/i,'');
			$("#templateContent").val(text);
			var html = getFormHtml();
			$("#templateContent").insertContent(html);
		}	
	}
	
	//关闭属性列表
	closeDialog();
}

function getFormHtml(){
	var html = '<form id="tempForm" method="post" action="' +actionUrl+ '" data-ke-src="' +actionUrl+ '" style="margin:0;">';
	html += '<table border=0 cellspacing=0 cellpadding=5>';
	$('#propWrap li').each(function(){
		if($(this).find('input[name=props]').prop('checked') == true){
			var title = $(this).find('.tt span').text();
			var prop = $(this).find('.tt span').attr('data-prop');
			var cate = $(this).find('.tt span').attr('data-type');
			var required = $(this).find('input[name=required]').prop('checked') == true ? '*': '&nbsp;';
			var reqClass = $(this).find('input[name=required]').prop('checked') == true ? 'required': '';
			html += '<tr>';
			html += '<td width="80" align="right">' + title + '</td>';
			html += '<td width="6" style="color:red">' + required + '</td>';
			html += '<td><input type="text" data-req="' + reqClass + '" data-cate="' + cate + '" name="' + prop + '" style="width:200px;"/></td>';
			html += '</tr>';
		}
	});
	html += '<tr><td width="60"></td><td width="6"></td><td><input type="submit" value="注册" style="cursor:pointer"></td></tr>';
	html += '</table>';
	html += '<script src="http://139edm.com/form/jquery-1.7.1.min.js"></script>';
	html += '<script src="http://139edm.com/form/menu.js"></script>';
	html += '</form>';
	return html;
}

function getInputHtml(){
	var html = '';
	$('#propWrap li').each(function(){
		if($(this).find('input[name=props]').prop('checked') == true){	
			var title = $(this).find('.tt span').text();
			var prop = $(this).find('.tt span').attr('data-prop');
			var cate = $(this).find('.tt span').attr('data-type');
			var required = $(this).find('input[name=required]').prop('checked') == true ? '*': '&nbsp;';
			var reqClass = $(this).find('input[name=required]').prop('checked') == true ? 'required': '';
			html += '<li>';
			html += '<span>' + title + '</span>';
			html += '<strong>' + required + '</strong>';
			html += '<input type="text" class="input_01 CCmingcheng" data-req="' + reqClass + '" data-cate="' + cate + '" name="' + prop + '" title="' + title + '">';
			html += '</li>';
		}
	});
	return html;
}

//替换input字段名
function getDom(){
	var str = $('#templateContent').val();
	var $dom = $('<div>' + str + '</div>');
	//name在ie7下不兼容，不能更改
	$dom.find('form').find('input[name=email]').attr('name','haha');
	var newFormHtml = $dom.find('form').html();
	$dom = null;
	str=str.replace(/(<form[^>]*tempForm[^>]*>)([\s\S]*?)(<\/form>)/i,"$1" + newFormHtml + "$3");
	$('#templateContent').val(str);
}
