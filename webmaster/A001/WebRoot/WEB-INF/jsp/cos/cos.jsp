<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>自定义套餐</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta http-equiv="Content-Language" content="zh-cn" />
<meta http-equiv="pragma" content="no-cache" />
<meta http-equiv="cache-control" content="no-cache" />
<meta http-equiv="expires" content="0" />
<%@ include file="/WEB-INF/jsp/common/css.jsp" %>
<%@ include file="/WEB-INF/jsp/common/script.jsp" %>
<link rel="stylesheet" type="text/css" href="${ctx}/resources/css/cos.css?${version}" />
<script type="text/javascript" src="${ctx}/resources/script/cos/custom.js?${version}"></script>
</head>
<body>
	<div id="main" class="clearfix" style="">
		<h1 class="title">
			<!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->
			&nbsp;<span>自定义套餐</span>
		</h1>

		<form id="DetailForm" method="post" action="${ctx}/cos/save">

			<table width="100%" cellspacing="0" cellpadding="0" border="0"
				class="edit_table">
				<tbody>
					<tr>
						<th width="30%">套餐类型<font color="red">*</font></th>
						<td><select id="type" name="type" style="width: 53%">								
								<option value="1" selected="selected" >正式套餐</option>
								<option value="0">试用套餐</option>
						    </select><span id="type_span"></span>
						</td>
					</tr>
					<tr>
						<th width="30%">套餐名称<font color="red">*</font></th>
						<td><input id="cosName" name="cosName" type="text"
							style="width: 150px; height: 25px; border: #DCDCDC 1px solid;"
							maxlength="20" /> <span id="cosName_span" ></span></td>
					</tr>
					<tr>
						<th width="30%">允许子账号个数<font color="red">*</font></th>
						<td><input id="userCount" name="userCount" type="text"
							style="width: 150px; height: 25px; border: #DCDCDC 1px solid;"
							maxlength="2" /> <span id="userCount_span" ></span></td>
					</tr>
					<tr>
						<th width="30%">每日最大发送量<font color="red">*</font></th>
						<td><input id="daySend" name="daySend" type="text"
							style="width: 150px; height: 25px; border: #DCDCDC 1px solid;"
							maxlength="7" /> <span id="daySend_span"></span></td>
					</tr>
					<tr>
						<th width="30%">每周最大发送量<font color="red">*</font></th>
						<td><input id="weekSend" name="weekSend" type="text"
							style="width: 150px; height: 25px; border: #DCDCDC 1px solid;"
							maxlength="8" /> <span id="weekSend_span"></span></td>
					</tr>
					<tr>
						<th width="30%">每月最大发送量<font color="red">*</font></th>
						<td><input id="monthSend" name="monthSend" type="text"
							style="width: 150px; height: 25px; border: #DCDCDC 1px solid;"
							maxlength="9" /> <span id="monthSend_span"></span></td>
					</tr>
					<tr>
						<th width="30%">总共最大发送量<font color="red">*</font></th>
						<td><input id="totalSend" name="totalSend" type="text"
							style="width: 150px; height: 25px; border: #DCDCDC 1px solid;" maxlength="9" />
							 <span id="totalSend_span"></span></td>
					</tr>
				</tbody>
			</table>
			<div id="test" style="display:none;">
			<table width="100%" cellspacing="0" cellpadding="0" border="0" class="edittable">
				<tr>
					<th width="30%">试用时间<font color="red">*</font></th>
					<td>
						<ul class="search1 clearfix m_b10">
							<li><input maxlength="40" id="startTime" style="width:150px"
								name="startTime" class="date" type="text"
								onFocus="WdatePicker({dateFmt:'yyyy-MM-dd HH:mm:ss',maxDate:'#F{$dp.$D(\'endTime\',{d:-7})}'})" /></li>
							<li>至</li>
							<li><input maxlength="40" id="endTime" style="width:150px"
								name="endTime" class="date" type="text"
								onFocus="WdatePicker({dateFmt:'yyyy-MM-dd HH:mm:ss',minDate:'#F{$dp.$D(\'startTime\',{d:7})}'})" /></li>
						</ul>
					</td>
				</tr>
				<tr>
					<th width="30%">是否开启试用提醒<font color="red">*</font></th>
					<td>
		                <input type="checkbox" id="checkbox_d1" class="chk_4" value ="1" name="remind" />		                   
					</td>
				</tr>
			</table>
			</div>
			<br/>

				<table align="center">
					<tr>
						<td><input id="btn_save" type="button" class="input_yes"
							value="保 存" title="保存" onclick="saveCos()"> <input
								type="button" class="input_no" value="关 闭" title="关闭"
								onclick="closeWin()"></td>
					</tr>
					<tr>
						<td align="center"><img style="display: none" id="prompt"
							src="${ctx}/resources/img/loading3.gif"></td>
					</tr>	
			    </table>			
		</form>
		<br />
	</div>
	<input type="hidden" name="msg" id="msg" value="${message}" />
	<input type="hidden" name="token" value="${token}" />
</body>
</html>
