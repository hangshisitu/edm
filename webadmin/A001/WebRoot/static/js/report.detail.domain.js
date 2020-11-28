$(document).ready(function() {
	var color_list = ["#603787", "#72429A", "#846FAB", "#9F8EBC", "#A595C5", "#BBB9D0"];
	if(open_names.length > 0 && open_datas.length > 0) {
		$("#open_pie").highcharts({
	        title: {
				text: "<b>打开人数百分比 基于同域名（打开人数/发送总数）</b>",
				style: { color: "#A8A8C0", fontSize: "12px" } 
			},
	        credits: { enabled: false },
	        colors: color_list,
	        tooltip: { 
	        	formatter: function() {
	        		return open_names[this.point.x][1] + "<br />发送总数：" + open_names[this.point.x][2] + "<br />打开人数：" + open_names[this.point.x][3];
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
	                        return "<b>"+ open_names[this.point.x][1] + "</b>：" + this.percentage.toFixed(2) +" %";
	                    }
	                }
	            }
	        },
	        series: [{
	            type: "pie",
	            data: open_datas,
	            showInLegend: false
	        }]
	    });
	}
	if(click_names.length > 0 && click_datas.length > 0) {
		$("#click_pie").highcharts({
	        title: {
				text: "<b>点击人数百分比 基于同域名（点击人数/发送总数）</b>",
				style: { color: "#A8A8C0", fontSize: "12px" } 
			},
	        credits: { enabled: false },
	        colors: color_list,
	        tooltip: { 
	        	formatter: function() {
	        		return click_names[this.point.x][1] + "<br />发送总数：" + click_names[this.point.x][2] + "<br />点击人数：" + click_names[this.point.x][3];
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
	                        return "<b>"+ click_names[this.point.x][1] + "</b>：" + this.percentage.toFixed(2) +" %";
	                    }
	                }
	            }
	        },
	        series: [{
	            type: "pie",
	            data: click_datas,
	            showInLegend: false
	        }]
	    });
	}
});

function task_exports(tid) {
	if(tid == "") {
		error_message("没有需要导出的域名报告", 1);
		return false;
	}
	
	location.assign("/report/export/task/domain?taskId=" + tid);
}

function campaign_exports(cid) {
	if(cid == "") {
		error_message("没有需要导出的域名报告", 1);
		return false;
	}
	
	location.assign("/report/export/campaign/domain?campaignId=" + cid);
}