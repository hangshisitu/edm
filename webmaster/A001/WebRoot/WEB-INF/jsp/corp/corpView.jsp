<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<table width="100%" cellspacing="0" cellpadding="0" border="0" class="pop_table" style="">
	<tbody>
		<tr>
			<td width="100" align="right">
				机构名称：
			</td>
			<td width="10" class="red">&nbsp;</td>
			<td>
				${corp.company}
			</td>
		</tr>
		<tr>
			<td align="right">
				套餐类型：
			</td>
			<td width="10" class="red">&nbsp;</td>
			<td>
				${cos.cosName}
			</td>
		</tr>
		<tr>
			<td align="right">
			</td>
			<td width="10" class="red">&nbsp;</td>
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
			<td width="10" class="red">&nbsp;</td>
			<td>
				<c:forEach items="${resourceList}" var="res" varStatus="status">
				   <c:if test="${corp.formalId==res.resourceId}">${res.resourceName}</c:if>
				</c:forEach>
			</td>
		</tr>
		<tr>
			<td align="right">
				测试通道：
			</td>
			<td width="10" class="red">&nbsp;</td>
			<td>
				<c:forEach items="${resourceList}" var="res">
					<c:if test="${corp.testedId==res.resourceId}">${res.resourceName}</c:if>
				</c:forEach>
			</td>
		</tr>
		<tr>
			<td align="right">
				网络发件人：
			</td>
			<td width="10" class="red">&nbsp;</td>
			<td>
				<c:if test="${robotsList!=null}">
				   <c:forEach items="${robotsList}" var="r" varStatus="status">
				      ${r.email} </br>
				   </c:forEach>
				</c:if>
			</td>
		</tr>
		<%-- <tr>
			<td align="right">
				短信设置：
			</td>
			<td width="10" class="red">&nbsp;</td>
			<td>
				<c:if test="${corp.openSms == 1}">需要</c:if>
				<c:if test="${corp.openSms == 0}">不需要</c:if>
			</td>
		</tr> --%>
		<tr>
			<td align="right">
				发件人验证：
			</td>
			<td width="10" class="red">&nbsp;</td>
			<td>
				<c:if test="${corp.senderValidate == 1}">需要</c:if>
				<c:if test="${corp.senderValidate == 0}">不需要</c:if>
			</td>
		</tr>
		<%-- <tr>
			<td align="right">
				系统侧审核：
			</td>
			<td width="10" class="red">&nbsp;</td>
			<td>
				<c:if test="${corp.auditPath != ''}">需要</c:if>
				<c:if test="${corp.auditPath == ''}">不需要</c:if>
			</td>
		</tr> --%>
		<tr>
			<td align="right">
				API投递业务：
			</td>
			<td width="10" class="red">&nbsp;</td>
			<td>
				<c:if test="${apiTrigger ne null and apiTrigger.status==1}">开通触发业务</c:if> </br>
				<c:if test="${corp.joinApi == 1}">开通群发业务</c:if>
			</td>
		</tr>
	</tbody>
</table>