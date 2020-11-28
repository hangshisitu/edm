$(document).ready(function() {
	var color_list = ["#72429A", "#846FAB", "#9F8EBC", "#A595C5"];
	
	$('#period_list').UIselect({
		width: 110,
		id_id: "#periodId",
		arrdata: new Array(["week", "周数据对比"], ["month", "月度数据对比"], ["season", "季度数据对比"]),
		ho_fun:"swicthBox()"
	})
	
	$('#week_list').UIselect({
		width: 110,
		id_id: "#weekId",
		arrdata: new Array(["2", "最近2周"], ["3", "最近3周"], ["4", "最近4周"]),
		ho_fun:"postParam()"
	})
	
	$('#month_list').UIselect({
		width: 110,
		id_id: "#monthId",
		arrdata: new Array(["2", "最近2个月"], ["3", "最近3个月"], ["4", "最近4个月"],["otherMonth", "其他月份对比"]),
		ho_fun:"postParam()" 
	})
	
	$('#season_list').UIselect({
		width: 110,
		id_id: "#seasonId",
		arrdata: new Array(["2", "最近2个季度"], ["3", "最近3个季度"], ["4", "最近4个季度"],["otherSeason", "其他季度对比"]),
		ho_fun:"postParam()" 
	})
	
	//默认显示周月季
	var text = $('#periodId').val();
	$('.prd_box').hide();
	$('#'+text+'Box').show();
    
    //生成其他月份列表
    $('#monthPad .setting .a_select').each(function(i){
    	$("#m_list_"+i).UIselect({
			width: 150,
			id_id: "#m_"+i,
			arrdata: dateArr
		});
    })
    
     //生成其他季度列表
    $('#monthPad .setting .a_select').each(function(i){
    	$("#s_list_"+i).UIselect({
			width: 150,
			id_id: "#s_"+i,
			arrdata: seasonArr
		});
    })
	
	//其他月份对比
	$('#smt').bind('click',function(){
		if($('#m_0').val()=='' || $('#m_1').val()==''){
			$('#monthPad .err').text('请选择必填项');
			return;
		}
		
		var arr=[];
		$('input[name=mth]').each(function(){
			if($(this).val()!=''){
				arr.push($(this).val());
			}
		})
		//不允许重复
		if(isRepeat(arr)){
			$('#monthPad .err').text('请去除重复选择');
			return false;
		}
		var text = $('#periodId').val();
		var href = 'compare?campaignId=' + cid + '&m=' +text+ '&checkeds=' + arr.join(',');

		location.assign(href);		
		closeDialog();
	})
	
	//其他季度对比
	$('#seasonBtn').bind('click',function(){
		if($('#s_0').val()=='' || $('#s_1').val()==''){
			$('#seasonPad .err').text('请选择必填项');
			return;
		}
		
		var arr=[];
		$('input[name=sth]').each(function(){
			if($(this).val()!=''){
				arr.push($(this).val());
			}
		})
		
		//不允许重复
		if(isRepeat(arr)){
			$('#seasonPad .err').text('请去除重复选择');
			return false;
		}
		var text = $('#periodId').val();
		var href = 'compare?campaignId=' + cid + '&m=' +text+ '&checkeds=' + arr.join(',');
		location.assign(href);		
		closeDialog();
	})
	
	if(!isEmptyArr(datas)){
		$('#bar').highcharts({
	        chart: { type: "bar" },
	        title: { text: "" },
	        colors: color_list,
	        credits: { enabled: false },
	        xAxis: { categories: names },
	        yAxis: { title: { text: "" }, min: 0 },
	        tooltip: {
	        	formatter: function() {
	        		return this.series.name + "<br />" + names[this.point.x] + ": " + this.point.y;
	        	},
	        	style: { fontSize: "10px", padding: "4px" }
	        },
	        legend: {
	            borderWidth: 1,
	            borderRadius: 0,
	            itemStyle: {
	        	   fontSize: "10px"
	        	},
	    		reversed: true
		    },
		    plotOptions: {
	            column: {
	                pointPadding: 0.2,
	                borderWidth: 0
	            } 
	        },
	        series: datas
	    });
	}
    
    $(".color_grid").each(function(i){
    	$(this).css("backgroundColor",color_list[$(".color_grid").size() - (i + 1)]);
    });
    
});

//月季周联动
function swicthBox(){
	var text = $('#periodId').val();
	$('.prd_box').hide();
	$('#'+text+'Box').show();
	postParam();
}

//点击传参
function postParam(){
	var text = $('#periodId').val();
	var num = $('#'+text+'Box input[type=hidden]').val();
	if(num == 'otherMonth'){
		openPop(0,0,'#monthPad');
	}else if(num == 'otherSeason'){
		openPop(0,0,'#seasonPad');
	}else{
		var href = '/report/detail/campaign/compare?campaignId=' + cid + '&m=' +text+ '&recents=' + num;
		location.assign(href);
	}
}

function createDateArr(){
	//动态生成月份列表
    var nowYear = new Date().getFullYear();
    var dataYears = [nowYear-2,nowYear-1,nowYear];
    var nowMonth = new Date().getMonth()+1;
    var dataMonths = [1,2,3,4,5,6,7,8,9,10,11,12];
    for(i=0;i<dataYears.length;i++){
    	for(k=0;k<dataMonths.length;k++){
    		if(dataMonths[k]<10){
    			var val = ''+dataYears[i]+'0'+dataMonths[k];   			
    		}else{
    			var val = ''+dataYears[i]+dataMonths[k];
    		}
    		var text = dataYears[i]+'年'+dataMonths[k]+'月';
    		dateArr.unshift([val,text]);
    		
    		if(dataYears[i]==nowYear && dataMonths[k]==nowMonth){
    			break;
    		}
    	}
    }  
    //动态生成季度
    var dataSeasons = [1,2,3,4];
    var currentSeason = Math.floor( ( nowMonth % 3 == 0 ? ( nowMonth / 3 ) : ( nowMonth / 3 + 1 ) ) );
    for(i=0;i<dataYears.length;i++){
    	for(k=0;k<dataSeasons.length;k++){
    		var val = ''+dataYears[i]+dataSeasons[k];
    		var text = dataYears[i]+'年'+dataSeasons[k]+'季';
    		seasonArr.unshift([val,text]);
    		
    		if(dataYears[i]==nowYear && dataSeasons[k]==currentSeason){
    			break;
    		}
    	}
    }
}

function campaign_exports(cid) {
	if(cid == "") {
		error_message("没有需要导出的对比报告", 1);
		return false;
	}
	
	//周期单位
	var m = $('#periodId').val();
	var recents = $('.btn .prd_box:visible').find('input').val();
	//其它月季的参数列表
	var arr=[];
	if(recents == 'otherSeason'){
		$('input[name=sth]').each(function(){
			if($(this).val()!=''){
				arr.push($(this).val());
			}
		})
	}else if(recents == 'otherMonth'){
		$('input[name=mth]').each(function(){
			if($(this).val()!=''){
				arr.push($(this).val());
			}
		})
	}
	
	//导出
	location.assign("/report/export/campaign/compare?campaignId=" + cid + "&m=" + m + "&recents=" + recents + "&checkeds=" + arr.join(','));
}
