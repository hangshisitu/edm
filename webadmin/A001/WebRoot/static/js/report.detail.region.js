$(document).ready(function() {
	var color_list = ["#72429A", "#846FA5", "#9F8EBC", "#A595C5"];
	if(!isEmptyArr(names)){
		$("#column").highcharts({
			chart: { type: 'column' },
		    title: { text: "" },
	        credits: { enabled: false },
	        colors: color_list,
		    xAxis: { categories: names },
		    yAxis: { title: { text: "" }, min: 0 },
		    legend: {
	            borderWidth: 1,
	            borderRadius: 0,
	            itemStyle: {
	        	   fontSize: "10px"
	        	}
		    },
		    tooltip: {
		    	headerFormat: "{point.key}<br />",
	            pointFormat: "{series.name}: " + "{point.y}",
	        	style: { fontSize: "10px", padding: "4px" }
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
});

function task_exports(tid) {
	if(tid == "") {
		error_message("没有需要导出的地域报告", 1);
		return false;
	}
	
	location.assign("/report/export/task/region?taskId=" + tid);
}

function campaign_exports(cid) {
	if(cid == "") {
		error_message("没有需要导出的地域报告", 1);
		return false;
	}
	
	location.assign("/report/export/campaign/region?campaignId=" + cid);
}