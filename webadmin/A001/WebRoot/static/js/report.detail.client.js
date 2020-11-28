$(document).ready(function() {
	var color_list = ["#603787", "#72429A", "#846FAB", "#9F8EBC", "#A595C5", "#BBB9D0"];
	$("#open_pie").highcharts({
		title : { text : "" },
		credits : { enabled : false },
        colors: color_list,
		tooltip : {
			formatter : function() {
				return browser_names[this.point.x][1] + "<br />人数：" + browser_names[this.point.x][2] + "<br />占比：" + browser_names[this.point.x][3] + " %";
			},
			style : {
				fontSize : "10px",
				padding : "4px"
			}
		},
		plotOptions : {
			pie : {
				size:'13',
				allowPointSelect : true,
				cursor : "pointer",
				dataLabels : {
					enabled : true,
					formatter : function() {
						return "<b>" + browser_names[this.point.x][1] + "</b>：" + this.percentage.toFixed(2) + " %";
					}
				}
			}
		},
		series : [{
			type : "pie",
			data : browser_datas,
			showInLegend : false
		}]
	});
	
	//操作系统
	$("#system_pie").highcharts({
		title : { text : "" },
		credits : { enabled : false },
		colors: color_list,
		tooltip : {
			formatter : function() {
				return system_names[this.point.x][1] + "<br />人数：" + system_names[this.point.x][2] + "<br />占比：" + system_names[this.point.x][3] + " %";
			},
			style : {
				fontSize : "10px",
				padding : "4px"
			}
		},
		plotOptions : {
			pie : {
				size:'13',
				allowPointSelect : true,
				cursor : "pointer",
				dataLabels : {
					enabled : true,
					formatter : function() {
						return "<b>" + system_names[this.point.x][1] + "</b>：" + this.percentage.toFixed(2) + " %";
					}
				}
			}
		},
		series : [{
			type : "pie",
			data : system_datas,
			showInLegend : false
		}]
	});
	
	//语言环境
	$("#language_pie").highcharts({
		title : { text : "" },
		credits : { enabled : false },
		colors: color_list,
		tooltip : {
			formatter : function() {
				return language_names[this.point.x][1] + "<br />人数：" + language_names[this.point.x][2] + "<br />占比：" + language_names[this.point.x][3] + " %";
			},
			style : {
				fontSize : "10px",
				padding : "4px"
			}
		},
		plotOptions : {
			pie : {
				size:'13',
				allowPointSelect : true,
				cursor : "pointer",
				dataLabels : {
					enabled : true,
					formatter : function() {
						return "<b>" + language_names[this.point.x][1] + "</b>：" + this.percentage.toFixed(2) + " %";
					}
				}
			}
		},
		series : [{
			type : "pie",
			data : language_datas,
			showInLegend : false
		}]
	});

});

function campaign_exports(cid) {
	if(cid == "") {
		error_message("没有需要导出的终端报告", 1);
		return false;
	}
	
	location.assign("/report/export/campaign/client?campaignId=" + cid);
}