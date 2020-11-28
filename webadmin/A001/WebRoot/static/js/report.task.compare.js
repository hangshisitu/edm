$(document).ready(function() {
	var color_list = ["#BBB9D0", "#A595C5", "#9F8EBC", "#846FA5", "#72429A", "#603787"];
	if(!isEmptyArr(datas)){
		$('#task_bar').highcharts({
	        chart: { type: "bar" },
	        title: { text: "" },
	        colors: color_list,
	        credits: { enabled: false },
	        xAxis: { categories: names },
	        yAxis: { title: { text: "" }, min: 0 },
	        tooltip: {
	        	formatter: function() {
	        		return this.series.name + "<br />" + names[this.point.x] + ": " + this.point.y;
	        	},
	        	style: { fontSize: "10px", padding: "4px" }
	        },
	        legend: {
	            borderWidth: 1,
	            borderRadius: 0,
	            itemStyle: {
	        	   fontSize: "10px"
	        	},
	    		reversed: true
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
    
    $(".color_grid").each(function(i){
    	$(this).css("backgroundColor",color_list[$(".color_grid").size() - (i + 1)]);
    });
});