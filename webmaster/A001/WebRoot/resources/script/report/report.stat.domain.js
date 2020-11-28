$(document).ready(function() {
	 $('#stat_domain_pie').highcharts({
         chart: {
             plotBackgroundColor: null,
             plotBorderWidth: null,
             plotShadow: false
         },
         colors: ["#72429A", "#846FA5", "#9F8EBC", "#BBB9D0"],
         title: {
             text: '域名发送数百分比(发送数/发送总数)',
             style: {
                 fontFamily: 'Verdana, sans-serif'
             }
         },
         credits: { enabled: false },
         tooltip: {
     	    formatter: function() {
               //Highcharts.numberFormat(this.percentage,2)格式化数字，保留2位精度
               return this.point.name+':<b>'+Highcharts.numberFormat(this.percentage,2) +'%</b>';
     	    },
         	percentageDecimals: 2
         },
         plotOptions: {
             pie: {
                 allowPointSelect: true,
                 cursor: 'pointer',
                 dataLabels: {
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
             data: datas
         }]
     });
});


function exportDomain(url) {
	$("#ListForm").attr("action",url);
	$("#ListForm").submit();
} 