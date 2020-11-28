<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<script>
$(function(){
	$('#new_type_list').UIselect({
		width: 110,
		id_id: "#newTypeId",
		arrdata: new Array(["red", "红名单"], ["black", "黑名单"], ["refuse", "拒收"], ["expose", "举报"], ["unsubscribe", "退订"])
	});
	
	$('#im_type_list').UIselect({
		width: 110,
		id_id: "#imTypeId",
		arrdata: new Array(["red", "红名单"], ["black", "黑名单"])
	});
	
	$('#mod_type_list').UIselect({
		width: 110,
		id_id: "#modTypeId",
		arrdata: new Array(["red", "红名单"], ["black", "黑名单"])
	});
	
	//新建
	$('#newForm').submit(function(){    
		var form = $('#newForm')[0]; 
		if(!/.+@.+\.[a-zA-Z]{2,4}$/ig.test(form.email.value)){
			$('#newForm').find('.err').html('请输入正确邮箱地址');
			form.email.focus();
			return false;
		}
		if(!form.newTypeId.value){
			$('#newForm').find('.err').html('请选择类别');
			return false;
		}
	}); 
	
	//导入
	$('#importForm').submit(function(){    
		var form = $('#importForm')[0]; 
		if(!form.file.value){
			$('#importForm').find('.err').html('请选择文件');
			return false;
		}
		if(!form.imTypeId.value){
			$('#importForm').find('.err').html('请选择类别');
			return false;
		}
	}); 
});
</script>
<div style="width:400px" class="limiter">
	<div class="pop_title">新建</div>
	<div class="pop_content">
		<form id="newForm" method="post" action="" autocomplete="off">
			<table width="100%" cellspacing="0" cellpadding="0" border="0" style="" class="edit_tab">
				<tbody>
					<tr>
						<td width="62">
							邮箱地址
						</td>
						<td width='10' class='red'>*</td>
						<td>
							<input class="input_txt" type="text" maxlength="60" name="email">
						</td>
					</tr>
					<tr>
						<td width="62">
							描述
						</td>
						<td width='10' class='red'>&nbsp;</td>
						<td>
							<input class="input_txt" type="text" maxlength="60" name="desc">
						</td>
					</tr>
					<tr>
						<td>
							类型
						</td>
						<td width='10' class='red'>*</td>
						<td>
							<a style="width:240px" id='new_type_list' class="a_select b_select"></a>
							<input type='hidden' id='newTypeId' name='newTypeId' />
						</td>
					</tr>
				</tbody>
			</table>
			<table width="100%" cellspacing="0" cellpadding="0" border="0">
				<tbody>
					<tr>
						<td width="75">&nbsp;</td>
						<td><span class="err"></span>
						<ul class="f_ul">
							<li>
								<input type="submit" value="确定" class="input_yes" />
							</li>
							<li>
								<input type="button" value="取消" onclick="close_msg();" class="input_no" />
							</li>
						</ul></td>
					</tr>
				</tbody>
			</table>
		</form>
	</div>
</div>


<div style="width:400px" class="limiter">
	<div class="pop_title">导入</div>
	<div class="pop_content">
		<form id="importForm" method="post" action="" autocomplete="off">
			<table width="100%" cellspacing="0" cellpadding="0" border="0" style="" class="edit_tab">
				<tbody>
					<tr>
						<td width="62">
							选择文件
						</td>
						<td width='10' class='red'>*</td>
						<td>
							<input class="input_txt" type="file" maxlength="60" name="file">
						</td>
					</tr>
					<tr>
						<td width="62">
							&nbsp;
						</td>
						<td width='10' class='red'>&nbsp;</td>
						<td valign='top'>
							<span class='f666'>文件的格式为xls\xlsx\csv\txt</span>
						</td>
					</tr>
					<tr>
						<td width="62">
							描述
						</td>
						<td width='10' class='red'>&nbsp;</td>
						<td>
							<input class="input_txt" type="text" maxlength="60" name="desc">
						</td>
					</tr>
					<tr>
						<td>
							类型
						</td>
						<td width='10' class='red'>*</td>
						<td>
							<a style="width:240px" id='im_type_list' class="a_select b_select"></a>
							<input type='hidden' id='imTypeId' name='imTypeId' />
						</td>
					</tr>
				</tbody>
			</table>
			<table width="100%" cellspacing="0" cellpadding="0" border="0">
				<tbody>
					<tr>
						<td width="75">&nbsp;</td>
						<td><span class="err"></span>
						<ul class="f_ul">
							<li>
								<input type="submit" value="确定" class="input_yes" />
							</li>
							<li>
								<input type="button" value="取消" onclick="close_msg()" class="input_no" />
							</li>
						</ul></td>
					</tr>
				</tbody>
			</table>
		</form>
	</div>
</div>
