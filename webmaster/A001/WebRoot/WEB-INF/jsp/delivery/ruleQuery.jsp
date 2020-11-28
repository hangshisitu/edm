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
	<script type="text/javascript" src="${ctx}/resources/script/delivery/delivery.query.js?${version}"></script>
  </head>
  
  <body>
     <div id="main" class="clearfix">
		<h1 class="title">
			发件域配置查询
			<span></span>
		</h1>
		<form method="post" action="${ctx}/delivery/sendDomainMonitor" id="queryForm">
			<ul class="search clearfix m_b10">
				<li>发件域</li>
				<li style="width:12.5%">
					<input maxlength="64" type="text" id="search_fromDomain" name="search_fromDomain" value="">
				</li>
				<li>更新时间</li>
				<li>
					<input type="text" onfocus="WdatePicker({maxDate:'#F{$dp.$D(\'d4312\')}'})" value="" name="search_beginTimeStr" id="d4311" class="date" maxlength="10">
				</li>
				<li>到</li>
				<li>
					 <input type="text" onfocus="WdatePicker({minDate:'#F{$dp.$D(\'d4311\')}'})" value="" name="search_endTimeStr" id="d4312" class="date" maxlength="10">
				</li>
				<li>
					<input type="button" class="groovybutton" value="查 询" title="查询" onclick="search()">
				</li>
				<li>	
					<input type="button" class="groovybutton" value="重 置" title="重置" onclick="resetParam()">
				</li>
			</ul>
			<div class="ribbon clearfix">
				<div class="note">
					<p>共<font color="blue">100</font>记录</p>
				</div>
			</div>
			<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table delevery_table">
				<thead>
					<tr>
						<th>发件域</th>
						<th>创建时间</th>
						<th>更新时间</th>
						<th>操作</th>
					</tr>
				</thead>
				<tbody>
					<tr>
						<td>qq.com</td>
						<td>2013-08-02 14:16</td>
						<td>
							2013-08-02 14:16
						</td>	
						<td class="edit">
							<ul class="f_ul">
								<li><a href="javascript:;">修改</a></li>
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
  </body>
</html>
