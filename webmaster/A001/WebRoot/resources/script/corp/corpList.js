$(document).ready(function(){
	$.fn.zTree.init($("#treeDemo"), setting, zNodes);

	$("#treeDemo a").live('mouseenter',function(){
		if(!$(this).hasClass('curSelectedNode')){
			$(this).addClass('hide_edit');
		}
	})
});

var setting = {
	view: {
		addHoverDom: addHoverDom,
		removeHoverDom: removeHoverDom,
		selectedMulti: false,
		dblClickExpand: false
	},
	edit: {
		enable: true,
		editNameSelectAll: true,
		removeTitle: '删除',
		renameTitle: '修改'
	},
	data: {
		simpleData: {
			enable: true
		}
	},
	callback: {
		beforeDrag: beforeDrag,
		beforeEditName: beforeEditName,
		beforeRemove: beforeRemove,
		beforeRename: null,
		onRemove: null,
		onRename: null,
		onClick: zTreeOnClick
	}
};

var log, className = "dark";

//点击
function zTreeOnClick(event, treeId, treeNode) {
    //alert(treeNode.tId + ", " + treeNode.name + ", " + treeNode.id+ ", " + treeNode.pid);
    var $treeNode = $('#'+treeNode.tId+'_a');
    $treeNode.removeClass('hide_edit');
    //显示机构信息，不可编辑
	var pId = treeNode.pId ?  treeNode.pId : 0;
    getCorpHtml('#corpPad','/corp/corpView',{pId:pId,id:treeNode.id,type:'show'});
};

function beforeDrag(treeId, treeNodes) {
	return false;
}

function beforeEditName(treeId, treeNode) {
	className = (className === "dark" ? "":"dark");
	var zTree = $.fn.zTree.getZTreeObj("treeDemo");
	zTree.selectNode(treeNode);
	if(!confirm("进入节点 -- " + treeNode.name + " 的编辑状态吗？")){
		return false;
	}else{
		//修改机构	
		glb.fun = 'update';
		glb.node = treeNode;
		var pId = treeNode.pId ? treeNode.pId : 0;
		getCorpHtml('#corpPad','/corp/corpEdit',{pId:pId,id:treeNode.id,type:'update'});
		return false;
	}
}

function beforeRemove(treeId, treeNode) {
	className = (className === "dark" ? "":"dark");
	var zTree = $.fn.zTree.getZTreeObj("treeDemo");
	zTree.selectNode(treeNode);
	if(!confirm("确认删除 节点 -- " + treeNode.name + " 吗？")){
		return false; 
	}else{
		//删除机构
		if(treeNode.children){
			alert('当前机构下有子机构,不能删除！');
			return false;
		}
		
		if(treeNode.name == '默认机构'){
			alert('不能删除默认机构！');
			return false;
		}
		
		if(zTree.getNodes().length <= 1){
			alert('必须保留一个机构');
			return false;
		}
		top.loading();
		return crud('/corp/delete',{id:treeNode.id,type:'delete'},fnlist.remove);
	}	
}

function getTime() {
	var now= new Date(),
	h=now.getHours(),
	m=now.getMinutes(),
	s=now.getSeconds(),
	ms=now.getMilliseconds();
	return (h+":"+m+":"+s+ " " +ms);
}

function addHoverDom(treeId, treeNode) {
	var sObj = $("#" + treeNode.tId + "_span");
	if (treeNode.editNameFlag || $("#addBtn_"+treeNode.tId).length>0) return;
	var addStr = "<span class='button add' id='addBtn_" + treeNode.tId
		+ "' title='增加子机构' onfocus='this.blur();'></span>";
	sObj.after(addStr);
	var btn = $("#addBtn_"+treeNode.tId);
	if (btn) btn.bind("click", function(){	
		if(!confirm("要增加 -- " + treeNode.name + " 子机构吗？")){
			return false;
		}
		
		//新建子机构	
		if(treeNode.level >= 5){
			alert('机构最大层级为6级');
			return false;
		}
		glb.fun = 'addSub';
		glb.node = treeNode;
		getCorpHtml('#corpPad','/corp/createCorp',{pId:treeNode.id,type:'add'});
		return false;
	});
};

function removeHoverDom(treeId, treeNode) {
	$("#addBtn_"+treeNode.tId).unbind().remove();
};

//删除
function crud(url,info,fn){	
	var flag;
	$.ajax({
		url: url,
		type: "post",
		data:info,
		//锁住代码
		async:false,
		dataType: "json",
		success: function(data) {
			//后台返回
			//data = {code:1,id:2255,pid:0,name:'新建机构1',type:'add'};
			top.del_loading();
			flag = true;	
			if(data.code == 1){
				fnlist.remove(data);
				error_message(data.message,0);
			}else{
				error_message(data.message,1);
			}
		},
		error:function(){
			top.del_loading();
			alert('请求失败！');
			flag = false;	
		}		
	});
	return flag;
}

//新建一级机构
function addItem(){
	glb.fun = 'addPrt';
	getCorpHtml('#corpPad','/corp/createCorp',{pId:0,type:'add'});
}

//获取html
function getCorpHtml(boxId,url,data){
	var $boxId = $(boxId);
	$boxId.hide();
	if(url.indexOf("?")>0){
		url=url+"&rnd="+new Date().getTime();
	}else{
		url=url+"?rnd="+new Date().getTime();
	}
	
	$boxId.load(url, data, function(){	
		var boxW = $boxId.width();
		var boxH = $boxId.height();
		$boxId.fadeIn();
	});
}

//回调函数列表
var fnlist = {
	//添加一级机构
	addPrt : function(data){
		var zTree = $.fn.zTree.getZTreeObj("treeDemo");
		var newNode = {id:data.id,name:data.name};
		zTree.addNodes(null, newNode);
		//通过id找到新建的节点
		var node = zTree.getNodeByParam("id", newNode.id);
		zTree.selectNode(node);
		var $treeNode = $('#'+node.tId+'_a');
    	$treeNode.trigger('click');
	},
	//添加子机构
	addSub : function(data){
		var zTree = $.fn.zTree.getZTreeObj("treeDemo");
		var newNode = {id:data.id,name:data.name,pId:data.pId};
		var treeNode = glb.node;
		zTree.addNodes(treeNode, newNode);
		//通过id找到新建的节点
		var node = zTree.getNodeByParam("id", newNode.id);
		zTree.selectNode(node);
		var $treeNode = $('#'+node.tId+'_a');
    	$treeNode.trigger('click');
	},
	//修改机构
	update : function(data){
		var zTree = $.fn.zTree.getZTreeObj("treeDemo");
		var treeNode = glb.node;
		zTree.editName(treeNode);
		$("#"+treeNode.tId+'_input').val(data.name);
		$("#"+treeNode.tId+'_input').blur();
	},
	//删除机构
	remove : function(){
		$("#corpPad").html('');	
	}
	
}
