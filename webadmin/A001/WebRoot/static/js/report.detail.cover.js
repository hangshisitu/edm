$(document).ready(function() {
	$("#cover_list").UIselect({
		width: 70,
		id:"#cover_list",
		id_id: "#cover",
		ho_fun: "cover()",
		arrdata: new Array(["column", "柱状图"], ["funnel", "漏斗图"])
	});
	
	column();
});

var color_list = ["#72429A", "#846FA5", "#9F8EBC", "#A595C5"];

function column() {
	if(!isEmptyArr(datas)){
		$("#column").highcharts({
	    	chart: { type: "column" },
		    title: { text: "" },
	        credits: { enabled: false },
	        colors: color_list,
	        xAxis: { categories: names },
		    yAxis: { title: { text: "" }, min: 0 },
		    tooltip: { 
	        	formatter: function() {
	        		return names[this.point.x] + ": " + datas[this.point.x];
	        	},
	        	style: { fontSize: "10px", padding: "4px" }
	        },
	        plotOptions: {
	            column: {
	                pointPadding: 0.2,
	                borderWidth: 0
	            }
	        },
		    series: [{
		        data: datas,
		   	 	showInLegend: false
		    }]
	    });
	}		
}

function funnel() {
	if(!isEmptyArr(datas)){
		$("#funnel").highcharts({
	        chart: { type: "funnel", marginRight: 120 },
	        title: { 
	        	text: "<b>基于独立收件人行为统计</b>",
	        	style: { color: "#A8A8C0", fontSize: "12px" },
	        	x: -54
	        },
	        credits: { enabled: false },
	        colors: color_list,
	        tooltip: { 
	        	formatter: function() {
	        		return percents[this.point.x][0] + "：" + percents[this.point.x][1] + "%";
	        	},
	        	style: { fontSize: "10px", padding: "4px" }
	        },
	        plotOptions: {
	            series: {
	                dataLabels: {
	                    enabled: true,
	                    format: "<b>{point.name}</b> ({point.y:,.0f})",
	                    color: "black",
	                    softConnector: true
	                },
	                neckWidth: "30%",
	                neckHeight: "25%"
	            }
	        },
	        series: [{
	        	data: [
					["发送总数", datas[0]],
					["成功总数", datas[1]],
					["打开人数", datas[3]],
					["点击人数", datas[5]]
				],
		   	 	showInLegend: false
			}]
	    });
	}
}

function cover() {
	if($("#cover").val() == "column") {
		$("#funnel").hide();
		$("#column").show();
		column();
	} else {
		$("#column").hide();
		$("#funnel").show();
		funnel();
	}
}

function task_exports(tid) {
	if(tid == "") {
		error_message("没有需要导出的概览报告", 1);
		return false;
	}
	
	location.assign("/report/export/task/cover?taskId=" + tid);
}

function campaign_exports(cid) {
	if(cid == "") {
		error_message("没有需要导出的概览报告", 1);
		return false;
	}
	
	location.assign("/report/export/campaign/cover?campaignId=" + cid);
}

function plan_exports(pid) {
	if(pid == "") {
		error_message("没有需要导出的周期报告", 1);
		return false;
	}
	
	location.assign("/report/export/plan?planId=" + pid);
}