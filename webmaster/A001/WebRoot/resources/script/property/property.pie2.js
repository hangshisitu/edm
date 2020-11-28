
var industry_chart,society_chart,age_chart,gender_chart;
$(document).ready(function() {
	
	//行业属性
	industry_chart = new Highcharts.Chart({
         chart: {
        	 renderTo: 'stat_industry_pie',
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
             	 size:'29',
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
             type: 'pie'
            // data: industry_datas
         }]
     });
	
	
	//社会身份
	society_chart = new Highcharts.Chart({
         chart: {
        	 renderTo: 'stat_society_pie',
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
             	 size:'29',
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
             type: 'pie'
            // data: industry_datas
         }]
     });
	
	
	//年龄
	age_chart = new Highcharts.Chart({
         chart: {
        	 renderTo: 'stat_age_pie',
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
             	 size:'29',
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
             type: 'pie'
            // data: industry_datas
         }]
     });
	
	//性别
	gender_chart = new Highcharts.Chart({
         chart: {
        	 renderTo: 'stat_gender_pie',
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
             	 size:'29',
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
             type: 'pie'
            // data: industry_datas
         }]
     });
	
	 
	var url = $("#ajaxUrl").val();
	//异步请求数据
    $.ajax({
        type:"POST",
        url:url+"?rand="+new Date().getTime(),//提供数据的Servlet
        success:function(data){
        	
        	$("#totalEmails").html("<font color='blue'>"+data.propStat.totalEmailNum+"</font>");
        	
        	industryStatArray = [],
        	$("#industryLabelsNum").html(data.propStat.industryStat.labelNum);
        	$("#industryEmailsNum").html(data.propStat.industryStat.emailNum);
        	if(data.propStat.industryStat.emailNum==0) {
        		$("#industryDiv").html('暂无数据...').show();
        		$("#stat_industry_pie").hide();
        	}else {
        		//迭代，把异步获取的数据放到数组中
                $.each(data.propStat.industryStat.propertyVoList,function(i,d){
                	industryStatArray.push({name:d.labelName,y:d.percentage,num:d.num});
                });
                //设置数据
				$("#industryDiv").html('').hide();
				$("#stat_industry_pie").show();
                industry_chart.series[0].setData(industryStatArray);
        	}
            
            
            
            societyStatArray = [],
            $("#societyLabelsNum").html(data.propStat.societyStat.labelNum);
        	$("#societyEmailsNum").html(data.propStat.societyStat.emailNum);
        	if(data.propStat.societyStat.emailNum==0) {
        		$("#societyDiv").html('暂无数据...').show();
        		$("#stat_society_pie").hide();
        	}else {
        		 //迭代，把异步获取的数据放到数组中
                $.each(data.propStat.societyStat.propertyVoList,function(i,d){
                	societyStatArray.push({name:d.labelName,y:d.percentage,num:d.num});
                });
                //设置数据
				$("#societyDiv").html('').hide();
				$("#stat_society_pie").show();
                society_chart.series[0].setData(societyStatArray);				
        	}
           
            
            
            ageStatArray = [],
            $("#ageLabelsNum").html(data.propStat.ageStat.labelNum);
        	$("#ageEmailsNum").html(data.propStat.ageStat.emailNum);
        	if(data.propStat.ageStat.emailNum==0) {
        		$("#ageDiv").html('暂无数据...').show();
        		$("#stat_age_pie").hide();
        	}else {
        		 //迭代，把异步获取的数据放到数组中
                $.each(data.propStat.ageStat.propertyVoList,function(i,d){
                	ageStatArray.push({name:d.labelName,y:d.percentage,num:d.num});
                });
                //设置数据
				$("#ageDiv").html('').hide();
				$("#stat_age_pie").show();
                age_chart.series[0].setData(ageStatArray);
        	}
           
            
            genderStatArray = [],
            $("#genderLabelsNum").html(data.propStat.genderStat.labelNum);
        	$("#genderEmailsNum").html(data.propStat.genderStat.emailNum);
        	if(data.propStat.genderStat.emailNum==0) {
        		$("#genderDiv").html('暂无数据...').show();
        		$("#stat_gender_pie").hide();
        	}else {
        		//迭代，把异步获取的数据放到数组中
                $.each(data.propStat.genderStat.propertyVoList,function(i,d){
                	genderStatArray.push({name:d.labelName,y:d.percentage,num:d.num});
                });
                //设置数据
				$("#genderDiv").html('').hide();
				$("#stat_gender_pie").show();
                gender_chart.series[0].setData(genderStatArray);
        	}
            
        },
        error:function(e){
            $(".pie_tips").html('加载失败！');
        }
    });

	
	
	
});