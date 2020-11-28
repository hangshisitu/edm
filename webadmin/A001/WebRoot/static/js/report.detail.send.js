$(document).ready(function() {
	var color_list = ["#72429A", "#846FA5", "#9F8EBC", "#A595C5"];
	$("#send_list").UIselect({
		width: 70,
		id_id: "#send",
		ho_fun: "send()",
		arrdata: new Array(["success", "成功"], ["delay", "软弹回"], ["fail", "硬弹回"])
	});
	
	var names = ["发送总数", "成功数", "弹回数"];
	if(!isEmptyArr(column_datas)){
		$("#column").highcharts({
			chart: { type: "column" },
		    title: { text: "" },
	        credits: { enabled: false },
	        colors: color_list,
		    xAxis: { categories: names },
		    yAxis: { title: { text: "" }, min: 0 },
		    tooltip: { 
	        	formatter: function() {
	        		return names[this.point.x] + ": " + column_datas[this.point.x];
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
		        data: column_datas,
		   	 	showInLegend: false
		    }]
	    });
	}
    
	if(pie_names.length > 0 && pie_datas.length > 0) {
		$("#pie").highcharts({
	        title: { text: "" },
	        credits: { enabled: false },
	        colors: ["#603787", "#72429A", "#846FAB", "#9F8EBC", "#A595C5", "#BBB9D0"],
	        tooltip: { 
	        	formatter: function() {
	        		return pie_names[this.point.x][1] + "：" + pie_names[this.point.x][2];
	        	},
	        	style: { fontSize: "10px", padding: "4px" }
	        },
	        plotOptions: {
	            pie: {
	                allowPointSelect: true,
	                cursor: "pointer",
	                dataLabels: {
	                    enabled: true,
	                    formatter: function() {
	                        return pie_names[this.point.x][1] + "：" + this.percentage.toFixed(2) +" %";
	                    }
	                }
	            }
	        },
	        series: [{
	            type: "pie",
	            data: pie_datas,
	            showInLegend: false
	        }]
	    });
	}
});

function task_exports(tid) {
	if(tid == "") {
		error_message("没有需要导出的弹回报告", 1);
		return false;
	}
	
	location.assign("/report/export/task/send?taskId=" + tid);
}

function campaign_exports(cid) {
	if(cid == "") {
		error_message("没有需要导出的发送报告", 1);
		return false;
	}
	
	location.assign("/report/export/campaign/send?campaignId=" + cid);
}