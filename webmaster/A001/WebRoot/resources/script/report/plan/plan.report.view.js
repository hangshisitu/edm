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


function exportReportView(url) {
	location.assign(url);
}

function column() {
	var names = ['发送总数', '成功数', '弹回数', '打开人数', '打开次数', '点击人数', '点击次数', '退订人数', '转发人数','无效人数'];
	$("#stat_column").highcharts({
    	chart: { type: "column" },
	    title: { text: "" },
        credits: { enabled: false },
        colors: ["#72429A", "#846FA5", "#9F8EBC", "#BBB9D0"],
        xAxis: { categories: names},
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

function funnel() {
	$("#stat_funnel").highcharts({
        chart: { type: "funnel", marginRight: 120 },
        title: { 
        	text: "<b>基于独立收件人行为统计</b>",
        	style: { color: "#A8A8C0", fontSize: "12px" },
        	x: -54
        },
        credits: { enabled: false },
        colors: ["#72429A", "#846FA5", "#9F8EBC", "#BBB9D0"],
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

function cover() {
	if($("#cover").val() == "column") {
		$("#stat_funnel").hide();
		$("#stat_column").show();
		column();
	} else {
		$("#stat_column").hide();
		$("#stat_funnel").show();
		funnel();
	}
} 