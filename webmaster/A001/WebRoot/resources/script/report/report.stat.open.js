$(document).ready(function() {
	var names = [
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    	11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    	21, 22, 23
    ];
	
	$("#stat_line").highcharts({
		chart: { type: "line" },
        title: { text: "邮件打开情况统计" },
        credits: { enabled: false },
        colors: ["#72429A", "#846FA5", "#9F8EBC", "#BBB9D0"],
        xAxis: { categories: names,
        	     title: { text: "时间" }
        },
        yAxis: {
            title: { text: "开启量" },
            min: 0
        },
        tooltip: { 
        	formatter: function() {
        		return names[this.point.x] + ":00 " + datas[this.point.x] + "人";
        	},
        	style: { fontSize: "10px", padding: "4px" }
        },
        plotOptions: {
            line: {
                dataLabels: {
                    enabled: true
                },
                enableMouseTracking: true
            }
        },
        series: [{
            data: datas,
       	 	showInLegend: false
        }]
    });
	
	var url = $("#url").val();
	$.ajax({
		url: url,
		type: "post",
		dataType: "json",
		success: function(data) {
			$.each(data.openList, function(k, v) {
				var text = "";
				text += "<tr id=\"logger_" + (k + 1) + "\" onMouseOver=\"this.style.backgroundColor='#44b5a7'\" onMouseOut=\"this.style.backgroundColor=''\">";
				text += "<td>" + v.email + "</td>";
				text += "<td>" + v.region + "</td>";
				text += "<td>" + v.time + "</td>";
				text += "</tr>";
				$("#logger_" + k).after(text);
			});
		}
	});
});


function exportOpen(url) {
	$("#ListForm").attr("action",url);
	$("#ListForm").submit();
}