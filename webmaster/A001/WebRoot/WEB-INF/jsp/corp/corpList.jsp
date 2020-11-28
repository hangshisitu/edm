<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>机构管理</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta http-equiv="Content-Language" content="zh-cn" />
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="expires" content="0">    
<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
<link rel="stylesheet" href="${ctx}/resources/script/common/zTree/css/zTreeStyle/zTreeStyle.css" type="text/css">
<script type="text/javascript" src="${ctx}/resources/script/common/zTree/js/jquery.ztree.core-3.5.js"></script>
<script type="text/javascript" src="${ctx}/resources/script/common/zTree/js/jquery.ztree.exedit-3.5.js"></script>
<script type="text/javascript" src="${ctx}/resources/script/corp/corpList.js?${version}"></script>
<script type="text/javascript" src="${ctx}/resources/script/common/jquery.form-min.js?${version}"></script>
<script type="text/javascript">
	
var zNodes =[
   <c:forEach items="${corpList}" var="corp" varStatus="i">
	{ id:"${corp.corpId}", pId:"${corp.parentId}", name:"${corp.company}"},
   </c:forEach>
];

var glb = {fun:null, node:null};
</script>
</head>
<body>
	<div id="main" class="clearfix" style="">
		<h1 class="title">角色管理&nbsp;<span>管理前台的账户角色</span></h1>
		<div class="content_wrap">
			<div class="ribbon clearfix">
				<div class="note">
					<p class="f_l">共<span><font color="blue">${ corpTotal }</font></span>个一级机构&nbsp;&nbsp;</p>
					<ul class="btn">
					<li>
						<input type="button" onclick="addItem();"  value="添加一级机构" class="groovybutton"/>
					</li>
					</ul>
				</div>
			</div>
			<!--树形菜单区域-->
			<div class="zTreeDemoBackground f_l" style="width:400px">
				<ul id="treeDemo" class="ztree"></ul>
				<div style="height:50px;"></div>
			</div>
			<!--新建修改区域-->
			<div id="corpPad" class="corp_pad f_l" style="width:450px">
				<p style="margin-top:60px">点击机构名可预览配置信息</p>
			</div>
		</div>
	</div>
</body>
</html>
