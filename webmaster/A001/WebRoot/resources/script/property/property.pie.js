$(document).ready(function() {
	//行业属性
	 $('#stat_industry_pie').highcharts({
         chart: {
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
         tooltip: {
     	    formatter: function() {
              // Highcharts.numberFormat(this.percentage,2)格式化数字，保留2位精度
     	    	 return "<b>"+this.point.name+"</b><br/><b>收件人数："+this.point.num+"</b><br/><b>占比："+Highcharts.numberFormat(this.point.y,2)+'%</b>';
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
             data: industry_datas
         }]
     });
	 
	//社会身份
	 $('#stat_society_pie').highcharts({
         chart: {
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
         tooltip: {
     	    formatter: function() {
              // Highcharts.numberFormat(this.percentage,2)格式化数字，保留2位精度
     	    	 return "<b>"+this.point.name+"</b><br/><b>收件人数："+this.point.num+"</b><br/><b>占比："+Highcharts.numberFormat(this.point.y,2)+'%</b>';
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
             data: society_datas
         }]
     });
	 
	 
	//年龄段
	 $('#stat_age_pie').highcharts({
         chart: {
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
         tooltip: {
     	    formatter: function() {
              // Highcharts.numberFormat(this.percentage,2)格式化数字，保留2位精度
     	    	 return "<b>"+this.point.name+"</b><br/><b>收件人数："+this.point.num+"</b><br/><b>占比："+Highcharts.numberFormat(this.point.y,2)+'%</b>';
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
             data: age_datas
         }]
     });
	 
	//性别
	 $('#stat_gender_pie').highcharts({
         chart: {
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
         tooltip: {
     	    formatter: function() {
              // Highcharts.numberFormat(this.percentage,2)格式化数字，保留2位精度
     	    	 return "<b>"+this.point.name+"</b><br/><b>收件人数："+this.point.num+"</b><br/><b>占比："+Highcharts.numberFormat(this.point.y,2)+'%</b>';
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
             data: gender_datas
         }]
     });
	 
});