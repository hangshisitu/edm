<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>过滤管理</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/common/jquery.form-min.js?${version}"></script>
	<script type="text/javascript" src="${ctx}/resources/script/filter/recipientPage.js?${version}"></script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		<h1 class="title">过滤管理&nbsp;<span></span></h1>
		
		<form id="ListForm" action="${ctx}/account/operateAccountList" method="post">
				<ul class="search clearfix m_b10">
				<li>邮箱地址</li>
				<li>
					<input maxlength="20" type="text" id="search_userId" name="search_userId" value="">
				</li>
				<li>更新时间</li>
				<li>
					<input maxlength="10" class="date" id="d4311" id="search_beginTimeStr" name="search_beginTimeStr" value="${search_beginTimeStr}" class="Wdate" type="text" onFocus="WdatePicker({maxDate:'#F{$dp.$D(\'d4312\')}'})"/>
				</li>
				<li>到</li>
				<li>
					 <input maxlength="10"  class="date" id="d4312"  id="search_endTimeStr" name="search_endTimeStr" value="${search_endTimeStr}" class="Wdate" type="text" onFocus="WdatePicker({minDate:'#F{$dp.$D(\'d4311\')}'})"/>
				</li>
				<li>
					<input type="button" class="groovybutton" value="查 询" title="查询" onclick="search()">
				</li>
			</ul>
		</form>
		
		<ul class="tab clearfix">
			<li class="now"><a href="page?m=red">红名单</a></li>
			<li><a href="page?m=black">黑名单</a></li>
			<li><a href="page?m=reject">拒收</a></li>
			<li><a href="page?m=report">举报</a></li>
			<li><a href="page?m=unsubscribe">退订</a></li>
		</ul>
		<div class="ribbon clearfix">
			<div class="note">
				<p>共<span><font color="blue">11111</font></span>个</p>
			</div>
			<ul class="btn">
				<li>
					<input type="button" class="groovybutton" value="新建" onclick="open_msg('/filter/recipient/add')">
				</li>
				<li>
					<input type="button" class="groovybutton" value="导入" onclick="">
				</li>
				<li>
					<input type="button" class="groovybutton" value="删除" onclick="">
				</li>
			</ul>
		</div>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="main_table">
			<tbody>
				<tr>
					<th width="25">
						<input id="all_checkbox" type="checkbox">
					</th>
					<th>邮箱地址</th>
					<th>描述</th>
					<th>创建时间</th>
					<th>更新时间</th>
					<th>操作</th>
				</tr>
				<tr>
					<td>
					<input type="checkbox" value="351" name="checkedIds">
					</td>
					<td title="">nick23@aos.cm</td>
					<td>描述如下xxxx</td>
					<td>
						2011-5-25
					</td>
					<td>
						2011-5-26
					</td>
					<td class="edit">
						<ul class="f_ul">
							<li><a href="javascript:;">修改</a></li>
							<li><a href="javascript:;">删除</a></li>
						</ul>
					</td>
				</tr>
				<tr>
					<td>
					<input type="checkbox" value="351" name="checkedIds">
					</td>
					<td title="">nick23@aos.cm</td>
					<td>描述如下xxxx</td>
					<td>
						2011-5-25
					</td>
					<td>
						2011-5-26
					</td>
					<td class="edit">
						<ul class="f_ul">
							<li><a href="javascript:;">修改</a></li>
							<li><a href="javascript:;">删除</a></li>
						</ul>
					</td>
				</tr>
					
			</tbody>
		</table>
		
		<div class="ribbon clearfix">
			<%@ include file="/WEB-INF/jsp/common/page.jsp"%> 
		</div>
		
		<input type="hidden" id="totalPageCount" value="${pagination.pageCount}">
		<input type="hidden" name="token" value="${token}" />
		<br />
	<br />
	<br />
	</form>
	</div>
	
<div class="select_div"></div>
</body>
</html>