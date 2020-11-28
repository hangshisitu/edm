$(document).ready(function() {
	//新建记忆填充历史记录,注意位置在所有下拉初始化之前
	if( $('input[name=taskId]').val() == '' ){
		var isTemplateRefer = (location.href).indexOf('templateId=') > -1;
		//模板列表新建任务入口,去除模板和主题记忆
		if(isTemplateRefer){
			delete cookieArr.templateId;
			delete cookieArr.subject;
		}
		
		for(key in cookieArr){ 
			//如果值为空也不填充
			if(cookieArr[key] != ''){
				$('input[name='+ key +']').val(decodeURIComponent(cookieArr[key]));
			}	
		}	
	}
	
	//状态选择
	$("#status_list").UIselect({
		width: 118,
		id_id: "#m",
		arrdata: new Array(["all", "全部"], ["proofing", "待处理"], ["waited", "待发送"], ["processing", "发送中"], ["completed", "已发送"])
	});
	
	//插入变量打开
	$('#insertMore').click(function() {
		$(this).toggleClass("swtpand");
		$(this).toggleClass("swtclose");
		$(this).parents("tr").next().toggle();
	}); 
	
	//自定义信息打开
	$('#senderMore').click(function() {
		$(this).toggleClass("swtpand");
		$(this).toggleClass("swtclose");
		$("#advancedOption").toggle();
	}); 

	//企业
	$("#bind_corp_list").search({
	 	valueId:"#bindCorpId",
		boxId:"#slideSearch5",
		json:bind_corp_list,
		itemFn:"pickData(this,'#bind_corp_list')",
		cookieArr:cookieArr
	}); 
	
	//通道
	$("#resource_list").search({
	 	valueId:"#resourceId",
		boxId:"#slideSearch5",
		json:resource_list,
		itemFn:"pickData(this,'#resource_list')",
		cookieArr:cookieArr
	}); 
	
	 //收件人昵称
	$("#receiver_list").search({
		valueId:"#receiver",
		boxId:"#slideSearch5",
		json:receiver_list,
		itemFn:"pickData(this,'#receiver_list')",
		cookieArr:cookieArr
	});
	 
	 //短信提醒
	$("#sms_list").UIselect({
		width:390,
		id_id:"#sms",
		arrdata:new Array([0,"不带短信通知"],[19,"自主短信通知"],[11,"普通短信通知"],[14,"长短信通知"],[15,"免提短信通知"],[13,"WapPush"])
	}); 
	
	 //属性判断
	$("#judge_list").UIselect({
		width:390,
		id_id:"#judge",
		arrdata:new Array([0, "请选择属性判断的行为"], [1, "投递邮件"], [2, "打开邮件"])
	}); 
	
	//网络发件人
	$("#robot").search({
		valueId:"#robotId",
		boxId:"#slideSearch",
		json:robot_list,
		itemFn:"pickExclusion(this,'#robot',robotLimit)",
		cookieArr:cookieArr,
		deleteFn:reduceArr
	});
	
	//生成搜索
	$("#template").search({
		valueId:"#templateId",
		boxId:"#slideSearch",
		json:templateData,
		itemFn:"postSubject(this,'#template')",
		cookieArr:cookieArr,
		deleteFn:function(e){
			if(e.keyCode==8){
				$(this).val('');
				$(this).next().val('');
				$("#subject").val('');		
			}
			e.preventDefault();
		}
	});
	
	 //发送方式
	$("input[name=sendCode]").change(function() {
		var selectVal = $("input[name=sendCode]:checked").val();
		//定时选项显示
		if( selectVal == "schedule") {
			$("#jobTime").show();
		} else {
			$("#jobTime").val("").hide();
		}
		
		//周期选项显示
		if( selectVal == "plan") {
			//禁止活动
			$("#camp").prop("disabled",true);
			$("#camp").closest("tr").find("td:first-child").addClass('f666');
			$("#periodOption").show();
		} else {
			//开启活动
			$("#camp").prop("disabled",false);
			$("#camp").closest("tr").find("td:first-child").removeClass('f666');
			$("#periodOption").hide();
		}
	}); 
	
	 //时间选择
	$('.time').datetimepicker({
		changeMonth: true,
		changeYear: true,
		dateFormat:"yy-mm-dd",
		timeFormat:"hh:mm"
	}); 
	
	//关联属性选择
	if(typeof ass_prop != "undefined"){
		$("#propSet").search({
			textId:"#assProp",
			valueId:"#assPropId",
			boxId:"#slideSearchProp",
			json:ass_prop,
			itemFn:"addAssProp(this,'#assProp')",
			isHoriz:true,
			showSelectFn:addPropSelect,
			deleteFn:function(e){
				if(e.keyCode==8){
					var item=getDeleteItem(this);
					removeProp(item,"#assProp");
				}
				e.preventDefault();
			}
		});	
	}
	
	//发件人下拉
	if($("#senderEmail").attr('type') != 'text'){
		$("#sender_list").search({
			valueId:"#senderEmail",
			boxId:"#slideSearchSender",
			json:sender_list,
			itemFn:"postName(this,'#sender_list')",
			cookieArr:cookieArr
		});
	}
	
	//初始化发件人昵称
	postName();
	
	//选择活动
	 $("#camp").search({
	 	valueId:"#campId",
		boxId:"#slideSearch5",
		json:campaign_list,
		itemFn:"disPeriod(this)",
		cookieArr:cookieArr,
		deleteFn:function(e){
			if(e.keyCode==8){
				$(this).val('');
				$(this).next().val('');	
				//启用周期任务
				testSendCode();	
			}
			e.preventDefault();
		}
	}); 
	
	//如果记忆填充,处理周期发送
	testSendCode();
	
	//周期列表
	$("#period_list").UIselect({
		width:150,
		id_id:"#periodId",
		arrdata:period_list,
		ho_fun:"switchPeriod()"
	});
	//选择点
	$("#day_hour_list").UIselect({
		width:150,
		id_id:"#dayHourId",
		arrdata:hour_list
	});
	//选择周
	$("#week_day_list").UIselect({
		width:150,
		id_id:"#weekDayId",
		arrdata:week_day_list
	});
	$("#week_hour_list").UIselect({
		width:150,
		id_id:"#weekHourId",
		arrdata:hour_list
	});
	//选择月
	$("#month_day_list").UIselect({
		width:150,
		id_id:"#monthDayId",
		arrdata:day_list
	});
	$("#month_hour_list").UIselect({
		width:150,
		id_id:"#monthHourId",
		arrdata:hour_list
	});
	//初始化周期列表
	switchPeriod();

});

//有活动禁止周期发送，有文件禁止定时和周期发送
function testSendCode(){
	var str1 = $("#campId").val() || '';
	var str2 = $("#includeId").val() || '';
	var hasCamp = ( str1 != '');
	var hasFile = (str2.indexOf('EXPORT') > -1);
	
	if(hasCamp==false && hasFile==false){
		$("input[value=schedule]").prop('disabled',false).closest("div").removeClass('f666');
		$("input[value=plan]").prop('disabled',false).closest("div").removeClass('f666');
	}else if(hasCamp==false && hasFile==true){
		$("input[value=schedule]").prop('disabled',true).closest("div").addClass('f666');
		$("input[value=plan]").prop('disabled',true).closest("div").addClass('f666');
	}else if(hasCamp==true && hasFile==false){
		$("input[value=schedule]").prop('disabled',false).closest("div").removeClass('f666');
		$("input[value=plan]").prop('disabled',true).closest("div").addClass('f666');
	}else if(hasCamp==true && hasFile==true){
		$("input[value=schedule]").prop('disabled',true).closest("div").addClass('f666');
		$("input[value=plan]").prop('disabled',true).closest("div").addClass('f666');
	}
}

function insertCon(v){
	$("#subject").insertContent(v);
}

//赋值主题
function postSubject(obj,inputId){
	pickData(obj,inputId);
	var id=$("#templateId").val();
	for(var i in templateData){
		if(templateData[i].id==id){
			$("#subject").val(templateData[i].subject);
		}
	}	
}		

//赋值昵称
function postName(obj,id){
	pickData(obj,id);
	var email = $("#senderEmail").val();
	for(var i in sender_list){
		if(sender_list[i]['title'] == email){
			$("#senderName").val(sender_list[i]["info"]);
		}
	}	
}

//刷新发件人
function reloadSender(){
	$.ajax({
		url:"/account/sender/array?token=" + headers["token"],
		type:"POST",
		headers: headers,
		cache:false,
		dataType:"json",
		success:function(data){
			if(data.senderMaps != undefined){
				sender_list.length = 0;
				$.each(data.senderMaps, function(k, v) {
					sender_list.push({"id":k,"title":k,"info":v});
				});
			}
		},
		error:function(){
			
		}
	});
	reloadSelect('#sender_list');
}

//处理属性所选
function addAssProp(obj,inputId,fn){
	var itemArr = $(inputId).data('subArr');
	var tagLimit = 10;
	var cate_info = $(obj).attr("data-info");
	var val = $(obj).attr("data-id");
	var txt = $(obj).text();
	//已选的话就删除，没有选中就添加
	if($(obj).parents('li').hasClass("selected")){ 
		removeArrItem(itemArr,txt);
	}else{
		if(itemArr.length >= tagLimit){
			error_message('选择标签数不能大于' + tagLimit + '个！',1);
			return;
		}
		
		if(cate_info == "行业兴趣"){
			var new_arr = Array(val,txt,cate_info);
			filterArr(itemArr,new_arr);	
		}else{
			var new_arr = Array(val,txt);
			for(i=0;i<itemArr.length;i++){
				var id = itemArr[i][0];
				if($("a[data-id="+id+"]").attr("data-info") == cate_info){
					itemArr.splice(i,1);
				}
			}
			filterArr(itemArr,new_arr);	
		}
	}		
	addItemInput(itemArr,$(inputId));
	addPropSelect(inputId);
	if(typeof fn != 'undefined'){
		fn();
	}
}

//添加已选样式
function addPropSelect(inputId){
	$("#slideSearchProp ul li").removeClass("selected");
	var ids = $(inputId).next().val();
	if(ids == ""){return;}
	var arr = ids.split(',');
	if(arr.length < 1){return}
	for(i=0;i<arr.length;i++){	
		$("#slideSearchProp ul li a[data-id='" + arr[i] + "']").parents('li').addClass('selected');
	}	
}

//删除属性
function removeProp(item,inputId,fn){
	var itemArr = $(inputId).data('subArr');
	removeArrItem(itemArr,item);
	addItemInput(itemArr,$(inputId));
	addPropSelect(inputId);
	if(typeof fn != 'undefined'){
		fn();
	}
}

//网络发件人多选
function pickExclusion(obj,inputId,limit){
	var parentArr = $(inputId).data('subArr');
	if(typeof limit != 'undefined'){
		if(parentArr.length >= limit){
			error_message("不能超过"+ limit +"个",1);
			return;
		}
	}
	var val=$(obj).data("id");
	var txt=$(obj).text();
	//指定单个
	if(val == -1){
		parentArr.length = 0;
	}else{
		for(var i=0;i<parentArr.length;i++){
			if(parentArr[i][0] == -1){
				parentArr.splice(i,1);
			}
		}
	}
	var new_arr = Array(val, txt);
	filterArr(parentArr,new_arr);
	addItemInput(parentArr,$(inputId));
}

//周期列表
var period_list = [["day","每天"],["week","每周"],["month","每月"]];

var hour_list = [];
for(var i=0; i<=23; i++){
	var i2 = i;
	if (i < 10) i2 = "0" + i;
	hour_list.push([i2+":00", i2+":00"]);
	hour_list.push([i2+":30", i2+":30"]);
}

var week_day_list = [["1","周一"],["2","周二"],["3","周三"],["4","周四"],["5","周五"],["6","周六"],["7","周日"],["work_day","工作日（周一至周五）"],["holiday","休假日（周六至周日）"]];

var day_list = [];
for(var i=1; i<=31; i++){
	var i2 = i;
	if (i < 10) i2 = "0" + i;
	day_list.push([i,i2+"日"]);
}
day_list.push(["last_day","最后一天"]);

//切换周期
function switchPeriod(){
	var label = $("#periodId").val();
	var $dds = $("#periodId").parents("dl").find(".pad");
	$dds.hide().parent().find("."+label+"_pad").show();
}

function disPeriod(obj){
	pickData(obj,'#camp')
	if($("#campId").val() != ''){
		var $input = $("input[name=sendCode][value=plan]");
		$input.prop('disabled',true);
		$input.closest("div").addClass('f666');
	}	
}
     