/**
 * 控制上传控件的状态
 */
$(function() {
	$("input[type='radio']").click(function() {
		var id = $(this).attr("id");
		if (id == "newLogo") {
			$("#showLogo").css("display", "none");
			$(".button").removeAttr("disabled");
		} else {
			$("#showLogo").css("display", "block");			
			$(".button").attr("disabled", "disabled");
		}
	});
});

/*function commit(url) {

 var list= $('input:radio[name="type"]:checked').val();
 if( list =="2"){
 customForm = document.customForm;
 customForm.action = url;
 customForm.submit();
 $('body').messagebox("xxxxxxx",'', 1,3000);
 setTimeout("closeWin()",3000);
 }else{
 closeWin();
 }
 }*/

/** 
 * commit 表单提交 
 * @return void 
 **/
function subimtBtn() {
	var list = $('input:radio[name="type"]:checked').val();
	if (list == "2") {
		var form = $("#customForm");
		var options = {
			url : '/custom/save',
			type : 'post',
			success : function(data) {
				var jsondata = eval("(" + data + ")");
				if (jsondata.error == "0") {					
					$('body').messagebox("新建企业logo成功,如使用请激活。", '', 1, 2000);
					//window.dialogArguments.location.reload();
	 	     		setTimeout("closeWindow()",2000);							
				} else {
					$("#prompt").hide();
					$('body').messagebox("新建企业logo失败", '', 0, 3000);
				}
			}
		};
		$("#prompt").show();
		form.ajaxSubmit(options);//使用ajax异步请求
		//$("#fileForm").submit(); 
	} else {
		closeWin();
	}
}


//返回
function success() {
	var ctx = $("#ctx").val();
	var url = "/custom/page";
	window.location.href = ctx+url;
}

function closeWindow(){
	closeWin();
}

