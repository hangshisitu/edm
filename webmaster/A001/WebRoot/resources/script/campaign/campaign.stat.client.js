$(document).ready(function() {

	$("#open_pie").highcharts({
		title : {
			text : "<b>浏览器</b>",
			style : {
				color : "#A8A8C0",
				fontSize : "12px"
			}
		},
		credits : {
			enabled : false
		},
		tooltip : {
			formatter : function() {
				return browser_names[this.point.x][1] + "<br />人数：" + browser_names[this.point.x][2] + "<br />占比：" + browser_names[this.point.x][3] + ' %';
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
		title : {
			text : "<b>操作系统</b>",
			style : {
				color : "#A8A8C0",
				fontSize : "12px"
			}
		},
		credits : {
			enabled : false
		},
		tooltip : {
			formatter : function() {
				return system_names[this.point.x][1] + "<br />人数：" + system_names[this.point.x][2] + "<br />占比：" + system_names[this.point.x][3] + ' %';
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
		title : {
			text : "<b>语言环境</b>",
			style : {
				color : "#A8A8C0",
				fontSize : "12px"
			}
		},
		credits : {
			enabled : false
		},
		tooltip : {
			formatter : function() {
				return language_names[this.point.x][1] + "<br />人数：" + language_names[this.point.x][2] + "<br />占比：" + language_names[this.point.x][3] + ' %';
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



function exportCsv(url) {
	$("#ListForm").attr("action",url);
	$("#ListForm").submit();
} 