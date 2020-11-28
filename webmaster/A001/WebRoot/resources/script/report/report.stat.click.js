$(document).ready(function() {
	for(i=0;i<titles.length;i++){
		if(titles[i].length>15){
			names.push(titles[i].substr(0,15)+"...");
		}else{
			names.push(titles[i]);
		}
	}
	
	$("#stat_column").highcharts({
    	chart: { type: "column" },
	    title: { text: "" },
        credits: { enabled: false },
        colors: ["#72429A", "#846FA5", "#9F8EBC", "#BBB9D0"],
        xAxis: { 
        	categories: names,
        	labels: {
                style: {
                	color: '#333',fontSize: '12px',width:'100px',display:'inline-block'
                }
            }
        },
	    yAxis: { title: { text: "" }, min: 0 },
	    tooltip: { 
        	formatter: function() {
        		return "链接："+titles[this.point.x] + " 点击次数：" + datas[this.point.x];
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
});



function exportClick(url) {
	$("#ListForm").attr("action",url);
	$("#ListForm").submit();
}