<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>ip投递日志</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/delivery/delivery.ruleCustom.js?${version}"></script>
	<script type="text/javascript">
		var props=new Array(
			["", "请选择"],
			["qq.com", "qq.com"],
			["126.com", "126.com"],
			["sina.com", "sina.com"],
			["hotmail.com", "hotmail.com"],
			["yeah.net", "yeah.net"],
			["tom.com", "tom.com"],
			["sohu.com", "sohu.com"],
			["Gmail.com", "Gmail.com"],
			["foxmail.com", "foxmail.com"],
			["yahoo.com", "yahoo.com"],
			["其他", "其他"]
		);
		var prop_text = "";
		var len = 0;
		var total = 0; 
	</script>
  </head>
  
  <body>
  	<div id="main" class="clearfix" style="">
		<h1 class="title">
		<span class="f_r">
			<a href="${ctx}/delivery/sendDomainMonitor">
				<font color="blue">返回>></font>
			</a>
		</span>
			收件域投递规则配置
		<span></span>
		</h1>
		<form method="post" action="" id="ruleForm">
			<ul class="tab clearfix">
				<li><a href="${ctx}/delivery/ruleSet">默认设置</a></li>
				<li class="now"><a href="${ctx}/delivery/ruleCustomSet">个性化设置</a></li>
			</ul>
			<div class="ribbon clearfix">
				<div class="note">
					<p>针对具体的发件域进行特定的收件域投递规则配置</p>
				</div>
			</div>
			<table width="100%" cellspacing="0" cellpadding="0" border="0" class="user_table rules_table m_t10">
                <tbody>
					<tr>
						<td width="60" align="center">发件域</td>
						<td colspan="300">
							<input type="text" value="" name="senderDomain"  id="senderDomain" style="width:270px"/>
						</td>
						<td>
							&nbsp;
						</td>
					</tr>
			</table>
			<table width="100%" cellspacing="0" cellpadding="0" border="0" class="user_table rules_table m_t10" id="props">
                <tbody>
					<tr>
						<td width="60" align="center">收件域</td>
						<td width="150">
							<a style="width:100px" class="a_select b_select" id="prop_list_0">请选择</a>
							<input type="hidden" value="" name="prop_0" id="prop_0" class="i_select">
						</td>
						<td width="460">
							每15分钟<input type="text" value="" name="mins_0" class="add_text" id="mins_0"/><em>封</em>每小时<input type="text" value="" name="hours_0" class="add_text" id="hours_0"/><em>封</em>每天<input type="text" value="" name="days_0" class="add_text" id="days_0"/><em>封</em>
						</td>
						<td>
							<span onclick="add_prop()" class="input_set add_set">+</span>
						</td>
					</tr>
				</tbody>
			</table>
			<table width="100%" cellspacing="0" cellpadding="0" border="0" class="user_table rules_table m_t20">
                <tbody>
					<tr>
						<td width="60" align="center">&nbsp;</td>
						<td>
							<ul class="f_ul">
								<li><input type="submit" value="保存" class="input_yes" /></li>
								<li><input type="button" onclick="back()" value="取消" class="input_no" /></li>
							</ul>
						</td>
					</tr>
				</tbody>
			</table>
		</form>
		<br>
		<br>
		<br>
		<div style="height:20px;"></div>
	</div>
	<div class="select_div"></div>
  </body>
</html>
