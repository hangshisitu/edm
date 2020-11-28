$(document).ready(function() {
	//有数据才生成
	if(nameDatas.length>0){
		$("#stat_region_column").highcharts({
			chart: { type: 'column' },
		    title: { text: "" },
	        credits: { enabled: false },
	        colors: ["#72429A", "#BBB9D0"],
		    xAxis: { categories: nameDatas },
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
		    series: readUserDatas
		});
	}
	
});



function exportRegion(url) {
	$("#ListForm").attr("action",url);
	$("#ListForm").submit();
}