<%@ page language="java" pageEncoding="UTF-8"%>
<ul class="page">
	<li>每页显示</li>
	<li><select name="pageSize" onchange="paginationQuery('First')">
										<option vlaue="20"
											<c:if test="${pagination.pageSize==20}">selected="selected"</c:if>>20</option>
										<option vlaue="40"
											<c:if test="${pagination.pageSize==40}">selected="selected"</c:if>>40</option>
										<option vlaue="80"
											<c:if test="${pagination.pageSize==80}">selected="selected"</c:if>>80</option>
										<option vlaue="100"
											<c:if test="${pagination.pageSize==100}">selected="selected"</c:if>>100</option>
								</select></li>
	<c:if test="${pagination.firstPage}"><li><a href="javascript:paginationQuery('First')">首页</a></li></c:if>
	<c:if test="${!pagination.firstPage}"><li>首页</li></c:if>
	<c:if test="${pagination.hasPrePage}"><li><a href="javascript:paginationQuery('Previous')">上一页</a></li></c:if>
	<c:if test="${!pagination.hasPrePage}"><li>上一页</li></c:if>
	<c:if test="${pagination.hasNextPage}"><li><a href="javascript:paginationQuery('Next')">下一页</a></li></c:if>
	<c:if test="${!pagination.hasNextPage}"><li>下一页</li></c:if>
	<c:if test="${pagination.lastPage}"><li><a href="javascript:paginationQuery('Last')">末页</a></li></c:if>
	<c:if test="${!pagination.lastPage}"><li>末页</li></c:if>
	<li>共<font color="blue">${pagination.pageCount}</font>页</li>
	<li>转到</li>
	<li><input type="text" id="jumpPageNo" size="1" maxlength="10" onkeypress="return event.keyCode>=48&&event.keyCode<=57"
									onpaste="return !clipboardData.getData('text').match(/\D/)"
									ondragenter="return false" style="ime-mode:Disabled" value="${pagination.currentPage}"></li>
	<li><input type="button" value="GO" onclick="paginationQuery('go')"></li>
</ul>
<ul class="btn"></ul>