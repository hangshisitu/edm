<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>套餐</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/cos/cosList.js?${version}"></script>
	<script type="text/javascript">
        function back(){
        //	 var url = $("#backUrl").val();
	         var url = "/cos/cosList";
	         window.location.href = url;
        }
    </script>
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		
		<form id="DetailForm" method="post">
		<ul class="tab clearfix">
			<li class="now"><a href="javascript:void(0);" >查看套餐</a></li>
		</ul>
		<h3 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> --><span>基本信息</span></h3>
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="edit_table">
			<tbody>
				<tr>
					<th width="30%">套餐名</th>
					<td>
						${cos.cosName}
					</td>
					
				</tr>
				<tr>
					<th width="30%">套餐类型</th>
					<td>
						<c:choose>
						     <c:when test="${cos.type == 1}">正式</c:when>
						     <c:when test="${cos.type == 0}">测试</c:when>
						     <c:otherwise>无效</c:otherwise>
						</c:choose>
					</td>
					
				</tr>
				<tr>
					<th width="30%">允许最大子账号</th>
					<td>
						${cos.userCount}
					</td>
				</tr>
				<tr>
					<th width="30%">日发送量</th>
					<td>
						${cos.daySend}
					</td>
				</tr>
				<tr>
					<th width="30%">周发送量</th>
					<td>
						${cos.weekSend}
					</td>
				</tr>
				<tr>
					<th width="30%">月发送量</th>
					<td>
						${cos.monthSend}
					</td>
				</tr>
				<tr>
					<th width="30%">总发送量</th>
					<td>
						${cos.totalSend}
					</td>
				</tr>
				<tr>
					<th width="30%">状态</th>
					<td>
						<c:choose>
						     <c:when test="${cos.status == 1}">有效</c:when>
						     <c:when test="${cos.status == 0}">冻结</c:when>
						     <c:otherwise>无效</c:otherwise>
						</c:choose>
					</td>
				</tr>
				<tr>
					<th width="30%">创建时间</th>
					<td>
						<fmt:formatDate value="${cos.createTime}" pattern="yyyy-MM-dd HH:mm"/>
					</td>
				</tr>
				</tbody>
		</table>
		<c:choose>
		<c:when test="${cos.type == 0}">
		<table width="100%" cellspacing="0" cellpadding="0" border="0" class="edit_table">		    				
				<tr>				    
					<th width="30%">试用时间</th>
					<td>					    
						<fmt:formatDate value="${cos.startTime}" pattern="yyyy-MM-dd HH:mm"/>   
						 至  <fmt:formatDate value="${cos.endTime}" pattern="yyyy-MM-dd HH:mm"/>												
					</td>					
				</tr>
		</table>
		</c:when>
		</c:choose>
		
		<br/>
		<table align="center">
			<tr>
				<td>
				<!-- 					
					<sec:authorize ifAnyGranted="ROLE_ADM">
					   <input id="btn_submit" type="button" class="input_yes" value="激 活" title="激活" onclick="activate('${ctx}/account/activateAsny?userId=${user.userId}','${user.userId}')"/>
					</sec:authorize>
				 -->
					<input type="button" class="input_no" value="返 回" title="返回" onclick="back()" />
				</td>
			</tr>
			<tr>
				<td align="center">
					<img style="display:none" id="prompt" src="${ctx}/resources/img/loading3.gif" /> 
				</td>
			</tr>
		</table>
		<input type="hidden" name="token" value="${token}" />
		<input type="hidden" name="msg" id="msg" value="${message}" />

		
	</form>
	<br>
	</div>
  </body>
</html>
