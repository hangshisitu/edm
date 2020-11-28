$(document).ready(function(){
	K = KindEditor.create('textarea[id="templateContent"]', {
		uploadJson: "/upload",
		fileManagerJson: "/file_manager",
		allowFileManager: true,
		resizeType: 0,
		filterMode: false,
		wellFormatMode: false,
		afterFocus : function(){
		},
		items: []
	});
	
	$('.red_trg').click(function(){
		var k = $(window.frames[0].document.body);
		var word = k.find('.special').html();
		$('#redText').val(word);
		openPop(0,0,'#modEmail');
	})
	
	//保存提交
	$('#add').ajaxForm({    
		url: '3',
		type: 'post',
		headers: headers,
		resetForm: false,
		dataType: 'json', 
		beforeSerialize: function($form, options) { 
				K.sync();		
		},
		beforeSubmit: function(formData, jqForm, options) {   //验证
			loading();
			var form = jqForm[0];
			if(!form.tempConfirmSubject.value){
				 error_message("主题不能为空", 1);
				 del_loading();
				return false
			}
		},
		success: function(data) { 
			if (data.code == 1) {
				del_loading();
				var step = "5";
				if (data.emailPrompt == 1) {
					step = "4";
				}
				error_message(data.message, 0, 0, 1000, "/datasource/form/" + step + "?formId=" + data.formId);
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
})

function appendText(){
	var word = $('#redText').val();
	if($.trim(word) == ''){
		error_message("主题不能为空", 1);
		return false;
	}
	var k = $(window.frames[0].document.body);
	k.find('.special').text(word);
	$("#tempConfirmSubject").val(word);
}
