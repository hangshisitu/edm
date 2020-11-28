$(document).ready(function() {
	var color_list = ["#8B5BB3", "#1AADCE", "#8BBC21", "#2F7ED8", "#910000"];
	
	$('#stat_compare_column').highcharts({
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
    
    $(".color_grid").each(function(i){
    	$(this).css("backgroundColor",color_list[$(".color_grid").size() - (i + 1)]);
    });
});


function goBack() {
	if($("#fromFlag").val() == "apiGroupsByUser") {
		$("#ListForm1").attr("action", $("#apiGroupsByUserUrl").val());
	}else if($("#fromFlag").val() == "apiGroupsByAll"){
		$("#ListForm1").attr("action", $("#apiGroupsByAllUrl").val());
	}else if($("#fromFlag").val() == "customer"){
		$("#ListForm1").attr("action", $("#customerUrl").val());
	}else{
		$("#ListForm1").attr("action", $("#taskUrl").val());
	}
	$("#ListForm1").submit();
	
}