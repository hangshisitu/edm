<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!--可操作区域开始-->
<div id="addBox" style="">
	<div class="limiter">
		<div class="corp_title">
			机构管理
		</div>
		<div class="pop_content">
			<form method="post"  id="setting" autocomplete="off">
				<table width="100%" cellspacing="0" cellpadding="0" border="0" style="" class="pop_table">
					<tbody>
						<tr>
							<td width="100" align="right">
								机构名称：
							</td>
							<td width='10' class='red'>*</td>
							<td>
								<input id="company" name="company" type="text"  maxlength="15" value="${corp.company}"/>
								<span id="company_span"></span>
							</td>
						</tr>
						<tr>
							<td align="right">
								套餐类型：
							</td>
							<td width='10' class='red'>*</td>
							<td>
								<select onchange="getCosInfo(this)" name="cosId" id="cosId">
									<option value="">请选择</option>
									<c:if test="${testCosList!=null&&fn:length(testCosList)>0}">
									<option id="testcos" disabled="disabled">试用套餐</option>
									  <c:forEach items="${testCosList}" var="cos">
										<option <c:if test="${corp.cosId==cos.cosId}">selected="selected"</c:if> value="${cos.cosId}">&nbsp;&nbsp;&nbsp;&nbsp;${cos.cosName}</option>
									  </c:forEach>
								    </c:if>
								    <c:if test="${formalCosList!=null&&fn:length(formalCosList)>0}">
									    <option id="formalcos" disabled="disabled">正式套餐</option>
									  <c:forEach items="${formalCosList}" var="cos">
										<option <c:if test="${corp.cosId==cos.cosId}">selected="selected"</c:if> value="${cos.cosId}">&nbsp;&nbsp;&nbsp;&nbsp;${cos.cosName}</option>
									  </c:forEach>
								    </c:if>
								</select>
								<span id="cosId_span"></span>
								<a href="javascript:customCos('/cos/cos')" style="color:blue">自定义套餐</a>
							</td>
						</tr>
						<tr>
							<td align="right">
							</td>
							<td width='10' class='red'>&nbsp;</td>
							<td>
								允许子账号个数 <font color="red"><span id="subUserCount">${cos.userCount}</span></font>个<br>
								每日最大发送量 <font color="red"><span id="daySendCount">${cos.daySend}</span></font>封<br>
								每周最大发送量 <font color="red"><span id="weekSendCount">${cos.weekSend}</span></font>封<br>
								每月最大发送量 <font color="red"><span id="monthSendCount">${cos.monthSend}</span></font>封<br>
								总共最大发送量 <font color="red"><span id="totalSendCount">${cos.totalSend}</span></font>封
							</td>
						</tr>
						<tr>
							<td align="right">
								正式通道：
							</td>
							<td width='10' class='red'>*</td>
							<td>
								<select name="formalId" id="formalId">
									<option value="">请选择</option>
									<c:forEach items="${resourceList}" var="res" varStatus="status">
									  <option <c:if test="${corp.formalId==res.resourceId}">selected="selected"</c:if>
									     value="${res.resourceId}">${res.resourceName}</option>
								    </c:forEach>
								</select>
								<span id="formalId_span"></span>
							</td>
						</tr>
						<tr>
							<td align="right">
								测试通道：
							</td>
							<td width='10' class='red'>*</td>
							<td>
								<select name="testedId" id="testedId">
									<option value="">请选择</option>
									<c:forEach items="${resourceList}" var="res">
									  <option <c:if test="${corp.testedId==res.resourceId}">selected="selected"</c:if>
									     value="${res.resourceId}">${res.resourceName}</option>
								    </c:forEach>
								</select>
								<span id="testedId_span"></span>
							</td>
						</tr>
						<tr>
							<td align="right">
								网络发件人：
							</td>
							<td width='10' class='red'>*</td>
							<td>
								<div class="domainbox"></div>
								<c:if test="${robotsList==null||fn:length(robotsList)<1}">
									<div class="robot">
										<input id="robot_0" class="input_text count_spoke" type="text" maxlength="64" value="" name="robots">
										<span class="input_set add_set" onclick="addRobot()">+</span>
									</div>
								</c:if>
								<c:if test="${robotsList!=null}">
									
									<c:forEach items="${robotsList}" var="r" varStatus="status">
										<c:if test="${status.index==0}">
											<div class="robot">
												<input id="robot_0" class="input_text count_spoke" type="text" maxlength="64" value="${r.email}" name="robots">
												<span class="input_set add_set" onclick="addRobot()">+</span>
											</div>
										</c:if>
										<c:if test="${status.index!=0}">
											<div class="robot">
												<input id="robot_${status.index}" class="input_text count_spoke" type="text" maxlength="64" value="${r.email}" name="robots">
												<span class="input_set del_set" onclick="delRobot(${status.index})">-</span>
											</div>
										</c:if>
									</c:forEach>
								</c:if>
							</td>
						</tr>
						<%-- <tr>
							<td align="right">
								短信设置：
							</td>
							<td width='10' class='red'>*</td>
							<td>
								<input name="openSms" value="1" type="radio" <c:if test="${corp.openSms == 1}">checked="checked"</c:if>>需要&nbsp;&nbsp;
								<input name="openSms" value="0" type="radio" <c:if test="${corp.openSms == 0}">checked="checked"</c:if>>不需要
								<span id="openSms_span"></span>
							</td>
						</tr>
						<tr class="small">
							<td align="right">
							</td>
							<td width='10' class='red'>&nbsp;</td>
							<td>
								通道选择了"139投递平台"才能设置 "短信设置" 
							</td>
						</tr> --%>
						<tr>
							<td align="right">
								发件人验证：
							</td>
							<td width='10' class='red'>*</td>
							<td>
								<input name="senderValidate" value="1" type="radio" checked="checked" 
								   <c:if test="${corp.senderValidate == 1}">checked="checked"</c:if>>需要 &nbsp;&nbsp;
								<input name="senderValidate" value="0" type="radio" <c:if test="${corp.senderValidate == 0}">checked="checked"</c:if>>不需要
								<span id="senderValidate_span"></span>
							</td>
						</tr>
						<%-- <tr>
							<td align="right">
								系统侧审核：
							</td>
							<td width='10' class='red'>&nbsp;</td>
							<td>
								<input name="isNeedAudit" value="1" type="radio" checked="checked" 
								     <c:if test="${corp.auditPath != null}">checked="checked"</c:if>>需要 &nbsp;&nbsp;
								<input name="isNeedAudit" value="0" type="radio" <c:if test="${corp.auditPath == ''}">checked="checked"</c:if>>不需要
								<span id="isNeedAudit_span"></span>
							</td>
						</tr> --%>
						<tr>
							<td align="right">
								API投递业务：
							</td>
							<td width='10' class='red'>&nbsp;</td>
							<td>
								<label><input id="trigger" name="isTriggerApi" type="checkbox" value="1" 
								     <c:if test="${apiTrigger ne null and apiTrigger.status==1}">checked</c:if> />开通触发业务</label>
								<label><input id="groups" name="isGroupsApi" type="checkbox" value="1" 
								     <c:if test="${corp.joinApi == 1}">checked</c:if> />开通群发业务</label>
							</td>
						</tr>
					</tbody>
				</table>
				<table width="100%" cellspacing="0" cellpadding="0" border="0">
					<tbody>
						<tr>
							<td width="110">&nbsp;</td>
							<td><span class="err" style="color:red;line-height:28px;"></span>
							
							<ul class="f_ul" <c:if test="${onlyView == 1}">hidden</c:if> >
								<li>
									<input   type="submit" value="保存" class="input_yes" />
								</li>
								<li>
									<input type="button" value="取消" id="ccBtn" class="input_no" />
								</li>
							</ul>
							
							</td>
						</tr>
					</tbody>
				</table>
				<input type="hidden" name="token" value="${token}" />
			    <input type="hidden" name="exist" value="${corp.corpId}">
			    <input type="hidden" name="parentId" value="${parentId}">
			    <input type="hidden" name="msg" id="msg" value="${message}">
			    <input type="hidden" name="ctx" id="ctx" value="${ctx}">
			    <input type="hidden" name="backUrl" id="backUrl" value="${backUrl}">
			    <input type="hidden" name="successUrl" id="successUrl" value="${successUrl}">
			    <input type="hidden" name="cosUrl" id="cosUrl" value="${ctx}/cos/getByCosId">
			</form>
		</div>
	</div>
</div>
<!--可操作区域结束-->
<script type="text/javascript">
var domain_list = {
		<c:forEach items="${resourceList}" var="e" varStatus="a">
		"${e.resourceId}":[
			<c:forEach items="${fn:split(e.heloList, ',')}" var="o" varStatus="b">"@${o}"<c:if test="${!b.last}">,</c:if></c:forEach>
		]<c:if test="${!a.last}">,</c:if>
		</c:forEach>
	};
var $globalInput = null;
$(document).ready(function() {
	$('#setting').ajaxForm({
		url : '/corp/save',
		type : 'post',
		resetForm : false,
		dataType : 'json',
		beforeSubmit : function(formData, jqForm, options) {
			$(".err").html('');
			var form = jqForm[0];
			if($.trim(form.company.value) == '') {
				$(".err").html("提示：机构名称不能为空！");
				return false;
			}
			
			if(!$('#cosId').val()) {
				$(".err").html("提示：请选择套餐类型！");
				return false;
			}
			if(!$('#formalId').val()) {
				$(".err").html("提示：请选择正式通道！");
				return false;
			}
			if(!$('#testedId').val()) {
				$(".err").html("提示：请选择测试通道！");
				return false;
			}
			
			return checkRobots();
			
		},
		success : function(data) {
			//后台返回
			//data = {code:1,id:2255,pid:0,name:'新建机构1',type:'add'};
			//新建机构
			if(data.code == 1) {
				fnlist[glb.fun](data);
				error_message(data.message,0);
			}else{
				error_message(data.message,1);
			}
		},
		error:function(){
			error_message("提示：操作失败！");
		}
	});
			
	//初始化短信验证是否需要
	setForbidden();
	//初始化可输入的域
	setDomain();
	
	$('#formalId,#testedId').change(function(){
		setForbidden();
		setDomain();
	});
	
	//跟踪全局input
	$('.count_spoke').live('focus',function(){
		$globalInput = $(this);
	});
	
	$('#ccBtn').click(function(){
		$("#corpPad").html('<p style="margin-top:60px;">点击机构名可预览配置信息</p>');
	});
});

/**
 * 获取套餐信息
 * @param obj
 */
function getCosInfo(obj) {
	var cosUrl = $("#cosUrl").val();
	if(obj.value=="") {
		$("#subUserCount").html("");
		$("#daySendCount").html("");
		$("#weekSendCount").html("");
		$("#monthSendCount").html("");
		$("#totalSendCount").html("");
	}else {
		$.ajax({
	         type: "POST",
	         url: cosUrl,
	         data: {cosId:obj.value},
	         async: true,
	         success: function(data) {
	             //根据返回的数据进行业务处理
	        	if(data!=null) {
	        		$("#subUserCount").html(data.userCount);
	 	     		$("#daySendCount").html(data.daySend);
	 	     		$("#weekSendCount").html(data.weekSend);
	 	     		$("#monthSendCount").html(data.monthSend);
	 	     		$("#totalSendCount").html(data.totalSend);
	        	}
	           
	         },
	         error: function(e) {
	             alert(e);
	         }
	     });
	 }
	
}


/**
 * 自定义套餐
 */
function customCos(url){
	openWin(url,window,'480px','650px');
	var msg = $("#msg").val();
	if(msg != null&&msg!="") {
		$('body').messagebox(msg, '', 1, 2000);
	};
}

//验证网络发件人
function checkRobots() {
	var flag = false;
	var result = true;
	$("input[name='robots']").each(function(i){
		if($(this).val()!="") {
			flag = true;
		}
	});
	
	if(!flag) {
		$('.err').html("请输入网络发件人！");
		$("#robot_0").focus();
		result = false;
	}else {
		$("input[name='robots']").each(function(i) {
			var emailValue = $(this).val();
			if(emailValue!="") {
				if(!isEmail(emailValue)) {
					$('.err').html("网络发件人格式不正确！", '', 0, 2000);
					$(this).focus();
					result = false;
					return false;
				}
			}
		});
	}
	
	return result;
}

function setForbidden(){
	var txt1 = $('#formalId option:selected').val(),
		txt2 = $('#testedId option:selected').val();
	//正式通道和测试通道都没有出现139时
	if(txt1!=139 && txt2!=139){
		$('input[name=openSms]').prop('disabled',true);
		$('input[name=openSms][value=0]').prop('checked',true);
	}else{
		$('input[name=openSms]').prop('disabled',false);
	}
}

function setDomain(){
	var txt1 = $('#formalId option:selected').val();
	if(txt1 ==''){
		$('.domainbox').html('');
	}else{
		var html = '<div><p>根据所选的正式投递通道，您可以填写的域为：</p>';
		if(domain_list[txt1]){
			for(var i=0,arr=domain_list[txt1]; i<arr.length; i++){
				html += '<a href="javascript:insertDomain(\'' + arr[i] + '\');">'+ arr[i] +'</a>'
			}
		}
		html += '</div>';
		$('.domainbox').html(html);
	}
}

function insertDomain(str){
	if(!$globalInput){
		return;
	}else{
		$globalInput.val($globalInput.val()+str);
	}
}
</script>
