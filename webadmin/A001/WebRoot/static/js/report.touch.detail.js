$(document).ready(function() {
	var color_list = ["#603787", "#72429A", "#846FAB", "#9F8EBC", "#A595C5", "#BBB9D0"];
	if(open_names.length > 0 && open_datas.length > 0) {
		$("#open_pie").highcharts({
	        title: {
				text: "<b>各任务的触发总人数占比</b>",
				style: { color: "#A8A8C0", fontSize: "12px" } 
			},
			tooltip: {
        		formatter: function() {
	        		return open_names[this.point.x][1] + "<br/>触发人数：" + open_names[this.point.x][2];
	        	},
	        	style: { fontSize: "10px", padding: "4px" }
	        },
	        credits: { enabled: false },
	        colors: color_list,
	        plotOptions: {
	            pie: {
	            	size:'11',
	                allowPointSelect: true,
	                cursor: "pointer",
	                dataLabels: {
	                	distance:15,
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
				text: "<b>各任务的打开总人数占比</b>",
				style: { color: "#A8A8C0", fontSize: "12px" } 
			},
			tooltip: {
        		formatter: function() {
	        		return click_names[this.point.x][1] + "<br/>打开人数：" + click_names[this.point.x][2];
	        	},
	        	style: { fontSize: "10px", padding: "4px" }
	        },
	        credits: { enabled: false },
	        colors: color_list,
	        plotOptions: {
	            pie: {
	            	size:'11',
	                allowPointSelect: true,
	                cursor: "pointer",
	                dataLabels: {
	                	distance:15,
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

function touch_exports(taskId, templateId) {
	if(taskId == "") {
		error_message("没有需要导出的概览报告", 1);
		return false;
	}
	location.assign("/report/export/touch/touch?taskId=" + taskId + "&templateId=" + templateId);
}

function cover_exports(touchId, templateId) {
	if(touchId == "") {
		error_message("没有需要导出的概览报告", 1);
		return false;
	}
	location.assign("/report/export/touch/cover?touchId=" + touchId + "&templateId=" + templateId);
}

function recipients_exports(taskId, templateId) {
	if(taskId == "") {
		error_message("没有需要导出的解发计划报告", 1);
		return false;
	}
	
	location.assign("/report/export/touch/recipients?taskId=" + taskId + "&templateId=" + templateId);
}

