$(document).ready(function() {
	$('#addIP').ajaxForm({
		url : '/delivery/smtp/add',
		type : 'post',
		resetForm : false,
		dataType : 'json',
		beforeSubmit : function(formData, jqForm, options) {
			var form = jqForm[0];
			if(!testIP(form.ip)) {
				return false;
			}
			if(!testNum(form.quarter)) {
				return false;
			}
			if(!testNum(form.hour)) {
				return false;
			}
			if(!testNum(form.day)) {
				return false;
			}
			if(!testNum(form.size)) {
				return false;
			}
			if(parseInt(form.hour.value) <= parseInt(form.quarter.value)) {
				error_message("15分钟频率数值需小于1小时频率数值！", 1);
				return false;
			}
			if(parseInt(form.day.value) <= parseInt(form.hour.value)) {
				error_message("1小时频率数值需小于1天频率数值！", 1);
				return false;
			}
		},
		success : function(data) {
			if(data) {
				if("success" == data.result) {
					error_message(data.message, 0);
					setTimeout(function(){
						location.href = '/delivery/smtp/smtpList';
					}, 2000);
				} else {
					if( typeof (data.result) == "undefined") {
						error_message("提交失败！", 1);
					} else {
						error_message(data.message, 1);
					}
				}
			}
		}
	});
});


/*验证smtp控制项*/
function testNum(ele) {
	if(!ele.value) {
		error_message("请输入必填项", 1);
		ele.focus();
		return false;
	} else if(!(/^\d+$/.test(ele.value) && ele.value > 0)) {
		error_message("请输入大于0的整数", 1);
		ele.focus();
		return false;
	} else if(ele.value > 999999999) {
		error_message("输入的数值不能超过 9 位数字", 1);
		ele.focus();
		return false;
	}else {
		if(ele.id == "size") {
			var maxSize = 10240;
			if(ele.value > maxSize) {
				error_message("邮件大小不超过" + maxSize + "KB", 1);
				ele.focus();
				return false;
			}
		}
		return true;
	}
}

function testIP(ele) {
	if(!ele.value) {
		error_message("请输入ip地址", 1);
		ele.focus();
		return false;
	} else {
		var temp = ele.value.replace(/;(\s*)/g, ';');
		temp = temp.replace(/(^\s*)|(\s*$)/g, '');
		if(!/^((?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?);?)*$/ig.test(temp)) {
			error_message("请输入合法的ip地址,多个用英文分号隔开", 1);
			ele.focus();
			return false;
		} else {
			if(ele.value.split(";").length > 10) {
				error_message("每次提交不能超过10个ip", 1);
				ele.focus();
				return false;
			} else {
				return true;
			}
		}
	}
}
/**
 * 更新SMTP
 * @param url
 */
function updateSmtp(url) {
	location.href = url;
}
/**
 * 删除SMTP
 * @param url
 */
function deleteSmtp(url) {
	if(window.confirm("确定要删除吗？")) {
		$("#ListForm").attr("method", "POST");
		$("#ListForm").attr("action", url);
		$("#ListForm").submit();
	}
}
/**
 * 查询SMTP
 */
function search() {
	$("#currentPage").val(1);
	$("#ListForm").submit();
}


