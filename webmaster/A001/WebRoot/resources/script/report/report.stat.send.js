$(document).ready(function() {
	var names = ['发送总数', '成功数', '弹回数'];
	$("#stat_column").highcharts({
    	chart: { type: "column" },
	    title: { text: "" },
        credits: { enabled: false },
        colors: ["#72429A", "#846FA5", "#9F8EBC", "#BBB9D0"],
        xAxis: { categories: names },
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
    
    //弹回统计
    if((backDatas[0] + backDatas[1])>0){
    	response_chart = new Highcharts.Chart({
	         chart: {
	        	 renderTo: 'stat_response_pie',
	             plotBackgroundColor: null,
	             plotBorderWidth: null,
	             plotShadow: false
	         },
	         title: {
	             text: '',
	             style: {
	                 fontFamily: 'Verdana, sans-serif'
	             }
	         },
	         credits: { enabled: false },
	         colors: ["#72429A", "#846FA5", "#9F8EBC", "#BBB9D0"],
	         tooltip: {
	     	    formatter: function() {
	              // Highcharts.numberFormat(this.percentage,2)格式化数字，保留2位精度
	     	    	 return "<b>"+this.point.name+"</b><br/><b>弹回数："+this.point.num+"</b><br/><b>占比："+Highcharts.numberFormat(this.point.y,2)+'%</b>';
	     	    },
	     	   percentageDecimals: 2
	         },
	         plotOptions: {
	             pie: {
	                 allowPointSelect: true,
	                 cursor: 'pointer',
	                 dataLabels: {
	     				distance:20,
	                     enabled: true,
	                     color: '#000000',
	                     connectorColor: '#000000',
	                     formatter: function() {
	                    	 return '<b>'+ this.point.name +'</b>: '+ Highcharts.numberFormat(this.percentage,2) +'%';
	                     }
	                 }
	             }
	         },
	         series: [{
	             type: 'pie',
	             data: response_datas
	         }]
	     });
    }
});



function exportSend(url) {
	$("#ListForm").attr("action",url);
	$("#ListForm").submit();
} 