$(document).ready(function(){
	$("#enSet").search({
		textId:"#enTag",
		valueId:"#enTagId",
		boxId:"#slideSearchProp",
		json:ass_prop,
		itemFn:"addAssProp(this,'#enTag',enTagVal,callReloadSum)",
		subArr:enTagVal,
		isHoriz:true,
		showSelectFn:addPropSelect
	});
	
	$("#enTag").live('keydown',function(e){
		if(e.keyCode==8){
			var item=getDeleteItem($(this)[0]);
			removeProp(enTagVal,item,"#enTag",callReloadSum);
		}
		e.preventDefault();
	});
	
	$("#outSet").search({
		textId:"#outTag",
		valueId:"#outTagId",
		boxId:"#slideSearchProp",
		json:ass_prop,
		itemFn:"addAssProp(this,'#outTag',outTagVal,callReloadSum)",
		subArr:outTagVal,
		isHoriz:true,
		showSelectFn:addPropSelect
	});
	
	$("#outTag").live('keydown',function(e){
		if(e.keyCode==8){
			var item=getDeleteItem($(this)[0]);
			removeProp(outTagVal,item,"#outTag",callReloadSum);
		}
		e.preventDefault();
	});
	
	//初始化属性人数
	callReloadSum();
	
});
