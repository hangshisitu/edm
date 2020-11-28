$(document).ready(function() {
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
        		return percents[this.point.x][0] + ": " + percents[this.point.x][1] + "%";
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
				["打开人数", datas[2]],
				["点击人数", datas[3]]
			],
	   	 	showInLegend: false
		}]
    });
});



