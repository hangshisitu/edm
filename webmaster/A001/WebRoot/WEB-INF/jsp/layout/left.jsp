<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<ul id="nav">
	<li class="subwrap">
		<a class="sublink" href="javascript:void(0)">账号管理</a>
		<ul>		    
			<c:if test="${roleCatalog == true}">
			<li><a href="${ctx}/role/roleList" target="main">角色管理</a></li>
			</c:if>
			<li><a href="${ctx}/account/accountList" target="main">账号列表</a></li>
			<li><a href="${ctx}/corp/corpList" target="main">机构管理</a></li>
			<li><a href="${ctx}/cos/cosList" target="main">套餐管理</a></li>
		</ul>
	</li>
	<li class="subwrap">
		<a class="sublink" href="javascript:void(0)">任务管理</a>
		<ul>
			<li><a href="${ctx}/task/unauditList" target="main">待审核任务</a></li>
			<li><a href="${ctx}/task/auditedList" target="main">已审核任务</a></li>
			<li><a href="${ctx}/task/needlessAuditList" target="main">不需要审核任务</a></li>
		</ul>
	</li>
	<li><a href="${ctx}/resource/resUserList" target="main">通道管理</a></li>
	<li class="subwrap">
		<a class="sublink" href="javascript:void(0)">数据报告</a>
		<ul>
			<li><a href="${ctx}/historyReport/list" target="main">历史发送总览</a></li>
			<li><a href="${ctx}/senderDomain/list" target="main">域名发送总览</a></li>
			<li><a href="${ctx}/customerReport/list" target="main">客户发送统计</a></li>
			<li><a href="${ctx}/taskReport/list?type=0" target="main">任务发送统计</a></li>
			<li><a href="${ctx}/campaign/list" target="main">活动发送统计</a></li>
			<li><a href="${ctx}/apiReport/apiTriggerList" target="main">API发送统计</a></li>
		</ul>
	</li>
	<li class="subwrap">
		<a class="sublink" href="javascript:void(0)">投递管理</a>
		<ul>
			<%-- <li><a href="${ctx}/delivery/sendDomainMonitor" target="main">发件域监控</a></li> --%>
			<c:if test="${hrCatalog == true}">
			   <li><a href="${ctx}/delivery/ipDeliveryLog" target="main">ip投递日志</a></li>
			</c:if>
			<li><a href="${ctx}/task/priority/taskPriorityList" target="main">任务优先级</a></li>
			<c:if test="${isShowSmtpCatalog == true}">
				<li><a href="${ctx}/delivery/smtp/smtpList" target="main">SMTP设置</a></li>
			</c:if>
			
		</ul>
	</li>
	
	<%-- <li class="subwrap">
		<a class="sublink" href="javascript:void(0)">ISP备案管理</a>
		<ul>
			<li><a href="${ctx}/tengxun/list" target="main">腾讯备案管理</a></li>
			<li><a href="${ctx}/wangyi/list" target="main">网易备案管理</a></li>
			<li><a href="${ctx}/sina/list" target="main">新浪备案管理</a></li>
		</ul>
	</li> --%>
	
<%-- 	<li>
		<a class="sublink" href="${ctx}/filter/recipient/page?m=red" target="main">过滤管理</a>
	</li>
	<c:if test="${hrCatalog == true}">
	   <li><a href="${ctx}/property/propertyIndex" target="main">属性库</a></li>
	</c:if> --%>
	<li><a href="${ctx}/history/myHistory" target="main">系统管理</a></li>
	
</ul>