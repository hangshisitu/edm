<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<title>API群发发送统计</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
		<meta http-equiv="Content-Language" content="zh-cn" />
		<meta http-equiv="pragma" content="no-cache">
		<meta http-equiv="cache-control" content="no-cache">
		<meta http-equiv="expires" content="0">
	
		<%@ include file="/WEB-INF/jsp/common/css.jsp"%>
		<%@ include file="/WEB-INF/jsp/common/script.jsp"%>
		<script type="text/javascript" src="${ctx}/resources/script/report/getDateRange.js?${version}"></script>
		<script type="text/javascript" src="${ctx}/resources/script/report/api/api.groups.list.js?${version}"></script>
	</head>
	
	<body>
		<div id="main" class="clearfix" style="position: relative">
			<h1 class="title">
				API群发发送统计&nbsp;<span></span>
			</h1>
			<form id="ListForm" action="${ctx}/apiReport/apiGroupsList" method="get">
				<input type="hidden" name="apiType" value="apiGroups" />
				<input type="hidden" name="taskType" value="${taskType}" />
				
				<ul class="search clearfix m_b00">
					<li>任务名称</li>
					<li>
						<input maxlength="20" type="text" id="search_taskName" name="search_taskName" value="${search_taskName}">
					</li>
					<li>机构名称</li>
					<li>
						<input maxlength="20" type="text" id="search_corpPath" name="search_corpPath" value="${search_corpPath}">
					</li>
					<li>创建时间</li>
					<li>
						<input maxlength="10" class="date" id="d5311" name="search_beginTimeStr" value="${search_beginTimeStr}" onFocus="WdatePicker({maxDate:'#F{$dp.$D(\'d5312\')}'})" />
					</li>
					<li style="width: 3%">到</li>
					<li>
						<input maxlength="10" class="date" id="d5312" name="search_endTimeStr" value="${search_endTimeStr}" class="Wdate" type="text" onFocus="WdatePicker({minDate:'#F{$dp.$D(\'d5311\')}'})" />
					</li>
					<li style="width: 10%">
						<select id="search_cycle2" onchange="getDateRange(this.value,'d5311','d5312')" name="search_cycle2" style="width: 100%; background-color: #F5F5F5">
							<option value="">选择周期</option>
							<option <c:if test="${search_cycle2==6}">selected="selected"</c:if> value="6">近一周</option>
							<option <c:if test="${search_cycle2==30}">selected="selected"</c:if> value="30">近一月</option>
							<option <c:if test="${search_cycle2==91}">selected="selected"</c:if> value="91">近一季度</option>
							<option <c:if test="${search_cycle2==365}">selected="selected"</c:if> value="365">近一年</option>
						</select>
					</li>
					<c:if test="${hrCatalog == true}">
					<li>数据状态</li>
					<li style="width:10%">
						<select id="search_labelStatus" name="search_labelStatus" style="width:100%;background-color: #F5F5F5">
							<option value="">全部</option>
							<option <c:if test="${search_labelStatus==0}">selected="selected"</c:if> value="0">未导入</option>
							<option <c:if test="${search_labelStatus==2}">selected="selected"</c:if> value="2">处理中</option>
							<option <c:if test="${search_labelStatus==1}">selected="selected"</c:if> value="1">已导入</option>
							<option <c:if test="${search_labelStatus==3}">selected="selected"</c:if> value="3">导入失败</option>
						</select>
					</li>
					</c:if>
					<li>
						<input type="button" class="groovybutton" value="查 询" title="查询" onclick="search()">
					</li>
				</ul>
				<ul class="tab clearfix">
					<li><a href="${ctx}/apiReport/apiTriggerList">触发任务</a></li>
					<li class="now"><a href="${ctx}/apiReport/apiGroupsList">群发任务</a></li>
				</ul>
				<div class="ribbon clearfix">
					<div class="note">
						<p>
							共<span><font color="blue">${pagination.recordCount}</font></span>个
						</p>
					</div>
					<ul class="btn">
						<li>
							<input type="button" class="groovybutton" value="对 比" title="对比" onclick="compareTask('${ctx}/taskDetailStat/compareTask')">
						</li>
						<li>
							<input type="button" class="groovybutton" value="导 出" title="导出" onclick="exportTaskStatNew('${ctx}/taskReport/exportTaskStatNew')">
						</li>
						<li>
							<input type="button" class="groovybutton" value="打 印" title="打印" onclick="printTaskStat('${ctx}/taskReport/printTaskStat')">
						</li>
					</ul>
				</div>
				<table width="100%" cellspacing="0" cellpadding="0" border="0"
					class="main_table">
					<tbody>
						<tr>
							<th width="25"><input type="checkbox" id="all_checkbox" /></th>
							<th>任务名称</th>
							<th>机构名称</th>
							<th>发送总数</th>
							<th>成功总数</th>
							<th>弹回总数</th>
							<th>打开人数</th>
							<th>打开次数</th>
							<th>点击人数</th>
							<th>点击次数</th>
							<th>退订总数</th>
							<th>转发总数</th>
							<th>无效人数</th>
							<c:if test="${hrCatalog == true}">
							<th>数据状态</th>
							</c:if>
							<th width="150px;">操作</th>
						</tr>
						<c:forEach items="${pagination.recordList}" var="r" varStatus="i">
							<tr>
								<td><input type="checkbox" name="checkedIds" value="${r.taskId}" /></td>
								<td title="${r.taskName}">${r.taskName}</td>
								<td title="${r.corpPath}">${r.corpPath}</td>
								<td>${r.sentCount}</td>
								<td>${r.reachCount}</td>
								<td>${r.softBounceCount+r.hardBounceCount}</td>
								<td>${r.readUserCount}</td>
								<td>${r.readCount}</td>
								<td>${r.clickUserCount}</td>
								<td>${r.clickCount}</td>
								<td>${r.unsubscribeCount}</td>
								<td>${r.forwardCount}</td>
								<td>${r.afterNoneCount}</td>
								<c:if test="${hrCatalog == true}">
								<td>
									<c:if test="${r.labelStatus eq 0 || r.labelStatus eq null}">未导入</c:if>
									<c:if test="${r.labelStatus eq 1 }">已导入</c:if>
									<c:if test="${r.labelStatus eq 2 }">处理中</c:if>
									<c:if test="${r.labelStatus eq 3 }">导入失败</c:if>
								</td>
								</c:if>
								<td class="edit">
									<ul class="f_ul">
										<li>
											<li><a href="javascript:view('${ctx}/task/view?taskId=${r.taskId}')">查看</a></li>
											<li><a href="javascript:detail('${ctx}/taskDetailStat/overview?taskId=${r.taskId}&taskType=${taskType}')">报告</a></li>
											<li><a href="javascript:exportOneTaskStat('${ctx}/taskReport/exportTaskStat?checkedIds=${r.taskId}&taskType=${taskType}')">导出</a></li>
											<c:if test="${(r.labelStatus eq 0 || r.labelStatus eq null|| r.labelStatus eq 3) && (r.isCanImport eq 'Y') && (hrCatalog == true)}">
												<li><a href="javascript:importPropLib('${ctx}/label/import?taskId=${r.taskId}')">导入</a></li>
											</c:if>
										</li>
									</ul>
								</td>
							</tr>
						</c:forEach>
	
					</tbody>
				</table>
	
				<c:if test="${pagination.recordList==null||fn:length(pagination.recordList)<1}">
					<div class="no_data">没有匹配的数据</div>
				</c:if>
	
				<div class="ribbon clearfix">
					<%@ include file="/WEB-INF/jsp/common/page.jsp"%>
				</div>
	
				<%--当前页--%>
				<input type="hidden" id="currentPage" name="currentPage" value="${pagination.currentPage}">
				<%--总页数--%> 
				<input type="hidden" id="totalPageCount" value="${pagination.pageCount}">
				<input type="hidden" name="fromFlag" id="fromFlag" value="apiGroupsByAll">
				<input type="hidden" name="type" id="type" value="4">
			</form>
			<input type="hidden" name="msg" id="msg" value="${message}"> 
			<input type="hidden" name="listUrl" id="listUrl" value="${ctx}/apiReport/apiGroupsList">
			<br /> <br /> <br />
		</div>
	</body>
</html>
