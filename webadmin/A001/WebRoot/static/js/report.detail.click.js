$(document).ready(function() {
	var color_list = ["#72429A", "#846FA5", "#9F8EBC", "#A595C5"];
	var names = [
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    	11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    	21, 22, 23
    ];
	
	if(!isEmptyArr(datas)){
		$("#line").highcharts({
			chart: { type: "line" },
	        title: { text: "" },
	        credits: { enabled: false },
	        colors: color_list,
	        xAxis: { categories: names },
	        yAxis: {
	            title: { text: "" },
	            min: 0
	        },
	        tooltip: { 
	        	formatter: function() {
	        		return names[this.point.x] + ":00 " + datas[this.point.x] + "人";
	        	},
	        	style: { fontSize: "10px", padding: "4px" }
	        },
	        series: [{
	            data: datas,
	       	 	showInLegend: false
	        }]
	    });	
	}	
});

function task_exports(tid) {
	if(tid == "") {
		error_message("没有需要导出的点击报告", 1);
		return false;
	}
	
	location.assign("/report/export/task/click?taskId=" + tid);
}

function campaign_exports(cid) {
	if(cid == "") {
		error_message("没有需要导出的点击报告", 1);
		return false;
	}
	
	location.assign("/report/export/campaign/click?campaignId=" + cid);
}